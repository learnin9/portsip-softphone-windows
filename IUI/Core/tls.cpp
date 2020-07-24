// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include <stddef.h>

// #ifdef _DEBUG
// #define new	IUI_DEBUG_NEW
// #endif // _DEBUG


#define DEBUG_ONLY(f)      (f)


/////////////////////////////////////////////////////////////////////////////
// CIUISimpleList

void CIUISimpleList::AddHead(void *p)
{
	_ASSERT(p != NULL);
	_ASSERT(*GetNextPtr(p) == NULL);

	*GetNextPtr(p) = m_pHead;
	m_pHead = p;
}

BOOL CIUISimpleList::Remove(void *p)
{
	_ASSERT(p != NULL);

	if (m_pHead == NULL)
	{
		return FALSE;
	}

	BOOL bResult = FALSE;
	if (m_pHead == p)
	{
		m_pHead = *GetNextPtr(p);
		DEBUG_ONLY(*GetNextPtr(p) = NULL);
		bResult = TRUE;
	}
	else
	{
		void *pTest = m_pHead;
		while (pTest != NULL && *GetNextPtr(pTest) != p)
		{
			pTest = *GetNextPtr(pTest);
		}
		if (pTest != NULL)
		{
			*GetNextPtr(pTest) = *GetNextPtr(p);
			DEBUG_ONLY(*GetNextPtr(p) = NULL);
			bResult = TRUE;
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CIUINoTrackObject

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
void *PASCAL CIUINoTrackObject::operator new (size_t nSize, LPCSTR, int)
{
	return CIUINoTrackObject::operator new (nSize);
}

void PASCAL CIUINoTrackObject::operator delete (void *pObject, LPCSTR, int)
{
	if (pObject != NULL)
	{
		::LocalFree(pObject);
	}
}
#endif

void *PASCAL CIUINoTrackObject::operator new (size_t nSize)
{
	void *p = ::LocalAlloc(LPTR, nSize);
	if (p == NULL)
	{
		//AfxThrowMemoryException();
	}
	return p;
}

void PASCAL CIUINoTrackObject::operator delete (void *p)
{
	if (p != NULL)
	{
		::LocalFree(p);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CIUIThreadSlotData

// global _iuiThreadData used to allocate thread local indexes
BYTE __iuiThreadData[sizeof(CIUIThreadSlotData)];
CIUIThreadSlotData *_iuiThreadData = NULL;

namespace IUI
{
	struct CIUIThreadData : public CIUINoTrackObject
	{
		CIUIThreadData *pNext; // required to be member of CIUISimpleList
		int nCount;         // current size of pData
		LPVOID *pData;      // actual thread local data (indexed by nSlot)
	};

	struct CIUISlotData
	{
		DWORD dwFlags;      // slot flags (allocated/not allocated)
		HINSTANCE hInst;    // module which owns this slot
	};

} // namespace IUI
// flags used for CIUISlotData::dwFlags above
#define SLOT_USED   0x01    // slot is allocated

CIUIThreadSlotData::CIUIThreadSlotData()
{
	m_list.Construct(offsetof(CIUIThreadData, pNext));

	// initialize state and allocate TLS index
	m_nAlloc = 0;
	m_nRover = 1;   // first slot (0) is always reserved
	m_nMax = 0;
	m_pSlotData = NULL;

	// init m_tlsIndex to -1 if !bThreadLocal, otherwise TlsAlloc
	m_tlsIndex = TlsAlloc();
	if (m_tlsIndex == (DWORD)(-1))
	{
		//AfxThrowMemoryException();
		return;
	}

	InitializeCriticalSection(&m_sect);
}

CIUIThreadSlotData::~CIUIThreadSlotData()
{
	CIUIThreadData *pData = m_list;
	while (pData != NULL)
	{
		CIUIThreadData *pDataNext = pData->pNext;
		DeleteValues(pData, NULL);
		pData = pDataNext;
	}

	if (m_tlsIndex != (DWORD)(-1))
	{
		TlsFree(m_tlsIndex);
		DEBUG_ONLY(m_tlsIndex = (DWORD)(-1));
	}

	if (m_pSlotData != NULL)
	{
		HGLOBAL hSlotData = GlobalHandle(m_pSlotData);
		GlobalUnlock(hSlotData);
		GlobalFree(hSlotData);
		DEBUG_ONLY(m_pSlotData = NULL);
	}

	DeleteCriticalSection(&m_sect);
}

int CIUIThreadSlotData::AllocSlot()
{
	EnterCriticalSection(&m_sect);
	int nAlloc = m_nAlloc;
	int nSlot = m_nRover;
	if (nSlot >= nAlloc || (m_pSlotData[nSlot].dwFlags & SLOT_USED))
	{
		// search for first free slot, starting at beginning
		for (nSlot = 1;
			nSlot < nAlloc && (m_pSlotData[nSlot].dwFlags & SLOT_USED); nSlot++)
			;

		// if none found, need to allocate more space
		if (nSlot >= nAlloc)
		{
			// realloc memory for the bit array and the slot memory
			int nNewAlloc = m_nAlloc + 32;
			HGLOBAL hSlotData;
			if (m_pSlotData == NULL)
			{
				hSlotData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
						nNewAlloc * sizeof(CIUISlotData));
			}
			else
			{
				hSlotData = GlobalHandle(m_pSlotData);
				GlobalUnlock(hSlotData);
				hSlotData = GlobalReAlloc(hSlotData, nNewAlloc * sizeof(CIUISlotData),
						GMEM_MOVEABLE | GMEM_SHARE);
			}

			if (hSlotData == NULL)
			{
				if (m_pSlotData != NULL)
				{
					GlobalLock(GlobalHandle(m_pSlotData));
				}
				LeaveCriticalSection(&m_sect);
				//AfxThrowMemoryException();
				return nSlot;
			}
			CIUISlotData *pSlotData = (CIUISlotData *)GlobalLock(hSlotData);

			// always zero initialize after success
			memset(pSlotData + m_nAlloc, 0, (nNewAlloc - m_nAlloc)*sizeof(CIUISlotData));
			m_nAlloc = nNewAlloc;
			m_pSlotData = pSlotData;
		}
	}
	_ASSERT(nSlot != 0); // first slot (0) is reserved

	// adjust m_nMax to largest slot ever allocated
	if (nSlot >= m_nMax)
	{
		m_nMax = nSlot + 1;
	}

	_ASSERT(!(m_pSlotData[nSlot].dwFlags & SLOT_USED));
	m_pSlotData[nSlot].dwFlags |= SLOT_USED;
	// update m_nRover (likely place to find a free slot is next one)
	m_nRover = nSlot + 1;

	LeaveCriticalSection(&m_sect);
	return nSlot;   // slot can be used for FreeSlot, GetValue, SetValue
}

void CIUIThreadSlotData::FreeSlot(int nSlot)
{
	EnterCriticalSection(&m_sect);
	_ASSERT(nSlot != 0 && nSlot < m_nMax);
	_ASSERT(m_pSlotData != NULL);
	_ASSERT(m_pSlotData[nSlot].dwFlags & SLOT_USED);
	if (nSlot <= 0 || nSlot >= m_nMax)  // check for retail builds.
	{
		LeaveCriticalSection(&m_sect);
		return;
	}

	// delete the data from all threads/processes
	CIUIThreadData *pData = m_list;
	while (pData != NULL)
	{
		if (nSlot < pData->nCount)
		{
			delete (CIUINoTrackObject *)pData->pData[nSlot];
			pData->pData[nSlot] = NULL;
		}
		pData = pData->pNext;
	}
	// free the slot itself
	m_pSlotData[nSlot].dwFlags &= ~SLOT_USED;
	LeaveCriticalSection(&m_sect);
}

// special version of CIUIThreadSlotData::GetData that only works with
// thread local storage (and not process local storage)
// this version is inlined and simplified for speed
inline void *CIUIThreadSlotData::GetThreadValue(int nSlot)
{
	EnterCriticalSection(&m_sect);
	_ASSERT(nSlot != 0 && nSlot < m_nMax);
	_ASSERT(m_pSlotData != NULL);
	_ASSERT(m_pSlotData[nSlot].dwFlags & SLOT_USED);
	_ASSERT(m_tlsIndex != (DWORD)(-1));
	if (nSlot <= 0 || nSlot >= m_nMax)  // check for retail builds.
	{
		LeaveCriticalSection(&m_sect);
		return NULL;
	}

	CIUIThreadData *pData = (CIUIThreadData *)TlsGetValue(m_tlsIndex);
	if (pData == NULL || nSlot >= pData->nCount)
	{
		LeaveCriticalSection(&m_sect);
		return NULL;
	}
	void *pRetVal = pData->pData[nSlot];
	LeaveCriticalSection(&m_sect);
	return pRetVal;
}

void CIUIThreadSlotData::SetValue(int nSlot, void *pValue)
{
	EnterCriticalSection(&m_sect);
	_ASSERT(nSlot != 0 && nSlot < m_nMax);
	_ASSERT(m_pSlotData != NULL);
	_ASSERT(m_pSlotData[nSlot].dwFlags & SLOT_USED);
	if (nSlot <= 0 || nSlot >= m_nMax)  // check for retail builds.
	{
		LeaveCriticalSection(&m_sect);
		return;
	}

	CIUIThreadData *pData = (CIUIThreadData *)TlsGetValue(m_tlsIndex);
	if (pData == NULL || nSlot >= pData->nCount && pValue != NULL)
	{
		// if pData is NULL then this thread has not been visited yet
		if (pData == NULL)
		{
			//TRY
			{
				pData = new CIUIThreadData;
			}
			//CATCH_ALL(e)
			//{
			//	LeaveCriticalSection(&m_sect);
			//	THROW_LAST();
			//}
			//END_CATCH_ALL
			pData->nCount = 0;
			pData->pData = NULL;
			DEBUG_ONLY(pData->pNext = NULL);

			m_list.AddHead(pData);
		}

		// grow to now current size
		void **ppvTemp;
		if (pData->pData == NULL)
		{
			ppvTemp = (void **)LocalAlloc(LMEM_FIXED, m_nMax * sizeof(LPVOID));
		}
		else
		{
			ppvTemp = (void **)LocalReAlloc(pData->pData, m_nMax * sizeof(LPVOID),
					LMEM_MOVEABLE);
		}
		if (ppvTemp == NULL)
		{
			LeaveCriticalSection(&m_sect);
			//AfxThrowMemoryException();
			return;
		}
		pData->pData = ppvTemp;

		// initialize the newly allocated part
		memset(pData->pData + pData->nCount, 0,
			(m_nMax - pData->nCount) * sizeof(LPVOID));
		pData->nCount = m_nMax;
		TlsSetValue(m_tlsIndex, pData);
	}
	_ASSERT(pData->pData != NULL && nSlot < pData->nCount);
	if (pData->pData != NULL && nSlot < pData->nCount)
	{
		pData->pData[nSlot] = pValue;
	}
	LeaveCriticalSection(&m_sect);
}

void CIUIThreadSlotData::AssignInstance(HINSTANCE hInst)
{
	EnterCriticalSection(&m_sect);
	_ASSERT(m_pSlotData != NULL);
	_ASSERT(hInst != NULL);

	for (int i = 1; i < m_nMax; i++)
	{
		if (m_pSlotData[i].hInst == NULL && (m_pSlotData[i].dwFlags & SLOT_USED))
		{
			m_pSlotData[i].hInst = hInst;
		}
	}
	LeaveCriticalSection(&m_sect);
}

void CIUIThreadSlotData::DeleteValues(CIUIThreadData *pData, HINSTANCE hInst)
{
	_ASSERT(pData != NULL);

	// free each element in the table
	BOOL bDelete = TRUE;
	for (int i = 1; i < pData->nCount; i++)
	{
		if (hInst == NULL || m_pSlotData[i].hInst == hInst)
		{
			// delete the data since hInst matches (or is NULL)
			delete (CIUINoTrackObject *)pData->pData[i];
			pData->pData[i] = NULL;
		}
		else if (pData->pData[i] != NULL)
		{
			// don't delete thread data if other modules still alive
			bDelete = FALSE;
		}
	}

	if (bDelete)
	{
		// remove from master list and free it
		EnterCriticalSection(&m_sect);
		m_list.Remove(pData);
		LeaveCriticalSection(&m_sect);
		LocalFree(pData->pData);
		delete pData;

		// clear TLS index to prevent from re-use
		TlsSetValue(m_tlsIndex, NULL);
	}
}

void CIUIThreadSlotData::DeleteValues(HINSTANCE hInst, BOOL bAll)
{
	EnterCriticalSection(&m_sect);
	if (!bAll)
	{
		// delete the values only for the current thread
		CIUIThreadData *pData = (CIUIThreadData *)TlsGetValue(m_tlsIndex);
		if (pData != NULL)
		{
			DeleteValues(pData, hInst);
		}
	}
	else
	{
		// delete the values for all threads
		CIUIThreadData *pData = m_list;
		while (pData != NULL)
		{
			CIUIThreadData *pDataNext = pData->pNext;
			DeleteValues(pData, hInst);
			pData = pDataNext;
		}
	}
	LeaveCriticalSection(&m_sect);
}

/////////////////////////////////////////////////////////////////////////////
// CIUIThreadLocalObject

CIUINoTrackObject *CIUIThreadLocalObject::GetData(
	CIUINoTrackObject * (*pfnCreateObject)())
{
	ENSURE(pfnCreateObject);

	if (m_nSlot == 0)
	{
		if (_iuiThreadData == NULL)
		{
			//
			// 1. TlsAlloc。
			//

			// 第一次执行，运行到这里，分配全局线程数据_iuiThreadData，CIUIThreadSlotData
			// 的构造函数调用TlsAlloc，查询空闲线程数据槽索引，并保存到
			// CIUIThreadSlotData::m_tlsIndex中。
			// 特别注意：一旦调用TlsAlloc后返回一个空闲的数据槽索引，那么表示进程中所有线程
			// 及之后创建的新线程的这个索引的数据槽都被我们占用了。 这个数据槽索引不是线程的。
			// 所以把它保存到了全局对象_iuiThreadData的m_tlsIndex变量中。
			_iuiThreadData = new (__iuiThreadData) CIUIThreadSlotData;
			ENSURE(_iuiThreadData != NULL);
		}

		// 第一次执行，运行到这里，分配32个CSlotData对象空间，并保存到CThreadSlotData::m_pSlotData中，
		// 并把m_pSlotData中索引为1的元素标记为SLOT_USED，并返回索引1。
		m_nSlot = _iuiThreadData->AllocSlot();
		ENSURE(m_nSlot != 0);
	}

	// 第一次执行，运行到这里，pValue返回NULL，因为我们还从未往线程索引为m_tlsIndex
	//（CThreadSlotData类型的_afxThreadData中保存，与m_nSlot有对应关系）的
	// 槽中放置过数据，自然也得不到数据
	CIUINoTrackObject *pValue = static_cast<CIUINoTrackObject *>(_iuiThreadData->GetThreadValue(m_nSlot));
	if (pValue == NULL)
	{
		// allocate zero-init object
		pValue = (*pfnCreateObject)();

		// set tls data to newly created object
		_iuiThreadData->SetValue(m_nSlot, pValue);
		_ASSERT(_iuiThreadData->GetThreadValue(m_nSlot) == pValue);
	}
	return pValue;
}

CIUINoTrackObject *CIUIThreadLocalObject::GetDataNA()
{
	if (m_nSlot == 0 || _iuiThreadData == NULL)
	{
		return NULL;
	}

	CIUINoTrackObject *pValue =
		(CIUINoTrackObject *)_iuiThreadData->GetThreadValue(m_nSlot);
	return pValue;
}

CIUIThreadLocalObject::~CIUIThreadLocalObject()
{
	if (m_nSlot != 0 && _iuiThreadData != NULL)
	{
		_iuiThreadData->FreeSlot(m_nSlot);
	}
	m_nSlot = 0;
}

CIUINoTrackObject *CIUIProcessLocalObject::GetData(
	CIUINoTrackObject * (*pfnCreateObject)())
{
	if (m_pObject == NULL)
	{
		IUILockGlobals(CRIT_PROCESSLOCAL);
		//TRY
		{
			if (m_pObject == NULL)
			{
				m_pObject = (*pfnCreateObject)();
			}
		}
		//CATCH_ALL(e)
		//{
		//	AfxUnlockGlobals(CRIT_PROCESSLOCAL);
		//	THROW_LAST();
		//}
		//END_CATCH_ALL
		IUIUnlockGlobals(CRIT_PROCESSLOCAL);
	}
	return m_pObject;
}

CIUIProcessLocalObject::~CIUIProcessLocalObject()
{
	if (m_pObject != NULL)
	{
		delete m_pObject;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Init/Term for thread/process local data

//void AFXAPI AfxInitLocalData(HINSTANCE hInst)
//{
//	if (_iuiThreadData != NULL)
//	{
//		_iuiThreadData->AssignInstance(hInst);
//	}
//}
//
void IUIAPI IUI::IUITermLocalData(HINSTANCE hInst, BOOL bAll)
{
	if (_iuiThreadData != NULL)
	{
		_iuiThreadData->DeleteValues(hInst, bAll);
	}
}

// This reference count is needed to support Win32s, such that the
// thread-local and process-local data is not destroyed prematurely.
// It is basically a reference count of the number of processes that
// have attached to the MFC DLL.

//AFX_STATIC_DATA long _afxTlsRef = 0;

//void AFXAPI AfxTlsAddRef()
//{
//	++_afxTlsRef;
//}
//
void IUIAPI IUI::IUITlsRelease()
{
	//if (_afxTlsRef == 0 || --_afxTlsRef == 0)
	{
		if (_iuiThreadData != NULL)
		{
			_iuiThreadData->~CIUIThreadSlotData();
			_iuiThreadData = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

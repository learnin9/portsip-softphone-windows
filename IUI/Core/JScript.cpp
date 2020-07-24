#include "stdafx.h"

#if(_MSC_VER > 1200) // VC6.0
#include <atlcomcli.h>
#else
#include <ATLBASE.H>
#endif

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


CJScript::CJScript()
{
	m_pDoc = NULL;
}

CJScript::~CJScript()
{

}

//<<Begin>>////////////////////////////////////////////////////////////////////////

const CIUIString CJScript::GetSystemErrorMessage(DWORD dwError)
{
	CIUIString strError;
	LPTSTR lpBuffer;

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,  dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			(LPTSTR) &lpBuffer, 0, NULL))

	{
		strError = "FormatMessage Netive Error" ;
	}
	else
	{
		strError = lpBuffer;
		LocalFree(lpBuffer);
	}
	return strError;
}

CIUIString CJScript::GetNextToken(CIUIString &strSrc, const CIUIString strDelim, BOOL bTrim, BOOL bFindOneOf)
{
	CIUIString strToken;
	int idx = bFindOneOf ? strSrc.FindOneOf(strDelim) : strSrc.Find(strDelim);
	if (idx != -1)
	{
		strToken  = strSrc.Left(idx);
		strSrc = strSrc.Right(strSrc.GetLength() - (idx + 1));
	}
	else
	{
		strToken = strSrc;
		strSrc.Empty();
	}
	if (bTrim)
	{
		strToken.TrimLeft();
		strToken.TrimRight();
	}
	return strToken;
}

BOOL CJScript::SetScriptDocument(LPDISPATCH pDisp)
{
	if (pDisp == NULL)
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	m_pDoc = NULL;

	CComPtr<IDispatch> spDisp = pDisp;

	HRESULT hr = spDisp->QueryInterface(IID_IHTMLDocument2, (void **)&m_pDoc);
	if (FAILED(hr))
	{
		OutputDebugString(_T("Failed to get HTML document COM object\n"));
		return FALSE;
	}

	return TRUE;
}

BOOL CJScript::GetJScript(IDispatch **pDisp)
{
	if (m_pDoc == NULL)
	{
		return FALSE;
	}

	HRESULT hr = m_pDoc->get_Script(pDisp);
	_ASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

BOOL CJScript::GetJScripts(IHTMLElementCollection *pColl)
{
	if (m_pDoc == NULL)
	{
		return FALSE;
	}

	HRESULT hr = m_pDoc->get_scripts(&pColl);
	_ASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

CIUIString CJScript::ScanJScript(CIUIString &strAText, std::vector<CIUIString> &args)
{
	args.clear();
	CIUIString strDelim(" \n\r\t"), strSrc(strAText);
	BOOL bFound = FALSE;
	while (!strSrc.IsEmpty())
	{
		CIUIString strStart = GetNextToken(strSrc, strDelim);
		if (strStart == "function")
		{
			bFound = TRUE;
			break;
		}
		if (strStart == "/*")
		{
			// Skip comments
			while (!strSrc.IsEmpty())
			{
				CIUIString strStop = GetNextToken(strSrc, strDelim);
				if (strStop == "*/")
				{
					break;
				}
			}
		}
	}

	if (!bFound)
	{
		return _T("");
	}

	CIUIString strFunc = GetNextToken(strSrc, _T("("), TRUE);
	CIUIString strArgs = GetNextToken(strSrc, _T(")"), TRUE);

	// Parse arguments
	CIUIString strArg;
	while (!(strArg = GetNextToken(strArgs, _T(","))).IsEmpty())
	{
		args.push_back(strArg);
	}

	strAText = strSrc;
	return strFunc;
}

BOOL CJScript::JScriptOK()
{
	return (m_pDoc != NULL);
}

// C++调用JS函数
// 调用方式：CallJScript(_T("AttachADCallBack"), _T("sdfsdf"));
// JS调用C++函数，是通过window.navigate，传递事先约定的参数格式，由DISPID_BEFORENAVIGATE2响应
BOOL CJScript::CallJScript(const CIUIString strFunc, _variant_t *pVarResult)
{
	std::vector<CIUIString> paramArray;
	return CallJScript(strFunc, paramArray, pVarResult);
}

BOOL CJScript::CallJScript(const CIUIString strFunc, const CIUIString strArg1, _variant_t *pVarResult)
{
	std::vector<CIUIString> paramArray;
	paramArray.push_back(strArg1);
	return CallJScript(strFunc, paramArray, pVarResult);
}

BOOL CJScript::CallJScript(const CIUIString strFunc, const CIUIString strArg1, const CIUIString strArg2, _variant_t *pVarResult)
{
	std::vector<CIUIString> paramArray;
	paramArray.push_back(strArg1);
	paramArray.push_back(strArg2);
	return CallJScript(strFunc, paramArray, pVarResult);
}

BOOL CJScript::CallJScript(const CIUIString strFunc, const CIUIString strArg1, const CIUIString strArg2, const CIUIString strArg3, _variant_t *pVarResult)
{
	std::vector<CIUIString> paramArray;
	paramArray.push_back(strArg1);
	paramArray.push_back(strArg2);
	paramArray.push_back(strArg3);
	return CallJScript(strFunc, paramArray, pVarResult);
}

BOOL CJScript::CallJScript(const CIUIString strFunc, const std::vector<CIUIString> &paramArray, _variant_t *pVarResult)
{
	CComPtr<IDispatch> spScript;
	if (!GetJScript(&spScript))
	{
		OutputDebugString(_T("函数GetJScrip调用失败！\n"));
		return FALSE;
	}
	CComBSTR bstrFunc(strFunc);
	DISPID dispid = NULL;
	HRESULT hr = spScript->GetIDsOfNames(IID_NULL, &bstrFunc, 1,
			LOCALE_SYSTEM_DEFAULT, &dispid);
	if (FAILED(hr))
	{
		OutputDebugString(GetSystemErrorMessage(hr));
		return FALSE;
	}

	INT_PTR arraySize = paramArray.size();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = (UINT)arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];

	for (int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray[arraySize - 1 - i]; // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	_variant_t vaResult;
	UINT nArgErr = (UINT)(-1); // initialize to invalid arg

	hr = spScript->Invoke(dispid, IID_NULL, 0,
			DISPATCH_METHOD, &dispparams, &vaResult, &excepInfo, &nArgErr);

	delete [] dispparams.rgvarg;
	if (FAILED(hr))
	{
		OutputDebugString(GetSystemErrorMessage(hr));
		return FALSE;
	}

	if (pVarResult)
	{
		*pVarResult = vaResult;
	}

	return TRUE;
}

// 修改网页元素的内容
BOOL CJScript::PutElementHtml(CIUIString ElemID, CIUIString Html)
{
	IHTMLDocument2 *pDoc = NULL;
	IHTMLElementCollection *pElemColl = NULL;
	IDispatch *pElemDisp = NULL;
	IHTMLElement *pElem = NULL;

	BOOL Result = FALSE;

	if (m_pDoc != NULL)
	{
		pDoc->get_all(&pElemColl);
		if (pElemColl != NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(LONG(0));
			HRESULT hr = pElemColl->item(varID, varIdx, &pElemDisp);
			if (SUCCEEDED(hr))
			{
				if (pElemDisp != NULL)
				{
					hr = pElemDisp->QueryInterface(IID_IHTMLElement, (void **)&pElem);
					if (SUCCEEDED(hr))
					{
						BSTR bstrHtml = Html.AllocSysString();
						pElem->put_innerHTML(bstrHtml);
						pElem->Release();
						::SysFreeString(bstrHtml);
						Result = TRUE;
					}
					pElemDisp->Release();
				}
			}
		}//if(pAllElem!=NULL)
		pDoc->Release();
	}//if(pDoc!=NULL)
	return Result;
}

BOOL CJScript::GetElementValue(CIUIString ElemID, CIUIString &Value)
{
	Value.Empty();

	IHTMLElementCollection *pAllElem = NULL;
	IDispatch *pElemDisp = NULL;
	IHTMLElement *pElem = NULL;

	IHTMLInputElement *pInput = NULL;
	IHTMLSelectElement *pSelect = NULL;
	IHTMLTextAreaElement *pTextArea = NULL;


	BOOL Result = FALSE;
	HRESULT hr = S_FALSE;

	if (m_pDoc != NULL)
	{
		hr = m_pDoc->get_all(&pAllElem);
		if (SUCCEEDED(hr) && pAllElem != NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(LONG(0));
			hr = pAllElem->item(varID, varIdx, &pElemDisp);
			if (SUCCEEDED(hr) && pElemDisp != NULL)
			{
				hr = pElemDisp->QueryInterface(IID_IHTMLElement, (void **)&pElem);
				if (SUCCEEDED(hr) && pElem != NULL)
				{
					BSTR bstrTagName;
					hr = pElem->get_tagName(&bstrTagName);
					if (SUCCEEDED(hr))
					{
						CIUIString strTagName(bstrTagName);
						SysFreeString(bstrTagName);
						strTagName.MakeUpper();
						if (strTagName == "INPUT")
						{
							hr = pElem->QueryInterface(&pInput);
							if (SUCCEEDED(hr) && pInput != NULL)
							{
								BSTR bstrType;
								hr = pInput->get_type(&bstrType);
								if (SUCCEEDED(hr))
								{
									CIUIString strType(bstrType);
									strType.MakeUpper();
									SysFreeString(bstrType);
									if (strType == "TEXT" || strType == "HIDDEN")
									{
										BSTR bstrValue;
										hr = pInput->get_value(&bstrValue);
										if (SUCCEEDED(hr))
										{
											CIUIString strValue(bstrValue);
											SysFreeString(bstrValue);
											Value = strValue;
											Result = TRUE;
										}
									} // if(strType == "TEXT" || strType == "HIDDEN")
									else if (strType == "CHECKBOX" || strType == "RADIO")
									{
										VARIANT_BOOL Check;
										hr = pInput->get_checked(&Check);
										if (SUCCEEDED(hr))
										{
											if (Check == VARIANT_TRUE)
											{
												Value = "1";
											}
											else
											{
												Value = "0";
											}
											Result = TRUE;
										}
									} // else if(strType=="CHECKBOX" || strType == "RADIO")
								} // if(SUCCEEDED(hr))
								pInput->Release();
							} // if(SUCCEEDED(hr) && pInput!=NULL)
						} // if(strTagName == "INPUT")
						else if (strTagName == "SELECT")
						{
							hr = pElem->QueryInterface(&pSelect);
							if (SUCCEEDED(hr) && pSelect != NULL)
							{
								long index;
								hr = pSelect->get_selectedIndex(&index);
								if (SUCCEEDED(hr))
								{
									Value.Format(_T("%d"), index);
									Result = TRUE;
								}
								pSelect->Release();
							} // if(SUCCEEDED(hr) && pSelect!=NULL)
						} // else if(strTagName == "SELECT")
						else if (strTagName == "TEXTAREA")
						{
							hr = pElem->QueryInterface(&pTextArea);
							if (SUCCEEDED(hr) && pTextArea != NULL)
							{
								BSTR bstrValue;
								hr = pTextArea->get_value(&bstrValue);
								if (SUCCEEDED(hr))
								{
									CIUIString strValue(bstrValue);
									SysFreeString(bstrValue);
									Value = strValue;
									Result = TRUE;
								}
								pTextArea->Release();
							}
						} // else if(strTagName == "TEXTAREA")
					}
					pElem->Release();
				}
			} // if(SUCCEEDED(hr) && pElem != NULL)
			pAllElem->Release();
		} // if(SUCCEEDED(hr) && pAllElem != NULL)
		m_pDoc->Release();
	} // if(pDoc != NULL)

	return Result;
}

BOOL CJScript::PutElementValue(CIUIString ElemID, CIUIString Value)
{
	IHTMLElementCollection *pAllElem = NULL;
	IDispatch *pElemDisp = NULL;
	IHTMLElement *pElem = NULL;

	IHTMLInputElement *pInput = NULL;
	IHTMLSelectElement *pSelect = NULL;
	IHTMLTextAreaElement *pTextArea = NULL;


	BOOL Result = FALSE;
	HRESULT hr = S_FALSE;

	if (m_pDoc != NULL)
	{
		hr = m_pDoc->get_all(&pAllElem);
		if (SUCCEEDED(hr) && pAllElem != NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(LONG(0));
			hr = pAllElem->item(varID, varIdx, &pElemDisp);
			if (SUCCEEDED(hr) && pElemDisp != NULL)
			{
				hr = pElemDisp->QueryInterface(IID_IHTMLElement, (void **)&pElem);
				if (SUCCEEDED(hr) && pElem != NULL)
				{
					BSTR bstrTagName;
					hr = pElem->get_tagName(&bstrTagName);
					if (SUCCEEDED(hr))
					{
						CIUIString strTagName(bstrTagName);
						SysFreeString(bstrTagName);
						strTagName.MakeUpper();
						if (strTagName == "INPUT")
						{
							hr = pElem->QueryInterface(&pInput);
							if (SUCCEEDED(hr) && pInput != NULL)
							{
								BSTR bstrType;
								hr = pInput->get_type(&bstrType);
								if (SUCCEEDED(hr))
								{
									CIUIString strType(bstrType);
									strType.MakeUpper();
									SysFreeString(bstrType);
									if (strType == "TEXT" || strType == "HIDDEN")
									{
										BSTR bstrValue;
										bstrValue = Value.AllocSysString();
										hr = pInput->put_value(bstrValue);
										SysFreeString(bstrValue);
										if (SUCCEEDED(hr))
										{
											Result = TRUE;
										}
									} // if(strType == "TEXT" || strType == "HIDDEN")
									else if (strType == "CHECKBOX" || strType == "RADIO")
									{
										VARIANT_BOOL Check;
										if (Value == "1")
										{
											Check = VARIANT_TRUE;
										}
										else
										{
											Check = VARIANT_FALSE;
										}

										hr = pInput->put_checked(Check);
										if (SUCCEEDED(hr))
										{
											Result = TRUE;
										}
									} // else if(strType=="CHECKBOX" || strType == "RADIO")
								} // if(SUCCEEDED(hr))
								pInput->Release();
							} // if(SUCCEEDED(hr) && pInput!=NULL)
						} // if(strTagName == "INPUT")
						else if (strTagName == "SELECT")
						{
							hr = pElem->QueryInterface(&pSelect);
							if (SUCCEEDED(hr) && pSelect != NULL)
							{
								long index;
								index = LongFromString(Value);
								hr = pSelect->put_selectedIndex(index);
								if (SUCCEEDED(hr))
								{
									Result = TRUE;
								}
								pSelect->Release();
							} // if(SUCCEEDED(hr) && pSelect!=NULL)
						} // else if(strTagName == "SELECT")
						else if (strTagName == "TEXTAREA")
						{
							hr = pElem->QueryInterface(&pTextArea);
							if (SUCCEEDED(hr) && pTextArea != NULL)
							{
								BSTR bstrValue;
								bstrValue = Value.AllocSysString();
								hr = pTextArea->put_value(bstrValue);
								SysFreeString(bstrValue);
								if (SUCCEEDED(hr))
								{
									Result = TRUE;
								}
								pTextArea->Release();
							}
						} // else if(strTagName == "TEXTAREA")
					}
					pElem->Release();
				}
			} // if(SUCCEEDED(hr) && pElem != NULL)
			pAllElem->Release();
		} // if(SUCCEEDED(hr) && pAllElem != NULL)
		m_pDoc->Release();
	} // if(pDoc != NULL)

	return Result;
}

void CJScript::ElementSetFocus(CIUIString ElemID)
{
	IHTMLElementCollection *pAllElem = NULL;
	IHTMLElement *pElem = NULL;
	IHTMLControlElement *pControl = NULL;
	HRESULT hr = S_FALSE;

	if (m_pDoc != NULL)
	{
		hr = m_pDoc->get_all(&pAllElem);
		if (SUCCEEDED(hr) && pAllElem != NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(LONG(0));
			hr = pAllElem->item(varID, varIdx, (LPDISPATCH *)&pElem);
			if (SUCCEEDED(hr) && pElem != NULL)
			{
				pElem->QueryInterface(&pControl);
				if (pControl != NULL)
				{
					pControl->focus();
					pControl->Release();
				}
				pElem->Release();
			} // if(SUCCEEDED(hr) && pElem != NULL)
			pAllElem->Release();
		}//if(pAllElem!=NULL)
		m_pDoc->Release();
	}//if(pDoc!=NULL)
}

IHTMLStyle *CJScript::GetElementStyle(CIUIString ElemID)
{
	IHTMLElementCollection *pElemColl = NULL;
	IDispatch *pElemDisp = NULL;
	IHTMLElement *pElem = NULL;
	IHTMLStyle *pStyle = NULL;

	BOOL Result = FALSE;

	if (m_pDoc != NULL)
	{
		m_pDoc->get_all(&pElemColl);
		if (pElemColl != NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(LONG(0));
			HRESULT hr = pElemColl->item(varID, varIdx, &pElemDisp);
			if (SUCCEEDED(hr))
			{
				if (pElemDisp != NULL)
				{
					hr = pElemDisp->QueryInterface(IID_IHTMLElement, (void **)&pElem);
					if (SUCCEEDED(hr))
					{
						hr = pElem->get_style(&pStyle);
						if (SUCCEEDED(hr))
						{
							Result = TRUE;
						}
						pElem->Release();
					}
					pElemDisp->Release();
				}
			}
		}//if(pAllElem!=NULL)
		m_pDoc->Release();
	}//if(pDoc!=NULL)

	return pStyle;
}

HRESULT CJScript::GetElement(LPCTSTR szElementId, IDispatch **ppdisp,
	BOOL *pbCollection /*= NULL*/)
{
	CComPtr<IHTMLElementCollection> sphtmlAll;
	CComPtr<IHTMLElementCollection> sphtmlColl;
	CComPtr<IDispatch> spdispElem;
	CComVariant varName;
	CComVariant varIndex;
	HRESULT hr = S_OK;
	USES_CONVERSION;

	*ppdisp = NULL;

	if (pbCollection)
	{
		*pbCollection = FALSE;
	}

	if (m_pDoc == NULL)
	{
		return hr;
	}

	varName.vt = VT_BSTR;
	varName.bstrVal = T2BSTR(szElementId);
	if (!varName.bstrVal)
	{
		hr = E_OUTOFMEMORY;
		goto __Error;
	}

	hr = m_pDoc->get_all(&sphtmlAll);
	if (sphtmlAll == NULL)
	{
		goto __Error;
	}
	hr = sphtmlAll->item(varName, varIndex, &spdispElem);
	if (spdispElem == NULL)
	{
		hr = E_NOINTERFACE;
		goto __Error;
	}

	spdispElem->QueryInterface(__uuidof(IHTMLElementCollection), (void **) &sphtmlColl);
	if (sphtmlColl)
	{
		if (pbCollection)
		{
			*pbCollection = TRUE;
		}
#ifdef _DEBUG
		else
		{
			IUITRACE(_T("[IUI]===Warning: duplicate IDs or NAMEs.\n"));
			_ASSERT(FALSE);
		}
#endif

	}
__Error:
	if (SUCCEEDED(hr))
	{
		*ppdisp = spdispElem;
		if (spdispElem)
		{
			(*ppdisp)->AddRef();
		}
	}
	return hr;
}

HRESULT CJScript::GetElement(LPCTSTR szElementId, IHTMLElement **pphtmlElement)
{
	return GetElementInterface(szElementId, __uuidof(IHTMLElement), (void **) pphtmlElement);
}

HRESULT CJScript::GetElementInterface(LPCTSTR szElementId, REFIID riid, void **ppvObj)
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObj = NULL;
	CComPtr<IDispatch> spdispElem;

	hr = GetElement(szElementId, &spdispElem);

	if (spdispElem)
	{
		hr = spdispElem->QueryInterface(riid, ppvObj);
	}
	return hr;
}

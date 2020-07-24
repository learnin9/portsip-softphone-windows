#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{


	//////////////////////////////////////////////////////////////////////////////////////////
	// Constructors

	UIVARIANT::UIVARIANT()
	{
		Init();
	}

	UIVARIANT::UIVARIANT(const _variant_t &varSrc)
	{
		varC4275.varThis = varSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(const _variant_t *pSrc)
	{
		varC4275.varThis = pSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(const UIVARIANT &varSrc)
	{
		varC4275.varThis = varSrc.varC4275.varThis;
		Init();
		IUI_VARIANT_Copy(varSrc);
	}

	UIVARIANT::UIVARIANT(_variant_t &varSrc, bool fCopy)
	{
		varC4275.varThis = _variant_t(varSrc, fCopy);
		Init();
	}

	UIVARIANT::UIVARIANT(short sSrc, VARTYPE vtSrc)
	{
		varC4275.varThis = _variant_t(sSrc, vtSrc);
		Init();
	}

	UIVARIANT::UIVARIANT(long lSrc, VARTYPE vtSrc)
	{
		varC4275.varThis = _variant_t(lSrc, vtSrc);
		Init();
	}

	UIVARIANT::UIVARIANT(float fltSrc)
	{
		varC4275.varThis = fltSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(double dblSrc, VARTYPE vtSrc)
	{
		varC4275.varThis = _variant_t(dblSrc, vtSrc);
		Init();
	}

	UIVARIANT::UIVARIANT(const CY &cySrc)
	{
		varC4275.varThis = cySrc;
		Init();
	}

	UIVARIANT::UIVARIANT(const _bstr_t &bstrSrc)
	{
		varC4275.varThis = bstrSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(const wchar_t *pSrc)
	{
		varC4275.varThis = pSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(const char *pSrc)
	{
		varC4275.varThis = pSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(IDispatch *pSrc, bool fAddRef)
	{
		varC4275.varThis = _variant_t(pSrc, fAddRef);
		Init();
	}

	UIVARIANT::UIVARIANT(bool boolSrc)
	{
		varC4275.varThis = boolSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(IUnknown *pSrc, bool fAddRef)
	{
		varC4275.varThis = _variant_t(pSrc, fAddRef);
		Init();
	}

	UIVARIANT::UIVARIANT(const DECIMAL &decSrc)
	{
		varC4275.varThis = decSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(BYTE bSrc)
	{
		varC4275.varThis =  bSrc;
		Init();
	}

#if(_MSC_VER > 1200) // VC6.0
	UIVARIANT::UIVARIANT(char cSrc)
	{
		varC4275.varThis = cSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(unsigned short usSrc)
	{
		varC4275.varThis = usSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(unsigned long ulSrc)
	{
		varC4275.varThis = ulSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(int iSrc)
	{
		varC4275.varThis = iSrc;
		Init();
	}

	UIVARIANT::UIVARIANT(unsigned int uiSrc)
	{
		varC4275.varThis = uiSrc;
		Init();
	}

#if (_WIN32_WINNT >= 0x0501)
	UIVARIANT::UIVARIANT(__int64 i8Src)
	{
		varC4275.varThis = i8Src;
		Init();
	}

	UIVARIANT::UIVARIANT(unsigned __int64 ui8Src)
	{
		varC4275.varThis = ui8Src;
		Init();
	}
#endif // (_WIN32_WINNT >= 0x0501)
#endif // (_MSC_VER > 1200) // VC6.0

	UIVARIANT::UIVARIANT(BYTE *pbVal)
	{
		Init();

		V_VT(&(varC4275.varThis)) = (VT_BYREF | VT_UI1);
		V_UI1REF(&(varC4275.varThis)) = pbVal;
	}

	UIVARIANT::UIVARIANT(RECT rcSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
		varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		memcpy(pData, &rcSrc, sizeof(RECT));
		SafeArrayUnaccessData(varC4275.varThis.parray);
	}

	UIVARIANT::UIVARIANT(POINT ptSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
		varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 2);
		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		memcpy(pData, &ptSrc, sizeof(POINT));
		SafeArrayUnaccessData(varC4275.varThis.parray);
	}

	UIVARIANT::UIVARIANT(LOGFONT lfSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_UI1;
		varC4275.varThis.parray = SafeArrayCreateVector(VT_UI1, 0, sizeof(LOGFONT));
		BYTE *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		memcpy(pData, &lfSrc, sizeof(LOGFONT));
		SafeArrayUnaccessData(varC4275.varThis.parray);
	}

	UIVARIANT::UIVARIANT(CONTROL_TYPE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_CONTROL_TYPE;
	}

	UIVARIANT::UIVARIANT(IMAGE_RESIZE_MODE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_IMAGE_RESIZE_MODE;
	}

	UIVARIANT::UIVARIANT(CONTROL_LAYOUT_HOR eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_LAYOUT_HOR;
	}

	UIVARIANT::UIVARIANT(CONTROL_LAYOUT_VER eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_LAYOUT_VER;
	}

	UIVARIANT::UIVARIANT(DOCK eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_DOCK;
	}

	UIVARIANT::UIVARIANT(ROOT_LAYOUT_TYPE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_ROOT_LAYOUT_TYPE;
	}

	UIVARIANT::UIVARIANT(TEXT_ALIGN_HOR eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_TEXT_ALIGN_HOR;
	}

	UIVARIANT::UIVARIANT(TEXT_ALIGN_VER eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_TEXT_ALIGN_VER;
	}

	UIVARIANT::UIVARIANT(FOREGROUND_ALIGN_HOR eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_FOREGROUND_ALIGN_HOR;
	}

	UIVARIANT::UIVARIANT(FOREGROUND_ALIGN_VER eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_FOREGROUND_ALIGN_VER;
	}

	UIVARIANT::UIVARIANT(BTN_TYPE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_BUTTON_TYPE;
	}

	UIVARIANT::UIVARIANT(CHK_TYPE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_CHECKBOX_TYPE;
	}

	UIVARIANT::UIVARIANT(WINDOW_TYPE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_WINDOW_TYPE;
	}

	UIVARIANT::UIVARIANT(CMB_STYLE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_COMBOBOX_STYLE;
	}

	UIVARIANT::UIVARIANT(SPN_ALIGN eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_SPN_ALIGN;
	}

	UIVARIANT::UIVARIANT(SLD_EXT_STYLE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_SLD_EXT_STYLE;
	}

	UIVARIANT::UIVARIANT(LST_STYLE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_LST_STYLE;
	}

	UIVARIANT::UIVARIANT(LST_ALIGN eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_LST_ALIGN;
	}

	UIVARIANT::UIVARIANT(LST_SORT eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_LST_SORT;
	}

	UIVARIANT::UIVARIANT(DATETIME_FORMAT eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_DATETIME_FORMAT;
	}

	UIVARIANT::UIVARIANT(BACKGROUND_TYPE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_BACKGROUND_TYPE;
	}

	UIVARIANT::UIVARIANT(PROGRESS_THUMB_MODE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_PROGRESS_THUMB_MODE;
	}

	UIVARIANT::UIVARIANT(PROGRESS_THUMB_POSITION eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_PROGRESS_THUMB_POSITION;
	}

	UIVARIANT::UIVARIANT(ALPHA_FORMAT eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_ALPHA_FORMAT;
	}

	UIVARIANT::UIVARIANT(MENU_BORDER_STYLE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_MENU_BORDER_STYLE;
	}

	UIVARIANT::UIVARIANT(SPLITTER_ORIENTATION eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_SPLITTER_ORIENTATION;
	}

	UIVARIANT::UIVARIANT(LINE_PEN_STYLE eSrc)
	{
		Init();

		V_VT(&(varC4275.varThis)) = VT_INT;
		V_INT(&(varC4275.varThis)) = eSrc;
		vtEx = VT_EX_PEN_STYLE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Extractors

#if(_MSC_VER <= 1200) // VC6.0

	UIVARIANT::operator int() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_I4)
		{
			return V_I4(&(varC4275.varThis));
		}

		_variant_t varDest;

		varDest.ChangeType(VT_I4, &(varC4275.varThis));

		return V_I4(&varDest);
	}

#endif

	UIVARIANT::operator LONG() const
	{
		return varC4275.varThis.operator LONG();
	}

	UIVARIANT::operator bool() const
	{
		return varC4275.varThis.operator bool();
	}

	UIVARIANT::operator BYTE *() const
	{
		if (V_VT(&(varC4275.varThis)) != (VT_BYREF | VT_UI1))
		{
			_ASSERT(FALSE);
			return NULL;
		}

		return V_UI1REF(&(varC4275.varThis));
	}

	UIVARIANT::operator _variant_t() const
	{
		return varC4275.varThis;
	}

	UIVARIANT::operator LPCWSTR() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_BSTR)
		{
			return V_BSTR(&(varC4275.varThis));
		}

		_variant_t varDest;
		varDest.ChangeType(VT_BSTR, &(varC4275.varThis));

		return V_BSTR(&varDest);
	}

	UIVARIANT::operator LPCSTR() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_BSTR)
		{
			if (strA != NULL)
			{
				delete [] strA;
			}

			strA = _com_util::ConvertBSTRToString(V_BSTR(&(varC4275.varThis)));
			return strA;
		}

		_variant_t varDest;
		varDest.ChangeType(VT_BSTR, &(varC4275.varThis));

		if (strA != NULL)
		{
			delete [] strA;
		}

		strA = _com_util::ConvertBSTRToString(V_BSTR(&(varC4275.varThis)));
		return strA;
	}

	UIVARIANT::operator RECT() const
	{
		if (V_VT(&(varC4275.varThis)) == (VT_ARRAY | VT_I4) &&
			(vtEx == VT_EX_RECT || vtEx == VT_EX_RECT_WH))
		{
			RECT rect;
			LONG *pData = NULL;
			SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
			memcpy(&rect, pData, sizeof(RECT));
			SafeArrayUnaccessData(varC4275.varThis.parray);

			return rect;
		}

		RECT rcRet;
		rcRet.left = rcRet.right = rcRet.top = rcRet.bottom = -1;

		return rcRet;
	}

	UIVARIANT::operator POINT() const
	{
		if (V_VT(&(varC4275.varThis)) == (VT_ARRAY | VT_I4) && vtEx == VT_EX_POINT)
		{
			POINT pt;
			LONG *pData = NULL;
			SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
			memcpy(&pt, pData, sizeof(POINT));
			SafeArrayUnaccessData(varC4275.varThis.parray);

			return pt;
		}

		POINT ptRet;
		ptRet.x = ptRet.y = -1;

		return ptRet;
	}

	UIVARIANT::operator COLORREF() const
	{
		if (vtEx != VT_EX_COLOR || V_VT(&(varC4275.varThis)) != VT_I4)
		{
			_ASSERT(FALSE);
			return -1;
		}

		return V_I4(&(varC4275.varThis));
	}

	UIVARIANT::operator LOGFONT() const
	{
		LOGFONT lf;
		ZeroMemory(&lf, sizeof(LOGFONT));

		if (V_VT(&(varC4275.varThis)) == (VT_ARRAY | VT_UI1) && vtEx == VT_EX_FONT)
		{
			BYTE *pData = NULL;
			SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
			memcpy(&lf, pData, sizeof(LOGFONT));
			SafeArrayUnaccessData(varC4275.varThis.parray);
		}

		return lf;
	}

	UIVARIANT::operator CONTROL_TYPE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_CONTROL_TYPE)
		{
			return (CONTROL_TYPE)V_INT(&(varC4275.varThis));
		}

		return CT_ARROW;
	}

	UIVARIANT::operator IMAGE_RESIZE_MODE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_IMAGE_RESIZE_MODE)
		{
			return (IMAGE_RESIZE_MODE)V_INT(&(varC4275.varThis));
		}

		return IRM_UNKNOWN;
	}

	UIVARIANT::operator CONTROL_LAYOUT_HOR() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_LAYOUT_HOR)
		{
			return (CONTROL_LAYOUT_HOR)V_INT(&(varC4275.varThis));
		}

		return CLH_UNKNOWN;
	}

	UIVARIANT::operator CONTROL_LAYOUT_VER() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_LAYOUT_VER)
		{
			return (CONTROL_LAYOUT_VER)V_INT(&(varC4275.varThis));
		}

		return CLV_UNKNOWN;
	}

	UIVARIANT::operator DOCK() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_DOCK)
		{
			return (DOCK)V_INT(&(varC4275.varThis));
		}

		return DOCK_LEFT;
	}

	UIVARIANT::operator ROOT_LAYOUT_TYPE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_ROOT_LAYOUT_TYPE)
		{
			return (ROOT_LAYOUT_TYPE)V_INT(&(varC4275.varThis));
		}

		return ROOT_LAYOUT_TYPE_UNKNOWN;
	}

	UIVARIANT::operator TEXT_ALIGN_HOR() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_TEXT_ALIGN_HOR)
		{
			return (TEXT_ALIGN_HOR)V_INT(&(varC4275.varThis));
		}

		return TAH_UNKNOWN;
	}

	UIVARIANT::operator TEXT_ALIGN_VER() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_TEXT_ALIGN_VER)
		{
			return (TEXT_ALIGN_VER)V_INT(&(varC4275.varThis));
		}

		return TAV_UNKNOWN;
	}

	UIVARIANT::operator FOREGROUND_ALIGN_HOR() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_FOREGROUND_ALIGN_HOR)
		{
			return (FOREGROUND_ALIGN_HOR)V_INT(&(varC4275.varThis));
		}

		return FAH_UNKNOWN;
	}

	UIVARIANT::operator FOREGROUND_ALIGN_VER() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_FOREGROUND_ALIGN_VER)
		{
			return (FOREGROUND_ALIGN_VER)V_INT(&(varC4275.varThis));
		}

		return FAV_UNKNOWN;
	}

	UIVARIANT::operator BTN_TYPE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_BUTTON_TYPE)
		{
			return (BTN_TYPE)V_INT(&(varC4275.varThis));
		}

		return BT_UNKNOWN;
	}

	UIVARIANT::operator CHK_TYPE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_CHECKBOX_TYPE)
		{
			return (CHK_TYPE)V_INT(&(varC4275.varThis));
		}

		return CT_UNKNOWN;
	}

	UIVARIANT::operator WINDOW_TYPE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_WINDOW_TYPE)
		{
			return (WINDOW_TYPE)V_INT(&(varC4275.varThis));
		}

		return WT_UNKNOWN;
	}

	UIVARIANT::operator CMB_STYLE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_COMBOBOX_STYLE)
		{
			return (CMB_STYLE)V_INT(&(varC4275.varThis));
		}

		return CS_UNKNOWN;
	}

	UIVARIANT::operator SPN_ALIGN() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_SPN_ALIGN)
		{
			return (SPN_ALIGN)V_INT(&(varC4275.varThis));
		}

		return SPNA_UNKNOWN;
	}

	UIVARIANT::operator SLD_EXT_STYLE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_SLD_EXT_STYLE)
		{
			return (SLD_EXT_STYLE)V_INT(&(varC4275.varThis));
		}

		return SLDES_UNKNOWN;
	}

	UIVARIANT::operator LST_STYLE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_LST_STYLE)
		{
			return (LST_STYLE)V_INT(&(varC4275.varThis));
		}

		return LSTS_UNKNOWN;
	}

	UIVARIANT::operator LST_ALIGN() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_LST_ALIGN)
		{
			return (LST_ALIGN)V_INT(&(varC4275.varThis));
		}

		return LSTA_UNKNOWN;
	}

	UIVARIANT::operator LST_SORT() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_LST_SORT)
		{
			return (LST_SORT)V_INT(&(varC4275.varThis));
		}

		return LSTST_UNKNOWN;
	}

	UIVARIANT::operator DATETIME_FORMAT() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_DATETIME_FORMAT)
		{
			return (DATETIME_FORMAT)V_INT(&(varC4275.varThis));
		}

		return DTF_UNKNOWN;
	}

	UIVARIANT::operator BACKGROUND_TYPE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_BACKGROUND_TYPE)
		{
			return (BACKGROUND_TYPE)V_INT(&(varC4275.varThis));
		}

		return BKT_UNKNOWN;
	}

	UIVARIANT::operator PROGRESS_THUMB_MODE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_PROGRESS_THUMB_MODE)
		{
			return (PROGRESS_THUMB_MODE)V_INT(&(varC4275.varThis));
		}

		return PTM_UNKNOWN;
	}

	UIVARIANT::operator PROGRESS_THUMB_POSITION() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_PROGRESS_THUMB_POSITION)
		{
			return (PROGRESS_THUMB_POSITION)V_INT(&(varC4275.varThis));
		}

		return PTP_UNKNOWN;
	}

	UIVARIANT::operator ALPHA_FORMAT() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_ALPHA_FORMAT)
		{
			return (ALPHA_FORMAT)V_INT(&(varC4275.varThis));
		}

		return AF_UNKNOWN;
	}

	UIVARIANT::operator MENU_BORDER_STYLE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_MENU_BORDER_STYLE)
		{
			return (MENU_BORDER_STYLE)V_INT(&(varC4275.varThis));
		}

		return MBS_UNKNOWN;
	}

	UIVARIANT::operator SPLITTER_ORIENTATION() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_SPLITTER_ORIENTATION)
		{
			return (SPLITTER_ORIENTATION)V_INT(&(varC4275.varThis));
		}

		return SO_UNKNOWN;
	}

	UIVARIANT::operator LINE_PEN_STYLE() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT && vtEx == VT_EX_PEN_STYLE)
		{
			return (LINE_PEN_STYLE)V_INT(&(varC4275.varThis));
		}

		return LPS_UNKNOWN;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Assignment operations

	UIVARIANT &UIVARIANT::operator=(const _variant_t &varSrc)
	{
		varC4275.varThis.operator = (varSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const _variant_t *pSrc)
	{
		varC4275.varThis.operator = (pSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const UIVARIANT &varSrc)
	{
		varC4275.varThis = varSrc.varC4275.varThis;
		IUI_VARIANT_Copy(varSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(short sSrc)
	{
		varC4275.varThis.operator = (sSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(long lSrc)
	{
		varC4275.varThis.operator = (lSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(float fltSrc)
	{
		varC4275.varThis.operator = (fltSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(double dblSrc)
	{
		varC4275.varThis.operator = (dblSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const CY &cySrc)
	{
		varC4275.varThis.operator = (cySrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const _bstr_t &bstrSrc)
	{
		varC4275.varThis.operator = (bstrSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const wchar_t *pSrc)
	{
		varC4275.varThis.operator = (pSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const char *pSrc)
	{
		varC4275.varThis.operator = (pSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(IDispatch *pSrc)
	{
		varC4275.varThis.operator = (pSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(bool boolSrc)
	{
		varC4275.varThis.operator = (boolSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(IUnknown *pSrc)
	{
		varC4275.varThis.operator = (pSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const DECIMAL &decSrc)
	{
		varC4275.varThis.operator = (decSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(BYTE bSrc)
	{
		varC4275.varThis.operator = (bSrc);

		return *this;
	}

#if(_MSC_VER > 1200) // VC6.0
	UIVARIANT &UIVARIANT::operator=(char cSrc)
	{
		varC4275.varThis.operator = (cSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(unsigned short usSrc)
	{
		varC4275.varThis.operator = (usSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(unsigned long ulSrc)
	{
		varC4275.varThis.operator = (ulSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(int iSrc)
	{
		varC4275.varThis.operator = (iSrc);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(unsigned int uiSrc)
	{
		varC4275.varThis.operator = (uiSrc);

		return *this;
	}

#if (_WIN32_WINNT >= 0x0501)
	UIVARIANT &UIVARIANT::operator=(__int64 i8Src)
	{
		varC4275.varThis.operator = (i8Src);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(unsigned __int64 ui8Src)
	{
		varC4275.varThis.operator = (ui8Src);

		return *this;
	}
#endif // (_WIN32_WINNT >= 0x0501)
#endif // (_MSC_VER > 1200) // VC6.0

	UIVARIANT &UIVARIANT::operator=(BYTE *pbVal)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_BYREF | VT_UI1))
		{
			varC4275.varThis.Clear();
			V_VT(&(varC4275.varThis)) = (VT_UI1 | VT_BYREF);
		}

		V_UI1REF(&(varC4275.varThis)) = pbVal;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const RECT &rcSrc)
	{
		// Release the old data and memory
		IUI_VARIANT_Clear();

		V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;

		if (vtEx != VT_EX_RECT && vtEx != VT_EX_RECT_WH)
		{
			vtEx = VT_EX_RECT;
		}

		varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		memcpy(pData, &rcSrc, sizeof(RECT));
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const POINT &ptSrc)
	{
		// Release the old data and memory
		IUI_VARIANT_Clear();

		V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
		vtEx = VT_EX_POINT;

		varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 2);
		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		memcpy(pData, &ptSrc, sizeof(POINT));
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(const LOGFONT &lfSrc)
	{
		// Release the old data and memory
		IUI_VARIANT_Clear();

		V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_UI1;
		vtEx = VT_EX_FONT;

		varC4275.varThis.parray = SafeArrayCreateVector(VT_UI1, 0, sizeof(LOGFONT));
		BYTE *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		memcpy(pData, &lfSrc, sizeof(LOGFONT));
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(CONTROL_TYPE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_CONTROL_TYPE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(IMAGE_RESIZE_MODE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_IMAGE_RESIZE_MODE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(CONTROL_LAYOUT_HOR eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_LAYOUT_HOR;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(CONTROL_LAYOUT_VER eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_LAYOUT_VER;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(DOCK eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_DOCK;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(ROOT_LAYOUT_TYPE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_ROOT_LAYOUT_TYPE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(TEXT_ALIGN_HOR eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_TEXT_ALIGN_HOR;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(TEXT_ALIGN_VER eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_TEXT_ALIGN_VER;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(FOREGROUND_ALIGN_HOR eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_FOREGROUND_ALIGN_HOR;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(FOREGROUND_ALIGN_VER eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_FOREGROUND_ALIGN_VER;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(BTN_TYPE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_BUTTON_TYPE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(CHK_TYPE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_CHECKBOX_TYPE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(WINDOW_TYPE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_WINDOW_TYPE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(CMB_STYLE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_COMBOBOX_STYLE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(SPN_ALIGN eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_SPN_ALIGN;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(SLD_EXT_STYLE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_SLD_EXT_STYLE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(LST_STYLE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_LST_STYLE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(LST_ALIGN eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_LST_ALIGN;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(LST_SORT eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_LST_SORT;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(DATETIME_FORMAT eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_DATETIME_FORMAT;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(BACKGROUND_TYPE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_BACKGROUND_TYPE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(PROGRESS_THUMB_MODE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_PROGRESS_THUMB_MODE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(PROGRESS_THUMB_POSITION eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_PROGRESS_THUMB_POSITION;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(ALPHA_FORMAT eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_ALPHA_FORMAT;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(MENU_BORDER_STYLE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_MENU_BORDER_STYLE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(SPLITTER_ORIENTATION eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_SPLITTER_ORIENTATION;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	UIVARIANT &UIVARIANT::operator=(LINE_PEN_STYLE eSrc)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			IUI_VARIANT_Clear();
		}

		V_VT(&(varC4275.varThis)) = VT_INT;
		vtEx = VT_EX_PEN_STYLE;
		V_INT(&(varC4275.varThis)) = eSrc;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Comparison operations

	bool UIVARIANT::operator==(const _variant_t &varSrc) const
	{
		if (V_VT(&(varC4275.varThis)) == (VT_ARRAY | VT_UI1) && vtEx == VT_EX_FONT)
		{
			BYTE *pData1 = NULL;
			SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData1);

			BYTE *pData2 = NULL;
			SafeArrayAccessData(varSrc.parray, (void **)&pData2);

			if (memcmp(pData1, pData2, sizeof(LOGFONT)) != 0)
			{
				SafeArrayUnaccessData(varC4275.varThis.parray);
				SafeArrayUnaccessData(varSrc.parray);
				return false;
			}

			SafeArrayUnaccessData(varC4275.varThis.parray);
			SafeArrayUnaccessData(varSrc.parray);

			return true;
		}

		if (V_VT(&(varC4275.varThis)) == (VT_ARRAY | VT_I4))
		{
			if (vtEx == VT_EX_RECT || vtEx == VT_EX_RECT_WH)
			{
				LONG *pData1 = NULL;
				SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData1);

				LONG *pData2 = NULL;
				SafeArrayAccessData(varSrc.parray, (void **)&pData2);

				if (memcmp(pData1, pData2, sizeof(RECT)) != 0)
				{
					SafeArrayUnaccessData(varC4275.varThis.parray);
					SafeArrayUnaccessData(varSrc.parray);
					return false;
				}

				SafeArrayUnaccessData(varC4275.varThis.parray);
				SafeArrayUnaccessData(varSrc.parray);

				return true;
			}
			else if (vtEx == VT_EX_POINT)
			{
				LONG *pData1 = NULL;
				SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData1);

				LONG *pData2 = NULL;
				SafeArrayAccessData(varSrc.parray, (void **)&pData2);

				if (memcmp(pData1, pData2, sizeof(POINT)) != 0)
				{
					SafeArrayUnaccessData(varC4275.varThis.parray);
					SafeArrayUnaccessData(varSrc.parray);
					return false;
				}

				SafeArrayUnaccessData(varC4275.varThis.parray);
				SafeArrayUnaccessData(varSrc.parray);

				return true;
			}
		}

		return (varC4275.varThis) == &varSrc;
	}

#pragma warning(push)
#pragma warning(disable: 4702) // unreachable code

	bool UIVARIANT::operator==(const _variant_t *pSrc) const
	{
		return varC4275.varThis.operator == (pSrc);
	}

#pragma warning(pop)

	bool UIVARIANT::operator==(const UIVARIANT &varSrc) const
	{
		return operator==(varC4275.varThis);
	}

	bool UIVARIANT::operator!=(const _variant_t &varSrc) const
	{
		if (V_VT(&(varC4275.varThis)) == (VT_ARRAY | VT_UI1) && vtEx == VT_EX_FONT)
		{
			BYTE *pData1 = NULL;
			SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData1);

			BYTE *pData2 = NULL;
			SafeArrayAccessData(varSrc.parray, (void **)&pData2);

			if (memcmp(pData1, pData2, sizeof(LOGFONT)) != 0)
			{
				SafeArrayUnaccessData(varC4275.varThis.parray);
				SafeArrayUnaccessData(varSrc.parray);
				return true;
			}

			SafeArrayUnaccessData(varC4275.varThis.parray);
			SafeArrayUnaccessData(varSrc.parray);

			return false;
		}

		return !((varC4275.varThis) == &varSrc);
	}

	bool UIVARIANT::operator!=(const _variant_t *pSrc) const
	{
		return !((varC4275.varThis) == pSrc);
	}

	bool UIVARIANT::operator==(LPCTSTR lpszSrc) const
	{
		if (varC4275.varThis.vt != VT_BSTR)
		{
			_ASSERT(FALSE);
			return false;
		}

		CIUIString str = (TCHAR *)_bstr_t(varC4275.varThis.bstrVal);
		return (str == lpszSrc);
	}

	bool UIVARIANT::operator!=(LPCTSTR lpszSrc) const
	{
		if (varC4275.varThis.vt != VT_BSTR)
		{
			_ASSERT(FALSE);
			return true;
		}

		CIUIString str = (TCHAR *)_bstr_t(varC4275.varThis.bstrVal);
		return (str != lpszSrc);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Low-level operations

	void UIVARIANT::Init()
	{
		m_pBindCtrlProp = NULL;
		bSet = false;
		vtEx = VT_EMPTY;
		strA = NULL;
		bSaveToSkinFile = true;
		bNeedToConvert = false;
		bReadOnly = false;
		varC4251.strIUIAttributeName = "Value";
		bShowInPropertiesWnd = true;
		bGroup = false;
		nMinValue = 0;
		nMaxValue = 0;

		bDisabledWhileBindIs = false;
		bDisabledWhileBindIsnot = false;
		pvarBind = NULL;
		pvarCompare = NULL;
		lBeBindCount = 0;
	}

	void UIVARIANT::IUI_VARIANT_Copy(const UIVARIANT &varSrc)
	{
		if (&varSrc == this)
		{
			return;
		}

		bSet = varSrc.bSet;
		vtEx = varSrc.vtEx;
		bSaveToSkinFile = varSrc.bSaveToSkinFile;
		bReadOnly = varSrc.bReadOnly;
		varC4251.strVarName = varSrc.varC4251.strVarName;
		varC4251.varDefaultValue = varSrc.varC4251.varDefaultValue;
		varC4251.strIUIElementName = varSrc.varC4251.strIUIElementName;
		varC4251.strIUIAttributeName = varSrc.varC4251.strIUIAttributeName;
		strAttributeValue = varSrc.strAttributeValue;
		bShowInPropertiesWnd = varSrc.bShowInPropertiesWnd;
		bGroup = varSrc.bGroup;
		strGroupName = varSrc.strGroupName;
		strPropWndName = varSrc.strPropWndName;
		strDescription = varSrc.strDescription;
		varC4251.lstStringOptions = varSrc.varC4251.lstStringOptions;
		nMinValue = varSrc.nMinValue;
		nMaxValue = varSrc.nMaxValue;
	}

	void UIVARIANT::IUI_VARIANT_Clear()
	{
		varC4275.varThis.Clear();    // Clear 调用的VariantClear会自动释放数组的。所以，并不需要调用SafeArrayDestroy来释放。

		if (strA != NULL)
		{
			delete [] strA;
			strA = NULL;
		}

		if (pvarCompare != NULL)
		{
			delete pvarCompare;
			pvarCompare = NULL;
		}
	}

	_variant_t &UIVARIANT::GetVARIANT()
	{
		return *(_variant_t *) this;
	}

	_variant_t *UIVARIANT::GetAddress()
	{
		varC4275.varThis.Clear();
		return (_variant_t *) this;
	}

	void UIVARIANT::SetString(const char *pSrc)
	{
		operator=(pSrc);
	}

	int UIVARIANT::SetPointX(LONG x)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 2);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[0] = x;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetPointY(LONG y)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 2);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[1] = y;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectLeftNoSize(LONG lLeft)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		LONG lWidth = pData[2] - pData[0];
		pData[0] = lLeft;
		pData[2] = pData[0] + lWidth;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectTopNoSize(LONG lTop)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		LONG lHeight = pData[3] - pData[1];
		pData[1] = lTop;
		pData[3] = pData[1] + lHeight;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectLeft(LONG lLeft)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[0] = lLeft;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectTop(LONG lTop)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[1] = lTop;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectRight(LONG lRight)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[2] = lRight;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectBottom(LONG lBottom)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[3] = lBottom;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectWidth(LONG lWidth)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[2] = pData[0] + lWidth;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	int UIVARIANT::SetRectHeight(LONG lHeight)
	{
		if (V_VT(&(varC4275.varThis)) != (VT_ARRAY | VT_I4))
		{
			// Clear the VARIANT and create a VT_RECT
			//
			IUI_VARIANT_Clear();
			V_VT(&(varC4275.varThis)) = VT_ARRAY | VT_I4;
			varC4275.varThis.parray = SafeArrayCreateVector(VT_I4, 0, 4);
		}

		LONG *pData = NULL;
		SafeArrayAccessData(varC4275.varThis.parray, (void **)&pData);
		pData[3] = pData[1] + lHeight;
		SafeArrayUnaccessData(varC4275.varThis.parray);

		return 0;
	}

	_bstr_t UIVARIANT::GetEnumString() const
	{
		if (V_VT(&(varC4275.varThis)) == VT_INT)
		{
			_bstr_t str;

			if (vtEx == VT_EX_IMAGE_RESIZE_MODE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case IRM_9GRID:
					str = _T("9Grid");
					break;

				case IRM_STRETCH:
					str = _T("Stretch");
					break;

				case IRM_TILE:
					str = _T("Tile");
					break;

				case IRM_CENTER:
					str = _T("Center");
					break;

				case IRM_KEEPRATE:
					str = _T("KeepRate");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_LAYOUT_HOR)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case CLH_LEFT:
					str = _T("Left");
					break;

				case CLH_RIGHT:
					str = _T("Right");
					break;

				case CLH_CENTER:
					str = _T("Center");
					break;

				case CLH_FILL:
					str = _T("Fill");
					break;

				case CLH_FREE:
					str = _T("Free");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_LAYOUT_VER)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case CLV_TOP:
					str = _T("Top");
					break;

				case CLV_BOTTOM:
					str = _T("Bottom");
					break;

				case CLV_CENTER:
					str = _T("Center");
					break;

				case CLV_FILL:
					str = _T("Fill");
					break;

				case CLV_FREE:
					str = _T("Free");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_DOCK)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case DOCK_LEFT:
					str = _T("Left");
					break;

				case DOCK_TOP:
					str = _T("Top");
					break;

				case DOCK_RIGHT:
					str = _T("Right");
					break;

				case DOCK_BOTTOM:
					str = _T("Bottom");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_ROOT_LAYOUT_TYPE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case ROOT_LAYOUT_TYPE_CANVAS:
					str = _T("Canvas");
					break;

				case ROOT_LAYOUT_TYPE_VERT:
					str = _T("Vertical");
					break;

				case ROOT_LAYOUT_TYPE_HORZ:
					str = _T("Horizontal");
					break;

				case ROOT_LAYOUT_TYPE_TAB:
					str = _T("Tab");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_TEXT_ALIGN_HOR)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case TAH_LEFT:
					str = _T("Left");
					break;

				case TAH_RIGHT:
					str = _T("Right");
					break;

				case TAH_CENTER:
					str = _T("Center");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_TEXT_ALIGN_VER)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case TAV_TOP:
					str = _T("Top");
					break;

				case TAV_BOTTOM:
					str = _T("Bottom");
					break;

				case TAV_CENTER:
					str = _T("Center");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_FOREGROUND_ALIGN_HOR)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case FAH_LEFT:
					str = _T("Left");
					break;

				case FAH_RIGHT:
					str = _T("Right");
					break;

				case FAH_CENTER:
					str = _T("Center");
					break;

				case FAH_STRETCH:
					str = _T("Stretch");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_FOREGROUND_ALIGN_VER)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case FAV_TOP:
					str = _T("Top");
					break;

				case FAV_BOTTOM:
					str = _T("Bottom");
					break;

				case FAV_CENTER:
					str = _T("Center");
					break;

				case FAV_STRETCH:
					str = _T("Stretch");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_BUTTON_TYPE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case BT_NORMAL:
					str = _T("Normal");
					break;

				case BT_MINIMIZEBOX:
					str = _T("MinimizeBox");
					break;

				case BT_CLOSEBOX:
					str = _T("CloseBox");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_CHECKBOX_TYPE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case CT_NORMAL:
					str = _T("Normal");
					break;

				case CT_MAXIMIZEBOX:
					str = _T("MaximizeBox");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_WINDOW_TYPE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case WT_NORMAL:
					str = _T("Normal");
					break;

				case WT_TOOLTIPS:
					str = _T("Tooltips");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_COMBOBOX_STYLE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case CS_SIMPLE:
					str = _T("Simple");
					break;

				case CS_DROPDOWN:
					str = _T("DropDown");
					break;

				case CS_DROPDOWNLIST:
					str = _T("DropDown List");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_SPN_ALIGN)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case SPNA_UNATTACHED:
					str = _T("Unattached");
					break;

				case SPNA_LEFT:
					str = _T("Left");
					break;

				case SPNA_RIGHT:
					str = _T("Right");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_SLD_EXT_STYLE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case SLDES_NORMAL:
					str = _T("Normal");
					break;

				case SLDES_PROGRESS:
					str = _T("Progress");
					break;

				case SLDES_SPLITPROGRESS:
					str = _T("Split Progress");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_LST_STYLE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case LSTS_ICON:
					str = _T("Icon");
					break;

				case LSTS_SMALLICON:
					str = _T("Small Icon");
					break;

				case LSTS_LIST:
					str = _T("List");
					break;

				case LSTS_REPORT:
					str = _T("Report");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_LST_ALIGN)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case LSTA_TOP:
					str = _T("Top");
					break;

				case LSTA_LEFT:
					str = _T("Left");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_LST_SORT)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case LSTST_UNSET:
					str = _T("Unset");
					break;

				case LSTST_ASCENDING:
					str = _T("Ascending");
					break;

				case LSTST_DESCENDING:
					str = _T("Descending");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_DATETIME_FORMAT)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case DTF_SHORT_DATE:
					str = _T("Short Date");
					break;

				case DTF_LONG_DATE:
					str = _T("Long Date");
					break;

				case DTF_TIME:
					str = _T("Time");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_BACKGROUND_TYPE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case BKT_COLOR:
					str = _T("Color");
					break;

				case BKT_IMAGE:
					str = _T("Image");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_PROGRESS_THUMB_MODE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case PTM_STRETCH:
					str = _T("Stretch");
					break;

				case PTM_TILE:
					str = _T("Tile");
					break;

				case PTM_CLIP:
					str = _T("Clip");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_PROGRESS_THUMB_POSITION)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case PTP_DEFAULT:
					str = _T("Default");
					break;

				case PTP_SPECIFIED:
					str = _T("Specified");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_ALPHA_FORMAT)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case AF_NONE:
					str = _T("None");
					break;

				case AF_AC_SRC_ALPHA:
					str = _T("AC_SRC_ALPHA");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_MENU_BORDER_STYLE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case MBS_NORMAL:
					str = _T("Normal");
					break;

				case MBS_FLAT:
					str = _T("Flat");
					break;

				case MBS_NONE:
					str = _T("None");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_SPLITTER_ORIENTATION)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case SO_WE:
					str = _T("WestEast");
					break;

				case SO_NS:
					str = _T("NorthSouth");
					break;

				default:
					break;
				}

				return str;
			}
			else if (vtEx == VT_EX_PEN_STYLE)
			{
				switch (V_I4(&(varC4275.varThis)))
				{
				case LPS_SOLID:
					str = _T("PS_SOLID");
					break;

				case LPS_DASH:
					str = _T("PS_DASH");
					break;

				case LPS_DOT:
					str = _T("PS_DOT");
					break;

				case LPS_DASHDOT:
					str = _T("PS_DASHDOT");
					break;

				case LPS_DASHDOTDOT:
					str = _T("PS_DASHDOTDOT");
					break;

				case LPS_NULL:
					str = _T("PS_NULL");
					break;

				case LPS_INSIDEFRAME:
					str = _T("PS_INSIDEFRAME");
					break;

				case LPS_USERSTYLE:
					str = _T("PS_USERSTYLE");
					break;

				case LPS_ALTERNATE:
					str = _T("PS_ALTERNATE");
					break;

				default:
					break;
				}

				return str;
			}
		}

		return varC4275.varThis.operator _bstr_t();
	}

	int UIVARIANT::InitByEnumString(LPCTSTR lpszValue)
	{
		if (V_VT(&(varC4275.varThis)) != VT_INT)
		{
			return -1;
		}

		CIUIString str = lpszValue;

		if (vtEx == VT_EX_IMAGE_RESIZE_MODE)
		{
			if (str.CompareNoCase(UIVARIANT(IRM_9GRID).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = IRM_9GRID;
			}
			else if (str.CompareNoCase(UIVARIANT(IRM_STRETCH).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = IRM_STRETCH;
			}
			else if (str.CompareNoCase(UIVARIANT(IRM_TILE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = IRM_TILE;
			}
			else if (str.CompareNoCase(UIVARIANT(IRM_CENTER).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = IRM_CENTER;
			}
			else if (str.CompareNoCase(UIVARIANT(IRM_KEEPRATE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = IRM_KEEPRATE;
			}
		}
		else if (vtEx == VT_EX_LAYOUT_HOR)
		{
			if (str.CompareNoCase(UIVARIANT(CLH_LEFT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLH_LEFT;
			}
			else if (str.CompareNoCase(UIVARIANT(CLH_RIGHT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLH_RIGHT;
			}
			else if (str.CompareNoCase(UIVARIANT(CLH_CENTER).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLH_CENTER;
			}
			else if (str.CompareNoCase(UIVARIANT(CLH_FILL).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLH_FILL;
			}
			else if (str.CompareNoCase(UIVARIANT(CLH_FREE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLH_FREE;
			}
		}
		else if (vtEx == VT_EX_LAYOUT_VER)
		{
			if (str.CompareNoCase(UIVARIANT(CLV_TOP).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLV_TOP;
			}
			else if (str.CompareNoCase(UIVARIANT(CLV_BOTTOM).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLV_BOTTOM;
			}
			else if (str.CompareNoCase(UIVARIANT(CLV_CENTER).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLV_CENTER;
			}
			else if (str.CompareNoCase(UIVARIANT(CLV_FILL).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLV_FILL;
			}
			else if (str.CompareNoCase(UIVARIANT(CLV_FREE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = CLV_FREE;
			}
		}
		else if (vtEx == VT_EX_DOCK)
		{
			if (str.CompareNoCase(UIVARIANT(DOCK_LEFT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = DOCK_LEFT;
			}
			else if (str.CompareNoCase(UIVARIANT(DOCK_TOP).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = DOCK_TOP;
			}
			else if (str.CompareNoCase(UIVARIANT(DOCK_RIGHT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = DOCK_RIGHT;
			}
			else if (str.CompareNoCase(UIVARIANT(DOCK_BOTTOM).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = DOCK_BOTTOM;
			}
		}
		else if (vtEx == VT_EX_ROOT_LAYOUT_TYPE)
		{
			if (str.CompareNoCase(UIVARIANT(ROOT_LAYOUT_TYPE_CANVAS).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = ROOT_LAYOUT_TYPE_CANVAS;
			}
			else if (str.CompareNoCase(UIVARIANT(ROOT_LAYOUT_TYPE_VERT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = ROOT_LAYOUT_TYPE_VERT;
			}
			else if (str.CompareNoCase(UIVARIANT(ROOT_LAYOUT_TYPE_HORZ).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = ROOT_LAYOUT_TYPE_HORZ;
			}
			else if (str.CompareNoCase(UIVARIANT(ROOT_LAYOUT_TYPE_TAB).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = ROOT_LAYOUT_TYPE_TAB;
			}
		}
		else if (vtEx == VT_EX_TEXT_ALIGN_HOR)
		{
			if (str.CompareNoCase(_T("Left")) == 0)
			{
				V_I4(&(varC4275.varThis)) = TAH_LEFT;
			}
			else if (str.CompareNoCase(_T("Right")) == 0)
			{
				V_I4(&(varC4275.varThis)) = TAH_RIGHT;
			}
			else if (str.CompareNoCase(_T("Center")) == 0)
			{
				V_I4(&(varC4275.varThis)) = TAH_CENTER;
			}
		}
		else if (vtEx == VT_EX_TEXT_ALIGN_VER)
		{
			if (str.CompareNoCase(_T("Top")) == 0)
			{
				V_I4(&(varC4275.varThis)) = TAV_TOP;
			}
			else if (str.CompareNoCase(_T("Bottom")) == 0)
			{
				V_I4(&(varC4275.varThis)) = TAV_BOTTOM;
			}
			else if (str.CompareNoCase(_T("Center")) == 0)
			{
				V_I4(&(varC4275.varThis)) = TAV_CENTER;
			}
		}
		else if (vtEx == VT_EX_FOREGROUND_ALIGN_HOR)
		{
			if (str.CompareNoCase(_T("Left")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAH_LEFT;
			}
			else if (str.CompareNoCase(_T("Right")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAH_RIGHT;
			}
			else if (str.CompareNoCase(_T("Center")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAH_CENTER;
			}
			else if (str.CompareNoCase(_T("Stretch")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAH_STRETCH;
			}
		}
		else if (vtEx == VT_EX_FOREGROUND_ALIGN_VER)
		{
			if (str.CompareNoCase(_T("Top")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAV_TOP;
			}
			else if (str.CompareNoCase(_T("Bottom")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAV_BOTTOM;
			}
			else if (str.CompareNoCase(_T("Center")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAV_CENTER;
			}
			else if (str.CompareNoCase(_T("Stretch")) == 0)
			{
				V_I4(&(varC4275.varThis)) = FAV_STRETCH;
			}
		}
		else if (vtEx == VT_EX_BUTTON_TYPE)
		{
			if (str.CompareNoCase(_T("Normal")) == 0)
			{
				V_I4(&(varC4275.varThis)) = BT_NORMAL;
			}
			else if (str.CompareNoCase(_T("MinimizeBox")) == 0)
			{
				V_I4(&(varC4275.varThis)) = BT_MINIMIZEBOX;
			}
			else if (str.CompareNoCase(_T("CloseBox")) == 0)
			{
				V_I4(&(varC4275.varThis)) = BT_CLOSEBOX;
			}
		}
		else if (vtEx == VT_EX_CHECKBOX_TYPE)
		{
			if (str.CompareNoCase(_T("Normal")) == 0)
			{
				V_I4(&(varC4275.varThis)) = CT_NORMAL;
			}
			else if (str.CompareNoCase(_T("MaximizeBox")) == 0)
			{
				V_I4(&(varC4275.varThis)) = CT_MAXIMIZEBOX;
			}
		}
		else if (vtEx == VT_EX_WINDOW_TYPE)
		{
			if (str.CompareNoCase(UIVARIANT(WT_NORMAL).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = WT_NORMAL;
			}
			else if (str.CompareNoCase(UIVARIANT(WT_TOOLTIPS).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = WT_TOOLTIPS;
			}
		}
		else if (vtEx == VT_EX_COMBOBOX_STYLE)
		{
			if (str.CompareNoCase(_T("Simple")) == 0)
			{
				V_I4(&(varC4275.varThis)) = CS_SIMPLE;
			}
			else if (str.CompareNoCase(_T("DropDown")) == 0)
			{
				V_I4(&(varC4275.varThis)) = CS_DROPDOWN;
			}
			else if (str.CompareNoCase(_T("DropDown List")) == 0)
			{
				V_I4(&(varC4275.varThis)) = CS_DROPDOWNLIST;
			}
		}
		else if (vtEx == VT_EX_SPN_ALIGN)
		{
			if (str.CompareNoCase(_T("Unattached")) == 0)
			{
				V_I4(&(varC4275.varThis)) = SPNA_UNATTACHED;
			}
			else if (str.CompareNoCase(_T("Left")) == 0)
			{
				V_I4(&(varC4275.varThis)) = SPNA_LEFT;
			}
			else if (str.CompareNoCase(_T("Right")) == 0)
			{
				V_I4(&(varC4275.varThis)) = SPNA_RIGHT;
			}
		}
		else if (vtEx == VT_EX_SLD_EXT_STYLE)
		{
			if (str.CompareNoCase(_T("Normal")) == 0)
			{
				V_I4(&(varC4275.varThis)) = SLDES_NORMAL;
			}
			else if (str.CompareNoCase(_T("Progress")) == 0)
			{
				V_I4(&(varC4275.varThis)) = SLDES_PROGRESS;
			}
			else if (str.CompareNoCase(_T("Split Progress")) == 0)
			{
				V_I4(&(varC4275.varThis)) = SLDES_SPLITPROGRESS;
			}
		}
		else if (vtEx == VT_EX_LST_STYLE)
		{
			if (str.CompareNoCase(_T("Icon")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTS_ICON;
			}
			else if (str.CompareNoCase(_T("Small Icon")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTS_SMALLICON;
			}
			else if (str.CompareNoCase(_T("List")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTS_LIST;
			}
			else if (str.CompareNoCase(_T("Report")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTS_REPORT;
			}
		}
		else if (vtEx == VT_EX_LST_ALIGN)
		{
			if (str.CompareNoCase(_T("Top")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTA_TOP;
			}
			else if (str.CompareNoCase(_T("Left")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTA_LEFT;
			}
		}
		else if (vtEx == VT_EX_LST_SORT)
		{
			if (str.CompareNoCase(_T("Unset")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTST_UNSET;
			}
			else if (str.CompareNoCase(_T("Ascending")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTST_ASCENDING;
			}
			else if (str.CompareNoCase(_T("Descending")) == 0)
			{
				V_I4(&(varC4275.varThis)) = LSTST_DESCENDING;
			}
		}
		else if (vtEx == VT_EX_DATETIME_FORMAT)
		{
			if (str.CompareNoCase(UIVARIANT(DTF_SHORT_DATE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = DTF_SHORT_DATE;
			}
			else if (str.CompareNoCase(UIVARIANT(DTF_LONG_DATE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = DTF_LONG_DATE;
			}
			else if (str.CompareNoCase(UIVARIANT(DTF_TIME).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = DTF_TIME;
			}
		}
		else if (vtEx == VT_EX_BACKGROUND_TYPE)
		{
			if (str.CompareNoCase(UIVARIANT(BKT_COLOR).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = BKT_COLOR;
			}
			else if (str.CompareNoCase(UIVARIANT(BKT_IMAGE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = BKT_IMAGE;
			}
		}
		else if (vtEx == VT_EX_PROGRESS_THUMB_MODE)
		{
			if (str.CompareNoCase(UIVARIANT(PTM_STRETCH).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = PTM_STRETCH;
			}
			else if (str.CompareNoCase(UIVARIANT(PTM_TILE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = PTM_TILE;
			}
			else if (str.CompareNoCase(UIVARIANT(PTM_CLIP).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = PTM_CLIP;
			}
		}
		else if (vtEx == VT_EX_PROGRESS_THUMB_POSITION)
		{
			if (str.CompareNoCase(UIVARIANT(PTP_DEFAULT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = PTP_DEFAULT;
			}
			else if (str.CompareNoCase(UIVARIANT(PTP_SPECIFIED).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = PTP_SPECIFIED;
			}
		}
		else if (vtEx == VT_EX_ALPHA_FORMAT)
		{
			if (str.CompareNoCase(UIVARIANT(AF_NONE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = AF_NONE;
			}
			else if (str.CompareNoCase(UIVARIANT(AF_AC_SRC_ALPHA).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = AF_AC_SRC_ALPHA;
			}
		}
		else if (vtEx == VT_EX_MENU_BORDER_STYLE)
		{
			if (str.CompareNoCase(UIVARIANT(MBS_NORMAL).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = MBS_NORMAL;
			}
			else if (str.CompareNoCase(UIVARIANT(MBS_FLAT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = MBS_FLAT;
			}
			else if (str.CompareNoCase(UIVARIANT(MBS_NONE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = MBS_NONE;
			}
		}
		else if (vtEx == VT_EX_SPLITTER_ORIENTATION)
		{
			if (str.CompareNoCase(UIVARIANT(SO_WE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = SO_WE;
			}
			else if (str.CompareNoCase(UIVARIANT(SO_NS).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = SO_NS;
			}
		}
		else if (vtEx == VT_EX_PEN_STYLE)
		{
			if (str.CompareNoCase(UIVARIANT(LPS_SOLID).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_SOLID;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_DASH).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_DASH;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_DOT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_DOT;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_DASHDOT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_DASHDOT;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_DASHDOTDOT).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_DASHDOTDOT;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_NULL).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_NULL;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_INSIDEFRAME).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_INSIDEFRAME;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_USERSTYLE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_USERSTYLE;
			}
			else if (str.CompareNoCase(UIVARIANT(LPS_ALTERNATE).GetEnumString()) == 0)
			{
				V_I4(&(varC4275.varThis)) = LPS_ALTERNATE;
			}
		}
		else
		{
			_ASSERT(FALSE);
		}

		return 0;
	}

	int UIVARIANT::SetBindVariant(const UIVARIANT *pBind)
	{
		_ASSERT(pBind != NULL);

		if (pvarBind == pBind)
		{
			return 0;
		}

		if (pvarBind != NULL)
		{
			pvarBind->lBeBindCount--;
		}

		pvarBind = (UIVARIANT *)pBind;
		pvarBind->lBeBindCount++;

		return 0;
	}

	int UIVARIANT::RemoveBindVariant()
	{
		if (pvarBind != NULL)
		{
			pvarBind->lBeBindCount--;
		}

		return 0;
	}

	const UIVARIANT *UIVARIANT::GetBindVariant() const
	{
		return pvarBind;
	}

	int UIVARIANT::SetBindCompareValue(const UIVARIANT &var)
	{
		if (pvarCompare != NULL)
		{
			delete pvarCompare;
			pvarCompare = NULL;
		}

		pvarCompare = new UIVARIANT;
		*pvarCompare = var;

		return 0;
	}

	const UIVARIANT UIVARIANT::GetBindCompareValue() const
	{
		UIVARIANT var;

		if (pvarCompare == NULL)
		{
			return var;
		}

		var = *pvarCompare;

		return var;
	}

	int UIVARIANT::SetVarName(LPCTSTR lpszName)
	{
		if (lpszName == NULL)
		{
			return -1;
		}

		varC4251.strVarName = lpszName;

		return 0;
	}

	LPCTSTR UIVARIANT::GetVarName()
	{
		return (LPCTSTR)varC4251.strVarName;
	}


	int UIVARIANT::SetDefaultValue(_variant_t varDefault)
	{
		varC4251.varDefaultValue = varDefault;

		return 0;
	}

	int UIVARIANT::SetAttributeValue(LPCTSTR lpszValue)
	{
		if (lpszValue == NULL)
		{
			return -1;
		}

		strAttributeValue = lpszValue;

		return 0;
	}

	LPCTSTR UIVARIANT::GetAttributeValue()
	{
		return (LPCTSTR)strAttributeValue;
	}

	int UIVARIANT::SetIUIElementName(LPCTSTR lpszIUIEleName)
	{
		if (lpszIUIEleName == NULL)
		{
			return -1;
		}

		varC4251.strIUIElementName = lpszIUIEleName;

		return 0;
	}

	LPCTSTR UIVARIANT::GetIUIElementName() const
	{
		return (LPCTSTR)varC4251.strIUIElementName;
	}

	int UIVARIANT::SetIUIAttributeName(LPCTSTR lpszIUIAttibuteName)
	{
		if (lpszIUIAttibuteName == NULL)
		{
			return -1;
		}

		varC4251.strIUIAttributeName = lpszIUIAttibuteName;

		return 0;
	}

	LPCTSTR UIVARIANT::GetIUIAttributeName()
	{
		return varC4251.strIUIAttributeName;
	}

	int UIVARIANT::SetGroupName(LPCTSTR lpszGroupName)
	{
		if (lpszGroupName == NULL)
		{
			return -1;
		}

		strGroupName = lpszGroupName;

		return 0;
	}

	LPCTSTR UIVARIANT::GetGroupName()
	{
		return strGroupName;
	}

	int UIVARIANT::SetPropWndName(LPCTSTR lpszName)
	{
		if (lpszName == NULL)
		{
			return -1;
		}

		strPropWndName = lpszName;

		return 0;
	}

	LPCTSTR UIVARIANT::GetPropWndName()
	{
		return strPropWndName;
	}

	int UIVARIANT::SetDescription(LPCTSTR lpszDes)
	{
		if (lpszDes == NULL)
		{
			return -1;
		}

		strDescription = lpszDes;

		return 0;
	}

	LPCTSTR UIVARIANT::GetDescription()
	{
		return strPropWndName;
	}

	int UIVARIANT::AddStringOption(LPCTSTR lpszOption)
	{
		if (lpszOption == NULL)
		{
			return -1;
		}

		varC4251.lstStringOptions.push_back(lpszOption);

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Destructor

	UIVARIANT::~UIVARIANT()
	{
		IUI_VARIANT_Clear();
	}

} // namespace IUI

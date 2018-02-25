// IDragDrop.c

#include <windows.h>
#include <shlobj.h>
#include <stdio.h>

#include "IDragDrop.h"
#include "Shell.h"

UINT LogPerfDropEffect = 0;
UINT PerfDropEffect    = 0;
UINT TargetCLSID       = 0;
BOOL fRecycleBin       = FALSE;
static HGLOBAL hGlob   = NULL;

DEFINE_GUID(CLSID_RecycleBin, 0x645FF040, 0x5081, 0x101B, 0x9F,0x08,
	0x00,0xAA,0x00,0x2F,0x95,0x4E);

char ss[90];
static BOOL IsWndRegisteredDragDrop(HWND hwnd)
{
	HWND h;
	if(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_ACCEPTFILES)
		return TRUE;

	while( (h = GetParent(hwnd)) != NULL)
	{
		if( GetWindowLong(h, GWL_EXSTYLE) & WS_EX_ACCEPTFILES )
			return TRUE;
		hwnd = h;
	}

	return FALSE;
}

static BOOL IsWndShellDefView(HWND hwnd)
{
	HWND h;

	while((h = GetParent(hwnd)) != NULL)
	{
		GetClassName(h, ss, 18);
		if(!strcmp(ss, "SHELLDLL_DefView"))
			return TRUE;
		hwnd = h;
	}
	return FALSE;
}

DWORD DragMoveImage(HWND hwndMain, HWND hwndList)
{
	HWND hWnd;
	POINT pt, pt1;
	HIMAGELIST hImageList;

	HCURSOR hOrig, hCur = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(21));
	HCURSOR hNot        = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(22));

	int idx = ListView_GetSelectionMark(hwndList);
	ListView_GetItemPosition(hwndList, idx, &pt);
	hImageList = (HIMAGELIST)ListView_CreateDragImage(hwndList, idx, (long)&pt);

	GetCursorPos(&pt1);
	ScreenToClient(hwndList, &pt1);

	ImageList_BeginDrag(hImageList, 0, pt1.x - pt.x, pt1.y - pt.y);
	ImageList_DragEnter(HWND_DESKTOP, pt.x, pt.y);
	hOrig = SetCursor(hCur);

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(hEvent);

	while(GetAsyncKeyState(VK_LBUTTON) && 0x8000 || GetAsyncKeyState(VK_RBUTTON) && 0x8000)
	{
		// wait for a mousemove message
		MsgWaitForMultipleObjects(1, &hEvent, TRUE, 10, QS_MOUSEMOVE);
		GetCursorPos(&pt);
		hWnd = WindowFromPoint(pt);
		if (IsWndShellDefView(hWnd))
		{
			SetCursor(hCur);
		}
		else if (IsWndRegisteredDragDrop(hWnd))
		{
			SetCursor(hCur);
		}
		else
		{
			SetCursor(hNot);
		}
		ImageList_DragMove(pt.x, pt.y);
	}
	ImageList_DragLeave(HWND_DESKTOP);
	ImageList_Destroy(hImageList);
	SetCursor(hOrig);
	CloseHandle(hEvent);

	return 0;
}

void FreeGlobalMem(void)
{
	// free the previous allocation, this is because the RecycleBin
	// needs this memory left intact for some time after draging onto it.
	// also call this as the app is terminating.
	if(hGlob)
	{
		GlobalFree(hGlob);
		hGlob = NULL;
	}
}

LPTSTR ErrorString(DWORD errno)
{

	static TCHAR szMsgBuf[0x1000];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
		    , NULL
		    , errno
		    , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
		    , (LPTSTR) &szMsgBuf
		    , 0x1000 * sizeof(TCHAR)
		    , NULL);

	return szMsgBuf;

}

DWORD HandleDragDrop(HWND hwndMain, HWND hwndList)
{
	DWORD 		ret = 0;
	DWORD       dwEffect;

	// free the previous allocation, this is because the RecycleBin
	// needs this memory left intact for some time after draging onto it.
	FreeGlobalMem();

	LogPerfDropEffect = RegisterClipboardFormat(CFSTR_LOGICALPERFORMEDDROPEFFECT);
	PerfDropEffect    = RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
	TargetCLSID       = RegisterClipboardFormat(CFSTR_TARGETCLSID); // recyclebin (not used at present)

	// Create instances of IDataObject, IDropSource and IDropTarget
	IDataObject * pDataObject = DataObject_CDataObject();
	IDropSource * pDropSource = DropSource_CDropSource();
	IDropTarget * pDropTarget = DropTarget_CDropTarget(hwndMain);
	// free instances if necessary
	if(!pDataObject || !pDropSource || !pDropTarget)
		goto cleanup;

	// cast to get pointers to two classes
	int nItems = ListView_GetSelectedCount(hwndList);
	if(!nItems)
		goto cleanup;

	FORMATETC   fe;
	STGMEDIUM   sm;
	int         i, nStart;
	DWORD       dwChars;

	//get the indexes of the selected items
	int anItems[nItems];
	for(i = 0, nStart = -1; i < nItems; i++)
	{
		nStart = anItems[i] = ListView_GetNextItem(hwndList, nStart, LVNI_SELECTED);
	}

	//get the length of the text for each selected item
	for(i = 0, dwChars = 0; i < nItems; i++)
	{
		TCHAR szText[MAX_PATH+100] = TEXT("");
		ListView_GetItemText(hwndList, anItems[i], 1, szText, MAX_PATH+100); // folder
		dwChars += lstrlen(szText);
		ListView_GetItemText(hwndList, anItems[i], 0, szText, MAX_PATH+100); // filename
		dwChars += lstrlen(szText) + 1;
	}

	//add one for the double-NULL terminator
	dwChars += 1;
	char s[dwChars];
	memset(s, 0, dwChars);
	char * pszFiles = s;
	char * pszThis;

	//get the text for each selected item
	for(i = 0, pszThis = pszFiles; i < nItems; i++)
	{
		ListView_GetItemText(hwndList, anItems[i], 1, pszThis, dwChars - (DWORD)(pszThis - pszFiles));
		pszThis += lstrlen(pszThis);
		ListView_GetItemText(hwndList, anItems[i], 0, pszThis, dwChars - (DWORD)(pszThis - pszFiles));
		pszThis += lstrlen(pszThis) + 1;
	}

	//double NULL terminate the list
	*pszThis = 0;

	//initialize the data object
	fe.cfFormat 	= CF_HDROP;
	fe.ptd 			= NULL;
	fe.lindex 		= -1;
	fe.dwAspect 	= DVASPECT_CONTENT;
	fe.tymed 		= TYMED_HGLOBAL;

	hGlob = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + ((dwChars + 1) * sizeof(TCHAR)));
	if(!hGlob)
		goto cleanup;

	char * psz;
	DROPFILES * pdf;

	POINT pt;
	GetCursorPos(&pt);

	if(hGlob)
	{
		pdf = GlobalLock(hGlob);
		pdf->pFiles = sizeof(DROPFILES);
		pdf->pt.x = 0;
		pdf->pt.y = 0;
		pdf->fNC  = FALSE;		// client coordinates
		pdf->fWide = FALSE;     // not unicode

		psz = (char*)pdf + sizeof(DROPFILES);
		memcpy(psz, pszFiles, dwChars);
		GlobalUnlock(hGlob);

	}
	else
	{
		goto cleanup;
	}

	if(pDropTarget)
		RegisterDragDrop(hwndMain, pDropTarget);

	sm.tymed = TYMED_HGLOBAL;
	sm.pUnkForRelease = NULL;
	sm.hGlobal = hGlob;

	pDataObject->lpVtbl->SetData(pDataObject, &fe, &sm, TRUE);

	IDragSourceHelper *pDragSourceHelper = NULL;

	CoCreateInstance(&CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER,
		&IID_IDragSourceHelper,	(LPVOID*)&pDragSourceHelper);

	// The pt should be screen coordinates not client coordinates as MS example uses.
	if(pDragSourceHelper)
	{
		int h = pDragSourceHelper->lpVtbl->InitializeFromWindow(pDragSourceHelper, hwndList, &pt, pDataObject);
		if(h){
//			MessageBox(0, ErrorString(h), "", MB_OK);
			goto cleanup;
		}
	}
	else
	    goto cleanup;

	// do the drag and drop DRAGDROP_S_DROP is returned
	DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);

	/*
	If a source needs to know which operation took place, it can call the data object's
	IDataObject_GetData method and request the CFSTR_LOGICALPERFORMEDDROPEFFECT format.
	This format essentially reflects what happens from the user's point of view after
	the operation is complete. If a new file is created and the original file is deleted,
	the user sees a move operation and the format's data value is set to DROPEFFECT_MOVE.
	If the original file is still there, the user sees a copy operation and the format's
	data value is set to DROPEFFECT_COPY. If a link was created, the format's data
	value will be DROPEFFECT_LINK.
	*/

	// use the CFSTR_PERFORMEDDROPEFFECT first as CFSTR_LOGICALPERFORMEDDROPEFFECT
	// may not be available before Internet Explorer 5

	fe.cfFormat 	= PerfDropEffect;
	fe.ptd 			= NULL;
	fe.lindex 		= -1;
	fe.dwAspect 	= DVASPECT_CONTENT;
	fe.tymed 		= TYMED_HGLOBAL;

	pDataObject->lpVtbl->GetData(pDataObject, &fe, &sm);

	if(fe.cfFormat == PerfDropEffect && sm.tymed == TYMED_HGLOBAL)
	{
		ret = *(DWORD*)(sm.hGlobal);
		if(ret == DROPEFFECT_MOVE)
			goto cleanup;
	}

	// if CFSTR_PERFORMEDDROPEFFECT did not return DROPEFFECT_MOVE
	// try CFSTR_LOGICALPERFORMEDDROPEFFECT
	fe.cfFormat 	= LogPerfDropEffect;
	fe.ptd 			= NULL;
	fe.lindex 		= -1;
	fe.dwAspect 	= DVASPECT_CONTENT;
	fe.tymed 		= TYMED_HGLOBAL;

	pDataObject->lpVtbl->GetData(pDataObject, &fe, &sm);

	if(fe.cfFormat == LogPerfDropEffect && sm.tymed == TYMED_HGLOBAL)
		ret = *(DWORD*)(sm.hGlobal);

	pDataObject->lpVtbl->GetData(pDataObject, &fe, &sm);

cleanup:
	// release interfaces
	if(pDragSourceHelper) pDragSourceHelper->lpVtbl->Release(pDragSourceHelper);
	if(pDataObject) pDataObject->lpVtbl->Release(pDataObject);
	if(pDropSource) pDropSource->lpVtbl->Release(pDropSource);
	if(pDropTarget) pDropTarget->lpVtbl->Release(pDropTarget);

	RevokeDragDrop(hwndMain);

	//-----------------------------------------------
	// Example of not using the lpVtbl
	//-----------------------------------------------
	// one could cast pDropSource to (DropSourceEx *)
	// and use the functions without the ->lpVtbl->
	// DropSourceEx * pDSEx = (DropSourceEx *) pDropSource;
	// pDSEx->Release(pDropSource);
	//--------------------------------------------
	return ret;
}

// IEnumFormatEtc.c
//--------------------------------------------------------------
// IEnumFORMATETC vtable
//--------------------------------------------------------------
struct IEnumFORMATETCVtbl ienumFORMATETCVtbl = {
	EnumFORMATETC_QueryInterface,
	EnumFORMATETC_AddRef,
	EnumFORMATETC_Release,
	EnumFORMATETC_Next,
	EnumFORMATETC_Skip,
	EnumFORMATETC_Reset,
	EnumFORMATETC_Clone
};
IEnumFORMATETC EnumFORMATETC = {&ienumFORMATETCVtbl};

//--------------------------------------------------------------
// IEnumFORMATETC Ex constructor
//--------------------------------------------------------------
IEnumFORMATETC * EnumFORMATETC_CEnumFormatEtc(LPFORMATETC pFE, int nItems)
{
	// alloc enough mem for interface and any class vars
	IEnumFORMATETC * pEnumFORMATETC = malloc(sizeof(EnumFORMATETCEx));
	if(!pEnumFORMATETC)
		return NULL;

	// copy the interface into the EnumFORMATETCEx
	memcpy(pEnumFORMATETC, &EnumFORMATETC, sizeof(ienumFORMATETCVtbl));

	// cast the local EnumFORMATETCEx pointer
	EnumFORMATETCEx * pEFEx = (EnumFORMATETCEx *)pEnumFORMATETC;

	// init the vars
	pEFEx->m_pStrFE    = NULL;
	pEFEx->m_iCur      = 0;
	pEFEx->m_cItems	   = nItems;
	pEFEx->m_cRefCount = 0;

	pEFEx->m_pStrFE = malloc(sizeof(FORMATETC)*nItems);
	if(!pEFEx->m_pStrFE)
		return NULL;

	memcpy(pEFEx->m_pStrFE, pFE, sizeof(FORMATETC) * nItems);

	// increment the class reference count
	pEnumFORMATETC->lpVtbl->AddRef(pEnumFORMATETC);
	return pEnumFORMATETC;
}

//-----------------------------------------------------------------
// IUnknown Interface
//-----------------------------------------------------------------
STDMETHODIMP EnumFORMATETC_QueryInterface(IEnumFORMATETC *this, REFIID riid, LPVOID *ppvOut)
{
	if(IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IEnumFORMATETC))
	{
		*ppvOut = (void*)this;
		this->lpVtbl->AddRef(this);
		return S_OK;
	}
	*ppvOut = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) EnumFORMATETC_AddRef(IEnumFORMATETC *this)
{
	// cast the local EnumFORMATETCEx pointer
	EnumFORMATETCEx * pEFEx = (EnumFORMATETCEx *)this;
	return ++pEFEx->m_cRefCount;
}

STDMETHODIMP_(ULONG) EnumFORMATETC_Release(IEnumFORMATETC *this)
{
	EnumFORMATETCEx * pEFEx = (EnumFORMATETCEx *)this;
	if(--pEFEx->m_cRefCount == 0)
	{
		if(pEFEx->m_pStrFE)
			free(pEFEx->m_pStrFE);
		free(this);
		return 0;
	}
	return pEFEx->m_cRefCount;
}

//--------------------------------------------------------------
// IEnumFORMATETC Interface
//--------------------------------------------------------------
STDMETHODIMP EnumFORMATETC_Next(IEnumFORMATETC *this, ULONG celt, LPFORMATETC pFE, ULONG *puFetched)
{
	EnumFORMATETCEx * pEFEx = (EnumFORMATETCEx *)this;
	ULONG cReturn = 0L;

	if(celt <= 0 || pFE == NULL || pEFEx->m_iCur >= pEFEx->m_cItems)
		return S_FALSE;

	if(puFetched == NULL && celt != 1)
		return S_FALSE;

	if(puFetched != NULL)
		*puFetched = 0;

	while(pEFEx->m_iCur < pEFEx->m_cItems && celt > 0)
	{
		*pFE++ = pEFEx->m_pStrFE[pEFEx->m_iCur++];
		cReturn++;
		celt--;
	}

	if(NULL != puFetched)
		*puFetched = (cReturn - celt);

	return S_OK;
}

STDMETHODIMP EnumFORMATETC_Skip(IEnumFORMATETC *this, ULONG celt)
{
	EnumFORMATETCEx * pEFEx = (EnumFORMATETCEx *)this;
	if((pEFEx->m_iCur + celt) >= pEFEx->m_cItems)
		return S_FALSE;

	pEFEx->m_iCur += celt;

	return S_OK;
}

STDMETHODIMP EnumFORMATETC_Reset(IEnumFORMATETC *this)
{
	EnumFORMATETCEx * pEFEx = (EnumFORMATETCEx *)this;
	pEFEx->m_iCur = 0;
	return S_OK;
}

STDMETHODIMP EnumFORMATETC_Clone(IEnumFORMATETC *this, IEnumFORMATETC **ppCloneEnumFormatEtc)
{
	EnumFORMATETCEx * pEFEx = (EnumFORMATETCEx *)this;
	IEnumFORMATETC * newEnum;

	if(NULL == ppCloneEnumFormatEtc)
		return S_FALSE;

	newEnum = EnumFORMATETC_CEnumFormatEtc(pEFEx->m_pStrFE, pEFEx->m_cItems);
	if(!newEnum)
	{
		*ppCloneEnumFormatEtc = NULL;
		return E_OUTOFMEMORY;
	}

	*ppCloneEnumFormatEtc = newEnum;

	// cast another local EnumFORMATETCEx pointer to the new interface
	EnumFORMATETCEx * pEFExtmp = (EnumFORMATETCEx *)newEnum;

	pEFExtmp->m_iCur = pEFEx->m_iCur;

	return S_OK;
}


// IDataObject.c
//--------------------------------------------------------------
// IDataObject vtable
//--------------------------------------------------------------
IDataObjectVtbl dataObjectVtbl = {
	DataObject_QueryInterface,
	DataObject_AddRef,
	DataObject_Release,
	DataObject_GetData,
	DataObject_GetDataHere,
	DataObject_QueryGetData,
	DataObject_GetCanonicalFormatEtc,
	DataObject_SetData,
	DataObject_EnumFormatEtc,
	DataObject_DAdvise,
	DataObject_DUnadvise,
	DataObject_EnumDAdvise
};
IDataObject DataObject = {&dataObjectVtbl};

//--------------------------------------------------------------
// IDataObject Ex constructor
//--------------------------------------------------------------
IDataObject * DataObject_CDataObject(void)
{
	// alloc enough mem for interface and the class variables
	IDataObject * pIDataObject = malloc(sizeof(DataObjectEx));
	if(!pIDataObject)
		return NULL;

	// cast the local DropSourceEx pointer
	DataObjectEx * pDOEx = (DataObjectEx *) pIDataObject;

	// copy the interface into the DropSourceEx
	memcpy(pIDataObject, &DataObject, sizeof(dataObjectVtbl));

	// init the vars
	pDOEx->m_cRefCount = 0;
	pDOEx->m_pFormatEtc = NULL;
	pDOEx->m_pStgMedium = NULL;
	pDOEx->m_cDataCount = 0;

	// increment the class reference count
	pIDataObject->lpVtbl->AddRef(pIDataObject);
	return pIDataObject;
}

//-----------------------------------------------------------------
// IUnknown Interface
//-----------------------------------------------------------------
STDMETHODIMP DataObject_QueryInterface(IDataObject *this, REFIID riid, LPVOID *ppvObject)
{
	if(IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid , &IID_IDataObject))
	{
		*ppvObject = (void*)this;
		this->lpVtbl->AddRef(this);
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DataObject_AddRef(IDataObject *this)
{
	// cast to get a pointer to the dataobject class
	DataObjectEx * pDOEx = (DataObjectEx *)this;

	// increase reference count
	return ++pDOEx->m_cRefCount;
}

STDMETHODIMP_(ULONG) DataObject_Release(IDataObject *this)
{
	DataObjectEx * pDOEx = (DataObjectEx *)this;

	if(--pDOEx->m_cRefCount == 0)
	{
		if(pDOEx->m_pStgMedium) 
		{
			// crashes in XP sometimes
			// int i;
			// for(i = 0; i < pDOEx->m_cDataCount; i++)
      		//   ReleaseStgMedium(&pDOEx->m_pStgMedium[i]);

   			free(pDOEx->m_pStgMedium);
   		}
		free(this);
		return 0;
	}
	return pDOEx->m_cRefCount;
}

//--------------------------------------------------------------
// IDataObject Interface
//--------------------------------------------------------------
STDMETHODIMP DataObject_GetData(IDataObject *this, LPFORMATETC pFE, LPSTGMEDIUM pSM)
{
	DataObjectEx * pDOEx = (DataObjectEx *)this;
	if(pFE == NULL || pSM == NULL)
		return E_INVALIDARG;

	memset(pSM, 0, sizeof(STGMEDIUM));

	DWORD i;

	for(i = 0; i < pDOEx->m_cDataCount; i++)
	{
		if((pFE->tymed & pDOEx->m_pFormatEtc[i].tymed) && (pFE->dwAspect == pDOEx->m_pFormatEtc[i].dwAspect) &&
			    (pFE->cfFormat == pDOEx->m_pFormatEtc[i].cfFormat) && (pFE->lindex   == pDOEx->m_pFormatEtc[i].lindex))
		{
			HRESULT  hr;

			hr = CopyStgMedium(&pDOEx->m_pStgMedium[i], pSM);

			if(pSM->tymed == TYMED_HGLOBAL)
			{
				//this tell's the caller not to free the global memory
				this->lpVtbl->QueryInterface(this, (REFIID)&IID_IUnknown, (LPVOID*)&pSM->pUnkForRelease);
			}
			return hr;
		}
	}

	return DV_E_FORMATETC;
}

STDMETHODIMP DataObject_GetDataHere(IDataObject *this, LPFORMATETC pFE, LPSTGMEDIUM pSM)
{
	return E_NOTIMPL;
}

STDMETHODIMP DataObject_QueryGetData(IDataObject *this, LPFORMATETC pFE)
{
	DataObjectEx * pDOEx = (DataObjectEx *)this;
	if(!pFE)
		return DV_E_FORMATETC;

	if(!(DVASPECT_CONTENT & pFE->dwAspect))
		return DV_E_DVASPECT;

	//now check for an appropriate TYMED.
	DWORD    i;
	HRESULT  hr = DV_E_TYMED;

	for(i = 0; i < pDOEx->m_cDataCount; i++)
	{
		if(pFE->tymed & pDOEx->m_pFormatEtc[i].tymed)
		{
			if(pFE->cfFormat == pDOEx->m_pFormatEtc[i].cfFormat)
				return S_OK;
			else
			    hr = DV_E_CLIPFORMAT;
		}
		else
			hr = DV_E_TYMED;
	}

	return hr;
}

STDMETHODIMP DataObject_GetCanonicalFormatEtc(IDataObject *this, LPFORMATETC pFE, LPFORMATETC pFE1)
{
	return DATA_S_SAMEFORMATETC;
}

STDMETHODIMP DataObject_SetData(IDataObject *this, LPFORMATETC pFE, LPSTGMEDIUM pSM, BOOL fRelease)
{
	DataObjectEx * pDOEx = (DataObjectEx *)this;

	if(pFE->cfFormat == TargetCLSID && pSM->tymed == TYMED_HGLOBAL)
		fRecycleBin = TRUE;

	LPFORMATETC pfeNew;
	LPSTGMEDIUM psmNew;

	pDOEx->m_cDataCount++;

	pfeNew = calloc(1, sizeof(FORMATETC)*pDOEx->m_cDataCount);
	psmNew = calloc(1, sizeof(STGMEDIUM)*pDOEx->m_cDataCount);

	//copy the existing data
	if(pDOEx->m_pFormatEtc)
		memcpy(pfeNew, pDOEx->m_pFormatEtc, sizeof(FORMATETC) * (pDOEx->m_cDataCount - 1));

	if(pDOEx->m_pStgMedium)
		memcpy(psmNew, pDOEx->m_pStgMedium, sizeof(STGMEDIUM) * (pDOEx->m_cDataCount - 1));

	//add the new data
	pfeNew[pDOEx->m_cDataCount - 1] = *pFE;
	if(fRelease)
		psmNew[pDOEx->m_cDataCount - 1] = *pSM;
	else
		CopyStgMedium(pSM, &psmNew[pDOEx->m_cDataCount - 1]);

	LPFORMATETC pfeTemp = pDOEx->m_pFormatEtc;
	pDOEx->m_pFormatEtc = pfeNew;
	pfeNew = pfeTemp;

	LPSTGMEDIUM psmTemp = pDOEx->m_pStgMedium;
	pDOEx->m_pStgMedium = psmNew;
	psmNew = psmTemp;

	if(pfeNew) free(pfeNew);
	if(psmNew) free(psmNew);

	if(pDOEx->m_pFormatEtc && pDOEx->m_pStgMedium)
		return S_OK;

	return E_OUTOFMEMORY;
}

STDMETHODIMP DataObject_EnumFormatEtc(IDataObject *this, DWORD dwDir, LPENUMFORMATETC *ppEnum)
{
	DataObjectEx * pDOEx = (DataObjectEx *)this;
	*ppEnum = NULL;

	switch(dwDir)
	{
	case DATADIR_GET:
		*ppEnum = EnumFORMATETC_CEnumFormatEtc(pDOEx->m_pFormatEtc, pDOEx->m_cDataCount);

		if(*ppEnum)
			return S_OK;
		else
		    return E_OUTOFMEMORY;

		break;

	default:
		return OLE_S_USEREG;
	}
}

STDMETHODIMP DataObject_DAdvise(IDataObject *this, LPFORMATETC pFE, DWORD advf, LPADVISESINK pAdvSink, LPDWORD pdwConnection)
{
	return E_NOTIMPL;
}

STDMETHODIMP DataObject_DUnadvise(IDataObject *this, DWORD dwConnection)
{
	return E_NOTIMPL;
}

STDMETHODIMP DataObject_EnumDAdvise(IDataObject *this, LPENUMSTATDATA *ppenumAdvise)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

// IDropSource.c
//-----------------------------------------------------------------
// vtable is an array of pointers to the functions of the interface
//-----------------------------------------------------------------
//--------------------------------------------------------------
// IDropSource vtable
//--------------------------------------------------------------
IDropSourceVtbl dropSourceVtbl = {
	DropSource_QueryInterface,
	DropSource_AddRef,
	DropSource_Release,
	DropSource_QueryContinueDrag,
	DropSource_GiveFeedback
};
IDropSource DropSource = {&dropSourceVtbl};

//-----------------------------------------------------------------
// IDropSource Ex constructor
//-----------------------------------------------------------------
IDropSource * DropSource_CDropSource(void)
{
	// alloc enough mem for interface and any class vars
	IDropSource * pIDropSource = malloc(sizeof(DropSourceEx));
	if(!pIDropSource)
		return NULL;

	// copy the interface into the DropSourceEx
	memcpy(pIDropSource, &DropSource, sizeof(dropSourceVtbl));

	// cast the local DropSourceEx pointer
	DropSourceEx * pDSEx = (DropSourceEx *) pIDropSource;
	pDSEx->m_cRefCount = 0;
	pDSEx->m_hCur      = NULL;

	// increment the class reference count
	pIDropSource->lpVtbl->AddRef(pIDropSource);
	return pIDropSource;
}

//-----------------------------------------------------------------
// IUnknown Interface
//-----------------------------------------------------------------
STDMETHODIMP DropSource_QueryInterface(IDropSource * this, REFIID riid, LPVOID* ppvObject)
{
	if(IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid , &IID_IDropSource))
	{
		*ppvObject = (void *)this;
		this->lpVtbl->AddRef(this);
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DropSource_AddRef(IDropSource *this)
{
	// cast to get a pointer to the class
	DropSourceEx * pDSEx = (DropSourceEx *)this;

	// increase reference count
	return ++pDSEx->m_cRefCount;
}

STDMETHODIMP_(ULONG) DropSource_Release(IDropSource *this)
{
	DropSourceEx * pDSEx = (DropSourceEx *)this;
	// decrease reference count
	if(--pDSEx->m_cRefCount == 0)
	{
		// free the memory if ref count is zero
		free(this);
		return 0;
	}
	return pDSEx->m_cRefCount;
}

//--------------------------------------------------------------
// IDropSource Interface
//--------------------------------------------------------------
STDMETHODIMP DropSource_GiveFeedback(IDropSource *this, DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

STDMETHODIMP DropSource_QueryContinueDrag(IDropSource *this, BOOL fEscapePressed, DWORD grfKeyState)
{
	if(fEscapePressed)
		return DRAGDROP_S_CANCEL;

	if((grfKeyState & MK_RBUTTON) || (grfKeyState & MK_LBUTTON))
		return S_OK;

	if(!(grfKeyState & MK_LBUTTON))
		return DRAGDROP_S_DROP;

	return S_OK;
}

//------------------------------------------------------------------------
// IDropTarget Interface
//------------------------------------------------------------------------
IDropTargetVtbl dropTargetVtbl = {
	DropTarget_QueryInterface,
	DropTarget_AddRef,
	DropTarget_Release,
	DropTarget_DragEnter,
	DropTarget_DragOver,
	DropTarget_DragLeave,
	DropTarget_Drop
};
IDropTarget DropTarget = {&dropTargetVtbl};

IDropTarget * DropTarget_CDropTarget(HWND hwnd)
{
	// alloc enough mem for interface and any class vars
	IDropTarget * pIDropTarget = malloc(sizeof(DropTargetEx));
	if(!pIDropTarget)
		return NULL;

	// copy the interface into the DropSourceEx
	memcpy(pIDropTarget, &DropTarget, sizeof(dropTargetVtbl));

	// cast the local DropTargetEx pointer
	DropTargetEx * pDTEx 		= (DropTargetEx *) pIDropTarget;
	pDTEx->m_cRefCount 			= 0;
	pDTEx->m_fAcceptFmt 		= FALSE;
	pDTEx->m_pDropTargetHelper  = NULL;
	pDTEx->m_hwnd 				= hwnd;

	CoCreateInstance(&CLSID_DragDropHelper,
		NULL,
		CLSCTX_INPROC_SERVER,
		&IID_IDropTargetHelper,
		(LPVOID*)&pDTEx->m_pDropTargetHelper);

	// increment the class reference count
	pIDropTarget->lpVtbl->AddRef(pIDropTarget);
	return pIDropTarget;
}

STDMETHODIMP DropTarget_QueryInterface(IDropTarget *this, REFIID riid, void ** ppvObject)
{
	if(IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid , &IID_IDropTarget))
	{
		*ppvObject = (void*)&DropTarget;
		this->lpVtbl->AddRef(this);
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) DropTarget_AddRef(IDropTarget *this)
{
	// cast the local DropTargetEx pointer
	DropTargetEx * pDTEx = (DropTargetEx *)this;
	return ++pDTEx->m_cRefCount;
}

STDMETHODIMP_(ULONG) DropTarget_Release(IDropTarget *this)
{
	DropTargetEx * pDTEx = (DropTargetEx *)this;
	if(--pDTEx->m_cRefCount == 0)
	{
		if(pDTEx->m_pDropTargetHelper)
		{
			pDTEx->m_pDropTargetHelper->lpVtbl->Release(pDTEx->m_pDropTargetHelper);
			pDTEx->m_pDropTargetHelper = NULL;
		}
		free(this);
		return 0;
	}

	return pDTEx->m_cRefCount;
}

STDMETHODIMP DropTarget_DragEnter(IDropTarget *this, LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
	DropTargetEx * pDTEx = (DropTargetEx *)this;
	if(pDTEx->m_pDropTargetHelper)
	{
		pDTEx->m_pDropTargetHelper->lpVtbl->DragEnter(pDTEx->m_pDropTargetHelper,
			pDTEx->m_hwnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
	}

	pDTEx->m_pObjDragging = pDataObj;

	FORMATETC fe;

	fe.cfFormat = CF_HDROP;
	fe.ptd      = NULL;
	fe.dwAspect = DVASPECT_CONTENT;
	fe.lindex   = -1;
	fe.tymed    = TYMED_HGLOBAL;

	// Does the drag source provide the clipboard format we are looking for?
	pDTEx->m_fAcceptFmt = (S_OK == pDataObj->lpVtbl->QueryGetData(pDataObj, &fe)) ? TRUE : FALSE;

	GetDropEffectFromKeyState(this, grfKeyState, pdwEffect);

	return S_OK;
}

STDMETHODIMP DropTarget_DragOver(IDropTarget *this, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
	DropTargetEx * pDTEx = (DropTargetEx *)this;
	if(pDTEx->m_pDropTargetHelper)
		pDTEx->m_pDropTargetHelper->lpVtbl->DragOver(pDTEx->m_pDropTargetHelper, (LPPOINT)&pt, *pdwEffect);

	GetDropEffectFromKeyState(this, grfKeyState, pdwEffect);
	*pdwEffect = DROPEFFECT_NONE;
	return S_OK;
}

STDMETHODIMP DropTarget_DragLeave(IDropTarget *this)
{
	DropTargetEx * pDTEx = (DropTargetEx *)this;
	if(pDTEx->m_pDropTargetHelper)
		pDTEx->m_pDropTargetHelper->lpVtbl->DragLeave(pDTEx->m_pDropTargetHelper);

	pDTEx->m_fAcceptFmt = FALSE;
	return S_OK;
}

STDMETHODIMP DropTarget_Drop(IDropTarget *this, LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
	DropTargetEx * pDTEx = (DropTargetEx *)this;
	if(pDTEx->m_pDropTargetHelper)
		pDTEx->m_pDropTargetHelper->lpVtbl->Drop(pDTEx->m_pDropTargetHelper, pDataObj, (LPPOINT)&pt, *pdwEffect);

	FORMATETC   fe;
	STGMEDIUM   sm;
	HRESULT     hr = E_FAIL;

	if(GetDropEffectFromKeyState(this, grfKeyState, pdwEffect))
	{
		fe.cfFormat = CF_HDROP;
		fe.ptd 		= NULL;
		fe.dwAspect = DVASPECT_CONTENT;
		fe.lindex 	= -1;
		fe.tymed 	= TYMED_HGLOBAL;

		// User has dropped on us. Get the data from drag source
		hr = pDataObj->lpVtbl->GetData(pDataObj, &fe, &sm);
		if(SUCCEEDED(hr))
		{
			// release it.
			ReleaseStgMedium(&sm);
		}
	}

	*pdwEffect = DROPEFFECT_NONE; // not accepting any drops here.

	return hr;
}

/* OleStdGetDropEffect
** -------------------
**
** Convert a keyboard state into a DROPEFFECT.
**
** returns the DROPEFFECT value derived from the key state.
**    the following is the standard interpretation:
**          no modifier -- Default Drop     (0 is returned)
**          CTRL        -- DROPEFFECT_COPY
**          SHIFT       -- DROPEFFECT_MOVE
**          CTRL-SHIFT  -- DROPEFFECT_LINK
**
**    Default Drop: this depends on the type of the target application.
**    this is re-interpretable by each target application. a typical
**    interpretation is if the drag is local to the same document
**    (which is source of the drag) then a MOVE operation is
**    performed. if the drag is not local, then a COPY operation is
**    performed.
*/

STDAPI_(DWORD) OleStdGetDropEffect( DWORD grfKeyState )
{

	if (grfKeyState & MK_CONTROL) 
	{
		if (grfKeyState & MK_SHIFT)
			return DROPEFFECT_LINK;
		else
		    return DROPEFFECT_COPY;

	}
	else if (grfKeyState & MK_SHIFT)
		return DROPEFFECT_MOVE;

	return 0;    // no modifier -- do default operation
}

BOOL GetDropEffectFromKeyState(IDropTarget *this, DWORD grfKeyState, LPDWORD pdwEffect)
{
	DropTargetEx * pDTEx = (DropTargetEx *)this;
	DWORD dwOKEffects = *pdwEffect;

	if(!pDTEx->m_fAcceptFmt)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return FALSE;
	}

	*pdwEffect = OleStdGetDropEffect(grfKeyState);

	if(*pdwEffect == 0)
	{
		// No modifier keys used by user while dragging.
		if (DROPEFFECT_COPY & dwOKEffects)
		{
			*pdwEffect = DROPEFFECT_COPY;
		}
		else if (DROPEFFECT_MOVE & dwOKEffects)
		{
			*pdwEffect = DROPEFFECT_MOVE;
		}
		else if (DROPEFFECT_LINK & dwOKEffects)
		{
			*pdwEffect = DROPEFFECT_LINK;
		}
		else
		{
			*pdwEffect = DROPEFFECT_NONE;
		}
	}
	else
	{
		// Check if the drag source application allows the drop effect desired by user.
		// The drag source specifies this in DoDragDrop
		if(!(*pdwEffect & dwOKEffects))
			*pdwEffect = DROPEFFECT_NONE;

		// links
		if(*pdwEffect == DROPEFFECT_LINK)
			*pdwEffect = DROPEFFECT_NONE;
	}

	return (DROPEFFECT_NONE == *pdwEffect) ? FALSE : TRUE;
}


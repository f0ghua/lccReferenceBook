// IDragDrop.h

#ifndef _IDRAGDROP_H_
#define _IDRAGDROP_H_
//----------------------------------------------------------------------------
// Three interfaces, IDropSource, IDataObject and IEnumFORMATETC
//----------------------------------------------------------------------------

#include <windows.h>
#include <ole2.h>
#include <shlobj.h>
#include <shellapi.h>
DWORD DragMoveImage(HWND hwndMain, HWND hwndList);
DWORD HandleDragDrop(HWND hwndMain, HWND hwndList);
void FreeGlobalMem(void);

DEFINE_GUID(IID_IDropTargetHelper, 0x4657278b, 0x411b, 0x11d2, 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);
DEFINE_GUID(CLSID_DragDropHelper,   0x4657278a, 0x411b, 0x11d2, 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);
DEFINE_GUID(IID_IDragSourceHelper, 0xde5bf786, 0x477a, 0x11d2, 0x83, 0x9d, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);

//--------------------------------------------------------------
// Forward declarations for the IDropSource Interface
//--------------------------------------------------------------
STDMETHODIMP DropSource_QueryInterface(IDropSource *this, REFIID riid, LPVOID *ppvOut);
STDMETHODIMP_(ULONG) DropSource_AddRef(IDropSource *this);
STDMETHODIMP_(ULONG) DropSource_Release(IDropSource *this);
STDMETHODIMP DropSource_QueryContinueDrag(IDropSource *this, BOOL fEscapePressed, DWORD grfKeyState);
STDMETHODIMP DropSource_GiveFeedback(IDropSource *this, DWORD dwEffect);

//--------------------------------------------------------------
// Forward declaration for the IDropSource constructor
//-----------------------------------------
IDropSource * DropSource_CDropSource(void);

//--------------------------------------------------------------
// IDropSource Ex struct, the size is large enough
// to hold the interface members and the class variables
//--------------------------------------------------------------
typedef struct _DropSourceEx
{	// first part for the vtable
	HRESULT (_stdcall *QueryInterface)(IDropSource *,REFIID,void * *);
	ULONG   (_stdcall *AddRef)(IDropSource *);
	ULONG   (_stdcall *Release)(IDropSource *);
	HRESULT (_stdcall *QueryContinueDrag)(IDropSource *,BOOL,DWORD);
	HRESULT (_stdcall *GiveFeedback)(IDropSource *,DWORD);
	// second part for the variables
	DWORD   m_cRefCount;
	HCURSOR m_hCur;
}DropSourceEx;

//--------------------------------------------------------------
//  Forward declarations for the IDataObject interface
//--------------------------------------------------------------
STDMETHODIMP DataObject_QueryInterface(IDataObject *this, REFIID riid, LPVOID *ppvOut);
STDMETHODIMP_(ULONG) DataObject_AddRef(IDataObject *this);
STDMETHODIMP_(ULONG) DataObject_Release(IDataObject *this);
STDMETHODIMP DataObject_GetData(IDataObject *this, LPFORMATETC pFE, LPSTGMEDIUM pSM);
STDMETHODIMP DataObject_GetDataHere(IDataObject *this, LPFORMATETC pFE, LPSTGMEDIUM pSM);
STDMETHODIMP DataObject_QueryGetData(IDataObject *this, LPFORMATETC pFE);
STDMETHODIMP DataObject_GetCanonicalFormatEtc(IDataObject *this, LPFORMATETC pFE1, LPFORMATETC pFE2);
STDMETHODIMP DataObject_SetData(IDataObject *this, LPFORMATETC pFE , LPSTGMEDIUM pSM, BOOL fRelease);
STDMETHODIMP DataObject_EnumFormatEtc(IDataObject *this, DWORD dwDir, LPENUMFORMATETC *ppEnum);
STDMETHODIMP DataObject_DAdvise(IDataObject *this, LPFORMATETC pFE, DWORD advf, LPADVISESINK pAdvSink, LPDWORD pdwConnection);
STDMETHODIMP DataObject_DUnadvise(IDataObject *this, DWORD dwConnection);
STDMETHODIMP DataObject_EnumDAdvise(IDataObject *this, LPENUMSTATDATA *ppenumAdvise);

//--------------------------------------------------------------
// Forward declaration for the IDataObject constructor
//--------------------------------------------------------------
IDataObject * DataObject_CDataObject(void);

//--------------------------------------------------------------
// IDataObject Ex struct to hold the interface and variables
//--------------------------------------------------------------
typedef struct _DataObjectEx
{	// first part for the vtable
	HRESULT (_stdcall *QueryInterface)( IDataObject*,REFIID,void **);
	ULONG   (_stdcall *AddRef)(IDataObject*);
	ULONG   (_stdcall *Release)(IDataObject*);
	HRESULT (_stdcall *GetData)(IDataObject*,FORMATETC*,STGMEDIUM*);
	HRESULT (_stdcall *GetDataHere)(IDataObject*,FORMATETC*,STGMEDIUM*);
	HRESULT (_stdcall *QueryGetData)(IDataObject*,FORMATETC*);
	HRESULT (_stdcall *GetCanonicalFormatEtc)(IDataObject*,FORMATETC*,FORMATETC*);
	HRESULT (_stdcall *SetData)( IDataObject*,FORMATETC*,STGMEDIUM*,BOOL);
	HRESULT (_stdcall *EnumFormatEtc)( IDataObject*,DWORD,IEnumFORMATETC **);
	HRESULT (_stdcall *DAdvise)( IDataObject*,FORMATETC*,DWORD,IAdviseSink*,DWORD*);
	HRESULT (_stdcall *DUnadvise)( IDataObject*,DWORD);
	HRESULT (_stdcall *EnumDAdvise)( IDataObject*,IEnumSTATDATA **);
	// second part for the variables
	DWORD 		m_cRefCount;
	LPFORMATETC m_pFormatEtc;
	LPSTGMEDIUM m_pStgMedium;
	DWORD 		m_cDataCount;
}DataObjectEx;

//--------------------------------------------------------------
// Forward declarations for the IEnumFORMATETC Interface
//--------------------------------------------------------------
STDMETHODIMP EnumFORMATETC_QueryInterface(IEnumFORMATETC *this, REFIID riid, LPVOID *ppvOut);
STDMETHODIMP_(ULONG) EnumFORMATETC_AddRef(IEnumFORMATETC *this);
STDMETHODIMP_(ULONG) EnumFORMATETC_Release(IEnumFORMATETC *this);
STDMETHODIMP EnumFORMATETC_Next(IEnumFORMATETC *this, ULONG celt, LPFORMATETC pFE, ULONG *puFetched);
STDMETHODIMP EnumFORMATETC_Skip(IEnumFORMATETC *this, ULONG celt);
STDMETHODIMP EnumFORMATETC_Reset(IEnumFORMATETC *this);
STDMETHODIMP EnumFORMATETC_Clone(IEnumFORMATETC *this, IEnumFORMATETC **ppCloneEnumFormatEtc);

// Forward declaration for IEnumFORMATETC constructor
IEnumFORMATETC * EnumFORMATETC_CEnumFormatEtc(LPFORMATETC pFE, int nItems);

//--------------------------------------------------------------
// IEnumFORMATETC Ex struct to hold the interface and variables
//--------------------------------------------------------------
typedef struct _EnumFORMATETCEx
{	// first part for the vtable
	HRESULT (_stdcall *QueryInterface)( IEnumFORMATETC*,REFIID,void **);
	ULONG   (_stdcall *AddRef)( IEnumFORMATETC*);
	ULONG   (_stdcall *Release)(IEnumFORMATETC*);
	HRESULT (_stdcall *Next)( IEnumFORMATETC*,ULONG,FORMATETC*,ULONG*);
	HRESULT (_stdcall *Skip)( IEnumFORMATETC*,ULONG );
	HRESULT (_stdcall *Reset)( IEnumFORMATETC*);
	HRESULT (_stdcall *Clone)( IEnumFORMATETC*,IEnumFORMATETC **);
	// second part for the variables
	DWORD 		m_cRefCount;
	LPFORMATETC m_pStrFE;
	ULONG 		m_iCur;
	ULONG 		m_cItems;
}EnumFORMATETCEx;

//--------------------------------------------------------------
// IDropTarget Interface
//--------------------------------------------------------------
STDMETHODIMP DropTarget_QueryInterface(IDropTarget *this, REFIID riid, void ** ppvObject);
STDMETHODIMP_(ULONG) DropTarget_AddRef(IDropTarget *this);
STDMETHODIMP_(ULONG) DropTarget_Release(IDropTarget *this);
STDMETHODIMP DropTarget_DragEnter(IDropTarget *this, LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
STDMETHODIMP DropTarget_DragOver(IDropTarget *this, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
STDMETHODIMP DropTarget_Drop(IDropTarget *this, LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
STDMETHODIMP DropTarget_DragLeave(IDropTarget *this);
BOOL ResolveLink(LPCSTR szLinkFile, LPSTR szPath);

// Forward declaration for IDropTarget constructor and GetDropEffectFromKeyState()
IDropTarget * DropTarget_CDropTarget(HWND hwnd);
BOOL GetDropEffectFromKeyState(IDropTarget *this, DWORD grfKeyState, LPDWORD pdwEffect);

//--------------------------------------------------------------
// IDropTarget Ex struct to hold the interface and variables
//--------------------------------------------------------------
typedef struct _DropTargetEx
{	// first part for the vtable
	HRESULT (_stdcall *QueryInterface)(IDropTarget *,REFIID,void * *);
	ULONG   (_stdcall *AddRef)(IDropTarget *);
	ULONG   (_stdcall *Release)(IDropTarget *);
	HRESULT (_stdcall *DragEnter)(IDropTarget *,IDataObject *,DWORD,POINTL,DWORD *);
	HRESULT (_stdcall *DragOver)(IDropTarget *,DWORD,POINTL,DWORD *);
	HRESULT (_stdcall *DragLeave)(IDropTarget *);
	HRESULT (_stdcall *Drop)(IDropTarget *,IDataObject *,DWORD,POINTL,DWORD *);
	// second part for the variables
	DWORD 		m_cRefCount;
	BOOL 		m_fAcceptFmt;
	LPSTGMEDIUM m_pStgMedium;
	IDropTargetHelper * m_pDropTargetHelper;
	HWND        m_hwnd;
	LPDATAOBJECT m_pObjDragging;
}DropTargetEx;

#endif //_IDRAGDROP_H_

#ifndef __ssapi_h_
#define __ssapi_h_


#ifndef __RPC_H__
#include "rpc.h"
#endif
#ifndef __RPCNDR_H__
#include "rpcndr.h"
#endif
#include <ole2.h>
#ifndef __unknwn_h__
#include "unknwn.h"
#endif
#ifndef __objidl_h__
#include "objidl.h"
#endif
#ifndef __oaidl_h__
#include "oaidl.h"
#endif
#ifndef __ocidl_h__
#include "ocidl.h"
#endif

#ifndef LIBID_ssapi_defined
#define LIBID_ssapi_defined
	const IID LIBID_ssapi = {0x783cd4e0,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

extern const IID LIBID_ssapi;

/*
Microsoft SourceSafe 6.0 Type Library
*/

/* forward declares */
struct IVSSItemOld;
struct IVSSItem;
struct IVSSVersions;
struct IVSSVersionOld;
struct IVSSVersion;
struct IVSSItems;
struct IVSSCheckouts;
struct IVSSCheckout;
struct IVSSDatabaseOld;
struct IVSSDatabase;
struct IVSSUser;
struct IVSSUsers;
struct IVSS;
struct IVSSEventsOld;
struct IVSSEvents;
struct IVSSEventHandler;

typedef enum VSSFlags
{
	VSSFLAG_USERRONO = 1,
	VSSFLAG_USERROYES = 2,
	VSSFLAG_TIMENOW = 4,
	VSSFLAG_TIMEMOD = 8,
	VSSFLAG_TIMEUPD = 12,
	VSSFLAG_EOLCR = 16,
	VSSFLAG_EOLLF = 32,
	VSSFLAG_EOLCRLF = 48,
	VSSFLAG_REPASK = 64,
	VSSFLAG_REPREPLACE = 128,
	VSSFLAG_REPSKIP = 192,
	VSSFLAG_REPMERGE = 256,
	VSSFLAG_CMPFULL = 512,
	VSSFLAG_CMPTIME = 1024,
	VSSFLAG_CMPCHKSUM = 1536,
	VSSFLAG_CMPFAIL = 2048,
	VSSFLAG_RECURSNO = 4096,
	VSSFLAG_RECURSYES = 8192,
	VSSFLAG_FORCEDIRNO = 16384,
	VSSFLAG_FORCEDIRYES = 32768,
	VSSFLAG_KEEPNO = 65536,
	VSSFLAG_KEEPYES = 131072,
	VSSFLAG_DELNO = 262144,
	VSSFLAG_DELYES = 524288,
	VSSFLAG_DELNOREPLACE = 786432,
	VSSFLAG_BINTEST = 1048576,
	VSSFLAG_BINBINARY = 2097152,
	VSSFLAG_BINTEXT = 3145728,
	VSSFLAG_DELTAYES = 4194304,
	VSSFLAG_DELTANO = 8388608,
	VSSFLAG_UPDASK = 16777216,
	VSSFLAG_UPDUPDATE = 33554432,
	VSSFLAG_UPDUNCH = 50331648,
	VSSFLAG_GETYES = 67108864,
	VSSFLAG_GETNO = 134217728,
	VSSFLAG_CHKEXCLUSIVEYES = 268435456,
	VSSFLAG_CHKEXCLUSIVENO = 536870912,
	VSSFLAG_HISTIGNOREFILES = 1073741824,
} VSSFlags;

typedef enum VSSFileStatus
{
	VSSFILE_NOTCHECKEDOUT = 0,
	VSSFILE_CHECKEDOUT = 1,
	VSSFILE_CHECKEDOUT_ME = 2,
} VSSFileStatus;

typedef enum VSSItemType
{
	VSSITEM_PROJECT = 0,
	VSSITEM_FILE = 1,
} VSSItemType;

typedef enum VSSRights
{
	VSSRIGHTS_READ = 1,
	VSSRIGHTS_CHKUPD = 2,
	VSSRIGHTS_ADDRENREM = 4,
	VSSRIGHTS_DESTROY = 8,
	VSSRIGHTS_ALL = 15,
	VSSRIGHTS_INHERITED = 16,
} VSSRights;

#ifndef CLSID_VSSItem_defined
#define CLSID_VSSItem_defined
	const CLSID CLSID_VSSItem={0x783cd4e3,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

extern const CLSID CLSID_VSSItem;

#ifndef CLSID_VSSVersion_defined
#define CLSID_VSSVersion_defined
	const CLSID CLSID_VSSVersion={0x783cd4ec,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

extern const CLSID CLSID_VSSVersion;

#ifndef CLSID_VSSCheckout_defined
#define CLSID_VSSCheckout_defined
	const CLSID CLSID_VSSCheckout={0x2a0de0e0,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

extern const CLSID CLSID_VSSCheckout;

#ifndef CLSID_VSSDatabase_defined
#define CLSID_VSSDatabase_defined
	const CLSID CLSID_VSSDatabase={0x783cd4e4,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

extern const CLSID CLSID_VSSDatabase;

#ifndef CLSID_VSSUser_defined
#define CLSID_VSSUser_defined
	const CLSID CLSID_VSSUser={0x2a0de0e5,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

extern const CLSID CLSID_VSSUser;

#ifndef CLSID_VSSApp_defined
#define CLSID_VSSApp_defined
	const CLSID CLSID_VSSApp={0x2a0de0e1,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

extern const CLSID CLSID_VSSApp;

#ifndef IID_IVSSItemOld_defined
#define IID_IVSSItemOld_defined
const IID IID_IVSSItemOld={0x783cd4e1,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSItemOld IVSSItemOld;

extern const IID IID_IVSSItemOld;

#undef INTERFACE
#define INTERFACE IVSSItemOld

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSItemOld *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSItemOld *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSItemOld *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSItemOld *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSItemOld *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSItemOld *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSItemOld *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Spec)(IVSSItemOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Binary)(IVSSItemOld *This, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *put_Binary)(IVSSItemOld *This, VARIANT_BOOL);
	HRESULT (STDMETHODCALLTYPE *get_Deleted)(IVSSItemOld *This, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *put_Deleted)(IVSSItemOld *This, VARIANT_BOOL);
	HRESULT (STDMETHODCALLTYPE *get_Type)(IVSSItemOld *This, INT*);
	HRESULT (STDMETHODCALLTYPE *get_LocalSpec)(IVSSItemOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *put_LocalSpec)(IVSSItemOld *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_Name)(IVSSItemOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *put_Name)(IVSSItemOld *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_Parent)(IVSSItemOld *This, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *get_VersionNumber)(IVSSItemOld *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Items)(IVSSItemOld *This, VARIANT_BOOL, struct IVSSItems**);
	HRESULT (STDMETHODCALLTYPE *Get)(IVSSItemOld *This, BSTR*, LONG);
	HRESULT (STDMETHODCALLTYPE *Checkout)(IVSSItemOld *This, BSTR, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *Checkin)(IVSSItemOld *This, BSTR, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *UndoCheckout)(IVSSItemOld *This, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *get_IsCheckedOut)(IVSSItemOld *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Checkouts)(IVSSItemOld *This, struct IVSSCheckouts**);
	HRESULT (STDMETHODCALLTYPE *get_IsDifferent)(IVSSItemOld *This, BSTR, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *Add)(IVSSItemOld *This, BSTR, BSTR, LONG, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *NewSubproject)(IVSSItemOld *This, BSTR, BSTR, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *Share)(IVSSItemOld *This, struct IVSSItem*, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *Destroy)(IVSSItemOld *This);
	HRESULT (STDMETHODCALLTYPE *Move)(IVSSItemOld *This, struct IVSSItem*);
	HRESULT (STDMETHODCALLTYPE *Label)(IVSSItemOld *This, BSTR, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_Versions)(IVSSItemOld *This, LONG, struct IVSSVersions**);
	HRESULT (STDMETHODCALLTYPE *get_Version)(IVSSItemOld *This, VARIANT, struct IVSSItem**);
};

#define IVSSItemOld_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSItemOld_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSItemOld_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSItemOld_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSItemOld_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSItemOld_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSItemOld_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSItemOld_get_Spec(p, a) (p)->lpVtbl->get_Spec(p, a)
#define IVSSItemOld_get_Binary(p, a) (p)->lpVtbl->get_Binary(p, a)
#define IVSSItemOld_put_Binary(p, a) (p)->lpVtbl->put_Binary(p, a)
#define IVSSItemOld_get_Deleted(p, a) (p)->lpVtbl->get_Deleted(p, a)
#define IVSSItemOld_put_Deleted(p, a) (p)->lpVtbl->put_Deleted(p, a)
#define IVSSItemOld_get_Type(p, a) (p)->lpVtbl->get_Type(p, a)
#define IVSSItemOld_get_LocalSpec(p, a) (p)->lpVtbl->get_LocalSpec(p, a)
#define IVSSItemOld_put_LocalSpec(p, a) (p)->lpVtbl->put_LocalSpec(p, a)
#define IVSSItemOld_get_Name(p, a) (p)->lpVtbl->get_Name(p, a)
#define IVSSItemOld_put_Name(p, a) (p)->lpVtbl->put_Name(p, a)
#define IVSSItemOld_get_Parent(p, a) (p)->lpVtbl->get_Parent(p, a)
#define IVSSItemOld_get_VersionNumber(p, a) (p)->lpVtbl->get_VersionNumber(p, a)
#define IVSSItemOld_get_Items(p, a, b) (p)->lpVtbl->get_Items(p, a, b)
#define IVSSItemOld_Get(p, a, b) (p)->lpVtbl->Get(p, a, b)
#define IVSSItemOld_Checkout(p, a, b, c) (p)->lpVtbl->Checkout(p, a, b, c)
#define IVSSItemOld_Checkin(p, a, b, c) (p)->lpVtbl->Checkin(p, a, b, c)
#define IVSSItemOld_UndoCheckout(p, a, b) (p)->lpVtbl->UndoCheckout(p, a, b)
#define IVSSItemOld_get_IsCheckedOut(p, a) (p)->lpVtbl->get_IsCheckedOut(p, a)
#define IVSSItemOld_get_Checkouts(p, a) (p)->lpVtbl->get_Checkouts(p, a)
#define IVSSItemOld_get_IsDifferent(p, a, b) (p)->lpVtbl->get_IsDifferent(p, a, b)
#define IVSSItemOld_Add(p, a, b, c, d) (p)->lpVtbl->Add(p, a, b, c, d)
#define IVSSItemOld_NewSubproject(p, a, b, c) (p)->lpVtbl->NewSubproject(p, a, b, c)
#define IVSSItemOld_Share(p, a, b, c) (p)->lpVtbl->Share(p, a, b, c)
#define IVSSItemOld_Destroy(p) (p)->lpVtbl->Destroy(p)
#define IVSSItemOld_Move(p, a) (p)->lpVtbl->Move(p, a)
#define IVSSItemOld_Label(p, a, b) (p)->lpVtbl->Label(p, a, b)
#define IVSSItemOld_get_Versions(p, a, b) (p)->lpVtbl->get_Versions(p, a, b)
#define IVSSItemOld_get_Version(p, a, b) (p)->lpVtbl->get_Version(p, a, b)


interface IVSSItemOld { CONST_VTBL struct IVSSItemOldVtbl *lpVtbl; };

#ifndef IID_IVSSItem_defined
#define IID_IVSSItem_defined
const IID IID_IVSSItem={0x2a0de0e7,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

typedef interface IVSSItem IVSSItem;

extern const IID IID_IVSSItem;

#undef INTERFACE
#define INTERFACE IVSSItem

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSItem *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSItem *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSItem *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSItem *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSItem *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSItem *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSItem *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Spec)(IVSSItem *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Binary)(IVSSItem *This, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *put_Binary)(IVSSItem *This, VARIANT_BOOL);
	HRESULT (STDMETHODCALLTYPE *get_Deleted)(IVSSItem *This, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *put_Deleted)(IVSSItem *This, VARIANT_BOOL);
	HRESULT (STDMETHODCALLTYPE *get_Type)(IVSSItem *This, INT*);
	HRESULT (STDMETHODCALLTYPE *get_LocalSpec)(IVSSItem *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *put_LocalSpec)(IVSSItem *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_Name)(IVSSItem *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *put_Name)(IVSSItem *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_Parent)(IVSSItem *This, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *get_VersionNumber)(IVSSItem *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Items)(IVSSItem *This, VARIANT_BOOL, struct IVSSItems**);
	HRESULT (STDMETHODCALLTYPE *Get)(IVSSItem *This, BSTR*, LONG);
	HRESULT (STDMETHODCALLTYPE *Checkout)(IVSSItem *This, BSTR, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *Checkin)(IVSSItem *This, BSTR, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *UndoCheckout)(IVSSItem *This, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *get_IsCheckedOut)(IVSSItem *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Checkouts)(IVSSItem *This, struct IVSSCheckouts**);
	HRESULT (STDMETHODCALLTYPE *get_IsDifferent)(IVSSItem *This, BSTR, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *Add)(IVSSItem *This, BSTR, BSTR, LONG, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *NewSubproject)(IVSSItem *This, BSTR, BSTR, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *Share)(IVSSItem *This, struct IVSSItem*, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *Destroy)(IVSSItem *This);
	HRESULT (STDMETHODCALLTYPE *Move)(IVSSItem *This, struct IVSSItem*);
	HRESULT (STDMETHODCALLTYPE *Label)(IVSSItem *This, BSTR, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_Versions)(IVSSItem *This, LONG, struct IVSSVersions**);
	HRESULT (STDMETHODCALLTYPE *get_Version)(IVSSItem *This, VARIANT, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *get_Links)(IVSSItem *This, struct IVSSItems**);
	HRESULT (STDMETHODCALLTYPE *Branch)(IVSSItem *This, BSTR, LONG, struct IVSSItem**);
};

#define IVSSItem_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSItem_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSItem_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSItem_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSItem_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSItem_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSItem_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSItem_get_Spec(p, a) (p)->lpVtbl->get_Spec(p, a)
#define IVSSItem_get_Binary(p, a) (p)->lpVtbl->get_Binary(p, a)
#define IVSSItem_put_Binary(p, a) (p)->lpVtbl->put_Binary(p, a)
#define IVSSItem_get_Deleted(p, a) (p)->lpVtbl->get_Deleted(p, a)
#define IVSSItem_put_Deleted(p, a) (p)->lpVtbl->put_Deleted(p, a)
#define IVSSItem_get_Type(p, a) (p)->lpVtbl->get_Type(p, a)
#define IVSSItem_get_LocalSpec(p, a) (p)->lpVtbl->get_LocalSpec(p, a)
#define IVSSItem_put_LocalSpec(p, a) (p)->lpVtbl->put_LocalSpec(p, a)
#define IVSSItem_get_Name(p, a) (p)->lpVtbl->get_Name(p, a)
#define IVSSItem_put_Name(p, a) (p)->lpVtbl->put_Name(p, a)
#define IVSSItem_get_Parent(p, a) (p)->lpVtbl->get_Parent(p, a)
#define IVSSItem_get_VersionNumber(p, a) (p)->lpVtbl->get_VersionNumber(p, a)
#define IVSSItem_get_Items(p, a, b) (p)->lpVtbl->get_Items(p, a, b)
#define IVSSItem_Get(p, a, b) (p)->lpVtbl->Get(p, a, b)
#define IVSSItem_Checkout(p, a, b, c) (p)->lpVtbl->Checkout(p, a, b, c)
#define IVSSItem_Checkin(p, a, b, c) (p)->lpVtbl->Checkin(p, a, b, c)
#define IVSSItem_UndoCheckout(p, a, b) (p)->lpVtbl->UndoCheckout(p, a, b)
#define IVSSItem_get_IsCheckedOut(p, a) (p)->lpVtbl->get_IsCheckedOut(p, a)
#define IVSSItem_get_Checkouts(p, a) (p)->lpVtbl->get_Checkouts(p, a)
#define IVSSItem_get_IsDifferent(p, a, b) (p)->lpVtbl->get_IsDifferent(p, a, b)
#define IVSSItem_Add(p, a, b, c, d) (p)->lpVtbl->Add(p, a, b, c, d)
#define IVSSItem_NewSubproject(p, a, b, c) (p)->lpVtbl->NewSubproject(p, a, b, c)
#define IVSSItem_Share(p, a, b, c) (p)->lpVtbl->Share(p, a, b, c)
#define IVSSItem_Destroy(p) (p)->lpVtbl->Destroy(p)
#define IVSSItem_Move(p, a) (p)->lpVtbl->Move(p, a)
#define IVSSItem_Label(p, a, b) (p)->lpVtbl->Label(p, a, b)
#define IVSSItem_get_Versions(p, a, b) (p)->lpVtbl->get_Versions(p, a, b)
#define IVSSItem_get_Version(p, a, b) (p)->lpVtbl->get_Version(p, a, b)
#define IVSSItem_get_Links(p, a) (p)->lpVtbl->get_Links(p, a)
#define IVSSItem_Branch(p, a, b, c) (p)->lpVtbl->Branch(p, a, b, c)


interface IVSSItem { CONST_VTBL struct IVSSItemVtbl *lpVtbl; };

#ifndef IID_IVSSVersions_defined
#define IID_IVSSVersions_defined
const IID IID_IVSSVersions={0x783cd4e7,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSVersions IVSSVersions;

extern const IID IID_IVSSVersions;

#undef INTERFACE
#define INTERFACE IVSSVersions

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSVersions *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSVersions *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSVersions *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSVersions *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSVersions *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSVersions *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSVersions *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *_NewEnum)(IVSSVersions *This, IUnknown**);
};

#define IVSSVersions_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSVersions_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSVersions_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSVersions_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSVersions_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSVersions_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSVersions_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSVersions__NewEnum(p, a) (p)->lpVtbl->_NewEnum(p, a)


interface IVSSVersions { CONST_VTBL struct IVSSVersionsVtbl *lpVtbl; };

#ifndef IID_IVSSVersionOld_defined
#define IID_IVSSVersionOld_defined
const IID IID_IVSSVersionOld={0x783cd4e8,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSVersionOld IVSSVersionOld;

extern const IID IID_IVSSVersionOld;

#undef INTERFACE
#define INTERFACE IVSSVersionOld

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSVersionOld *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSVersionOld *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSVersionOld *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSVersionOld *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSVersionOld *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSVersionOld *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSVersionOld *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Username)(IVSSVersionOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_VersionNumber)(IVSSVersionOld *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Action)(IVSSVersionOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Date)(IVSSVersionOld *This, DATE*);
	HRESULT (STDMETHODCALLTYPE *get_Comment)(IVSSVersionOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Label)(IVSSVersionOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_VSSItem)(IVSSVersionOld *This, struct IVSSItem**);
};

#define IVSSVersionOld_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSVersionOld_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSVersionOld_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSVersionOld_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSVersionOld_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSVersionOld_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSVersionOld_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSVersionOld_get_Username(p, a) (p)->lpVtbl->get_Username(p, a)
#define IVSSVersionOld_get_VersionNumber(p, a) (p)->lpVtbl->get_VersionNumber(p, a)
#define IVSSVersionOld_get_Action(p, a) (p)->lpVtbl->get_Action(p, a)
#define IVSSVersionOld_get_Date(p, a) (p)->lpVtbl->get_Date(p, a)
#define IVSSVersionOld_get_Comment(p, a) (p)->lpVtbl->get_Comment(p, a)
#define IVSSVersionOld_get_Label(p, a) (p)->lpVtbl->get_Label(p, a)
#define IVSSVersionOld_get_VSSItem(p, a) (p)->lpVtbl->get_VSSItem(p, a)


interface IVSSVersionOld { CONST_VTBL struct IVSSVersionOldVtbl *lpVtbl; };

#ifndef IID_IVSSVersion_defined
#define IID_IVSSVersion_defined
const IID IID_IVSSVersion={0x2a0de0e9,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

typedef interface IVSSVersion IVSSVersion;

extern const IID IID_IVSSVersion;

#undef INTERFACE
#define INTERFACE IVSSVersion

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSVersion *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSVersion *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSVersion *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSVersion *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSVersion *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSVersion *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSVersion *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Username)(IVSSVersion *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_VersionNumber)(IVSSVersion *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Action)(IVSSVersion *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Date)(IVSSVersion *This, DATE*);
	HRESULT (STDMETHODCALLTYPE *get_Comment)(IVSSVersion *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Label)(IVSSVersion *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_VSSItem)(IVSSVersion *This, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *get_LabelComment)(IVSSVersion *This, BSTR*);
};

#define IVSSVersion_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSVersion_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSVersion_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSVersion_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSVersion_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSVersion_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSVersion_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSVersion_get_Username(p, a) (p)->lpVtbl->get_Username(p, a)
#define IVSSVersion_get_VersionNumber(p, a) (p)->lpVtbl->get_VersionNumber(p, a)
#define IVSSVersion_get_Action(p, a) (p)->lpVtbl->get_Action(p, a)
#define IVSSVersion_get_Date(p, a) (p)->lpVtbl->get_Date(p, a)
#define IVSSVersion_get_Comment(p, a) (p)->lpVtbl->get_Comment(p, a)
#define IVSSVersion_get_Label(p, a) (p)->lpVtbl->get_Label(p, a)
#define IVSSVersion_get_VSSItem(p, a) (p)->lpVtbl->get_VSSItem(p, a)
#define IVSSVersion_get_LabelComment(p, a) (p)->lpVtbl->get_LabelComment(p, a)


interface IVSSVersion { CONST_VTBL struct IVSSVersionVtbl *lpVtbl; };

#ifndef IID_IVSSItems_defined
#define IID_IVSSItems_defined
const IID IID_IVSSItems={0x783cd4e5,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSItems IVSSItems;

extern const IID IID_IVSSItems;

#undef INTERFACE
#define INTERFACE IVSSItems

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSItems *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSItems *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSItems *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSItems *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSItems *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSItems *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSItems *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Count)(IVSSItems *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Item)(IVSSItems *This, VARIANT, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *_NewEnum)(IVSSItems *This, IUnknown**);
};

#define IVSSItems_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSItems_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSItems_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSItems_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSItems_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSItems_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSItems_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSItems_get_Count(p, a) (p)->lpVtbl->get_Count(p, a)
#define IVSSItems_get_Item(p, a, b) (p)->lpVtbl->get_Item(p, a, b)
#define IVSSItems__NewEnum(p, a) (p)->lpVtbl->_NewEnum(p, a)


interface IVSSItems { CONST_VTBL struct IVSSItemsVtbl *lpVtbl; };

#ifndef IID_IVSSCheckouts_defined
#define IID_IVSSCheckouts_defined
const IID IID_IVSSCheckouts={0x8903a770,0xf55f,0x11cf,0x92, 0x27, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

typedef interface IVSSCheckouts IVSSCheckouts;

extern const IID IID_IVSSCheckouts;

#undef INTERFACE
#define INTERFACE IVSSCheckouts

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSCheckouts *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSCheckouts *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSCheckouts *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSCheckouts *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSCheckouts *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSCheckouts *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSCheckouts *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Count)(IVSSCheckouts *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Item)(IVSSCheckouts *This, VARIANT, struct IVSSCheckout**);
	HRESULT (STDMETHODCALLTYPE *_NewEnum)(IVSSCheckouts *This, IUnknown**);
};

#define IVSSCheckouts_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSCheckouts_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSCheckouts_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSCheckouts_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSCheckouts_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSCheckouts_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSCheckouts_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSCheckouts_get_Count(p, a) (p)->lpVtbl->get_Count(p, a)
#define IVSSCheckouts_get_Item(p, a, b) (p)->lpVtbl->get_Item(p, a, b)
#define IVSSCheckouts__NewEnum(p, a) (p)->lpVtbl->_NewEnum(p, a)


interface IVSSCheckouts { CONST_VTBL struct IVSSCheckoutsVtbl *lpVtbl; };

#ifndef IID_IVSSCheckout_defined
#define IID_IVSSCheckout_defined
const IID IID_IVSSCheckout={0x783cd4e6,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSCheckout IVSSCheckout;

extern const IID IID_IVSSCheckout;

#undef INTERFACE
#define INTERFACE IVSSCheckout

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSCheckout *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSCheckout *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSCheckout *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSCheckout *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSCheckout *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSCheckout *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSCheckout *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Username)(IVSSCheckout *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Date)(IVSSCheckout *This, DATE*);
	HRESULT (STDMETHODCALLTYPE *get_LocalSpec)(IVSSCheckout *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Machine)(IVSSCheckout *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Project)(IVSSCheckout *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Comment)(IVSSCheckout *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_VersionNumber)(IVSSCheckout *This, LONG*);
};

#define IVSSCheckout_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSCheckout_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSCheckout_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSCheckout_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSCheckout_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSCheckout_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSCheckout_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSCheckout_get_Username(p, a) (p)->lpVtbl->get_Username(p, a)
#define IVSSCheckout_get_Date(p, a) (p)->lpVtbl->get_Date(p, a)
#define IVSSCheckout_get_LocalSpec(p, a) (p)->lpVtbl->get_LocalSpec(p, a)
#define IVSSCheckout_get_Machine(p, a) (p)->lpVtbl->get_Machine(p, a)
#define IVSSCheckout_get_Project(p, a) (p)->lpVtbl->get_Project(p, a)
#define IVSSCheckout_get_Comment(p, a) (p)->lpVtbl->get_Comment(p, a)
#define IVSSCheckout_get_VersionNumber(p, a) (p)->lpVtbl->get_VersionNumber(p, a)


interface IVSSCheckout { CONST_VTBL struct IVSSCheckoutVtbl *lpVtbl; };

#ifndef IID_IVSSDatabaseOld_defined
#define IID_IVSSDatabaseOld_defined
const IID IID_IVSSDatabaseOld={0x783cd4e2,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSDatabaseOld IVSSDatabaseOld;

extern const IID IID_IVSSDatabaseOld;

#undef INTERFACE
#define INTERFACE IVSSDatabaseOld

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSDatabaseOld *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSDatabaseOld *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSDatabaseOld *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSDatabaseOld *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSDatabaseOld *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSDatabaseOld *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSDatabaseOld *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *Open)(IVSSDatabaseOld *This, BSTR, BSTR, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_SrcSafeIni)(IVSSDatabaseOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_DatabaseName)(IVSSDatabaseOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Username)(IVSSDatabaseOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_CurrentProject)(IVSSDatabaseOld *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *put_CurrentProject)(IVSSDatabaseOld *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_VSSItem)(IVSSDatabaseOld *This, BSTR, VARIANT_BOOL, struct IVSSItem**);
};

#define IVSSDatabaseOld_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSDatabaseOld_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSDatabaseOld_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSDatabaseOld_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSDatabaseOld_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSDatabaseOld_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSDatabaseOld_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSDatabaseOld_Open(p, a, b, c) (p)->lpVtbl->Open(p, a, b, c)
#define IVSSDatabaseOld_get_SrcSafeIni(p, a) (p)->lpVtbl->get_SrcSafeIni(p, a)
#define IVSSDatabaseOld_get_DatabaseName(p, a) (p)->lpVtbl->get_DatabaseName(p, a)
#define IVSSDatabaseOld_get_Username(p, a) (p)->lpVtbl->get_Username(p, a)
#define IVSSDatabaseOld_get_CurrentProject(p, a) (p)->lpVtbl->get_CurrentProject(p, a)
#define IVSSDatabaseOld_put_CurrentProject(p, a) (p)->lpVtbl->put_CurrentProject(p, a)
#define IVSSDatabaseOld_get_VSSItem(p, a, b, c) (p)->lpVtbl->get_VSSItem(p, a, b, c)


interface IVSSDatabaseOld { CONST_VTBL struct IVSSDatabaseOldVtbl *lpVtbl; };

#ifndef IID_IVSSDatabase_defined
#define IID_IVSSDatabase_defined
const IID IID_IVSSDatabase={0x2a0de0e2,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

typedef interface IVSSDatabase IVSSDatabase;

extern const IID IID_IVSSDatabase;

#undef INTERFACE
#define INTERFACE IVSSDatabase

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSDatabase *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSDatabase *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSDatabase *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSDatabase *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSDatabase *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSDatabase *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSDatabase *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *Open)(IVSSDatabase *This, BSTR, BSTR, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_SrcSafeIni)(IVSSDatabase *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_DatabaseName)(IVSSDatabase *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_Username)(IVSSDatabase *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *get_CurrentProject)(IVSSDatabase *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *put_CurrentProject)(IVSSDatabase *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_VSSItem)(IVSSDatabase *This, BSTR, VARIANT_BOOL, struct IVSSItem**);
	HRESULT (STDMETHODCALLTYPE *AddUser)(IVSSDatabase *This, BSTR, BSTR, VARIANT_BOOL, struct IVSSUser**);
	HRESULT (STDMETHODCALLTYPE *get_User)(IVSSDatabase *This, BSTR, struct IVSSUser**);
	HRESULT (STDMETHODCALLTYPE *get_Users)(IVSSDatabase *This, struct IVSSUsers**);
	HRESULT (STDMETHODCALLTYPE *get_ProjectRightsEnabled)(IVSSDatabase *This, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *put_ProjectRightsEnabled)(IVSSDatabase *This, VARIANT_BOOL);
	HRESULT (STDMETHODCALLTYPE *get_DefaultProjectRights)(IVSSDatabase *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *put_DefaultProjectRights)(IVSSDatabase *This, LONG);
};

#define IVSSDatabase_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSDatabase_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSDatabase_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSDatabase_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSDatabase_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSDatabase_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSDatabase_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSDatabase_Open(p, a, b, c) (p)->lpVtbl->Open(p, a, b, c)
#define IVSSDatabase_get_SrcSafeIni(p, a) (p)->lpVtbl->get_SrcSafeIni(p, a)
#define IVSSDatabase_get_DatabaseName(p, a) (p)->lpVtbl->get_DatabaseName(p, a)
#define IVSSDatabase_get_Username(p, a) (p)->lpVtbl->get_Username(p, a)
#define IVSSDatabase_get_CurrentProject(p, a) (p)->lpVtbl->get_CurrentProject(p, a)
#define IVSSDatabase_put_CurrentProject(p, a) (p)->lpVtbl->put_CurrentProject(p, a)
#define IVSSDatabase_get_VSSItem(p, a, b, c) (p)->lpVtbl->get_VSSItem(p, a, b, c)
#define IVSSDatabase_AddUser(p, a, b, c, d) (p)->lpVtbl->AddUser(p, a, b, c, d)
#define IVSSDatabase_get_User(p, a, b) (p)->lpVtbl->get_User(p, a, b)
#define IVSSDatabase_get_Users(p, a) (p)->lpVtbl->get_Users(p, a)
#define IVSSDatabase_get_ProjectRightsEnabled(p, a) (p)->lpVtbl->get_ProjectRightsEnabled(p, a)
#define IVSSDatabase_put_ProjectRightsEnabled(p, a) (p)->lpVtbl->put_ProjectRightsEnabled(p, a)
#define IVSSDatabase_get_DefaultProjectRights(p, a) (p)->lpVtbl->get_DefaultProjectRights(p, a)
#define IVSSDatabase_put_DefaultProjectRights(p, a) (p)->lpVtbl->put_DefaultProjectRights(p, a)


interface IVSSDatabase { CONST_VTBL struct IVSSDatabaseVtbl *lpVtbl; };

#ifndef IID_IVSSUser_defined
#define IID_IVSSUser_defined
const IID IID_IVSSUser={0x2a0de0e3,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

typedef interface IVSSUser IVSSUser;

extern const IID IID_IVSSUser;

#undef INTERFACE
#define INTERFACE IVSSUser

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSUser *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSUser *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSUser *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSUser *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSUser *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSUser *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSUser *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *Delete)(IVSSUser *This);
	HRESULT (STDMETHODCALLTYPE *get_Name)(IVSSUser *This, BSTR*);
	HRESULT (STDMETHODCALLTYPE *put_Name)(IVSSUser *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *put_Password)(IVSSUser *This, BSTR);
	HRESULT (STDMETHODCALLTYPE *get_ReadOnly)(IVSSUser *This, VARIANT_BOOL*);
	HRESULT (STDMETHODCALLTYPE *put_ReadOnly)(IVSSUser *This, VARIANT_BOOL);
	HRESULT (STDMETHODCALLTYPE *get_ProjectRights)(IVSSUser *This, BSTR, LONG*);
	HRESULT (STDMETHODCALLTYPE *put_ProjectRights)(IVSSUser *This, BSTR, LONG);
	HRESULT (STDMETHODCALLTYPE *RemoveProjectRights)(IVSSUser *This, BSTR);
};

#define IVSSUser_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSUser_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSUser_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSUser_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSUser_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSUser_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSUser_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSUser_Delete(p) (p)->lpVtbl->Delete(p)
#define IVSSUser_get_Name(p, a) (p)->lpVtbl->get_Name(p, a)
#define IVSSUser_put_Name(p, a) (p)->lpVtbl->put_Name(p, a)
#define IVSSUser_put_Password(p, a) (p)->lpVtbl->put_Password(p, a)
#define IVSSUser_get_ReadOnly(p, a) (p)->lpVtbl->get_ReadOnly(p, a)
#define IVSSUser_put_ReadOnly(p, a) (p)->lpVtbl->put_ReadOnly(p, a)
#define IVSSUser_get_ProjectRights(p, a, b) (p)->lpVtbl->get_ProjectRights(p, a, b)
#define IVSSUser_put_ProjectRights(p, a, b) (p)->lpVtbl->put_ProjectRights(p, a, b)
#define IVSSUser_RemoveProjectRights(p, a) (p)->lpVtbl->RemoveProjectRights(p, a)


interface IVSSUser { CONST_VTBL struct IVSSUserVtbl *lpVtbl; };

#ifndef IID_IVSSUsers_defined
#define IID_IVSSUsers_defined
const IID IID_IVSSUsers={0x2a0de0e4,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

typedef interface IVSSUsers IVSSUsers;

extern const IID IID_IVSSUsers;

#undef INTERFACE
#define INTERFACE IVSSUsers

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSSUsers *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSSUsers *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSSUsers *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSSUsers *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSSUsers *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSSUsers *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSSUsers *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_Count)(IVSSUsers *This, LONG*);
	HRESULT (STDMETHODCALLTYPE *get_Item)(IVSSUsers *This, VARIANT, struct IVSSUser**);
	HRESULT (STDMETHODCALLTYPE *_NewEnum)(IVSSUsers *This, IUnknown**);
};

#define IVSSUsers_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSSUsers_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSSUsers_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSSUsers_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSSUsers_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSSUsers_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSSUsers_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSSUsers_get_Count(p, a) (p)->lpVtbl->get_Count(p, a)
#define IVSSUsers_get_Item(p, a, b) (p)->lpVtbl->get_Item(p, a, b)
#define IVSSUsers__NewEnum(p, a) (p)->lpVtbl->_NewEnum(p, a)


interface IVSSUsers { CONST_VTBL struct IVSSUsersVtbl *lpVtbl; };

#ifndef IID_IVSS_defined
#define IID_IVSS_defined
const IID IID_IVSS={0x783cd4eb,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSS IVSS;

extern const IID IID_IVSS;

#undef INTERFACE
#define INTERFACE IVSS

DECLARE_INTERFACE_(INTERFACE, IDispatch)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVSS *This, GUID*, void**);
	ULONG (STDMETHODCALLTYPE *AddRef)(IVSS *This);
	ULONG (STDMETHODCALLTYPE *Release)(IVSS *This);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IVSS *This, UINT*);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IVSS *This, UINT, ULONG, void**);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IVSS *This, GUID*, char**, UINT, ULONG, LONG*);
	HRESULT (STDMETHODCALLTYPE *Invoke)(IVSS *This, LONG, GUID*, ULONG, USHORT, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	HRESULT (STDMETHODCALLTYPE *get_VSSDatabase)(IVSS *This, struct IVSSDatabase**);
};

#define IVSS_QueryInterface(p, a, b) (p)->lpVtbl->QueryInterface(p, a, b)
#define IVSS_AddRef(p, a) (p)->lpVtbl->AddRef(p, a)
#define IVSS_Release(p, a) (p)->lpVtbl->Release(p, a)
#define IVSS_GetTypeInfoCount(p, a) (p)->lpVtbl->GetTypeInfoCount(p, a)
#define IVSS_GetTypeInfo(p, a, b, c) (p)->lpVtbl->GetTypeInfo(p, a, b, c)
#define IVSS_GetIDsOfNames(p, a, b, c, d, e) (p)->lpVtbl->GetIDsOfNames(p, a, b, c, d, e)
#define IVSS_Invoke(p, a, b, c, d, e, f, g, h) (p)->lpVtbl->Invoke(p, a, b, c, d, e, f, g, h)
#define IVSS_get_VSSDatabase(p, a) (p)->lpVtbl->get_VSSDatabase(p, a)


interface IVSS { CONST_VTBL struct IVSSVtbl *lpVtbl; };

#ifndef IID_IVSSEventsOld_defined
#define IID_IVSSEventsOld_defined
const IID IID_IVSSEventsOld={0x783cd4e9,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSEventsOld IVSSEventsOld;

extern const IID IID_IVSSEventsOld;

#undef INTERFACE
#define INTERFACE IVSSEventsOld

DECLARE_INTERFACE_(INTERFACE, IUnknown)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(THIS_ GUID*, void**);
	ULONG   (STDMETHODCALLTYPE *AddRef)(THIS);
	ULONG   (STDMETHODCALLTYPE *Release)(THIS);
	HRESULT (STDMETHODCALLTYPE *BeforeAdd)(IVSSEventsOld *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterAdd)(IVSSEventsOld *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeCheckout)(IVSSEventsOld *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterCheckout)(IVSSEventsOld *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeCheckin)(IVSSEventsOld *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterCheckin)(IVSSEventsOld *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeUndoCheckout)(IVSSEventsOld *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterUndoCheckout)(IVSSEventsOld *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeRename)(IVSSEventsOld *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterRename)(IVSSEventsOld *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeBranch)(IVSSEventsOld *This, struct IVSSItem*, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterBranch)(IVSSEventsOld *This, struct IVSSItem*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeEvent)(IVSSEventsOld *This, LONG, struct IVSSItem*, BSTR, VARIANT, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterEvent)(IVSSEventsOld *This, LONG, struct IVSSItem*, BSTR, VARIANT, HRESULT*);
};

#define IVSSEventsOld_BeforeAdd(p, a, b, c, d) (p)->lpVtbl->BeforeAdd(p, a, b, c, d)
#define IVSSEventsOld_AfterAdd(p, a, b, c) (p)->lpVtbl->AfterAdd(p, a, b, c)
#define IVSSEventsOld_BeforeCheckout(p, a, b, c, d) (p)->lpVtbl->BeforeCheckout(p, a, b, c, d)
#define IVSSEventsOld_AfterCheckout(p, a, b, c) (p)->lpVtbl->AfterCheckout(p, a, b, c)
#define IVSSEventsOld_BeforeCheckin(p, a, b, c, d) (p)->lpVtbl->BeforeCheckin(p, a, b, c, d)
#define IVSSEventsOld_AfterCheckin(p, a, b, c) (p)->lpVtbl->AfterCheckin(p, a, b, c)
#define IVSSEventsOld_BeforeUndoCheckout(p, a, b, c, d) (p)->lpVtbl->BeforeUndoCheckout(p, a, b, c, d)
#define IVSSEventsOld_AfterUndoCheckout(p, a, b, c) (p)->lpVtbl->AfterUndoCheckout(p, a, b, c)
#define IVSSEventsOld_BeforeRename(p, a, b, c, d) (p)->lpVtbl->BeforeRename(p, a, b, c, d)
#define IVSSEventsOld_AfterRename(p, a, b, c) (p)->lpVtbl->AfterRename(p, a, b, c)
#define IVSSEventsOld_BeforeBranch(p, a, b, c) (p)->lpVtbl->BeforeBranch(p, a, b, c)
#define IVSSEventsOld_AfterBranch(p, a, b) (p)->lpVtbl->AfterBranch(p, a, b)
#define IVSSEventsOld_BeforeEvent(p, a, b, c, d, e, f) (p)->lpVtbl->BeforeEvent(p, a, b, c, d, e, f)
#define IVSSEventsOld_AfterEvent(p, a, b, c, d, e) (p)->lpVtbl->AfterEvent(p, a, b, c, d, e)


interface IVSSEventsOld { CONST_VTBL struct IVSSEventsOldVtbl *lpVtbl; };

#ifndef IID_IVSSEvents_defined
#define IID_IVSSEvents_defined
const IID IID_IVSSEvents={0x2a0de0e8,0x2e9f,0x11d0,0x92, 0x36, 0x0, 0xaa, 0x0, 0xa1, 0xeb, 0x95};
#endif

typedef interface IVSSEvents IVSSEvents;

extern const IID IID_IVSSEvents;

#undef INTERFACE
#define INTERFACE IVSSEvents

DECLARE_INTERFACE_(INTERFACE, IUnknown)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(THIS_ GUID*, void**);
	ULONG   (STDMETHODCALLTYPE *AddRef)(THIS);
	ULONG   (STDMETHODCALLTYPE *Release)(THIS);
	HRESULT (STDMETHODCALLTYPE *BeforeAdd)(IVSSEvents *This, struct IVSSItem*, BSTR, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterAdd)(IVSSEvents *This, struct IVSSItem*, BSTR, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeCheckout)(IVSSEvents *This, struct IVSSItem*, BSTR, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterCheckout)(IVSSEvents *This, struct IVSSItem*, BSTR, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeCheckin)(IVSSEvents *This, struct IVSSItem*, BSTR, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterCheckin)(IVSSEvents *This, struct IVSSItem*, BSTR, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeUndoCheckout)(IVSSEvents *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterUndoCheckout)(IVSSEvents *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeRename)(IVSSEvents *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterRename)(IVSSEvents *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeBranch)(IVSSEvents *This, struct IVSSItem*, BSTR, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterBranch)(IVSSEvents *This, struct IVSSItem*, BSTR, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeginCommand)(IVSSEvents *This, LONG, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *EndCommand)(IVSSEvents *This, LONG, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *BeforeEvent)(IVSSEvents *This, LONG, struct IVSSItem*, BSTR, VARIANT, VARIANT_BOOL*, HRESULT*);
	HRESULT (STDMETHODCALLTYPE *AfterEvent)(IVSSEvents *This, LONG, struct IVSSItem*, BSTR, VARIANT, HRESULT*);
};

#define IVSSEvents_BeforeAdd(p, a, b, c, d, e) (p)->lpVtbl->BeforeAdd(p, a, b, c, d, e)
#define IVSSEvents_AfterAdd(p, a, b, c, d) (p)->lpVtbl->AfterAdd(p, a, b, c, d)
#define IVSSEvents_BeforeCheckout(p, a, b, c, d, e) (p)->lpVtbl->BeforeCheckout(p, a, b, c, d, e)
#define IVSSEvents_AfterCheckout(p, a, b, c, d) (p)->lpVtbl->AfterCheckout(p, a, b, c, d)
#define IVSSEvents_BeforeCheckin(p, a, b, c, d, e) (p)->lpVtbl->BeforeCheckin(p, a, b, c, d, e)
#define IVSSEvents_AfterCheckin(p, a, b, c, d) (p)->lpVtbl->AfterCheckin(p, a, b, c, d)
#define IVSSEvents_BeforeUndoCheckout(p, a, b, c, d) (p)->lpVtbl->BeforeUndoCheckout(p, a, b, c, d)
#define IVSSEvents_AfterUndoCheckout(p, a, b, c) (p)->lpVtbl->AfterUndoCheckout(p, a, b, c)
#define IVSSEvents_BeforeRename(p, a, b, c, d) (p)->lpVtbl->BeforeRename(p, a, b, c, d)
#define IVSSEvents_AfterRename(p, a, b, c) (p)->lpVtbl->AfterRename(p, a, b, c)
#define IVSSEvents_BeforeBranch(p, a, b, c, d) (p)->lpVtbl->BeforeBranch(p, a, b, c, d)
#define IVSSEvents_AfterBranch(p, a, b, c) (p)->lpVtbl->AfterBranch(p, a, b, c)
#define IVSSEvents_BeginCommand(p, a, b, c) (p)->lpVtbl->BeginCommand(p, a, b, c)
#define IVSSEvents_EndCommand(p, a, b) (p)->lpVtbl->EndCommand(p, a, b)
#define IVSSEvents_BeforeEvent(p, a, b, c, d, e, f) (p)->lpVtbl->BeforeEvent(p, a, b, c, d, e, f)
#define IVSSEvents_AfterEvent(p, a, b, c, d, e) (p)->lpVtbl->AfterEvent(p, a, b, c, d, e)


interface IVSSEvents { CONST_VTBL struct IVSSEventsVtbl *lpVtbl; };

#ifndef IID_IVSSEventHandler_defined
#define IID_IVSSEventHandler_defined
const IID IID_IVSSEventHandler={0x783cd4ea,0x9d54,0x11cf,0xb8, 0xee, 0x0, 0x60, 0x8c, 0xc9, 0xa7, 0x1f};
#endif

typedef interface IVSSEventHandler IVSSEventHandler;

extern const IID IID_IVSSEventHandler;

#undef INTERFACE
#define INTERFACE IVSSEventHandler

DECLARE_INTERFACE_(INTERFACE, IUnknown)
{
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(THIS_ GUID*, void**);
	ULONG   (STDMETHODCALLTYPE *AddRef)(THIS);
	ULONG   (STDMETHODCALLTYPE *Release)(THIS);
	HRESULT (STDMETHODCALLTYPE *Init)(IVSSEventHandler *This, struct IVSS*, HRESULT*);
};

#define IVSSEventHandler_Init(p, a, b) (p)->lpVtbl->Init(p, a, b)


interface IVSSEventHandler { CONST_VTBL struct IVSSEventHandlerVtbl *lpVtbl; };


#endif // ssapi_h_

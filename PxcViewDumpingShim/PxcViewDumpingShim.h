// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PXCVIEWDUMPINGSHIM_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PXCVIEWDUMPINGSHIM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
/*#ifdef PXCVIEWDUMPINGSHIM_EXPORTS
#define PXCVIEWDUMPINGSHIM_API __declspec(dllexport)
#else
#define PXCVIEWDUMPINGSHIM_API __declspec(dllimport)
#endif*/

#define PXCVIEWDUMPINGSHIM_API

const int BLOCK_SIZE = 512 * 1024;
const WCHAR *MSGBOX_TITLE = L"PDF-XChange Viewer File Dumping Shim";

extern "C"
{
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_Init(PXVDocument* pDoc, LPCSTR Key, LPCSTR DevCode);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_Delete(PXVDocument Doc);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_SetCallBack(PXVDocument Doc, PXV36_CALLBACK_FUNC pProc, LPARAM UserData);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReadDocumentW(PXVDocument Doc, LPCWSTR pwFileName, DWORD Flags);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReadDocumentFromIStream(PXVDocument Doc, IStream* stream, DWORD Flags);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReadDocumentFromMemory(PXVDocument Doc, const BYTE* mem, UINT size, DWORD Flags);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_CheckPassword(PXVDocument Doc, BYTE* pPassword, DWORD PassLen);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_FinishReadDocument(PXVDocument Doc, DWORD Flags);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPermissions(PXVDocument Doc, DWORD* enclevel, DWORD* permFlags);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetDocumentInfoW(PXVDocument Doc, LPCSTR name, LPWSTR value, DWORD* valuebufLen);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPagesCount(PXVDocument Doc, DWORD* count);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPageDimensions(PXVDocument Doc, DWORD page_num, double* width, double* height);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPageRotation(PXVDocument Doc, DWORD page_num, LONG* angle);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_DrawPageToDC(PXVDocument Doc, DWORD page_num, HDC hDC, LPPXV_CommonRenderParameters pParams);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_DrawPageToDIBSection(PXVDocument Doc, DWORD page_num, LPPXV_CommonRenderParameters pParams,
		HDC hBaseDC, COLORREF backcolor, HBITMAP* pResDIBSection, HANDLE hSection, DWORD dwOffset);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_DrawPageToIStream(PXVDocument Doc, DWORD page_num, LPPXV_CommonRenderParameters pParams,
		COLORREF backcolor, LPPXV_DrawToImageParams pImageParams, IStream* pDest);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReleasePageCachedData(PXVDocument Doc, DWORD page_num, DWORD dwFlags);
	PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReleaseCachedData(PXVDocument Doc, DWORD dwFlags);
	PXCVIEWDUMPINGSHIM_API LONG PXV36_API Shim_PXCV_Err_FormatSeverity(HRESULT errorcode, LPSTR buf, LONG maxlen);
	PXCVIEWDUMPINGSHIM_API LONG PXV36_API Shim_PXCV_Err_FormatFacility(HRESULT errorcode, LPSTR buf, LONG maxlen);
	PXCVIEWDUMPINGSHIM_API LONG PXV36_API Shim_PXCV_Err_FormatErrorCode(HRESULT errorcode, LPSTR buf, LONG maxlen);
}

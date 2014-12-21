// PxcViewDumpingShim.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PxcViewDumpingShim.h"

bool ShowSaveDialog(LPWSTR pPath)
{
	bool result = false;
	pPath[0] = L'\0';
	if (IsWindowsVistaOrGreater())
	{
		COMDLG_FILTERSPEC filter[] = { { L"PDF Document", L"*.pdf" } };
		IFileSaveDialog *psfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&psfd));
		if (SUCCEEDED(hr))
		{
			psfd->SetTitle(L"Dump To");
			psfd->SetFileTypes(ARRAYSIZE(filter), filter);
			psfd->SetDefaultExtension(L"pdf");
			psfd->SetFileTypeIndex(1);
			hr = psfd->Show(NULL);
			if (SUCCEEDED(hr))
			{
				IShellItem *itm = NULL;
				hr = psfd->GetResult(&itm);
				if (SUCCEEDED(hr))
				{
					LPWSTR disp;
					hr = itm->GetDisplayName(SIGDN_FILESYSPATH, &disp);
					if (SUCCEEDED(hr))
					{
						wcscpy_s(pPath, MAX_PATH, disp);
						result = true;
						CoTaskMemFree(disp);
					}
					itm->Release();
				}
			}
			psfd->Release();
		}
	}
	else
	{
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrTitle = L"Dump To";
		ofn.lpstrFile = pPath;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"PDF Document\0*.pdf\0";
		ofn.nFilterIndex = 1;
		result = GetSaveFileName(&ofn);
	}
	return result;
}

PXCVIEWDUMPINGSHIM_API HRESULT PXV36_API Shim_PXCV_Init(PXVDocument* pDoc, LPCSTR Key, LPCSTR DevCode)
{
	return PXCV_Init(pDoc, Key, DevCode);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_Delete(PXVDocument Doc)
{
	return PXCV_Delete(Doc);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_SetCallBack(PXVDocument Doc, PXV36_CALLBACK_FUNC pProc, LPARAM UserData)
{
	return PXCV_SetCallBack(Doc, pProc, UserData);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReadDocumentW(PXVDocument Doc, LPCWSTR pwFileName, DWORD Flags)
{
	return PXCV_ReadDocumentW(Doc, pwFileName, Flags);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReadDocumentFromIStream(PXVDocument Doc, IStream* stream, DWORD Flags)
{
	WCHAR pathBuf[MAX_PATH] = { L'\0' };
	if (ShowSaveDialog(pathBuf))
	{
		HANDLE hFile = CreateFile(pathBuf, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			ULARGE_INTEGER origPos;
			LARGE_INTEGER zeroPos = { 0ll };
			STATSTG strmStat;
			stream->Stat(&strmStat, STATFLAG_NONAME);
			stream->Seek(zeroPos, STREAM_SEEK_CUR, &origPos);
			stream->Seek(zeroPos, STREAM_SEEK_SET, NULL);

			bool failureFlag = false;
			BYTE buf[BLOCK_SIZE];
			unsigned long remainingBytes = strmStat.cbSize.QuadPart % BLOCK_SIZE;
			for (unsigned long long i = 0; i < strmStat.cbSize.QuadPart - remainingBytes && !failureFlag; i += BLOCK_SIZE)
			{
				ULONG nRead;
				stream->Read(buf, BLOCK_SIZE, &nRead);
				if (nRead == BLOCK_SIZE)
				{
					DWORD nWritten;
					WriteFile(hFile, buf, BLOCK_SIZE, &nWritten, NULL);
					if (nWritten != BLOCK_SIZE) failureFlag = true;
				}
				else
				{
					failureFlag = true;
				}
			}

			if (!failureFlag)
			{
				if (remainingBytes != 0)
				{
					ULONG nRead;
					stream->Read(buf, remainingBytes, &nRead);
					if (nRead == remainingBytes)
					{
						DWORD nWritten;
						WriteFile(hFile, buf, remainingBytes, &nWritten, NULL);
						if (nWritten != remainingBytes) failureFlag = true;
					}
					else
					{
						failureFlag = true;
					}
				}
			}

			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = NULL;

			if (failureFlag)
			{
				MessageBox(NULL, L"Error reading or writing file.", MSGBOX_TITLE, MB_ICONERROR | MB_OK);
			}
			else
			{
				MessageBox(NULL, L"File dumped.", MSGBOX_TITLE, MB_ICONINFORMATION | MB_OK);
			}

			LARGE_INTEGER origPosInt;
			origPosInt.QuadPart = origPos.QuadPart;
			stream->Seek(origPosInt, STREAM_SEEK_SET, NULL);
		}
		else
		{
			MessageBox(NULL, L"Couldn't open file.", MSGBOX_TITLE, MB_ICONERROR | MB_OK);
		}
	}

	return PXCV_ReadDocumentFromIStream(Doc, stream, Flags);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReadDocumentFromMemory(PXVDocument Doc, const BYTE* mem, UINT size, DWORD Flags)
{
	WCHAR pathBuf[MAX_PATH] = { L'\0' };
	if (ShowSaveDialog(pathBuf))
	{
		HANDLE hFile = CreateFile(pathBuf, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			bool failureFlag = false;
			unsigned long remainingBytes = size % BLOCK_SIZE;
			for (unsigned long long i = 0; i < size - remainingBytes && !failureFlag; i += BLOCK_SIZE)
			{
				DWORD nWritten;
				WriteFile(hFile, mem + i, BLOCK_SIZE, &nWritten, NULL);
				if (nWritten != BLOCK_SIZE) failureFlag = true;
			}

			if (!failureFlag)
			{
				if (remainingBytes != 0)
				{
					DWORD nWritten;
					WriteFile(hFile, mem + size - remainingBytes, remainingBytes, &nWritten, NULL);
					if (nWritten != remainingBytes) failureFlag = true;
				}
			}

			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = NULL;

			if (failureFlag)
			{
				MessageBox(NULL, L"Error writing file.", MSGBOX_TITLE, MB_ICONERROR | MB_OK);
			}
			else
			{
				MessageBox(NULL, L"File dumped.", MSGBOX_TITLE, MB_ICONINFORMATION | MB_OK);
			}
		}
		else
		{
			MessageBox(NULL, L"Couldn't open file.", MSGBOX_TITLE, MB_ICONERROR | MB_OK);
		}
	}

	return PXCV_ReadDocumentFromMemory(Doc, mem, size, Flags);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_CheckPassword(PXVDocument Doc, BYTE* pPassword, DWORD PassLen)
{
	return PXCV_CheckPassword(Doc, pPassword, PassLen);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_FinishReadDocument(PXVDocument Doc, DWORD Flags)
{
	return PXCV_FinishReadDocument(Doc, Flags);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPermissions(PXVDocument Doc, DWORD* enclevel, DWORD* permFlags)
{
	return PXCV_GetPermissions(Doc, enclevel, permFlags);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetDocumentInfoW(PXVDocument Doc, LPCSTR name, LPWSTR value, DWORD* valuebufLen)
{
	return PXCV_GetDocumentInfoW(Doc, name, value, valuebufLen);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPagesCount(PXVDocument Doc, DWORD* count)
{
	return PXCV_GetPagesCount(Doc, count);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPageDimensions(PXVDocument Doc, DWORD page_num, double* width, double* height)
{
	return PXCV_GetPageDimensions(Doc, page_num, width, height);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_GetPageRotation(PXVDocument Doc, DWORD page_num, LONG* angle)
{
	return PXCV_GetPageRotation(Doc, page_num, angle);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_DrawPageToDC(PXVDocument Doc, DWORD page_num, HDC hDC, LPPXV_CommonRenderParameters pParams)
{
	return PXCV_DrawPageToDC(Doc, page_num, hDC, pParams);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_DrawPageToDIBSection(PXVDocument Doc, DWORD page_num, LPPXV_CommonRenderParameters pParams,
	HDC hBaseDC, COLORREF backcolor, HBITMAP* pResDIBSection, HANDLE hSection, DWORD dwOffset)
{
	return PXCV_DrawPageToDIBSection(Doc, page_num, pParams, hBaseDC, backcolor, pResDIBSection, hSection, dwOffset);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_DrawPageToIStream(PXVDocument Doc, DWORD page_num, LPPXV_CommonRenderParameters pParams,
	COLORREF backcolor, LPPXV_DrawToImageParams pImageParams, IStream* pDest)
{
	return PXCV_DrawPageToIStream(Doc, page_num, pParams, backcolor, pImageParams, pDest);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReleasePageCachedData(PXVDocument Doc, DWORD page_num, DWORD dwFlags)
{
	return PXCV_ReleasePageCachedData(Doc, page_num, dwFlags);
}

PXCVIEWDUMPINGSHIM_API	HRESULT PXV36_API Shim_PXCV_ReleaseCachedData(PXVDocument Doc, DWORD dwFlags)
{
	return PXCV_ReleaseCachedData(Doc, dwFlags);
}

PXCVIEWDUMPINGSHIM_API LONG PXV36_API Shim_PXCV_Err_FormatSeverity(HRESULT errorcode, LPSTR buf, LONG maxlen)
{
	return PXCV_Err_FormatSeverity(errorcode, buf, maxlen);
}

PXCVIEWDUMPINGSHIM_API LONG PXV36_API Shim_PXCV_Err_FormatFacility(HRESULT errorcode, LPSTR buf, LONG maxlen)
{
	return PXCV_Err_FormatFacility(errorcode, buf, maxlen);
}

PXCVIEWDUMPINGSHIM_API LONG PXV36_API Shim_PXCV_Err_FormatErrorCode(HRESULT errorcode, LPSTR buf, LONG maxlen)
{
	return PXCV_Err_FormatErrorCode(errorcode, buf, maxlen);
}

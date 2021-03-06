/*

Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2009 Miranda ICQ/IM project, 
all portions of this codebase are copyrighted to the people 
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "commonheaders.h"
#include <shlobj.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file.h"

static void SetControlToUnixTime(HWND hwndDlg, UINT idCtrl, time_t unixTime)
{
	LARGE_INTEGER liFiletime;
	FILETIME filetime;
	SYSTEMTIME st;
	char szTime[64],szDate[64],szOutput[128];

	liFiletime.QuadPart=(BIGI(11644473600)+(__int64)unixTime)*10000000;
	filetime.dwHighDateTime=liFiletime.HighPart;
	filetime.dwLowDateTime=liFiletime.LowPart;
	FileTimeToSystemTime(&filetime,&st);
	GetTimeFormatA(LOCALE_USER_DEFAULT,0,&st,NULL,szTime,SIZEOF(szTime));
	GetDateFormatA(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,szDate,SIZEOF(szDate));
	mir_snprintf(szOutput, SIZEOF(szOutput), "%s %s",szDate,szTime);
	SetDlgItemTextA(hwndDlg,idCtrl,szOutput);
}

#define C_CONTEXTMENU  0
#define C_PROPERTIES   1
// not defined in VC++ 6.0 SE
#ifndef CMF_EXTENDEDVERBS
#define CMF_EXTENDEDVERBS 0x00000100
#endif
static void DoAnnoyingShellCommand(HWND hwnd,const TCHAR *szFilename,int cmd,POINT *ptCursor)
{
	IShellFolder *pDesktopFolder;
	if(SHGetDesktopFolder(&pDesktopFolder)==NOERROR) {
		ITEMIDLIST *pCurrentIdl;
		#if defined( _UNICODE )
			WCHAR* wszFilename = ( LPWSTR )szFilename;
		#else
			WCHAR wszFilename[MAX_PATH];
			MultiByteToWideChar(CP_ACP,0,szFilename,-1,wszFilename,SIZEOF(wszFilename));
		#endif
		if(pDesktopFolder->ParseDisplayName(NULL,NULL,wszFilename,NULL,&pCurrentIdl,NULL)==NOERROR) {
			if(pCurrentIdl->mkid.cb) {
				ITEMIDLIST *pidl,*pidlNext,*pidlFilename;
				IShellFolder *pFileFolder;

				for(pidl=pCurrentIdl;;) {
					pidlNext=(ITEMIDLIST*)((PBYTE)pidl+pidl->mkid.cb);
					if(pidlNext->mkid.cb==0) {
						pidlFilename = (ITEMIDLIST*)CoTaskMemAlloc(pidl->mkid.cb+sizeof(pidl->mkid.cb));
						CopyMemory(pidlFilename,pidl,pidl->mkid.cb+sizeof(pidl->mkid.cb));
						pidl->mkid.cb=0;
						break;
					}
					pidl=pidlNext;
				}
				if(pDesktopFolder->BindToObject(pCurrentIdl,NULL,IID_IShellFolder,(void**)&pFileFolder)==NOERROR) {
					IContextMenu *pContextMenu;
					if(pFileFolder->GetUIObjectOf(NULL,1,(LPCITEMIDLIST*)&pidlFilename,IID_IContextMenu,NULL,(void**)&pContextMenu)==NOERROR) {
						switch(cmd) {
							case C_PROPERTIES:
							{	CMINVOKECOMMANDINFO ici={0};
								ici.cbSize=sizeof(ici);
								ici.hwnd=hwnd;
								ici.lpVerb="properties";
								ici.nShow=SW_SHOW;
								pContextMenu->InvokeCommand(&ici);
								break;
							}
							case C_CONTEXTMENU:
							{	HMENU hMenu;
								hMenu=CreatePopupMenu();
								if(SUCCEEDED(pContextMenu->QueryContextMenu(hMenu,0,1000,65535,(GetKeyState(VK_SHIFT)&0x8000?CMF_EXTENDEDVERBS:0)|CMF_NORMAL))) {
									int cmd;
									cmd=TrackPopupMenu(hMenu,TPM_RETURNCMD,ptCursor->x,ptCursor->y,0,hwnd,NULL);
									if(cmd) {
										CMINVOKECOMMANDINFO ici={0};
										ici.cbSize=sizeof(ici);
										ici.hwnd=hwnd;
										ici.lpVerb=MAKEINTRESOURCEA(cmd-1000);
										ici.nShow=SW_SHOW;
										pContextMenu->InvokeCommand(&ici);
									}
								}
								DestroyMenu(hMenu);
								break;
							}
						}
						pContextMenu->Release();
					}
					pFileFolder->Release();
				}
				CoTaskMemFree(pidlFilename);
			}
			CoTaskMemFree(pCurrentIdl);
		}
		pDesktopFolder->Release();
	}
}

static WNDPROC pfnIconWindowProc;
static LRESULT CALLBACK IconCtrlSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PROTOFILETRANSFERSTATUS* pft = (PROTOFILETRANSFERSTATUS*)GetWindowLongPtr(GetParent(hwnd),GWLP_USERDATA);

	switch(msg) {
		case WM_LBUTTONDBLCLK:
			ShellExecute(hwnd,NULL,pft->tszCurrentFile,NULL,NULL,SW_SHOW);
			break;
		case WM_RBUTTONUP:
		{	POINT pt;
			pt.x=(short)LOWORD(lParam); pt.y=(short)HIWORD(lParam);
			ClientToScreen( hwnd, &pt );
			DoAnnoyingShellCommand( hwnd, pft->tszCurrentFile, C_CONTEXTMENU, &pt );
			return 0;
		}
	}
	return CallWindowProc(pfnIconWindowProc,hwnd,msg,wParam,lParam);
}

struct loadiconsstartinfo {
	HWND hwndDlg;
	TCHAR *szFilename;
};
void __cdecl LoadIconsAndTypesThread(void* param)
{
	loadiconsstartinfo *info = ( loadiconsstartinfo* )param;
	SHFILEINFO fileInfo;

	if ( SHGetFileInfo( info->szFilename, 0, &fileInfo, sizeof(fileInfo),SHGFI_TYPENAME|SHGFI_ICON|SHGFI_LARGEICON)) {
		TCHAR *pszExtension,*pszFilename;
		TCHAR szExtension[64];
		TCHAR szIconFile[MAX_PATH];

		pszFilename = _tcsrchr(info->szFilename,'\\');
		if ( pszFilename == NULL )
			pszFilename = info->szFilename;

		pszExtension = _tcsrchr( pszFilename, '.' );
		if ( pszExtension )
			lstrcpyn( szExtension, pszExtension+1, SIZEOF( szExtension ));
		else {
			pszExtension = _T(".");
			szExtension[0]='\0';
		}
		CharUpper(szExtension);
		if ( fileInfo.szTypeName[0]=='\0' )
			mir_sntprintf( fileInfo.szTypeName, SIZEOF(fileInfo.szTypeName), TranslateT("%s File"),szExtension);
		SetDlgItemText(info->hwndDlg,IDC_EXISTINGTYPE,fileInfo.szTypeName);
		SetDlgItemText(info->hwndDlg,IDC_NEWTYPE,fileInfo.szTypeName);
		SendDlgItemMessage(info->hwndDlg,IDC_EXISTINGICON,STM_SETICON,(WPARAM)fileInfo.hIcon,0);
		szIconFile[0]='\0';
		if ( !lstrcmp( szExtension, _T("EXE"))) {
			SRFile_GetRegValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons"),_T("2"),szIconFile,SIZEOF(szIconFile));
		}
		else {
			TCHAR szTypeName[MAX_PATH];
			if(SRFile_GetRegValue(HKEY_CLASSES_ROOT,pszExtension,NULL,szTypeName,SIZEOF(szTypeName))) {
				lstrcat(szTypeName,_T("\\DefaultIcon"));
				if(SRFile_GetRegValue(HKEY_CLASSES_ROOT,szTypeName,NULL,szIconFile,SIZEOF(szIconFile))) {
					if ( _tcsstr( szIconFile, _T("%1")))
						SRFile_GetRegValue(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons"),_T("0"),szIconFile,SIZEOF(szIconFile));
					else szIconFile[0]='\0';
		}	}	}

		if ( szIconFile[0]) {
			int iconIndex;
			HICON hIcon;
			TCHAR *pszComma = _tcsrchr(szIconFile,',');
			if ( pszComma == NULL )
				iconIndex=0;
			else {
				iconIndex = _ttoi(pszComma+1); *pszComma='\0';
			}
			hIcon = ExtractIcon( hMirandaInst, szIconFile, iconIndex );
			if ( hIcon )
				fileInfo.hIcon = hIcon;
		}
		SendDlgItemMessage(info->hwndDlg,IDC_NEWICON,STM_SETICON,(WPARAM)fileInfo.hIcon,0);
	}
	mir_free(info->szFilename);
	mir_free(info);
}

INT_PTR CALLBACK DlgProcFileExists(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PROTOFILETRANSFERSTATUS *fts;

	fts=(PROTOFILETRANSFERSTATUS*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
	switch(msg) {
	case WM_INITDIALOG:
	{
		TCHAR szSize[64];
		struct _stati64 statbuf;
		HWND hwndFocus;
		struct TDlgProcFileExistsParam *dat = (struct TDlgProcFileExistsParam *)lParam;

		SetPropA(hwndDlg,"Miranda.Preshutdown",HookEventMessage(ME_SYSTEM_PRESHUTDOWN,hwndDlg,M_PRESHUTDOWN));
		SetPropA(hwndDlg,"Miranda.ParentWnd",dat->hwndParent);

		TranslateDialogDefault(hwndDlg);
		fts=(PROTOFILETRANSFERSTATUS*)mir_alloc(sizeof(PROTOFILETRANSFERSTATUS));
		CopyProtoFileTransferStatus(fts,dat->fts);
		SetWindowLongPtr(hwndDlg,GWLP_USERDATA,(LONG_PTR)fts);
		SetDlgItemText(hwndDlg,IDC_FILENAME,fts->tszCurrentFile);
		SetControlToUnixTime(hwndDlg,IDC_NEWDATE,fts->currentFileTime);
		GetSensiblyFormattedSize(fts->currentFileSize,szSize,SIZEOF(szSize),0,1,NULL);
		SetDlgItemText(hwndDlg,IDC_NEWSIZE,szSize);

		pfnIconWindowProc=(WNDPROC)SetWindowLongPtr(GetDlgItem(hwndDlg,IDC_EXISTINGICON),GWLP_WNDPROC,(LONG_PTR)IconCtrlSubclassProc);

		hwndFocus=GetDlgItem(hwndDlg,IDC_RESUME);
		if ( _tstati64(fts->tszCurrentFile,&statbuf)==0) {
			SetControlToUnixTime(hwndDlg,IDC_EXISTINGDATE,statbuf.st_mtime);
			GetSensiblyFormattedSize(statbuf.st_size,szSize,SIZEOF(szSize),0,1,NULL);
			SetDlgItemText(hwndDlg,IDC_EXISTINGSIZE,szSize);
			if(statbuf.st_size>(int)fts->currentFileSize) {
				EnableWindow(GetDlgItem(hwndDlg,IDC_RESUME),FALSE);
				hwndFocus=GetDlgItem(hwndDlg,IDC_OVERWRITE);
		}	}

		loadiconsstartinfo *lisi = ( loadiconsstartinfo* )mir_alloc(sizeof(loadiconsstartinfo));
		lisi->hwndDlg=hwndDlg;
		lisi->szFilename = mir_tstrdup(fts->tszCurrentFile);
		//can be a little slow, so why not?
		forkthread(LoadIconsAndTypesThread,0,lisi);			
		SetFocus(hwndFocus);
		SetWindowLongPtr(hwndFocus,GWL_STYLE,GetWindowLongPtr(hwndFocus,GWL_STYLE)|BS_DEFPUSHBUTTON);
		return FALSE;
	}
	case WM_COMMAND:
	{	
		PROTOFILERESUME pfr={0};
		switch(LOWORD(wParam)) {
		case IDC_OPENFILE:
			ShellExecute( hwndDlg, NULL, fts->tszCurrentFile, NULL, NULL, SW_SHOW );
			return FALSE;

		case IDC_OPENFOLDER:
		{	
			TCHAR szFile[MAX_PATH];
			lstrcpyn( szFile, fts->tszCurrentFile, SIZEOF(szFile));
			TCHAR* pszLastBackslash = _tcsrchr( szFile, '\\' );
			if ( pszLastBackslash )
				*pszLastBackslash = '\0';
			ShellExecute(hwndDlg,NULL,szFile,NULL,NULL,SW_SHOW);
			return FALSE;
		}
		case IDC_PROPERTIES:
			DoAnnoyingShellCommand(hwndDlg,fts->tszCurrentFile,C_PROPERTIES,NULL);
			return FALSE;
		case IDC_RESUME:
			pfr.action=FILERESUME_RESUME;
			break;
		case IDC_RESUMEALL:
			pfr.action=FILERESUME_RESUMEALL;
			break;
		case IDC_OVERWRITE:
			pfr.action=FILERESUME_OVERWRITE;
			break;
		case IDC_OVERWRITEALL:
			pfr.action=FILERESUME_OVERWRITEALL;
			break;

		case IDC_AUTORENAME:
			pfr.action = FILERESUME_RENAMEALL;
			break;

		case IDC_SAVEAS:
		{	
			OPENFILENAME ofn={0};
			TCHAR filter[512],*pfilter;
			TCHAR str[MAX_PATH];

			lstrcpyn( str, fts->tszCurrentFile, SIZEOF(str));
			ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
			ofn.hwndOwner = hwndDlg;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
			_tcscpy( filter, TranslateT("All Files"));
			_tcscat( filter, _T(" (*)"));
			pfilter = filter + _tcslen(filter) + 1;
			_tcscpy( pfilter, _T("*"));
			pfilter = pfilter + _tcslen(pfilter) + 1;
			*pfilter='\0';
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = str;
			ofn.nMaxFile = SIZEOF(str);
			ofn.nMaxFileTitle = MAX_PATH;
			if(!GetSaveFileName(&ofn))
				return FALSE;

			pfr.szFilename = mir_tstrdup(str);
			pfr.action = FILERESUME_RENAME;
			break;
		}
		case IDC_SKIP:
			pfr.action=FILERESUME_SKIP;
			break;
		case IDCANCEL:
			pfr.action=FILERESUME_CANCEL;
			break;
		default:
			return FALSE;
		}
		{	PROTOFILERESUME *pfrCopy;
			pfrCopy=(PROTOFILERESUME*)mir_alloc(sizeof(pfr));
			CopyMemory(pfrCopy,&pfr,sizeof(pfr));
			PostMessage((HWND)GetPropA(hwndDlg,"Miranda.ParentWnd"),M_FILEEXISTSDLGREPLY,(WPARAM)mir_tstrdup(fts->tszCurrentFile),(LPARAM)pfrCopy);
			DestroyWindow(hwndDlg);
		}
		break;
	}

	case WM_CLOSE:
		PostMessage(hwndDlg,WM_COMMAND,MAKEWPARAM(IDCANCEL,BN_CLICKED),(LPARAM)GetDlgItem(hwndDlg,IDCANCEL));
		break;

	case M_PRESHUTDOWN:
		PostMessage(hwndDlg,WM_CLOSE,0,0);
		break;

	case WM_DESTROY:
		UnhookEvent(GetPropA(hwndDlg,"Miranda.Preshutdown")); // GetProp() will return NULL if it couldnt find anything
		RemovePropA(hwndDlg,"Miranda.Preshutdown");
		RemovePropA(hwndDlg,"Miranda.ParentWnd");
		DestroyIcon((HICON)SendDlgItemMessage(hwndDlg,IDC_EXISTINGICON,STM_GETICON,0,0));
		DestroyIcon((HICON)SendDlgItemMessage(hwndDlg,IDC_NEWICON,STM_GETICON,0,0));
		FreeProtoFileTransferStatus(fts);
		mir_free(fts);
		break;
	}
	return FALSE;
}

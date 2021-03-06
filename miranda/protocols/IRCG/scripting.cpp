/*
IRC plugin for Miranda IM

Copyright (C) 2003-05 Jurgen Persson
Copyright (C) 2007-09 George Hazan

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

#include "irc.h"

INT_PTR __cdecl CIrcProto::Scripting_InsertRawIn(WPARAM, LPARAM lParam)
{
	char* pszRaw = ( char* ) lParam;

	if ( m_bMbotInstalled && m_scriptingEnabled && pszRaw && IsConnected() ) {
		TCHAR* p = mir_a2t( pszRaw );
		InsertIncomingEvent( p );
		mir_free( p );
		return 0;
	}

	return 1;
}
 
INT_PTR __cdecl CIrcProto::Scripting_InsertRawOut( WPARAM, LPARAM lParam )
{
	char* pszRaw = ( char* ) lParam;
	if ( m_bMbotInstalled && m_scriptingEnabled && pszRaw && IsConnected() ) {	
		String S = pszRaw;
		ReplaceString( S, "%", "%%%%");
		NLSendNoScript((const unsigned char *)S.c_str(), lstrlenA(S.c_str()));
		return 0;
	}

	return 1;
}

INT_PTR __cdecl CIrcProto::Scripting_InsertGuiIn(WPARAM wParam,LPARAM lParam)
{
	GCEVENT* gce = (GCEVENT *) lParam;
	WPARAM_GUI_IN * wgi = (WPARAM_GUI_IN *) wParam;


	if ( m_bMbotInstalled && m_scriptingEnabled && gce ) {	
		TCHAR* p1 = NULL;
		CMString S;
		if ( gce->pDest && gce->pDest->ptszID ) {
			p1 = gce->pDest->ptszID;
			S = MakeWndID(gce->pDest->ptszID);
			gce->pDest->ptszID = ( TCHAR* )S.c_str();
		}
		gce->cbSize = sizeof(GCEVENT);

		CallServiceSync( MS_GC_EVENT, wgi?wgi->wParam:0, (LPARAM)gce);

		if ( p1 )
			gce->pDest->ptszID = p1;
		return 0;
	}

	return 1;
}

//helper functions
static void __stdcall OnHook(void * pi)
{
	GCHOOK* gch = ( GCHOOK* )pi;

	//Service_GCEventHook(1, (LPARAM) gch);

	if(gch->pszUID)
		free(gch->pszUID);
	if(gch->pszText)
		free(gch->pszText);
	if(gch->pDest->ptszID)
		free(gch->pDest->ptszID);
	if(gch->pDest->pszModule)
		free(gch->pDest->pszModule);
	delete gch->pDest;
	delete gch;
}

static void __cdecl GuiOutThread(LPVOID di)
{	
	GCHOOK* gch = ( GCHOOK* )di;
	CallFunctionAsync( OnHook, ( void* )gch );
}

INT_PTR __cdecl CIrcProto::Scripting_InsertGuiOut( WPARAM, LPARAM lParam )
{
	GCHOOK* gch = ( GCHOOK* )lParam;

	if ( m_bMbotInstalled && m_scriptingEnabled && gch ) {	
		GCHOOK* gchook = new GCHOOK;
		gchook->pDest = new GCDEST;

		gchook->dwData = gch->dwData;
		gchook->pDest->iType = gch->pDest->iType;
		if ( gch->ptszText )
			gchook->ptszText = _tcsdup( gch->ptszText );
		else gchook->pszText = NULL;

		if ( gch->ptszUID )
			gchook->ptszUID = _tcsdup( gch->ptszUID );
		else gchook->pszUID = NULL;

		if ( gch->pDest->ptszID ) {
			CMString S = MakeWndID( gch->pDest->ptszID );
			gchook->pDest->ptszID = _tcsdup( S.c_str());
		}
		else gchook->pDest->ptszID = NULL;

		if ( gch->pDest->pszModule )
			gchook->pDest->pszModule = _strdup(gch->pDest->pszModule);
		else gchook->pDest->pszModule = NULL;

		mir_forkthread( GuiOutThread, gchook );
		return 0;
	}

	return 1;
}

BOOL CIrcProto::Scripting_TriggerMSPRawIn( char** pszRaw )
{
	int iVal = CallService( MS_MBOT_IRC_RAW_IN, (WPARAM)m_szModuleName, (LPARAM)pszRaw);
	if ( iVal == 0 )
		return TRUE;

	return iVal > 0 ? FALSE : TRUE;
}

BOOL CIrcProto::Scripting_TriggerMSPRawOut(char ** pszRaw)
{
	int iVal =  CallService( MS_MBOT_IRC_RAW_OUT, (WPARAM)m_szModuleName, (LPARAM)pszRaw);
	if ( iVal == 0 )
		return TRUE;

	return iVal > 0 ? FALSE : TRUE;
}

BOOL CIrcProto::Scripting_TriggerMSPGuiIn(WPARAM * wparam, GCEVENT * gce)
{
	WPARAM_GUI_IN wgi = {0};

	wgi.pszModule = m_szModuleName;
	wgi.wParam = *wparam;
	if (gce->time == 0)
		gce->time = time(0);

	int iVal =  CallService( MS_MBOT_IRC_GUI_IN, (WPARAM)&wgi, (LPARAM)gce);
	if ( iVal == 0 ) {
		*wparam = wgi.wParam;
		return TRUE;
	}

	return iVal > 0 ? FALSE : TRUE;
}

BOOL CIrcProto::Scripting_TriggerMSPGuiOut(GCHOOK* gch)
{
	int iVal =  CallService( MS_MBOT_IRC_GUI_OUT, (WPARAM)m_szModuleName, (LPARAM)gch);
	if ( iVal == 0 )
		return TRUE;

	return iVal > 0 ? FALSE : TRUE;
}

INT_PTR __cdecl CIrcProto::Scripting_GetIrcData(WPARAM, LPARAM lparam)
{
	if ( m_bMbotInstalled && m_scriptingEnabled && lparam ) {
		String sString = ( char* ) lparam, sRequest;
		CMString sOutput, sChannel; 

		int i = sString.Find("|");
		if ( i != -1 ) {
			sRequest = sString.Mid(0, i);
			TCHAR* p = mir_a2t(( char* )sString.Mid(i+1, sString.GetLength()).c_str());
			sChannel = p;
			mir_free( p );
		}
		else sRequest = sString;

		sRequest.MakeLower();

		if (sRequest == "ownnick" && IsConnected())
			sOutput = m_info.sNick;

		else if (sRequest == "network" && IsConnected())
			sOutput = m_info.sNetwork;

		else if (sRequest == "primarynick")
			sOutput = m_nick;

		else if (sRequest == "secondarynick")
			sOutput = m_alternativeNick;

		else if (sRequest == "myip")
			return ( INT_PTR )mir_strdup( m_manualHost ? m_mySpecifiedHostIP : 
										( m_IPFromServer ) ? m_myHost : m_myLocalHost);

		else if (sRequest == "usercount" && !sChannel.IsEmpty()) {
			CMString S = MakeWndID(sChannel.c_str());
			GC_INFO gci = {0};
			gci.Flags = BYID|COUNT;
			gci.pszModule = m_szModuleName;
			gci.pszID = (TCHAR*)S.c_str();
			if ( !CallServiceSync( MS_GC_GETINFO, 0, (LPARAM)&gci )) {
				TCHAR szTemp[40];
				_sntprintf( szTemp, 35, _T("%u"), gci.iCount);
				sOutput = szTemp;
			}
		}
		else if (sRequest == "userlist" && !sChannel.IsEmpty()) {
			CMString S = MakeWndID(sChannel.c_str());
			GC_INFO gci = {0};
			gci.Flags = BYID|USERS;
			gci.pszModule = m_szModuleName;
			gci.pszID = ( TCHAR* )S.c_str();
			if ( !CallServiceSync( MS_GC_GETINFO, 0, (LPARAM)&gci ))
				return (INT_PTR)mir_strdup( gci.pszUsers );
		}
		else if (sRequest == "channellist") {
			CMString S = _T("");
			int i = CallServiceSync( MS_GC_GETSESSIONCOUNT, 0, (LPARAM)m_szModuleName);
			if ( i >= 0 ) {
				int j = 0;
				while (j < i) {
					GC_INFO gci = {0};
					gci.Flags = BYINDEX|ID;
					gci.pszModule = m_szModuleName;
					gci.iItem = j;
					if ( !CallServiceSync( MS_GC_GETINFO, 0, ( LPARAM )&gci )) {
						if ( lstrcmpi( gci.pszID, SERVERWINDOW)) {
							CMString S1 = gci.pszID;
							int k = S1.Find(_T(" "));
							if ( k != -1 )
								S1 = S1.Mid(0, k);
							S += S1 + _T(" ");
					}	}
					j++;
			}	}
			
			if ( !S.IsEmpty() )
				sOutput = ( TCHAR* )S.c_str();
		}
		// send it to mbot
		if ( !sOutput.IsEmpty())
			return ( INT_PTR )mir_t2a( sOutput.c_str() );
	}
	return 0;
}

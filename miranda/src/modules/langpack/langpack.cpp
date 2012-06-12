/*

Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2012 Miranda ICQ/IM project,
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

int LoadLangPackServices(void);

static BOOL bModuleInitialized = FALSE;

struct LangPackEntry {
	unsigned linePos;
	DWORD englishHash;
	char *english;	  //not currently used, the hash does everything
	char *local;
	wchar_t *wlocal;
};

struct LangPackStruct {
	TCHAR filename[MAX_PATH];
	char  language[64];
	char  lastModifiedUsing[64];
	char  authors[256];
	char  authorEmail[128];
	struct LangPackEntry *entry;
	int entryCount;
	LCID localeID;
	UINT defaultANSICp;
} static langPack;

static int IsEmpty(char *str)
{
	int i = 0;

	while (str[i])
	{
		if (str[i]!=' '&&str[i]!='\r'&&str[i]!='\n')
			return 0;
		i++;
	}
	return 1;
}

void ConvertBackslashes(char *str, UINT fileCp)
{
	char *pstr;
	for ( pstr = str; *pstr; pstr = CharNextExA( fileCp, pstr, 0 )) {
		if( *pstr == '\\' ) {
			switch( pstr[1] ) {
			case 'n': *pstr = '\n'; break;
			case 't': *pstr = '\t'; break;
			case 'r': *pstr = '\r'; break;
			default:  *pstr = pstr[1]; break;
			}
			memmove(pstr+1, pstr+2, strlen(pstr+2) + 1);
}	}	}

#ifdef _DEBUG
#pragma optimize( "gt", on )
#endif

// MurmurHash2 
unsigned int __fastcall hash(const void * key, unsigned int len)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	unsigned int h = len;

	// Mix 4 bytes at a time into the hash
	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array
	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
			h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

unsigned int __fastcall hashstrW(const char * key)
{
	if (key == NULL) return 0;
	const unsigned int len = (unsigned int)wcslen((const wchar_t*)key);
	char* buf = (char*)alloca(len + 1);
	for (unsigned i = 0; i <= len ; ++i)
		buf[i] = key[i << 1];
	return hash(buf, len);
}

#ifdef _DEBUG
#pragma optimize( "", on )
#endif

static int SortLangPackHashesProc(struct LangPackEntry *arg1,struct LangPackEntry *arg2)
{
	if(arg1->englishHash<arg2->englishHash) return -1;
	if(arg1->englishHash>arg2->englishHash) return 1;
	/* both source strings of the same hash (may not be the same string thou) put
	the one that was written first to be found first */
	if(arg1->linePos<arg2->linePos) return -1;
	if(arg1->linePos>arg2->linePos) return 1;
	return 0;
}


static int SortLangPackHashesProc2(struct LangPackEntry *arg1,struct LangPackEntry *arg2)
{
	if(arg1->englishHash<arg2->englishHash) return -1;
	if(arg1->englishHash>arg2->englishHash) return 1;
	return 0;
}

static int LoadLangPack(const TCHAR *szLangPack)
{
	FILE *fp;
	char line[4096] = "";
	char *pszColon;
	char *pszLine;
	int entriesAlloced;
	int startOfLine=0;
	unsigned int linePos=1;
	LCID langID;
	UINT fileCp = CP_ACP;

	lstrcpy(langPack.filename,szLangPack);
	fp = _tfopen(szLangPack,_T("rt"));
	if(fp==NULL) return 1;
	fgets(line,SIZEOF(line),fp);
	size_t lineLen = strlen(line);
	if (lineLen >= 3 && line[0]=='\xef' && line[1]=='\xbb' && line[2]=='\xbf')
	{
		fileCp = CP_UTF8;
		memmove(line, line + 3, lineLen - 2);
	}
	lrtrim(line);
	if(lstrcmpA(line,"Miranda Language Pack Version 1")) {fclose(fp); return 2;}
	//headers
	while(!feof(fp)) {
		startOfLine=ftell(fp);
		if(fgets(line,SIZEOF(line),fp)==NULL) break;
		lrtrim(line);
		if(IsEmpty(line) || line[0]==';' || line[0]==0) continue;
		if(line[0]=='[') break;
		pszColon=strchr(line,':');
		if(pszColon==NULL) {fclose(fp); return 3;}
		*pszColon++ = 0;
		if(!lstrcmpA(line,"Language")) {mir_snprintf(langPack.language,sizeof(langPack.language),"%s",pszColon); lrtrim(langPack.language);}
		else if(!lstrcmpA(line,"Last-Modified-Using")) {mir_snprintf(langPack.lastModifiedUsing,sizeof(langPack.lastModifiedUsing),"%s",pszColon); lrtrim(langPack.lastModifiedUsing);}
		else if(!lstrcmpA(line,"Authors")) {mir_snprintf(langPack.authors,sizeof(langPack.authors),"%s",pszColon); lrtrim(langPack.authors);}
		else if(!lstrcmpA(line,"Author-email")) {mir_snprintf(langPack.authorEmail,sizeof(langPack.authorEmail),"%s",pszColon); lrtrim(langPack.authorEmail);}
		else if(!lstrcmpA(line, "Locale")) {
			char szBuf[20], *stopped;

			lrtrim(pszColon + 1);
			langID = (USHORT)strtol(pszColon, &stopped, 16);
			langPack.localeID = MAKELCID(langID, 0);
			GetLocaleInfoA(langPack.localeID, LOCALE_IDEFAULTANSICODEPAGE, szBuf, 10);
			szBuf[5] = 0;                       // codepages have max. 5 digits
			langPack.defaultANSICp = atoi(szBuf);
			if (fileCp == CP_ACP)
				fileCp = langPack.defaultANSICp;
		}
	}

	//body
	fseek(fp,startOfLine,SEEK_SET);
	entriesAlloced=0;
	while(!feof(fp)) {
		if(fgets(line,SIZEOF(line),fp)==NULL) break;
		if(IsEmpty(line) || line[0]==';' || line[0]==0) continue;
		rtrim(line);
		ConvertBackslashes(line, fileCp);
		if(line[0]=='[' && line[lstrlenA(line)-1]==']') {
			if(langPack.entryCount && langPack.entry[langPack.entryCount-1].local==NULL) {
				if(langPack.entry[langPack.entryCount-1].english!=NULL) mir_free(langPack.entry[langPack.entryCount-1].english);
				langPack.entryCount--;
			}
			pszLine = line+1;
			line[lstrlenA(line)-1]='\0';
			if(++langPack.entryCount>entriesAlloced) {
				entriesAlloced+=128;
				langPack.entry=(struct LangPackEntry*)mir_realloc(langPack.entry,sizeof(struct LangPackEntry)*entriesAlloced);
			}
			langPack.entry[langPack.entryCount-1].english=NULL;
			langPack.entry[langPack.entryCount-1].englishHash=hashstr(pszLine);
			langPack.entry[langPack.entryCount-1].local=NULL;
			langPack.entry[langPack.entryCount-1].wlocal = NULL;
			langPack.entry[langPack.entryCount-1].linePos=linePos++;
		}
		else if(langPack.entryCount) {
			struct LangPackEntry* E = &langPack.entry[langPack.entryCount-1];

			if(E->local==NULL) {
				E->local=mir_strdup(line);
				if (fileCp == CP_UTF8)
					Utf8DecodeCP(E->local, langPack.defaultANSICp, NULL);

				{
					int iNeeded = MultiByteToWideChar(fileCp, 0, line, -1, 0, 0);
					E->wlocal = (wchar_t *)mir_alloc((iNeeded+1) * sizeof(wchar_t));
					MultiByteToWideChar(fileCp, 0, line, -1, E->wlocal, iNeeded);
				}
			}
			else {
				size_t iOldLenA = strlen(E->local);
				E->local = (char*)mir_realloc(E->local, iOldLenA + strlen(line) + 2);
				strcat(E->local, "\n");
				strcat(E->local, line);
				if (fileCp == CP_UTF8)
					Utf8DecodeCP(E->local + iOldLenA + 1, langPack.defaultANSICp, NULL);
				{
					int iNeeded = MultiByteToWideChar(fileCp, 0, line, -1, 0, 0);
					size_t iOldLen = wcslen(E->wlocal);
					E->wlocal = (wchar_t*)mir_realloc(E->wlocal, ( sizeof(wchar_t) * ( iOldLen + iNeeded + 2)));
					wcscat(E->wlocal, L"\n");
					MultiByteToWideChar(fileCp, 0, line, -1, E->wlocal + iOldLen+1, iNeeded);
				}
			}
		}
	}

	fclose(fp);

	qsort(langPack.entry,langPack.entryCount,sizeof(LangPackEntry),(int(*)(const void*,const void*))SortLangPackHashesProc);
	return 0;
}

char *LangPackTranslateString(const char *szEnglish, const int W)
{
	struct LangPackEntry key,*entry;

	if ( langPack.entryCount == 0 || szEnglish == NULL ) return (char*)szEnglish;

	key.englishHash = W ? hashstrW(szEnglish) : hashstr(szEnglish);
	entry=(struct LangPackEntry*)bsearch(&key,langPack.entry,langPack.entryCount,sizeof(struct LangPackEntry),(int(*)(const void*,const void*))SortLangPackHashesProc2);
	if(entry==NULL) return (char*)szEnglish;
	while(entry>langPack.entry)
	{
		entry--;
		if(entry->englishHash!=key.englishHash) {
			entry++;
			return W ? (char *)entry->wlocal : entry->local;
		}
	}
	return W ? (char *)entry->wlocal : entry->local;
}

int LangPackGetDefaultCodePage()
{
	return langPack.defaultANSICp;
}

int LangPackGetDefaultLocale()
{
	return (langPack.localeID == 0) ? LOCALE_USER_DEFAULT : langPack.localeID;
}

TCHAR* LangPackPcharToTchar( const char* pszStr )
{
	if ( pszStr == NULL )
		return NULL;

	#if defined( _UNICODE )
	{	int len = (int)strlen( pszStr );
		TCHAR* result = ( TCHAR* )alloca(( len+1 )*sizeof( TCHAR ));
		MultiByteToWideChar( LangPackGetDefaultCodePage(), 0, pszStr, -1, result, len );
		result[len] = 0;
		return mir_wstrdup( TranslateW( result ));
	}
	#else
		return mir_strdup( Translate( pszStr ));
	#endif
}

int LoadLangPackModule(void)
{
	HANDLE hFind;
	TCHAR szSearch[MAX_PATH];
	WIN32_FIND_DATA fd;

	bModuleInitialized = TRUE;

	ZeroMemory(&langPack,sizeof(langPack));
	LoadLangPackServices();
	pathToAbsoluteT(_T("langpack_*.txt"), szSearch, NULL); 
	hFind = FindFirstFile( szSearch, &fd );
	if( hFind != INVALID_HANDLE_VALUE ) {
		pathToAbsoluteT(fd.cFileName, szSearch, NULL); 
		FindClose(hFind);
		LoadLangPack(szSearch);
	}
	return 0;
}

void UnloadLangPackModule()
{
	int i;

	if ( !bModuleInitialized ) return;

	for ( i=0; i < langPack.entryCount; i++ ) {
		mir_free(langPack.entry[i].english);
		mir_free(langPack.entry[i].local);
		mir_free(langPack.entry[i].wlocal);
	}
	if ( langPack.entryCount ) {
		mir_free(langPack.entry);
		langPack.entry=0;
		langPack.entryCount=0;
}	}

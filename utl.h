
#include <Windows.h>
#include <CommDlg.h>

#include  "shlobj.h "

//void split(char *src, const char *separator, char **dest, int *num)
//{
//    char *pNext;
//    int count = 0; 
//    if (src == NULL || strlen(src) == 0) return;
//    if (separator == NULL || strlen(separator) == 0) return; 
//    pNext = strtok(src,separator);
//    while(pNext != NULL)
//    {
//        *dest++ = pNext;
//        ++count;
//        pNext = strtok(NULL,separator);
//    }
//    *num = count;
//}

BOOL GetShooseFile (HWND hWnd, PTSTR pstrFileName,char* szFilter,char* lpstrDefExt)
{
     OPENFILENAME ofn;

     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hWnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = lpstrDefExt ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;

     ofn.hwndOwner         = hWnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = NULL ;
     ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
     
     return GetOpenFileName (&ofn) ;
}
BOOL GetSaveFile (HWND hWnd, PTSTR pstrFileName,char* szFilter,char* lpstrDefExt)
{
     OPENFILENAME ofn;

     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hWnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = lpstrDefExt ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;

     ofn.hwndOwner         = hWnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = NULL ;
     ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
	
     return GetSaveFileNameA (&ofn) ;
}

bool GetShooseFoler(HWND hWnd,char* dest,char* szTitle)
{
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));

	bInfo.hwndOwner = hWnd;
	bInfo.lpszTitle = szTitle;
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS; 

	LPITEMIDLIST lpDlist; //用来保存返回信息的IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //显示选择对话框
	if(lpDlist != NULL) //用户按了确定按钮
	{		
		SHGetPathFromIDList(lpDlist, dest);//把项目标识列表转化成字符串	
		return TRUE;
	}
	return FALSE;
}


#define BLOCKLEN 8
int hex2bin(char* hexstr,unsigned char* outb)
{
	int len = strlen(hexstr);
	int i=0;
	for(;i<len;i+=2)
	{
		unsigned char v = 0;
		unsigned  char c1 = hexstr[i];
		unsigned  char c2 = hexstr[i+1];

		if( c1>='0' && c1 <='9')
		{
			v = (c1-'0')<<4;
		}else{
			v = ((c1-'a')+10)<<4;
		}
		
		if( c2>='0' && c2 <='9')
		{
			v += (c2-'0');
		}else{
			v += ((c2-'a')+10);
		}
		
		outb[0]=v;
		outb++;
	}

	return len/2;
}
void bin2hex(unsigned char* inb,int len,char* hexstr)
{
	int i=0;
	for(;i<len;i++)
	{
		unsigned  char v = inb[i];
		unsigned  char vh = (v&0xf0)>>4;
		unsigned  char vl = v&0x0f;

		if(vh>9)
		{
			hexstr[i*2] = vh-10+'a';
		}else{
			hexstr[i*2] = vh+'0';
		}
		if(vl>9)
		{
			hexstr[i*2+1] = vl-10+'a';
		}else{
			hexstr[i*2+1] = vl+'0';
		}
	}
	hexstr[i*2]=0;
}

void b2ws(unsigned char* bs,unsigned int* ws)
{
	int v = 0;
	v |= bs[0]<<24;
	v |= bs[1]<<16;
	v |= bs[2]<<8;
	v |= bs[3];
	ws[0] = v;
}
void w2bs(unsigned int ws,unsigned char* bs)
{
	bs[0] = (unsigned char)(ws>>24);
	bs[1] = (unsigned char)(ws>>16);
	bs[2] = (unsigned char)(ws>>8);
	bs[3] = (unsigned char)ws;
}
void iXor(unsigned char * deced, unsigned char  const* keyd)
{	
	int i=0;
	for(i=0;i<BLOCKLEN;i++)
	{
		deced[i] = deced[i]^keyd[i];
	}
}

void DecryptBlock(unsigned char  * ind, unsigned char * oud,unsigned char * kd)
{
	unsigned int delta=0x9e3779b9;
	unsigned int sum=0xC6EF3720, i=0;

	unsigned int v0 = 0;
	unsigned int v1 = 0;
	unsigned int tv1=0,tv2=0;
	
	b2ws(ind,&v0);
	b2ws(ind+4,&v1);
	
	//74726f716b64646d74726f716b63646d	

	for(i=0;i<32;i++)
	{		
		tv1 = (v0<<4) ^ (v0>>5);	
		tv2 = *(int*)(kd+ ((sum>>11)&3)*4);
				
		v1 = v1 - ( (sum + tv2) ^ ( tv1  + v0) );

		sum -= delta;			
		
		tv1 = (v1<<4) ^ (v1>>5);		
		tv2 = *(int*)(kd+ (sum&3)*4);	

		v0 = v0 - ( (sum + tv2) ^ ( tv1  + v1) );			
	}

	w2bs(v0,oud);	
	w2bs(v1,oud+4);
}
void EncryptBlock(unsigned char  * ind, unsigned char * oud,unsigned char * kd)
{
	unsigned int delta=0x9e3779b9;
	unsigned int sum=0, i=0;

	unsigned int v0 = 0;
	unsigned int v1 = 0;
	unsigned int tv1=0,tv2=0;
	
	b2ws(ind,&v0);
	b2ws(ind+4,&v1);	
	
	for(i=0;i<32;i++)
	{		
		tv1 = (v1<<4) ^ (v1>>5);	
		tv2 = *(int*)(kd+ (sum&3)*4);		

		v0 = v0 + ( (sum + tv2) ^ ( tv1  + v1) );

		sum += delta;			
		
		tv1 = (v0<<4) ^ (v0>>5);		
		tv2 = *(int*)(kd+ ((sum>>11)&3)*4);

		v1 = v1 + ( (sum + tv2) ^ ( tv1  + v0) );			
	}

	w2bs(v0,oud);	
	w2bs(v1,oud+4);
}
//74726f716b64646d74726f716b63646d
void CTEA_Decrypt(char* inputstr, char * outstr, char* keystr)
{
	unsigned char cachedat[64];

	unsigned char inputbytes[24];
	unsigned char outbytes[24];
	unsigned char keybytes[16];

	int iblock=0;
	int inputlen = strlen(inputstr)/2;

	hex2bin(inputstr,inputbytes);
	hex2bin(keystr,keybytes);

	unsigned char* pIn=inputbytes;
	unsigned char* pOu=outbytes;

	memset(cachedat,0x0,64);

	while(iblock*BLOCKLEN<inputlen)
	{
		//decblock
		DecryptBlock(pIn,pOu,keybytes);
		
		//xor
		iXor(pOu,cachedat);	
		pOu[BLOCKLEN]=0;

		memcpy(cachedat,inputbytes,BLOCKLEN);

		pIn += BLOCKLEN;
		pOu += BLOCKLEN;
		iblock++;
	}
		
	strcpy(outstr,(char*)outbytes);
}

void CTEA_Encrypt(char* plain,char* outstr,char* keystr)
{
	unsigned char cachedat[64];

	//unsigned char inputbytes[24];
	unsigned char outbytes[24];
	unsigned char keybytes[16];

	//hex2bin(inputstr,inputbytes);
	hex2bin(keystr,keybytes);

	int iblock=0;
	int inputlen = strlen(plain);

	unsigned char* pIn=(unsigned char*)plain;
	unsigned char* pOu=outbytes;

	memset(cachedat,0x0,64);

	while(iblock*BLOCKLEN<inputlen)
	{
		iXor(cachedat,pIn);

		EncryptBlock(cachedat,pOu,keybytes);

		memcpy(cachedat,pOu,BLOCKLEN);

		pIn += BLOCKLEN;
		pOu += BLOCKLEN;
		iblock++;
	}

	bin2hex(outbytes,inputlen,outstr);
}



void StrToLower(char* pstr)
{
	int i=0;
	if(NULL == pstr)
	{
		return;
	}

	while(pstr[i])
	{
		if(pstr[i]>='A' && pstr[i]<='Z')
		{
			pstr[i] += 32;			
		}

		i++;
	}

}
void split(char *src, const char *separator, char **dest, int *num)
{
    char *pNext;
    int count = 0; 
    if (src == NULL || strlen(src) == 0) return;
    if (separator == NULL || strlen(separator) == 0) return; 
    pNext = strtok(src,separator);
    while(pNext != NULL)
    {
        *dest++ = pNext;
        ++count;
        pNext = strtok(NULL,separator);
    }
    *num = count;
}
int BIG52GBK(char *instr,char** p_outstr_need_free)
{
	int r = MultiByteToWideChar(950,0,instr,strlen(instr),NULL,NULL);
	WCHAR* wsz = (WCHAR*)malloc((r+1)*2);
	MultiByteToWideChar(950,0,instr,strlen(instr),wsz,r+1);
	wsz[r]=0;

	r = WideCharToMultiByte(936,0,wsz,r,NULL,NULL,NULL,NULL);
	p_outstr_need_free[0]=(char*)malloc(r+1);
	WideCharToMultiByte(936,0,wsz,r,p_outstr_need_free[0],r+1,NULL,NULL);
	p_outstr_need_free[0][r]=0;

	char* psmpsz=(char*)malloc(r+1);
	r = LCMapStringA(0x0804,LCMAP_SIMPLIFIED_CHINESE, p_outstr_need_free[0], r, psmpsz, r+1);

	free(p_outstr_need_free[0]);

	p_outstr_need_free[0]=psmpsz;
	p_outstr_need_free[0][r]=0;
	
	free(wsz);
	
	return r;
}
int GBK2BIG5(char *instr,char** p_outstr_need_free)
{
	int r = strlen(instr);
	char* psmpsz=(char*)malloc(r+1);
	r = LCMapStringA(0x0804,LCMAP_TRADITIONAL_CHINESE, instr, r, psmpsz, r+1);
	psmpsz[r]=0;

	instr = psmpsz;		

	r = MultiByteToWideChar(936,0,instr,strlen(instr),NULL,NULL);
	WCHAR* wsz = (WCHAR*)malloc((r+1)*2);
	MultiByteToWideChar(936,0,instr,strlen(instr),wsz,r+1);
	wsz[r]=0;

	r = WideCharToMultiByte(950,0,wsz,r,NULL,NULL,NULL,NULL);
	p_outstr_need_free[0]=(char*)malloc(r+1);
	WideCharToMultiByte(950,0,wsz,r,p_outstr_need_free[0],r+1,NULL,NULL);
	p_outstr_need_free[0][r]=0;

	free(wsz);

	free(psmpsz);

	return r;
}
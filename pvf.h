#include "windows.h"

#include "stdlib.h"
#include "stdio.h"
#include "hashtable.h"

#include <vector>
using namespace std;

#define PVF_KEY 0x38638741

#define ENCRPT
#define DECRYPT

int is_use_encrypt=0;

unsigned int crc_table[0x100];
void init_crc_table()
{
	unsigned int var_4 = 1;
	unsigned int var_c = 0x80;
	memset(crc_table,0x0,0x100*4);
	while(var_c)
	{
		unsigned int t = var_4>>1;
		if( (var_4&1) == 0 )
		{
			var_4 = t ^ 0;
		}else{
			var_4 = t ^ 0xEDB88320;
		}

		unsigned int var_8 = 0;
		while(var_8<=0xff)	
		{
			crc_table[var_8 + var_c] = crc_table[var_8] ^ var_4;
			var_8 += var_c*2;
		}
		
		var_c = var_c>>1;
	}
}
bool decryptbuf(unsigned int arg_0,unsigned int crcv,unsigned int crcv2,char* buf,int len)
{
	unsigned int* var_18 = crc_table; 

	unsigned int* var_14 = (unsigned int*)buf;
	unsigned int* var_10 = (unsigned int*)( (len>>2) +var_14 );

	if(len & 0x3)
	{
		return -1;
	}
#ifdef ENCRPT
	if(is_use_encrypt)
	{
#ifdef DECRYPT
		int* l_buf = (int*)buf;
		int l_len = len;

		l_len = l_len >> 2;
		for(int i=0;i<l_len;i++)
		{
			l_buf[i]=l_buf[i]^PVF_KEY;//0x54648741 0x82A79013
		}
#endif
	}
#endif
	__asm
	{
		push eax
		mov eax,arg_0
		not eax
		mov arg_0,eax
		pop eax
	}

	while(var_14 != var_10)
	{
		var_14[0] = var_14[0] ^ crcv2;
		var_14[0] = var_14[0] ^ 0x81A79011;		

		unsigned int tv1 = var_14[0];
		unsigned int tv2 = 0;
		__asm
		{
			push eax
			mov eax,tv1
			ror eax,6
			mov tv2,eax
			pop eax
		}
		var_14[0] = tv2;

		unsigned char* var_c = (unsigned char*)var_14;

		unsigned int tv3 = 0;
		tv3 = (var_c[0] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);

		tv3 = (var_c[1] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);

		tv3 = (var_c[2] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);

		tv3 = (var_c[3] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);
	
		var_14++;
	}

	__asm
	{
		push eax
		mov eax,arg_0
		not eax
		mov arg_0,eax
		pop eax
	}

	if(crcv != arg_0)
	{
		return false;
	}

	return true;
}
/*
	// 0x5d49a6de 0x1c532d7e 4th
	// 0x8899eb4e 0xe00aae14 crc0 ?
	// 0x0004e6fd  0x0004e7d0 num
*/

unsigned int encryptbuf(unsigned int arg_0,char* buf,int len)
{
	unsigned int* var_18 = crc_table; 

	unsigned int* var_14 = (unsigned int*)buf;
	unsigned int* var_10 = (unsigned int*)( (len>>2) +var_14 );

	if(len & 0x3)
	{
		return -1;
	}

	__asm
	{
		push eax
		mov eax,arg_0
		not eax
		mov arg_0,eax
		pop eax
	}

	while(var_14 != var_10)
	{
		unsigned char* var_c = (unsigned char*)var_14;

		unsigned int tv3 = 0;
		tv3 = (var_c[0] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);

		tv3 = (var_c[1] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);

		tv3 = (var_c[2] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);

		tv3 = (var_c[3] ^ arg_0)&0xff;
		arg_0 = var_18[tv3] ^ (arg_0 >> 8);
	
		var_14++;
	}

	__asm
	{
		push eax
		mov eax,arg_0
		not eax
		mov arg_0,eax
		pop eax
	}

	var_14 = (unsigned int*)buf;
	while(var_14 != var_10)
	{
		unsigned int tv1 = var_14[0];
		unsigned int tv2 = 0;
		__asm
		{
			push eax
			mov eax,tv1
			rol eax,6
			mov tv2,eax
			pop eax
		}
		var_14[0] = tv2;

		var_14[0] = var_14[0] ^ 0x81A79011;		
		var_14[0] = var_14[0] ^ arg_0;
		
		var_14++;
	}
#ifdef ENCRPT	
	if(is_use_encrypt)
	{
		//二次XOR加密
		int* l_buf = (int*)buf;
		int l_len = len;

		l_len = l_len >> 2;
		for(int i=0;i<l_len;i++)
		{
			l_buf[i]=l_buf[i]^PVF_KEY;//0x54648741 0x82A79013
		}
	}
#endif
	return arg_0;
}

//
//void split(char *src, const char *separator, char **dest, int *num);
void StrToLower(char* pstr);
int GBK2BIG5(char *instr,char** p_outstr_need_free);
int BIG52GBK(char *instr,char** p_outstr_need_free);
//str,lst
typedef struct string_kor_str
{
	Hashtable* ph;
	char* buf;
	Hashtable* ph_ls;
};
string_kor_str equipment_kor_str_tab;
string_kor_str stackable_kor_str_tab;
string_kor_str npc_kor_str_tab;
string_kor_str itemshop_kor_str_tab;

string_kor_str equipment_lst_tab;
string_kor_str stackable_lst_tab;
string_kor_str npc_lst_tab;
string_kor_str itemshop_lst_tab;

void set_string_kor_str(string_kor_str* strhash,char* pos,unsigned int len)
{
	len = BIG52GBK(pos,&strhash->buf);//malloc in func for only display

	strhash->ph = new Hashtable(5*220000*10/7);

	pos = strhash->buf;
	char* end = pos + len;
	//

	while(pos < end-1)
	{
		char* pline = pos;
		char* pvalue = 0;
		while(pos < end-1 && (pos[0]!=0x0d || pos[1]!=0x0a))
		{
			if(pos[0]=='>')
			{
				pos[0]=0;
				pvalue = pos+1;
			}
			pos++;
		}
		pos[0]=0;
		pos[1]=0;

		//line,pvalue
		if(pvalue)
		{
			strhash->ph->AddPair(pline,(int)pvalue);
		}

		pos+=2;
	}	
}
int string_kor_str_get(string_kor_str* strhash,char* dest,char* name_str)
{	
	int vlen=0;

	if(hash_pair* ir = strhash->ph->Find(name_str))
	{
		dest[0]='>';
		dest++;
		vlen = strlen((char*)ir->value);
		memcpy(dest,(char*)ir->value,vlen);

		return vlen+1;
	}	
	return 0;
}

//stringtable.bin
typedef struct stringtable
{		
	int addrcnt;
	int* addrlist;
	char* strdatbuf;	
	int slen;//hole table len=indexlen+stringlen--->stringlen
	char* elebuf;//

	Hashtable* ph;
	char* buf;//for hash
	int bufeof;
	
	int updated;
};
stringtable stringTab;
int stringtab_get(stringtable* ptab,int idx,char* dest)
{
	char* s1 = ptab->strdatbuf+ptab->addrlist[idx];
	char sztemp[256];
	int vlen = 0;
	int tlen = 0;// 
	if(idx==ptab->addrcnt-1)
	{
		//0 1 2 3 4 5 6
		tlen = ptab->slen - ptab->addrlist[idx];
		//has padding at the end of strdat
		for(int i=0;i<tlen;i++)
		{
			if(s1[i]==0)
			{
				tlen=i;
				break;
			}
		}
	}else{
		tlen = ptab->addrlist[idx+1]-ptab->addrlist[idx];
	}
	
	memcpy(dest,s1,tlen);

	return tlen;
}
int stringtab_find_idx(stringtable* ptab,char* dest)
{
	/*int sl = strlen(dest);
	for(int i=0;i<ptab->addrcnt;i++)
	{
		char* s1 = ptab->strdatbuf+ptab->addrlist[i];
		if(memcmp(s1,dest,sl)==0)
		{
			if(i!= ptab->addrcnt -1 )
			{
				char*s2 = ptab->strdatbuf+ptab->addrlist[i+1];
				if(s1+sl != s2)
				{
					continue;
				}
			}else{
				if(s1+sl != ptab->strdatbuf+ptab->len)
				{
					continue;
				}
			}
			return i;
		}
	}*/
	hash_pair* ie=ptab->ph->Find(dest);
	if(ie)
	{
		return ie->value;
	}
	
	return -1;
}

void set_strtab(char* buf,int len)
{
	stringTab.addrcnt = *(int*)buf;buf+=4;
	stringTab.addrlist = (int*)buf;
	stringTab.strdatbuf = buf+stringTab.addrcnt*4;//skip indextable
	stringTab.slen = len-stringTab.addrcnt*4;
	
	stringTab.ph = new Hashtable(4*450000*10/7);
	stringTab.buf = (char*)malloc(len+450000);
	
	stringtable* ptab = &stringTab;
	//remalloc mem for add new strings	
	int indexlen = ptab->addrcnt*4;

	char* oindexmem = (char*)stringTab.addrlist;
	char* nindexmem = (char*)malloc(indexlen+0x100*4);//max 256 new string
	memcpy(nindexmem,oindexmem,indexlen);

	char* ostrmem = stringTab.strdatbuf;
	char* nstrmem = (char*)malloc(stringTab.slen+0x1000);//max 4000 new chars
	memcpy(nstrmem,ostrmem,stringTab.slen);

	stringTab.addrlist = (int*)nindexmem;
	stringTab.strdatbuf = nstrmem;
	//stringTab.addrcnt += 0x100;
	//stringTab.slen += 0x1000;

	//free(oindexmem); com from filereadbuf,can't free here
	//free(ostrmem);

	for(int i=0;i<ptab->addrcnt;i++)
	{
		ptab->addrlist[i] = ptab->addrlist[i] - indexlen;//readdr start at strdatbuf of sub indexlen
	}

	char* szdest=stringTab.buf;
	stringTab.bufeof = 0;
	for(int i=0;i<ptab->addrcnt;i++)
	{
		char* s1 = ptab->strdatbuf+ptab->addrlist[i];
		char* skey=szdest;
		if(i!= ptab->addrcnt -1 )
		{
			char*s2 = ptab->strdatbuf+ptab->addrlist[i+1];

			memcpy(szdest,s1,s2-s1);
			szdest+= (s2-s1);
			szdest[0]=0;			
			szdest++;
		}else{
			memcpy(szdest,s1,ptab->slen-ptab->addrlist[i]);
			szdest+= (ptab->slen-ptab->addrlist[i]);//
			szdest[0]=0;			
			szdest++;
		}
		stringTab.bufeof += strlen(skey)+1;
		ptab->ph->AddPair(skey,i);		
	}
}
int add_strtab(char* str)
{
	stringtable* ptab = &stringTab;

	int sl = strlen(str);
	memcpy(ptab->strdatbuf+ptab->slen,str,sl);
	
	ptab->addrlist[ptab->addrcnt]=ptab->slen;
	ptab->addrcnt++;
	ptab->slen+=sl;

	//hash str buf
	char* skey=stringTab.buf+stringTab.bufeof;

	strcpy(skey,str);
	stringTab.bufeof += sl+1;

	ptab->ph->AddPair(skey,ptab->addrcnt-1);	

	ptab->updated =1;

	return ptab->addrcnt-1;
}
stringtable* re_strtab()
{
	stringtable* ptab = &stringTab;
	int indexlen = ptab->addrcnt*4;
	char* ostrbuf = ptab->strdatbuf;

	int elelen = ptab->slen+indexlen+4;
	if(elelen & 0x3 )
	{
		elelen=elelen + 4 - (elelen & 0x3);
	}
	
	char* nstrbuf = (char*)malloc(elelen);

	*(int*)nstrbuf = ptab->addrcnt;
	memcpy(4+nstrbuf,ptab->addrlist,indexlen);
	memcpy(4+nstrbuf+indexlen,ostrbuf,ptab->slen);
	if(ptab->elebuf)
	{
		free(ptab->elebuf);
	}
	ptab->elebuf = nstrbuf;

	int* naddrlist = (int*)(4+nstrbuf);
	for(int i=0;i<ptab->addrcnt;i++)
	{
		naddrlist[i] = naddrlist[i] + indexlen;
	}
	return ptab;
}
/*
stringtable.bin

.lst
.str
*/

class indexEntry
{
public:
	int index;

public:
	indexEntry()
	{
		istringtablebin=0;
		unkbuf =  0;//

		datlen_org=0;
		datlen=0;
		crc32=0;
		datoff=0;
		szpath[0]=0;
	}
	
	int istringtablebin;

	int unkbuf;//0x0				?? always is set to 0

	char szpath[0x104];//0x4

	unsigned int datlen_org;//0x8,,  5,5,5,5     padding for align 4 bytes?
	unsigned int datlen;//_0x0c file size

	unsigned int crc32;//0x10

	unsigned int datoff;//data off 0x14

	char* datbuf;//0x18:  datblock	== sizeof (datlen)
	bool output_bin(char* path)
	{
		//crc32 = encryptbuf(index,datbuf,datlen);

		FILE* file=fopen(path,"wb");
		fwrite(datbuf,datlen,1,file);
		fclose(file);	

	/*	bool n_crc_r = decryptbuf(index,crc32,crc32,datbuf,datlen);
		if(!n_crc_r)
		{
			return false;	
		}*/
		return true;
	}
	bool input_bin(char* path)
	{
		FILE* file = fopen(path,"rb");

		fseek(file,0,2);
		datlen_org = ftell(file);
		datlen = (datlen_org+3)&0xFFFFFFFC;

		fread(datbuf,datlen_org,1,file);

		fclose(file);		
	
		return true;
	}

	bool input_rebuild(char* t_binbuf)
	{
		//if(memcmp(t_binbuf,datbuf,datlen))
		//{
		//	return false;
		//}

		datlen_org = datlen;
		if(datlen_org & 0x3 )
		{
			datlen=datlen_org + 4 - (datlen_org & 0x3);
		}

		

		//align 4 bytes
		free(datbuf);
		datbuf = (char*)malloc(datlen);
		memcpy(datbuf,t_binbuf,datlen);
		free(t_binbuf);

		if(datlen>datlen_org)
		{
			memset(datbuf+datlen_org,0x0,datlen-datlen_org);
		}

		//crc32
	/*	crc32 = encryptbuf(index,datbuf,datlen);
		bool n_crc_r = decryptbuf(index,crc32,crc32,datbuf,datlen);
		if(!n_crc_r)
		{
			return false;	
		}*/
		return true;
	}
	bool input_file(char* plainText,int iszlen=0,HWND info_ctrl_hd=0)//导入文件&解释脚本
	{
		char* t_binbuf = (char*)malloc(1024*1024*16);
		char* binbuf = t_binbuf;
		char sztag[128];
		unsigned char b = 0;
		int d = 0;

		int islst = 0;
		int szl = strlen(szpath);
		if( (szpath[szl-1]=='r' && szpath[szl-2]=='t' && szpath[szl-3]=='s')
			|| (szpath[szl-1]=='t' && szpath[szl-2]=='x' && szpath[szl-3]=='t'))
		{
			datlen = strlen(plainText);
			memcpy(binbuf,plainText,datlen);
			
			goto _data_did_ok;
		}
		
		if(szpath[szl-3]=='l' && szpath[szl-2]=='s' && szpath[szl-1]=='t')
		{
			islst=1;
		}

		binbuf[0]=0xb0;
		binbuf[1]=0xd0;
		binbuf+=2;
		
		char* pos = plainText;
		int last_p_cnt = 0;
		char p_cnt_sz[32];
		while(pos[0]!=0)
		{
			if(info_ctrl_hd && iszlen)
			{
				int p_cnt = (pos - plainText)*100/iszlen;	
				if(p_cnt - last_p_cnt > 0)
				{
					itoa(p_cnt,p_cnt_sz,10);
					::SetWindowTextA(info_ctrl_hd,p_cnt_sz);

					last_p_cnt = p_cnt;
				}
			}

			//skip @.. 
			if(pos[0]=='@')
			{
				pos++;
				while(pos[0]!='@')
				{
					pos++;
				}
				pos++;//skip sp
			}

			//05 xxxx
			if(pos[0]=='\r' && pos[1]=='\n' && pos[2]=='[')
			{
				char* tpos1 = pos+2;
				while(tpos1[0]!='\r' || tpos1[1]!='\n')
				{
					tpos1++;//end checking?
				}
				memcpy(sztag,pos+2,tpos1 -(pos+2));	
				sztag[tpos1 -(pos+2)]=0;
				tpos1+=2;

				b = 0x05;
				
				//find string address
				d = stringtab_find_idx(&stringTab,sztag);
				if(d == -1)
				{
					d=add_strtab(sztag);
					//return false;
				}

				binbuf[0]=b;
				*(int*)(binbuf+1)=d;
				binbuf+=5;

				pos = tpos1;//move forward
				continue;
			}
			//t,v
			if(pos[0] == 't' && pos[1]==':')
			{
				char* tpos1 = pos+2;				
				while(tpos1[0] != ',')
				{
					tpos1++;//end checking?
				}
				memcpy(sztag,pos+2,tpos1-(pos+2));
				sztag[tpos1-(pos+2)]=0;
				b = atoi(sztag);
				
				tpos1++;
				if(tpos1[0]=='v' && tpos1[1]==':')//end checking?
				{
					tpos1 +=2;
					char* tpos2 = tpos1;

					while(tpos2[0] != ' ')
					{
						tpos2++;//end checking?
					}
					memcpy(sztag,tpos1,tpos2-tpos1);
					sztag[tpos2-tpos1]=0;
					tpos2++;
					d = atoi(sztag);				

					binbuf[0]=b;
					*(int*)(binbuf+1)=d;
					binbuf+=5;

					pos = tpos2;//move forward
					continue;
				}
			}
			//07,0a xxxx
			if(pos[0] == '\'')
			{
				if(pos[1]=='<')
				{
					char* tpos1 = pos+2;
					while(tpos1[0]!='>')// || tpos1[1]!='\'' || tpos1[2]!=' ')
					{
						tpos1++;//end checking?
					}
					memcpy(sztag,pos+2,tpos1 -(pos+2));	
					sztag[tpos1 -(pos+2)]=0;
					//tpos1++; if empty string form named_str

					b = 0x0a;
					//find string address
					d = stringtab_find_idx(&stringTab,sztag);
					if(d == -1)
					{
						d=add_strtab(sztag);
						//return false;
					}
					
					while(tpos1[0]!='\'' || tpos1[1]!=' ')
					{
						tpos1++;
					}
					tpos1+=2;
										
					binbuf[0]=b;
					*(int*)(binbuf+1)=d;
					binbuf+=5;

					pos = tpos1;//move forward
					continue;

					
				}else{

					char* tpos1 = pos+1;
					while(tpos1[0]!='\'' || tpos1[1]!=' ')
					{
						tpos1++;//end checking?
					}
					memcpy(sztag,pos+1,tpos1 -(pos+1));	
					sztag[tpos1 -(pos+1)]=0;
					tpos1+=2;

					b = 0x07;
					//find string address
					d = stringtab_find_idx(&stringTab,sztag);
					if(d == -1)
					{
						d=add_strtab(sztag);
						//return false;
					}

					if(islst && tpos1[0]=='\r'&& tpos1[1]=='\n')//\r\n
					{
						tpos1+=2;
					}

										
					binbuf[0]=b;
					*(int*)(binbuf+1)=d;
					binbuf+=5;
					
					pos = tpos1;//move forward
					continue;
				}
			}
			if( (pos[0] >='0' && pos[0]<='9') || pos[0]=='-')
			{
				if(pos[0]=='-' && (pos[1] <'0' || pos[1]>'9') )
				{
					pos++;
					return false;
				}
				char* tpos1 = pos+1;
				int isfloat=0;
				while(1)
				{
					if( !((tpos1[0] >='0' && tpos1[0]<='9') || tpos1[0]=='.') )
					{
						if( tpos1[0]==' ')
						{
							break;
						}else{
							return false;
						}
					}
					if(tpos1[0]=='.')
					{
						isfloat=1;
					}
					tpos1++;//end checking?
				}				
				memcpy(sztag,pos,tpos1 -pos);
				
				sztag[tpos1 -pos]=0;				
				
				if(isfloat)
				{			
					b = 0x04;
					*(float*)&d=atof(sztag);
				}else{
					b = 0x02;
					d = atoi(sztag);					
				}
				binbuf[0]=b;
				*(int*)(binbuf+1)=d;
				binbuf+=5;

				pos = tpos1+1;//move forward
				continue;
			}

			pos++;//move forward
		}
		
		datlen = (int)binbuf - (int)t_binbuf;

		//
_data_did_ok:

		return input_rebuild(t_binbuf);
	}
	void output_common(char* szdest)
	{
		unsigned char* pos = (unsigned char* )datbuf;
		pos+=2;

		char* pszdest = szdest;
		char* tempsz = NULL;

		unsigned char* end = (unsigned char* )datbuf + datlen_org;
		while(pos <= end-5+1)
		{
			unsigned char b = pos[0];pos++;
			unsigned int  d = *(unsigned int*)pos;pos+=4;
			//?
			//07 const string
			//0a named string *
			//05 tag string *
			if(b == 0x05 || b == 0x07 || b == 0x0a)
			{
				if(b == 0x05)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';		
				}else if(b == 0x0a){
					pszdest[0] = '\'';
					pszdest++;
					pszdest[0] = '<';		
				}else{
					pszdest[0] = '\'';
				}
				pszdest++;

				pszdest += stringtab_get(&stringTab,d,pszdest);
				
				if(b == 0x05)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
				}else if(b == 0x0a){
					pszdest[0] = '>';
					pszdest++;
					pszdest[0] = '\'';		
					pszdest++;
					pszdest[0] = ' ';	

					//desc txt
				}else{
					pszdest[0] = '\'';
					pszdest++;					
					pszdest[0] = ' ';					
				}
				pszdest++;
			}else if(b == 0x02){				
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);

				pszdest[0] = ' ';
				pszdest++;
			}else if(b == 0x04){
				pszdest += sprintf(pszdest,"%f",*(float*)&d);
				pszdest[0] = ' ';
				pszdest++;
			}else{
				pszdest[0] = 't';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(b,pszdest,10);
				pszdest+=strlen(tempsz);
				pszdest[0] = ',';
				pszdest++;

				pszdest[0] = 'v';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);				

				pszdest[0] = ' ';
				pszdest++;
			}
		}	
		pszdest[0]=0;
	}
	
	void output_plain(char* szdest)
	{
		memcpy(szdest,datbuf,datlen);
		szdest[datlen]=0;
	}
	void output_lst(char* szdest)
	{
		unsigned char* pos = (unsigned char* )datbuf;
		pos+=2;

		char* pszdest = szdest;
		char* tempsz = NULL;

		unsigned char* end = (unsigned char* )datbuf + datlen_org;
		while(pos <= end-5+1)
		{
			unsigned char b = pos[0];pos++;
			unsigned int  d = *(unsigned int*)pos;pos+=4;
			
			if(b == 0x07)
			{				
				pszdest[0] = '\'';				
				pszdest++;

				pszdest += stringtab_get(&stringTab,d,pszdest);
								
				pszdest[0] = '\'';
				pszdest++;
					
				pszdest[0] = ' ';
				pszdest++;
				pszdest[0] = '\r';
				pszdest++;
				pszdest[0] = '\n';					
				
				pszdest++;
			}else if(b == 0x02){				
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);

				pszdest[0] = ' ';
				pszdest++;
			}
		}	
		pszdest[0]=0;
	}

	void output_equ_stk(char* szdest,string_kor_str* pHash)
	{
		unsigned char* pos = (unsigned char* )datbuf;
		pos+=2;

		char* pszdest = szdest;
		char* tempsz = NULL;
		int tlen = 0;
		int tle2 = 0;
		unsigned char* end = (unsigned char* )datbuf + datlen_org;
		while(pos <= end-5+1)
		{
			unsigned char b = pos[0];pos++;
			unsigned int  d = *(unsigned int*)pos;pos+=4;
			//?
			//07 const string
			//0a named string *
			//05 tag string *
			if(b == 0x05 || b == 0x07 || b == 0x0a)
			{
				if(b == 0x05)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';		
				}else if(b == 0x0a){
					pszdest[0] = '\'';
					pszdest++;
					pszdest[0] = '<';		
				}else{
					pszdest[0] = '\'';
				}
				pszdest++;

				tlen = stringtab_get(&stringTab,d,pszdest);
				pszdest[tlen]=0;
				tle2 = string_kor_str_get(pHash,pszdest+tlen,pszdest);
				pszdest+=tlen;
				pszdest+=tle2;
				
				if(b == 0x05)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
				}else if(b == 0x0a){
					pszdest[0] = '>';
					pszdest++;
					pszdest[0] = '\'';		
					pszdest++;
					pszdest[0] = ' ';	

					//desc txt
				}else{
					pszdest[0] = '\'';
					pszdest++;					
					pszdest[0] = ' ';					
				}
				pszdest++;
			}else if(b == 0x02){				
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);

				pszdest[0] = ' ';
				pszdest++;
			}else if(b == 0x04){
				pszdest += sprintf(pszdest,"%f",*(float*)&d);
				pszdest[0] = ' ';
				pszdest++;
			}else{
				pszdest[0] = 't';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(b,pszdest,10);
				pszdest+=strlen(tempsz);
				pszdest[0] = ',';
				pszdest++;

				pszdest[0] = 'v';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);				

				pszdest[0] = ' ';
				pszdest++;
			}
		}	
		pszdest[0]=0;
	}

	void output_shp(char* szdest,string_kor_str* pHash)
	{
		unsigned char* pos = (unsigned char* )datbuf;
		pos+=2;

		char tag[32];
		char* tag_s;
		int tag_i = 0;
		int tag_n = 10;
		char szIdx[32];

		char* pszdest = szdest;
		char* tempsz = NULL;
		int tlen = 0;
		int tle2 = 0;

		unsigned char* end = (unsigned char* )datbuf + datlen_org;
		while(pos <= end-5+1)
		{
			unsigned char b = pos[0];pos++;
			unsigned int  d = *(unsigned int*)pos;pos+=4;
			//?
			//07 const string
			//0a named string *
			//05 tag string *
			if(b == 0x05 || b == 0x07 || b == 0x0a)
			{
				if(b == 0x05)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	

					tag_s=pszdest+1;
				}else if(b == 0x0a){
					pszdest[0] = '\'';
					pszdest++;
					pszdest[0] = '<';		
				}else{
					pszdest[0] = '\'';
				}
				pszdest++;				
				
				tlen = stringtab_get(&stringTab,d,pszdest);
				pszdest[tlen]=0;
				tle2 = string_kor_str_get(pHash,pszdest+tlen,pszdest);
				pszdest+=tlen;
				pszdest+=tle2;			
				
				if(b == 0x05)
				{
					memcpy(tag,tag_s,pszdest-tag_s);
					tag[pszdest-tag_s]=0;
										
					if(strcmp(tag,"[sell item]")==0)
					{
						tag_n = 4;						
					}else{
						tag_n = 0;
					}
					
					tag_i = -1;//skip tag sec
					
					
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
				}else if(b == 0x0a){
					pszdest[0] = '>';
					pszdest++;
					pszdest[0] = '\'';		
					pszdest++;
					pszdest[0] = ' ';	

					//desc txt
				}else{
					pszdest[0] = '\'';
					pszdest++;					
					pszdest[0] = ' ';					
				}

				pszdest++;
			}else if(b == 0x02){				
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);

				pszdest[0] = ' ';
				pszdest++;

				if(tag_n)
				{
					//
					if(d==0xffffffff)//-1
					{
						pszdest[0] = '@';
						pszdest++;
						pszdest[0] = '@';
						pszdest++;
						pszdest[0] = ' ';
						pszdest++;
					}else{
						itoa(d,szIdx,10);
						hash_pair* pp = stackable_lst_tab.ph->Find(szIdx);
						if(NULL == pp)
						{
							pp = equipment_lst_tab.ph->Find(szIdx);
						}
						if(pp)
						{
							pszdest[0] = '@';
							pszdest++;
							int tszl = strlen((char*)pp->value);
							memcpy(pszdest,(char*)pp->value,tszl);
							pszdest+=tszl;	
							pszdest[0] = '@';
							pszdest++;
							pszdest[0] = ' ';
							pszdest++;
						}
					}
				}
			}else if(b == 0x04){
				pszdest += sprintf(pszdest,"%f",*(float*)&d);
				pszdest[0] = ' ';
				pszdest++;
			}else{
				pszdest[0] = 't';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(b,pszdest,10);
				pszdest+=strlen(tempsz);
				pszdest[0] = ',';
				pszdest++;

				pszdest[0] = 'v';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);				

				pszdest[0] = ' ';
				pszdest++;
			}

			//
			if(tag_n)
			{
				tag_i++;

				if(tag_i && (tag_i % tag_n) ==0)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
					pszdest++;
				}
			}
		}	
		pszdest[0]=0;
	}
	
	void output_newcashshop(char* szdest,string_kor_str* pHash)
	{
		unsigned char* pos = (unsigned char* )datbuf;
		pos+=2;

		char tag[32];
		char* tag_s;
		int tag_i = 0;
		int tag_n = 10;

		char* pszdest = szdest;
		char* tempsz = NULL;
		int tlen = 0;
		int tle2 = 0;

		unsigned char* end = (unsigned char* )datbuf + datlen_org;
		while(pos <= end-5+1)
		{
			unsigned char b = pos[0];pos++;
			unsigned int  d = *(unsigned int*)pos;pos+=4;
			//?
			//07 const string
			//0a named string *
			//05 tag string *
			if(b == 0x05 || b == 0x07 || b == 0x0a)
			{
				if(b == 0x05)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	

					tag_s=pszdest+1;
				}else if(b == 0x0a){
					pszdest[0] = '\'';
					pszdest++;
					pszdest[0] = '<';		
				}else{
					pszdest[0] = '\'';
				}
				pszdest++;				
				
				tlen = stringtab_get(&stringTab,d,pszdest);
				pszdest[tlen]=0;
				tle2 = string_kor_str_get(pHash,pszdest+tlen,pszdest);
				pszdest+=tlen;
				pszdest+=tle2;			
				
				if(b == 0x05)
				{
					memcpy(tag,tag_s,pszdest-tag_s);
					tag[pszdest-tag_s]=0;
										
					if(strcmp(tag,"[item]")==0 || strcmp(tag,"[premium]")==0)
					{
						tag_n = 10;
					}else if(strcmp(tag,"[package]")==0){
						tag_n = 11;
					}else{
						tag_n = 0;
					}
					
					tag_i = -1;//skip tag sec
					
					
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
				}else if(b == 0x0a){
					pszdest[0] = '>';
					pszdest++;
					pszdest[0] = '\'';		
					pszdest++;
					pszdest[0] = ' ';	

					//desc txt
				}else{
					pszdest[0] = '\'';
					pszdest++;					
					pszdest[0] = ' ';					
				}

				pszdest++;
			}else if(b == 0x02){				
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);

				pszdest[0] = ' ';
				pszdest++;
			}else if(b == 0x04){
				pszdest += sprintf(pszdest,"%f",*(float*)&d);
				pszdest[0] = ' ';
				pszdest++;
			}else{
				pszdest[0] = 't';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(b,pszdest,10);
				pszdest+=strlen(tempsz);
				pszdest[0] = ',';
				pszdest++;

				pszdest[0] = 'v';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);				

				pszdest[0] = ' ';
				pszdest++;
			}

			//
			if(tag_n)
			{
				tag_i++;

				if(tag_i && (tag_i % tag_n) ==0)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
					pszdest++;
				}
			}
		}	
		pszdest[0]=0;
	}

	void output_monsterdrop(char* szdest,string_kor_str* pHash,int ishell=0)
	{
		unsigned char* pos = (unsigned char* )datbuf;
		pos+=2;

		char tag[64];
		char* tag_s;
		bool tag_f= false;
		int tag_i = 0;
		int tag_n = 10;

		char* pszdest = szdest;
		char* tempsz = NULL;
		int tlen = 0;
		int tle2 = 0;

		unsigned char* end = (unsigned char* )datbuf + datlen_org;
		while(pos <= end-5+1)
		{
			unsigned char b = pos[0];pos++;
			unsigned int  d = *(unsigned int*)pos;pos+=4;
			//?
			//07 const string
			//0a named string *
			//05 tag string *
			if(b == 0x05 || b == 0x07 || b == 0x0a)
			{
				if(b == 0x05)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	

					tag_s=pszdest+1;
				}else if(b == 0x0a){
					pszdest[0] = '\'';
					pszdest++;
					pszdest[0] = '<';		
				}else{
					pszdest[0] = '\'';
				}
				pszdest++;				
				
				tlen = stringtab_get(&stringTab,d,pszdest);	
				pszdest+=tlen;
				
				if(b == 0x05)
				{
					memcpy(tag,tag_s,pszdest-tag_s);
					tag[pszdest-tag_s]=0;
										
					if(strcmp(tag,"[basis of rarity dicision]")==0 )
					{
						tag_n = 6;
						tag_i = -1;//skip tag sec	
						
					}else{
						tag_n = 0;
					}					
															
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
				}else if(b == 0x0a){
					pszdest[0] = '>';
					pszdest++;
					pszdest[0] = '\'';		
					pszdest++;
					pszdest[0] = ' ';	

					//desc txt
				}else{
					pszdest[0] = '\'';
					pszdest++;					
					pszdest[0] = ' ';					
				}

				pszdest++;
			}else if(b == 0x02){				
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);

				pszdest[0] = ' ';
				pszdest++;
			}else if(b == 0x04){
				pszdest += sprintf(pszdest,"%f",*(float*)&d);
				pszdest[0] = ' ';
				pszdest++;
			}else{
				pszdest[0] = 't';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(b,pszdest,10);
				pszdest+=strlen(tempsz);
				pszdest[0] = ',';
				pszdest++;

				pszdest[0] = 'v';
				pszdest++;
				pszdest[0] = ':';
				pszdest++;
				tempsz = itoa(d,pszdest,10);
				pszdest+=strlen(tempsz);				

				pszdest[0] = ' ';
				pszdest++;
			}

			//
			if(tag_n)
			{
				tag_i++;
				if(ishell && tag_i==1)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
					pszdest++;

					tag_i=0;
					ishell=0;
				}
				if(tag_i && (tag_i % tag_n) ==0)
				{
					pszdest[0] = '\r';
					pszdest++;
					pszdest[0] = '\n';	
					pszdest++;
				}
			}
		}	
		pszdest[0]=0;
	}
	void output_file(char* szdest)
	{
		unsigned char* pos = (unsigned char* )datbuf;
		char szext[8];
		int szl = strlen(szpath);		
		strcpy(szext,szpath+szl-4);

		if(strcmp(szext,".txt")==0 || strcmp(szext,".str")==0)
		{
			output_plain(szdest);
			return;
		}				
		if(pos[0] != 0xb0 || pos[1] != 0xd0)
		{				
			strcpy(szdest,"不支持的格式!");
			return;
		}

		if(strcmp(szext,".lst")==0)
		{
			output_lst(szdest);
			return;
		}

		if(strcmp(szext,".equ")==0)
		{
			output_equ_stk(szdest,&equipment_kor_str_tab);
			return;
		}
		if(strcmp(szext,".stk")==0)
		{
			output_equ_stk(szdest,&stackable_kor_str_tab);
			return;
		}
		if(strcmp(szext,".shp")==0)
		{
			output_shp(szdest,&itemshop_kor_str_tab);
			return;
		}

		if(strcmp(szpath,"etc/newcashshop.etc")==0)
		{
			output_newcashshop(szdest,&stackable_kor_str_tab);
			return;
		}

		if(strstr(szpath,"etc/itemdropinfo_mons"))
		{
			if(strstr(szpath,"_hell"))
			{
				output_monsterdrop(szdest,NULL,1);
			}else{
				output_monsterdrop(szdest,NULL);
			}
			return;
		}
		
		//
		output_common(szdest);
	}


	void read_info(char** ppindexbuf,int idx,char* bufdat)
	{
		//index[readid before] pathlen path datlenorg crc32 datoff
		index = idx;//4 bytes

		//unkbuf = readm(szpath,buf);
		char* pindexbuf = ppindexbuf[0];		

		int t1 = *(int*)pindexbuf;
		pindexbuf+=4;
		if(t1>0x103)
		{
			::MessageBox(NULL,szpath,"t1>0x103!",0);
			return ;
		}
		memcpy(szpath,pindexbuf,t1);
		szpath[t1]=0;
		pindexbuf += t1;

		unsigned int* pbuft = (unsigned int*)pindexbuf;

		datlen_org = pbuft[0];
		pbuft++;
		datlen = (datlen_org+3)&0xFFFFFFFC;

		crc32 = pbuft[0];
		pbuft++;

		datoff = pbuft[0];//0x06ad6880 4db6
		pbuft++;

		ppindexbuf[0] = (char*)pbuft;	//
				
		
		datbuf = (char*)malloc(datlen);

		memcpy(datbuf,bufdat+datoff,datlen);
		if(!decryptbuf(index,crc32,crc32,datbuf,datlen))
		{
			::MessageBox(NULL,szpath,"crc error!",0);
			return;
		}

		return ;
	}

	int cal_index_ele_off(int org_datof)
	{
		//
		if(istringtablebin==1 && stringTab.updated)
		{
			stringtable* ttab = re_strtab();
			datbuf = ttab->elebuf;
			datlen = ttab->slen+ttab->addrcnt*4+4;

			datlen_org = datlen;
			if(datlen_org & 0x3 )
			{
				datlen=datlen_org + 4 - (datlen_org & 0x3);
			}
			if(datlen>datlen_org)
			{
				memset(datbuf+datlen_org,0x0,datlen-datlen_org);
			}

			//crc32
			/*crc32 = encryptbuf(index,datbuf,datlen);
			bool n_crc_r = decryptbuf(index,crc32,crc32,datbuf,datlen);
			if(!n_crc_r)
			{
				::MessageBox(NULL,"stringtable.bin crc error!","",0);
				exit(0);
				return -1;	
			}			*/
		}

		datoff = org_datof;
		return datoff+datlen;
	}

	int write_index_ele(char* pDesMem)
	{
		//
		crc32 = encryptbuf(index,datbuf,datlen);

		char* orgDes = pDesMem;
		memcpy(pDesMem,&index,4);pDesMem+=4;

		int path_l = strlen(szpath);
		memcpy(pDesMem,&path_l,4);pDesMem+=4;
		memcpy(pDesMem,szpath,path_l);pDesMem+=path_l;
		
		memcpy(pDesMem,&datlen_org,4);pDesMem+=4;
		memcpy(pDesMem,&crc32,4);pDesMem+=4;
				
		memcpy(pDesMem,&datoff,4);pDesMem+=4;		

		return pDesMem-orgDes;
	}
	void write_ele_data(FILE* file)
	{
		//crc32
		fwrite(datbuf,datlen,1,file);
	}

	int fix_nj()
	{
		/*
		[minimum level] 0x000047f6
		50

		[durability]0x000062a4
		35 
		[weight] 0x00001d0c
		3100 
		*/
		char tag[256];
		unsigned int  d = 0;
		int x = 0;

		unsigned char* pos = (unsigned char*)datbuf;
		unsigned char* end = pos + datlen;
		if(pos[0] != 0xb0 || pos[1] != 0xd0)
		{
			return -1;
		}

		int lev=0;
		int nj=0;
		unsigned char* pWeight = 0;

		pos += 2;
		while(pos < end-5+1)
		{
			unsigned char b = pos[0];pos++;
			d = *(unsigned int*)pos;pos+=4;
		
			if(b==0x05)
			{
				if(d == 0x000047f6 && pos[0]==0x02)
				{
					lev = *(int*)(pos+1);
					if(lev<71)
					{
						return -2;
					}
					if(pWeight)
					{
						break;
					}
				}
				if(d == 0x000062a4 && pos[0]==0x02)
				{
					nj = *(int*)(pos+1);
					if(lev && pWeight)
					{
						break;
					}
				}
				if(d == 0x00001d0c && pos[0]==0x02)
				{
					pWeight  = pos - 5;
					if(lev)
					{
						break;
					}
				}
			}
		}

		if(lev>70 && nj==0 && pWeight)
		{
			//fixit
			char* t_binbuf = (char*)malloc(1024*1024);
			int lens=(DWORD)pWeight-(DWORD)datbuf;
			memcpy(t_binbuf,datbuf,lens);

			//add nj 35
			t_binbuf[lens]=0x05;
			*(int*)(t_binbuf+lens+1) = 0x000062a4;
			t_binbuf[lens+5]=0x02;
			*(int*)(t_binbuf+lens+1+5) = 35;

			memcpy(t_binbuf+lens+10,pWeight,datlen-lens);

			datlen = datlen + 10;
			input_rebuild(t_binbuf);

			return 1;
		}

		return 0;
	}
};

typedef struct _PvfIndex
{
	int index;
	int path_len;
	char path[256];
	int data_len;
	int crc32;
	int data_off;
}PvfIndex;
typedef struct _PvfStruct
{
	int tagLen;
	char tag[256];//sizeof tagLen
	int unk;//
	int index_size;
	int crc32;
	int indexnum;
	//index table of PvfIndex
	//data table
}PvfStruct;

class PVFPack
{
public:
	PVFPack(char* path)
	{
		FILE* file = fopen(path,"rb");
		
		//tag
		int tagLen=0;
		fread(&tagLen,4,1,file);
		fread(tag,tagLen,1,file);
		tag[tagLen]=0;

		fread((char*)&this_4_unk,4,1,file);//0xbc59

		fread((char*)&index_size,4,1,file);//buf size 
										// 0x5d49a6de 0x1c532d7e
		fread((char*)&crcold,4,1,file); //crc 0x8899eb4e 0xe00aae14
		fread((char*)&crc0_indexnum,4,1,file);	//crc_0 0x0004e6fd  0x0004e7d0

		indexbuf = (char*)malloc(index_size); //index buf
		fread(indexbuf,index_size,1,file);//buf
		//0x092d219f  -> 7e 2d 53 1c

		int x1 = *(int*)(indexbuf);
		//int x2 = 0x157e0ce1 ^ 0x092d219f;


		//0x54648741;//0x54648741 0x82A79013

		head_size = ftell(file);//headsize 0x0188ffec
				
		//crc32
		int tcrcold=0;
		if( (index_size & 3) == 0)
		{
			init_crc_table();//0x0004e6fb			0x0188ffb4
			if(!decryptbuf(crc0_indexnum,crcold,crcold,indexbuf,index_size))
			{
				free(indexbuf);
				fclose(file);
				loaded = 0;
				::MessageBox(NULL,"索引表crc错误","",0);
				return ;
			}
//0x0004e6fb=crc0_indexnum,0x0188ffb4=index_size
			//tcrcold = ::encryptbuf(crc0_indexnum,indexbuf,index_size);
	

		}else{
			::MessageBox(NULL,"索引表异常2","",0);
			return;
		}

		//
		fseek(file,0,2);
		int var_10 = ftell(file);//filesize?
		if(var_10<=0)
		{
			free(indexbuf);
			fclose(file);
			loaded = 0;
			return;
		}
		fseek(file,0,0);
		data_size = var_10 - head_size;

		databuf = (char*)malloc(data_size);//data buf 0x091688bc
		fseek(file,head_size,0);
		fread(databuf,data_size,1,file);		
		
		fclose(file);

		//ok.
		loaded = 1;

	}

	void Init()
	{
		if(loaded==0)
		{
			return;
		}
	
		pair<unsigned int,indexEntry> p;		
		v.resize(crc0_indexnum,p);

		ph = new Hashtable(5*330000*10/7);

		int cidxnum=0;
		char* tindexbuf=indexbuf;
		vector<pair<unsigned int,indexEntry>>::iterator iter; 
		for( iter = v.begin(); iter != v.end(); iter++ )
		{   
			iter->first = *(int*)tindexbuf;
			tindexbuf+=4;

			indexEntry* ie =  (indexEntry*)& iter->second;
			//equipment/character/common/jacket/cloth/vest_owool.equ
			ie->read_info(&tindexbuf,iter->first,databuf);
			
			if(0 != ph->AddPair(ie->szpath,(int)ie) )
			{
				//::MessageBox(NULL,ie->szpath,"hash索引创建失败",0);
			}

			cidxnum++;
		} 

		//特殊表解析
		ParseSpecialTabs();

		//释放DATA内存[已经读取到对应的indexEntry]
		free(databuf);
		//释放index内存[已经读取到对应的indexEntry]
		free(indexbuf);

		databuf = NULL;
	}
void set_lst_kor(string_kor_str* strhash,string_kor_str* nametab,unsigned char* pos,unsigned int len,char* cat,char* f_sec)
{
	strhash->buf = (char*)malloc(len/5*16);
	strhash->ph = new Hashtable(5*220000*10/7);
	strhash->ph_ls = new Hashtable(5*220000*10/7);
	unsigned char* end = pos + len;
	//
	if(pos[0] != 0xb0 || pos[1] != 0xd0)
	{
		return;
	}
	pos+=2;

	int f_name = 0;
	char sznpcid[16];
	if(strcmp(f_sec,"[name]")==0)
	{
		f_name=1;
	}

	char tmpPath[256];
	strcpy(tmpPath,cat);
	char* tmpShort = tmpPath+strlen(tmpPath);

	char* pszdest=strhash->buf;
	char* pline=pszdest;
	while(pos <= end-5+1)
	{
		unsigned char b = pos[0];pos++;
		unsigned int  d = *(unsigned int*)pos;pos+=4;
		if(b==0x02)
		{
			pline = itoa(d,pszdest,10);
			pszdest+=strlen(pszdest);
			pszdest[0]=0;
			pszdest++;
		}else if(b==0x07){		
			
				int il = stringtab_get(&stringTab,d,tmpShort);
				tmpShort[il]=0;
				StrToLower(tmpShort);

				hash_pair* ir = ph->Find(tmpPath);
				if(ir == NULL)
				{
					continue;
				}
				
				indexEntry* ele = (indexEntry*)ir->value;
				unsigned char* elebuf = (unsigned char*)ele->datbuf;
				unsigned char* eleend = elebuf+ele->datlen;
				int sec_found=0;
				if(elebuf[0] != 0xb0 || elebuf[1] != 0xd0)
				{
					continue;
				}							

				//find f_sec=[name],[npc]
				elebuf+=2;
				while(elebuf<eleend-5+1)
				{
					if(elebuf[0]==0x05)
					{
						char szname[256];
						int tlen = 0;// 
						tlen = stringtab_get(&stringTab,*(int*)(elebuf+1),szname);
						szname[tlen]=0;
						if(stricmp(szname,f_sec)==0)//
						{
							sec_found=1;
							break;
						}
					}					
					elebuf+=5;
				}
				if(sec_found==0)
				{
					continue;
				}

				if(f_name == 0)//f_sec:  equ_stk.[name] shp.[npc]
				{
					elebuf+=5;

					int shp_npcid = *(int*)(elebuf+1);//02 xxxx
					itoa(shp_npcid,sznpcid,10);
					hash_pair* p_npc = npc_lst_tab.ph->Find(sznpcid);
					if(p_npc == NULL)
					{
						continue;
					}
					char* npcname = (char*)p_npc->value;

					strhash->ph_ls->AddPair(p_npc->szkey,(int)ele->szpath);//npcid,shp_path
					strhash->ph->AddPair(p_npc->szkey,(int)npcname);	
									
					continue;
				}else{
					strhash->ph_ls->AddPair(pline,(int)ele->szpath);
				}
				
				//				
				elebuf+=10;//skip [name]{t,v}
				if(elebuf<eleend-5+1 && elebuf[0]==0x0a)
				{
					char szname[256];
					int tlen = 0;// 
					tlen = stringtab_get(&stringTab,*(int*)(elebuf+1),szname);
					szname[tlen]=0;

					char* pvalue = 0;
					if(hash_pair* ir = nametab->ph->Find(szname))
					{
						pvalue =  (char*)ir->value;
					}
					if(pvalue)
					{
						strhash->ph->AddPair(pline,(int)pvalue);	
					}
				}
		}
	}	
}
	void ParseSpecialTabs()
	{
		//
		hash_pair* ir = ph->Find("stringtable.bin");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* pStringTab_ele = (indexEntry*)ir->value;
		//
		pStringTab_ele->istringtablebin = 1;
		set_strtab(pStringTab_ele->datbuf,pStringTab_ele->datlen_org);

		//
		ir = ph->Find("equipment/equipment.kor.str");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* equipment_kor_str_ele = (indexEntry*)ir->value;
		set_string_kor_str(&equipment_kor_str_tab,equipment_kor_str_ele->datbuf,equipment_kor_str_ele->datlen);

		//
		ir = ph->Find("stackable/stackable.kor.str");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* stackable_kor_str_ele = (indexEntry*)ir->value;
		set_string_kor_str(&stackable_kor_str_tab,stackable_kor_str_ele->datbuf,stackable_kor_str_ele->datlen);

		//
		ir = ph->Find("npc/npc.kor.str");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* npc_kor_str_ele = (indexEntry*)ir->value;
		set_string_kor_str(&npc_kor_str_tab,npc_kor_str_ele->datbuf,npc_kor_str_ele->datlen);

		//itemshop_kor_str_tab
		ir = ph->Find("itemshop/itemshop.kor.str");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* itemshop_kor_str_ele = (indexEntry*)ir->value;
		set_string_kor_str(&itemshop_kor_str_tab,itemshop_kor_str_ele->datbuf,itemshop_kor_str_ele->datlen);


		//
		/**/
		ir = ph->Find("equipment/equipment.lst");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* equipment_lst_ele = (indexEntry*)ir->value;
		set_lst_kor(&equipment_lst_tab,&equipment_kor_str_tab,(BYTE*)equipment_lst_ele->datbuf,equipment_lst_ele->datlen,"equipment/","[name]");

		//
		ir = ph->Find("stackable/stackable.lst");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* stackable_lst_ele = (indexEntry*)ir->value;
		set_lst_kor(&stackable_lst_tab,&stackable_kor_str_tab,(BYTE*)stackable_lst_ele->datbuf,stackable_lst_ele->datlen,"stackable/","[name]");
		
		//
		ir = ph->Find("npc/npc.lst");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* npc_lst_ele = (indexEntry*)ir->value;
		set_lst_kor(&npc_lst_tab,&npc_kor_str_tab,(BYTE*)npc_lst_ele->datbuf,npc_lst_ele->datlen,"npc/","[name]");
		
		//
		ir = ph->Find("itemshop/itemshop.lst");
		if(NULL == ir)
		{
			return;
		}
		indexEntry* itemshop_lst_ele = (indexEntry*)ir->value;
		set_lst_kor(&itemshop_lst_tab,&itemshop_kor_str_tab,(BYTE*)itemshop_lst_ele->datbuf,itemshop_lst_ele->datlen,"itemshop/","[npc]");
	
	}
public:
	
	void Repack(char* topath)//重新打包
	{
		FILE* file = fopen(topath,"wb");
		int tag_len = strlen(tag);
		fwrite(&tag_len,4,1,file);
		fwrite(tag,tag_len,1,file);

		fwrite((char*)&this_4_unk,4,1,file);

		//rebuild stringtable.bin
	
		vector<pair<unsigned int,indexEntry>>::iterator iter; 

		//int ele_idx_c=0;
		int org_datof = 0;
		for( iter = v.begin(); iter != v.end(); iter++ )
		{   
			indexEntry* ie =  (indexEntry*)& iter->second;
			org_datof = ie->cal_index_ele_off(org_datof);	//return noff next

			//ie->index =ele_idx_c;  can't edit the index of ele?
			//ele_idx_c++;
		}

		char* pDesMem = (char*)malloc(index_size);//fixed
		char* tDesMem = pDesMem;
		for( iter = v.begin(); iter != v.end(); iter++ )
		{   
			indexEntry* ie =  (indexEntry*)& iter->second;
			int dwLen = ie->write_index_ele(tDesMem);
			tDesMem += dwLen;
		} 
		//crc32 pDesMem,write to file
		index_size = tDesMem-pDesMem;//org_size
		if(index_size & 0x3 )
		{
			index_size=index_size + 4 - (index_size & 0x3);
		}

		crcold = ::encryptbuf(crc0_indexnum,pDesMem,index_size);

		fwrite((char*)&index_size,4,1,file);//buf size
		fwrite((char*)&crcold,4,1,file); //crc
		fwrite((char*)&crc0_indexnum,4,1,file);	//crc_0	
		fwrite(pDesMem,index_size,1,file);
		free(pDesMem);

		for( iter = v.begin(); iter != v.end(); iter++ )
		{   
			indexEntry* ie =  (indexEntry*)& iter->second;
			ie->write_ele_data(file);	
		} 


		fclose(file);
	}

public:
	char tag[128];//unused

//
	int this_4_unk;

//
	int index_org;
	int index_size;
	int crcold;
	int crc0_indexnum;

	int head_size;
	int data_size;

//
	int loaded;
	char* indexbuf;
	char* databuf;//temp 

	Hashtable* ph;
	vector<pair<unsigned int,indexEntry>> v;	
};

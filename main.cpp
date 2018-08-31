#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <commctrl.h>
#include <process.h>

#include <Richedit.h>


#include "resource.h"

#include "pvf.h"
#include "utl.h"

#pragma comment(lib, "comctl32.lib")

HINSTANCE hInst;								// 当前实例
HWND hWnd;
HMODULE hmRichEdit;
HWND hRichEdit;

char szAppPath[MAX_PATH];

PVFPack* pPvfPack=NULL;

typedef struct _nodedata
{
	char text[256];
	int	index;

	struct _nodedata* first;
	struct _nodedata* next;
};
struct _nodedata* rootnode=NULL;

void appendtext2rhtxt(HWND hrich,char* txt)
{
	char sztsz[512];

	CHARRANGE chrg;
	SETTEXTEX settext;

	chrg.cpMin=-1;
	chrg.cpMax=-1;
	SendMessageA(hrich,EM_EXSETSEL,0,(LPARAM)&chrg);

	settext.flags = ST_SELECTION;
	settext.codepage = CP_ACP;

	sprintf(sztsz,"%s\r\n",txt);

	SendMessage(hrich,EM_SETTEXTEX,(WPARAM)&settext,(LPARAM)sztsz);		

}

struct _nodedata* lookupchildnode(struct _nodedata* snode,char* txt)
{	
	snode = snode->first;
	while(snode)
	{
		if(strcmp(snode->text,txt)==0)
		{
			return snode;
		}
		snode = snode->next;
	}

	return NULL;
}

void AddTreeNodeData(int ptrEntry,char** pathparts,int partnum,int idc_tree)
{
	//TVM_GETNEXTITEM:TVGN_NEXT/TVGN_CHILD /TVGN_CARET
	if(NULL == rootnode)//fist time , rootnode is null
	{
		rootnode = (struct _nodedata*)malloc(sizeof(_nodedata));
		memset(rootnode,0,sizeof(_nodedata));
	}

	//look up and add,start at root
	struct _nodedata* snode = rootnode;
	for(int i=0;i<partnum;i++)
	{
		struct _nodedata* tnode = lookupchildnode(snode,pathparts[i]);
		if(tnode)
		{
			snode = tnode;
		}else{
			tnode = (struct _nodedata*)malloc(sizeof(_nodedata));
			memset(tnode,0,sizeof(_nodedata));
			//if(strlen(pathparts[i])>=48)
			//{
			//	//::SetDlgItemTextA(hWnd,IDC_STATIC2,pathparts[i]);
			//	appendtext2rhtxt(hRichEdit,((indexEntry*)ptrEntry)->szpath);

			//	//
			//}
			
			strcpy(tnode->text,pathparts[i]);

			struct _nodedata* fc_node = snode->first;
			if(fc_node)
			{
				while(fc_node->next)
				{	
					fc_node = fc_node->next;
				}

				fc_node->next = tnode;
			}else{
				snode->first = tnode;
			}

			snode = tnode;
		}

		if(i==partnum-1)
		{
			tnode->index = ptrEntry;
		}else{
			tnode->index = -1;
		}
	}

	//
}




TV_INSERTSTRUCT tvinsert;
int GetTreeSel()
{
	char Text[255]="";
	TV_ITEM tv_i;

	memset(&tv_i,0,sizeof(tv_i));

	tv_i.mask=TVIF_TEXT;   // item text attrivute
	tv_i.pszText=Text;     // Text is the pointer to the text 
	tv_i.cchTextMax=256;   // size of text to retrieve.
	tv_i.hItem=(HTREEITEM)SendDlgItemMessage(hWnd,IDC_TREE1,TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)0);   // the selected item

	SendDlgItemMessage(hWnd,IDC_TREE1,TVM_GETITEM,TVGN_CARET,(LPARAM)&tv_i); // get the data of the selected item!![TVGN_CARET]

	if(tv_i.lParam != -1 && tv_i.lParam!=0)
	{
		return tv_i.lParam;
	}
	return NULL;
}
HTREEITEM PullTreeNodes(struct _nodedata* snode,HTREEITEM treeparent,HTREEITEM treeafter)
{
	tvinsert.hParent=treeparent; 
	// root level item attribute.
	tvinsert.hInsertAfter=TVI_SORT;
	// attributs
	tvinsert.item.mask=TVIF_TEXT|TVIF_PARAM;//|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	// item text 
	tvinsert.item.pszText = snode->text;
	//tvinsert.item.iImage=0; // not pressed pic
	//tvinsert.item.iSelectedImage=1; // pressed pic

	tvinsert.item.lParam = snode->index;//User data.


	HTREEITEM ttreenode=(HTREEITEM)SendDlgItemMessageA(hWnd,IDC_TREE1,
				  TVM_INSERTITEM,0,(LPARAM)&tvinsert);


	snode = snode->first;
	HTREEITEM aft = NULL;
	while(snode)
	{
		aft = PullTreeNodes(snode,ttreenode,aft);

		snode = snode->next;
	}

	return ttreenode;
}
void BuildTreeCtrl(PVFPack* pPack)
{
	char szpath[512];
	char* pathparts[16];
	int partnum=0;

	vector<pair<unsigned int,indexEntry>> *v = &pPack->v;
	vector<pair<unsigned int,indexEntry>>::iterator iter; 
	for( iter = v->begin(); iter != v->end(); iter++ )
	{   
		indexEntry* ie =  (indexEntry*)& iter->second;
		strcpy(szpath,ie->szpath);

		split(szpath,"/",pathparts,&partnum);

		AddTreeNodeData((int)ie,pathparts,partnum,IDC_TREE1);
	}

	//HBITMAP hBitMap1 = LoadBitmap(hInst,MAKEINTRESOURCE(IDI_ICON1)); 
	//HBITMAP hBitMap2 = LoadBitmap(hInst,MAKEINTRESOURCE(IDI_ICON2)); 

	//HIMAGELIST hImageList = ImageList_Create(16,16,ILC_COLOR16,2,10); 
	//ImageList_Add(hImageList,hBitMap1,NULL); 
	//ImageList_Add(hImageList,hBitMap2,NULL); 

	//SendDlgItemMessage(hWnd,IDC_TREE1,TVM_SETIMAGELIST,0,(LPARAM)hImageList);

	HTREEITEM ttreenode = NULL;

	tvinsert.hParent=NULL; 
	// root level item attribute.
	tvinsert.hInsertAfter=TVI_ROOT;
	// attributs
	tvinsert.item.mask=TVIF_TEXT;//|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	// item text 
	tvinsert.item.pszText = "";
	//tvinsert.item.iImage=0; // not pressed pic
	//tvinsert.item.iSelectedImage=1; // pressed pic

	tvinsert.item.lParam = -1;//User data.

	ttreenode=(HTREEITEM)SendDlgItemMessageA(hWnd,IDC_TREE1,
				  TVM_INSERTITEM,0,(LPARAM)&tvinsert);

	struct _nodedata* snode = rootnode->first;
	HTREEITEM aft = NULL;
	while(snode)
	{
		aft = PullTreeNodes(snode,ttreenode,aft);

		snode = snode->next;
	}

	//TVM_EXPAND TVE_EXPAND
	SendDlgItemMessageA(hWnd,IDC_TREE1,
				  TVM_EXPAND,TVM_EXPAND,(LPARAM)ttreenode);
}


char* plainBuf = NULL;


void SetRichText(char* text)
{
	::SetWindowTextA(hRichEdit,text);
}
int GetRichText(char* text,int cmax)
{
	return ::GetWindowTextA(hRichEdit,text,cmax);
}
bool SaveRichText(char* path)
{
	FILE* file = fopen(path,"wb");
	
	char* pText = (char*)malloc(1024*1024*16);
	int ilen = GetRichText(pText,1024*1024*16);
	fwrite(pText,ilen,1,file);
	fclose(file);	

	free(pText);
	return true;
}
bool LoadRichText(char* path)
{
	FILE* file = fopen(path,"rb");
	fseek(file,0,2);
	int ilen = ftell(file);//filesize?
	fseek(file,0,0);
	
	char* pText = (char*)malloc(ilen+1);	
	fread(pText,ilen,1,file);
	fclose(file);
	pText[ilen]=0;

	SetRichText(pText);

	free(pText);
	return true;
}

void DispEntry(int ptrEntry)
{
	if(NULL == plainBuf)
	{
		plainBuf = (char*)malloc(1024*1024*16);
	}

	indexEntry* ie = (indexEntry*)ptrEntry;

	ie->output_file(plainBuf);
	
	char* szpath=ie->szpath;
	int szl = strlen(szpath);
	if( (szpath[szl-1]=='r' && szpath[szl-2]=='t' && szpath[szl-3]=='s')
		|| (szpath[szl-1]=='t' && szpath[szl-2]=='x' && szpath[szl-3]=='t'))
	{
		//out_strtab(plainBuf);

		char* cv_str = NULL;
		BIG52GBK(plainBuf,&cv_str);
		//big5 -> gbk
		::SetRichText(cv_str);
		free(cv_str);
	}else
	
	{
		::SetRichText(plainBuf);
	}

	//
	::SetDlgItemTextA(hWnd,IDC_STATIC2,ie->szpath);
	::SetDlgItemTextA(hWnd,IDC_EDIT2,ie->szpath);
	//


	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON2),TRUE);
	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON4),TRUE);

	if(0 == strcmp(plainBuf,"不支持的格式!"))
	{
		::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON1),FALSE);
		//::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON2),FALSE);
		//::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON4),FALSE);
		::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON5),FALSE);
		return;
	}

	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON1),TRUE);
	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON5),TRUE);
}

//导出 文件块
void ExplotEntry()
{
	char pvf[512];
	strcpy(pvf,"temp.txt");
	TCHAR szFilter[] = TEXT ("所有文件\0*.*\0")  TEXT ("BIN文件 (*.txt)\0*.txt\0\0") ;

	int ptrEntry = GetTreeSel();
	if(!ptrEntry)
	{
		::MessageBox(hWnd,"未选择PVF路径","",0);
		return;
	}
	indexEntry* ie = (indexEntry*)ptrEntry;

	int szl = strlen(ie->szpath);
	int ssp = 0;
	for(int i=szl-1;i>=0;i--)
	{
		if(ie->szpath[i]=='/')
		{
			ssp = i+1;
			break;
		}
	}
	strcpy(pvf,ie->szpath+ssp);

	if(FALSE == GetSaveFile(hWnd,pvf,szFilter,"bin"))
	{
		return;
	}

	//	
	SaveRichText(pvf);
	
	::MessageBox(hWnd,"导出成功!","^_^",0);
}
//导入 文件块
void ImportEntry()
{
	char pvf[512];
	strcpy(pvf,"temp.txt");
	TCHAR szFilter[] = TEXT ("所有文件\0*.*\0")  TEXT ("BIN文件 (*.txt)\0*.txt\0\0") ;

	int ptrEntry = GetTreeSel();
	if(!ptrEntry)
	{
		::MessageBox(hWnd,"未选择PVF路径","",0);
		return;
	}
	indexEntry* ie = (indexEntry*)ptrEntry;	

	int szl = strlen(ie->szpath);
	int ssp = 0;
	for(int i=szl-1;i>=0;i--)
	{
		if(ie->szpath[i]=='/')
		{
			ssp = i+1;
			break;
		}
	}
	strcpy(pvf,ie->szpath+ssp);

	//
	if(GetShooseFile(hWnd,pvf,szFilter,NULL) == FALSE)
	{
		return;
	}	

	//do input : 重排PVF
	LoadRichText(pvf);
	
	//DispEntry(ptrEntry);

	::MessageBox(hWnd,"导入成功!","^_^",0);
}


//保存到PVF
void SaveEntry(void* th_p)
{
	char pvf[512];
	strcpy(pvf,"Script.pvf");

	int ptrEntry = GetTreeSel();
	if(!ptrEntry)
	{
		::MessageBox(hWnd,"未做任何修改","",0);
		return;
	}	

	TCHAR szFilter[] = TEXT ("PVF文件\0*.pvf\0")  TEXT ("PVF文件 (*.pvf)\0*.pvf\0\0") ;
	if(FALSE == GetSaveFile(hWnd,pvf,szFilter,"pvf"))
	{
		return;
	}

	//
	//indexEntry* ie = (indexEntry*)ptrEntry;

	//重新解释脚本内容
	//::GetDlgItemTextA(hWnd,IDC_EDIT1,plainBuf,1024*1024);
	//if(ie->input_file(plainBuf) == false)
	//{
	//	::MessageBox(hWnd,"解释脚本失败!","",0);
	//	return;
	//}

	::SetDlgItemTextA(hWnd,IDC_STATIC1,"正在打包PVF ...");
	pPvfPack->Repack(pvf);
	::SetDlgItemTextA(hWnd,IDC_STATIC1,"打包完成.");

	//重新解释脚本内容,重排
	MessageBox(hWnd,"保存完成!","",0);
}

//保存到内存
void SaveMem(void* th_p)
{
	int ptrEntry = GetTreeSel();
	if(!ptrEntry)
	{
		::MessageBox(hWnd,"未选择PVF路径","",0);
		return;
	}

	//
	indexEntry* ie = (indexEntry*)ptrEntry;

	//重新解释脚本内容
	int ilen = ::GetRichText(plainBuf,1024*1024*16);

	::SetDlgItemTextA(hWnd,IDC_STATIC1,"编译 ...");

	HWND info_ctrl_handle = ::GetDlgItem(hWnd,IDC_STATIC1);

	char* szpath=ie->szpath;
	int szl = strlen(szpath);
	if( (szpath[szl-1]=='r' && szpath[szl-2]=='t' && szpath[szl-3]=='s')
		|| (szpath[szl-1]=='t' && szpath[szl-2]=='x' && szpath[szl-3]=='t'))
	{
		//	
		char* cv_str = NULL;
		GBK2BIG5(plainBuf,&cv_str);

		if(ie->input_file(cv_str,ilen,info_ctrl_handle) == false)
		{
			::MessageBox(hWnd,"保存失败!","",0);
			return;
		}
		
		free(cv_str);
	}else
	{
		if(ie->input_file(plainBuf,ilen,info_ctrl_handle) == false)
		{
			::MessageBox(hWnd,"保存失败!","",0);
			return;
		}
	}
	::SetDlgItemTextA(hWnd,IDC_STATIC1,"编译完成.");
	
	MessageBox(hWnd,"保存成功!","",0);
}
//
struct _nodedata* treedata_finditem(char** pEles,int iNum)
{
	if(NULL == rootnode)
	{
		return NULL;
	}
	struct _nodedata* tnode = rootnode;//->first;

	int got=0;
	for(int i=0;i<iNum;i++)
	{
		got=0;

		tnode = tnode->first;
		while(tnode)
		{
			char* part_i = pEles[i];
			if(strcmp(tnode->text,part_i)==0)
			{
				got=1;
				break;
			}else{
				tnode = tnode->next;
			}
		}

		if(0 == got)
		{
			break;
		}

	}

	return tnode;
}
void treedata_each_subitem(struct _nodedata* node)
{
	struct _nodedata* tnode = node;	
	if(tnode->index>0)
	{

		if(1 == ((indexEntry*)tnode->index)->fix_nj())
		{

			appendtext2rhtxt(hRichEdit,((indexEntry*)tnode->index)->szpath);
		}
		return;
	}

	tnode = tnode->first;
	while(tnode)
	{
		treedata_each_subitem(tnode);	

		tnode = tnode->next;
	}
}
void Repair70up_nj()
{	
	char nodePath[]="equipment/character";
	char* pathparts[16];
	int partnum=0;
	split(nodePath,"/",pathparts,&partnum);

	struct _nodedata* node_equ_char = treedata_finditem(pathparts,partnum);
	if(NULL == node_equ_char)
	{
		return;
	}
	treedata_each_subitem(node_equ_char);

}
void PVFInit(void* th_p)
{
	char pvf[512];
	strcpy(pvf,"Script.pvf");

	TCHAR szFilter[] = TEXT ("PVF文件\0*.pvf\0")  TEXT ("PVF文件 (*.pvf)\0*.pvf\0\0") ;

	if(GetShooseFile(hWnd,pvf,szFilter,NULL) == FALSE)
	{
		return;
	}	
	//::SetDlgItemTextA(hWnd,IDC_STATIC1,pvf);

	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON3),FALSE);	

	::SetDlgItemTextA(hWnd,IDC_STATIC1,"正在加载PVF ...");
	pPvfPack = new PVFPack(pvf);	

	::SetDlgItemTextA(hWnd,IDC_STATIC1,"正在解包PVF ...");
	pPvfPack->Init();
	
	::SetDlgItemTextA(hWnd,IDC_STATIC1,"正在生成目录 ...");
	BuildTreeCtrl(pPvfPack);

	//::SetDlgItemTextA(hWnd,IDC_STATIC1,"正在修复70+道具耐久 ...");
	//Repair70up_nj();

	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON1),TRUE);
	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON7),TRUE);
	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON8),TRUE);
	::EnableWindow(::GetDlgItem(hWnd,IDC_BUTTON6),TRUE);

	::SetDlgItemTextA(hWnd,IDC_STATIC1,"处理完成.");
}

void ItemList()
{
	/*
	equipment_kor_str equipment_lst_tab;
equipment_kor_str stackable_lst_tab;
	*/
	FILE* f = fopen(".\\itemequ.txt","w");
	Hashtable* p = equipment_lst_tab.ph;
	for(int i=0;i<p->MaxSize;i++)
	{
		char* e = p->pInteralArray[i].szkey;
		char* v = (char*)p->pInteralArray[i].value;
		if(NULL == e)
		{
			continue;
		}
		fprintf(f,"%s:%s\r\n",e,v);
	}
	
	fclose(f);

	f = fopen(".\\itemstk.txt","w");
	p = stackable_lst_tab.ph;
	for(int i=0;i<p->MaxSize;i++)
	{
		char* e = p->pInteralArray[i].szkey;
		char* v = (char*)p->pInteralArray[i].value;
		if(NULL == e)
		{
			continue;
		}
		fprintf(f,"%s:%s\r\n",e,v);
	}
	
	fclose(f);

	MessageBox(hWnd,"保存成功!","",0);
}

void TEA_Code()
{
	char szpwd[128];
	char szou[128];
	::GetDlgItemTextA(hWnd,IDC_EDIT2,szpwd,128);
	int szl = strlen(szpwd);
	if(szl<16)
	{
		if(szl != 8)
		{
			::MessageBox(hWnd,"密码必须为8个字符","",0);
			return;
		}
		CTEA_Encrypt(szpwd,szou,"74726f716b64646d74726f716b63646d");
		strcpy(szou+16,"e8b10c1f8bc3595be8b10c1f8bc3595b");
	}else{
		szpwd[16]=0;
		CTEA_Decrypt(szpwd,szou,"74726f716b64646d74726f716b63646d");
	}
	

	::SetDlgItemTextA(hWnd,IDC_EDIT2,szou);

}
void Expand_Node(char* sp=NULL)
{
	char szpath[256];
	char* pathparts[16];
	int partnum=0;

	if(NULL ==sp)
	{
		int ir = GetDlgItemTextA(hWnd,IDC_EDIT2,szpath,256);
	}else{
		strcpy(szpath,sp);
	}
	::SetDlgItemTextA(hWnd,IDC_STATIC2,szpath);
	::SetDlgItemTextA(hWnd,IDC_EDIT2,szpath);

	split(szpath,"/",pathparts,&partnum);

	//IDC_TREE1 TVGN_ROOT
	char Text[255]="";
	TV_ITEM tv_i;

	memset(&tv_i,0,sizeof(tv_i));

	tv_i.mask=TVIF_TEXT;   // item text attrivute
	tv_i.pszText=Text;     // Text is the pointer to the text 
	tv_i.cchTextMax=256;   // size of text to retrieve.
	tv_i.hItem=(HTREEITEM)SendDlgItemMessage(hWnd,IDC_TREE1,TVM_GETNEXTITEM,TVGN_ROOT,(LPARAM)0);
	if(NULL == tv_i.hItem)
	{
		return;
	}
	tv_i.hItem = (HTREEITEM)SendDlgItemMessage(hWnd,IDC_TREE1,
					TVM_GETNEXTITEM,TVGN_CHILD,(LPARAM)tv_i.hItem);
	if(NULL == tv_i.hItem)
	{
		return;
	}

	//start at the first child of root
	int got=0;
	for(int i=0;i<partnum;i++)
	{
		char* part_i = pathparts[i];
		got=0;
		while(tv_i.hItem)
		{
			SendDlgItemMessage(hWnd,IDC_TREE1,TVM_GETITEM,TVGN_CARET,(LPARAM)&tv_i); // get the data of the item

			if(strcmp(tv_i.pszText,part_i)==0)
			{
				SendDlgItemMessageA(hWnd,IDC_TREE1,
				  TVM_EXPAND,TVM_EXPAND,(LPARAM)tv_i.hItem);
				if(i==partnum-1)
				{
					::SendDlgItemMessageA(hWnd,IDC_TREE1,TVM_SELECTITEM,TVGN_CARET,(LPARAM)tv_i.hItem);
				}else{
					tv_i.hItem = (HTREEITEM)SendDlgItemMessage(hWnd,IDC_TREE1,
						TVM_GETNEXTITEM,TVGN_CHILD,(LPARAM)tv_i.hItem);
				}

				got=1;
				break;
			}else{
				tv_i.hItem = (HTREEITEM)SendDlgItemMessage(hWnd,IDC_TREE1,
					TVM_GETNEXTITEM,TVGN_NEXT,(LPARAM)tv_i.hItem);
			}
		}
		
		if(got==0)
		{
			break;
		}
		//
	}
	if(got==1)
	{
		int ptrEntry = GetTreeSel();
		if(!ptrEntry)
		{
			return;
		}
		DispEntry(ptrEntry);
	}else{
		::MessageBoxA(NULL,"未找到相关路径","",0);
	}

	
	//TVGN_CHILD

}
// 此代码模块中包含的函数的前向声明:
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
WNDPROC oEditProc = NULL;
INT_PTR CALLBACK	EditProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	FindProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	EleProc(HWND, UINT, WPARAM, LPARAM);
HFONT m_Fontx = NULL;
void CreateRichEdit()
{
	
	/*hRichEdit = CreateWindowExA(WS_EX_CLIENTEDGE,"RichEdit20A",0
		,WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL | WS_HSCROLL | ES_NOHIDESEL
		,285,37,590,460,hWnd,NULL,hInst,0);*/
	hRichEdit = ::GetDlgItem(hWnd,IDC_RICHEDIT21);

	SendMessageA(hRichEdit,EM_LIMITTEXT,-1,0);//no 64k limit
	//SendMessage(hRichEdit,EM_SETEVENTMASK,0,ENM_MOUSEEVENTS);

	m_Fontx =  CreateFont(
				12,				// height of font
				0,						// average character width
				0,						// angle of escapement
				0,						// base-line orientation angle
				FW_NORMAL,				// font weight
				0,						// italic attribute option
				0,						// underline attribute option
				0,						// strikeout attribute option
				DEFAULT_CHARSET,		// character set identifier
				OUT_TT_PRECIS,			// output precision
				CLIP_DEFAULT_PRECIS,	// clipping precision
				ANTIALIASED_QUALITY,	// output quality
				DEFAULT_PITCH,			// pitch and family
				"宋体"				// typeface name
			);
	SendMessageA(hRichEdit,WM_SETFONT,(WPARAM)m_Fontx,MAKELONG(0,0));
	//SendMessageA(hRichEdit,EM_SETOPTIONS,ECOOP_OR,ECO_WANTRETURN);

	oEditProc = (WNDPROC)SetWindowLongA(hRichEdit,GWL_WNDPROC,(LONG)EditProc);

	//查找文本功能

	//内存->text ,text->内存


	::SetDlgItemTextA(hWnd,IDC_STATIC2,"/");
}
HWND hFindWin = 0;
HWND hEleWin = 0;
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	hInst = hInstance;

	//char pszdest[32];
	//int d = 103242423;
	//sprintf(pszdest,"%f",*(float*)&d);
	//::MessageBoxA(NULL,pszdest,"",0);

	hmRichEdit = ::LoadLibraryA("riched20.dll");

	INT_PTR ret = DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,About,NULL);

	//void init_crc_table();

	//char buf[]="f79sd7f9sdf796xcvx6cv6sfsfsx";
	//int len = strlen(buf);

	//int crcv = ::encryptbuf(23,buf,len);
	//bool crcr = ::decryptbuf(23,crcv,crcv,buf,len);

	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{	
		case WM_DESTROY:			
			PostQuitMessage(0);			
			break;	
		
		case WM_INITDIALOG:
			{
				hWnd = hDlg;

				InitCommonControls(); 			

#ifndef ENCRPT 
				::EnableWindow(::GetDlgItem(hDlg,IDC_CHECK1),FALSE);
#endif

				::SetWindowPos(hWnd,0,300,150,0,0,SWP_NOSIZE);

				CreateRichEdit();

				
			}

			return (INT_PTR)TRUE;
			break;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDC_BUTTON1)
			{
				_beginthread(PVFInit,0,0);
			}
		
			if(LOWORD(wParam) == IDC_BUTTON7)
			{
				ExplotEntry();
			}

			if(LOWORD(wParam) == IDC_BUTTON6)
			{
				_beginthread(SaveEntry,0,0);
			}

			if(LOWORD(wParam) == IDC_BUTTON8)
			{
				ImportEntry();
			}

			if(LOWORD(wParam) == IDC_BUTTON5)
			{
				_beginthread(SaveMem,0,0);
			}

			if(LOWORD(wParam) == IDC_BUTTON4)
			{
				//TEA_Code();
				Expand_Node();
			}
			if(LOWORD(wParam) == IDC_BUTTON2)
			{
				//ItemList();
				Expand_Node("etc/newcashshop.etc");
			}
			if(LOWORD(wParam) == IDC_BUTTON3)
			{
				{
					if(hEleWin==0)
					{
						hEleWin = CreateDialogParamA(hInst,(CHAR*)IDD_DIALOG3,hWnd,EleProc,0);
					}
					RECT rect;
					::GetWindowRect(hRichEdit,&rect);
					::SetWindowPos(hEleWin,HWND_TOP,rect.left,rect.top,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

					::SetFocus(::GetDlgItem(hEleWin,IDC_EDIT1));
				}
			}
			//

			if(LOWORD(wParam) == IDC_CHECK1)
			{
				//::MessageBox(NULL,"","",0);
				if(::SendDlgItemMessageA(hDlg,IDC_CHECK1,BM_GETCHECK,0,0)==BST_CHECKED)
				{
					is_use_encrypt=1;
				}else{
					is_use_encrypt=0;
				}
			}





			if(LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			
			break;
		case WM_NOTIFY:
			{
				  if(IDC_TREE1 == LOWORD(wParam))
				  {
					if( ((LPNMHDR)lParam)->code == NM_DBLCLK )
					{
						if(int selEntry = GetTreeSel())
						{
							DispEntry(selEntry);		
						}
					}
				  }
				  break;
			}
		default:
			break;
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK EditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHARRANGE chrg;

	switch (message)
	{
	case WM_KEYDOWN:
			if(GetAsyncKeyState(VK_CONTROL)&0x8000)
			{
				if('F'==wParam)
				{				
					if(hFindWin==0)
					{
						hFindWin = CreateDialogParamA(hInst,(CHAR*)IDD_DIALOG2,hWnd,FindProc,0);
					}
					RECT rect;
					::GetWindowRect(hRichEdit,&rect);
					::SetWindowPos(hFindWin,HWND_TOP,(rect.left+rect.right)/2,(rect.top+rect.bottom)/2,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

					::SetFocus(::GetDlgItem(hFindWin,IDC_EDIT1));

					return FALSE;
				}if('E'==wParam)
				{				
					if(hEleWin==0)
					{
						hEleWin = CreateDialogParamA(hInst,(CHAR*)IDD_DIALOG3,hWnd,EleProc,0);
					}
					RECT rect;
					::GetWindowRect(hRichEdit,&rect);
					::SetWindowPos(hEleWin,HWND_TOP,(rect.left+rect.right)/2,(rect.top+rect.bottom)/2,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

					::SetFocus(::GetDlgItem(hEleWin,IDC_EDIT1));

					return FALSE;
				}else if('Z'==wParam)
				{
					SendMessage(hRichEdit,EM_UNDO,0,0);
				}else if('Y'==wParam)
				{
					SendMessage(hRichEdit,EM_REDO,0,0);
				}else if('A'==wParam)
				{
					chrg.cpMin=0;
					chrg.cpMax=-1;
					SendMessageA(hRichEdit,EM_EXSETSEL,0,(LPARAM)&chrg);
				}else if('C'==wParam)
				{
					SendMessageA(hRichEdit,WM_COPY,0,0);
				}else if('X'==wParam)
				{
					SendMessageA(hRichEdit,WM_CUT,0,0);
				}else if('V'==wParam)
				{
					SendMessageA(hRichEdit,WM_PASTE,0,0);
				}

				return FALSE;
			}
		
		
			break;
		default:
			break;
	}

	return oEditProc(hDlg,message,wParam,lParam);
}


INT_PTR CALLBACK FindProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char szKey[64];
	char szRep[64];
	char szLin[32];
	FINDTEXTEX findtext;
	SETTEXTEX settext;

	switch (message)
	{					
		case WM_COMMAND:
			
			if(LOWORD(wParam) == IDC_BUTTON1 || IDC_BUTTON2 == LOWORD(wParam))
			{	
				bool bBakward =  SendDlgItemMessageA(hDlg,IDC_CHECK1,BM_GETCHECK,0,0)==BST_CHECKED;
				bool bCase =  SendDlgItemMessageA(hDlg,IDC_CHECK2,BM_GETCHECK,0,0)==BST_CHECKED;
				int ifnd = -1;
				::GetDlgItemTextA(hDlg,IDC_EDIT1,szKey,64-1);

				findtext.chrg.cpMin=0;
				findtext.chrg.cpMax=-1;
				findtext.lpstrText = szKey;

				SendMessageA(hRichEdit,EM_EXGETSEL,0,(LPARAM)&findtext.chrg);
				if(bBakward)
				{					
					findtext.chrg.cpMax=-1;
					ifnd = SendMessageA(hRichEdit,EM_FINDTEXTEX,(bCase?FR_MATCHCASE:0),(LPARAM)&findtext);
				}else{					
					findtext.chrg.cpMin=findtext.chrg.cpMax;
					findtext.chrg.cpMax=-1;					
					ifnd = SendMessageA(hRichEdit,EM_FINDTEXTEX,FR_DOWN|(bCase?FR_MATCHCASE:0),(LPARAM)&findtext);
				}				
				if(ifnd!=-1)
				{
					SendMessageA(hRichEdit,EM_EXSETSEL,0,(LPARAM)&findtext.chrgText);
				}else{
					::MessageBox(hDlg,"未找到.","Find",0);
				}

				if(IDC_BUTTON2 == LOWORD(wParam) && ifnd!=-1)
				{
					SendMessageA(hRichEdit,EM_EXSETSEL,0,(LPARAM)&findtext.chrgText);

					GetDlgItemTextA(hDlg,IDC_EDIT3,szRep,64-1);
				
					settext.flags = ST_SELECTION;
					settext.codepage = CP_ACP;
					
					SendMessage(hRichEdit,EM_SETTEXTEX,(WPARAM)&settext,(LPARAM)szRep);
					SetFocus(hRichEdit);
				}				
			}else if(LOWORD(wParam) == IDC_BUTTON4)
			{
				GetDlgItemTextA(hDlg,IDC_EDIT3,szLin,32-1);
				int iLin = atoi(szLin);
				int linNum = SendMessage(hRichEdit,EM_GETLINECOUNT,0,0);
				iLin = iLin<linNum?iLin:linNum-1;
				if(iLin<0)
				{
					iLin=0;
				}
				itoa(iLin,szLin,10);
				::SetDlgItemTextA(hDlg,IDC_EDIT3,szLin);

				int ilinPos = SendMessage(hRichEdit,EM_LINEINDEX,iLin,0);
				SendMessage(hRichEdit,EM_SETSEL,ilinPos,ilinPos);
				SetFocus(hRichEdit);
			}else if(LOWORD(wParam) == IDC_BUTTON3)
			{
				bool bBakward =  SendDlgItemMessageA(hDlg,IDC_CHECK1,BM_GETCHECK,0,0)==BST_CHECKED;
				bool bCase =  SendDlgItemMessageA(hDlg,IDC_CHECK2,BM_GETCHECK,0,0)==BST_CHECKED;
				int ifnd = -1;
				::GetDlgItemTextA(hDlg,IDC_EDIT1,szKey,64-1);

				findtext.chrg.cpMin=0;
				findtext.chrg.cpMax=-1;
				findtext.lpstrText = szKey;

				do
				{
					SendMessageA(hRichEdit,EM_EXGETSEL,0,(LPARAM)&findtext.chrg);
					if(bBakward)
					{					
						findtext.chrg.cpMax=-1;
						ifnd = SendMessageA(hRichEdit,EM_FINDTEXTEX,(bCase?FR_MATCHCASE:0),(LPARAM)&findtext);
					}else{					
						findtext.chrg.cpMin=findtext.chrg.cpMax;
						findtext.chrg.cpMax=-1;					
						ifnd = SendMessageA(hRichEdit,EM_FINDTEXTEX,FR_DOWN|(bCase?FR_MATCHCASE:0),(LPARAM)&findtext);
					}
					
					if(ifnd!=-1)
					{
						SendMessageA(hRichEdit,EM_EXSETSEL,0,(LPARAM)&findtext.chrgText);
						
						GetDlgItemTextA(hDlg,IDC_EDIT3,szRep,64-1);
					
						settext.flags = ST_SELECTION;
						settext.codepage = CP_ACP;
						
						SendMessage(hRichEdit,EM_SETTEXTEX,(WPARAM)&settext,(LPARAM)szRep);						
					}	
				}while(ifnd!=-1);
				SetFocus(hRichEdit);
			}
			
			if(LOWORD(wParam) == IDCANCEL)
			{
				::ShowWindow(hFindWin,0);
				return (INT_PTR)TRUE;
			}
			
			break;
	
		default:
			break;
	}

	return (INT_PTR)FALSE;
}

void SearchItem(HWND hWin,HWND hList,char* key)
{
	char szt[128];

	::SetFocus(hList);
	//::SendMessageA(hList,LVM_SETITEMTEXT,irow,(LPARAM)&lvItem);

	//
	string_kor_str* pTab = &equipment_lst_tab;
	//bool b1 =  SendDlgItemMessageA(hWin,IDC_RADIO1,BM_GETCHECK,0,0)==BST_CHECKED;
	bool b2 =  SendDlgItemMessageA(hWin,IDC_RADIO2,BM_GETCHECK,0,0)==BST_CHECKED;
	bool b3 =  SendDlgItemMessageA(hWin,IDC_RADIO3,BM_GETCHECK,0,0)==BST_CHECKED;
	bool b4 =  SendDlgItemMessageA(hWin,IDC_RADIO4,BM_GETCHECK,0,0)==BST_CHECKED;
	/*if(b1)
	{
		pTab = equipment_lst_tab;
	}*/
	if(b2)
	{
		pTab = &stackable_lst_tab;
	}
	if(b3)
	{
		pTab = &npc_lst_tab;
	}
	if(b4)
	{
		pTab = &itemshop_lst_tab;
	}
	Hashtable* p=pTab->ph;

	SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);
			
	for(int i=0;i<p->MaxSize;i++)
	{
		char* e = p->pInteralArray[i].szkey;
		char* v = (char*)p->pInteralArray[i].value;
		if(NULL == e)
		{
			continue;
		}
		if(strstr(v,key))
		{
			hash_pair* pp = pTab->ph_ls->Find(e);
			if(pp)
			{
				sprintf(szt,"%s:%s %s",e,v,(char*)pp->value);
			}else{
				sprintf(szt,"%s:%s",e,v);
			}
			//appendtext2rhtxt(hEdit,szt);
			//SendMessage(hEdit,LB_ADDSTRING ,0,(LPARAM)szt); 
			{
				LVITEM lvItem={0};
				
				int irow = ::SendMessageA(hList,LVM_INSERTITEM,0,(LPARAM)&lvItem);							
					
				lvItem.iSubItem = 0;				
				lvItem.pszText=e;
				::SendMessageA(hList,LVM_SETITEMTEXT,irow,(LPARAM)&lvItem);

				lvItem.iSubItem = 1;				
				lvItem.pszText=v;
				::SendMessageA(hList,LVM_SETITEMTEXT,irow,(LPARAM)&lvItem);
		
				lvItem.iSubItem = 2;				
				lvItem.pszText=(char*)pp->value;
				::SendMessageA(hList,LVM_SETITEMTEXT,irow,(LPARAM)&lvItem);


			}
		}
	}

}

HWND hList;
INT_PTR CALLBACK EleProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char szKey[32];
	switch (message)
	{					
		//
		case WM_INITDIALOG:
			SendDlgItemMessageA(hDlg,IDC_RADIO1,BM_SETCHECK,1,0);
			
			{
				RECT rect;
				::GetWindowRect(hDlg,&rect);

				hList = CreateWindowExA(NULL, TEXT("SysListView32"), NULL, LVS_REPORT | WS_CHILD | WS_VISIBLE
					, 0, 40, rect.right-rect.left-5, rect.bottom-rect.top-68, hDlg, (HMENU)2001, hInst, NULL);
				
				LV_COLUMN colmn;
				colmn.mask = LVCF_TEXT + LVCF_FMT + LVCF_WIDTH;// + LVCF_SUBITEM;
				colmn.fmt = LVCFMT_LEFT;

				colmn.pszText = "ID";
				colmn.cx = 60;
				::SendMessageA(hList,LVM_INSERTCOLUMN,0,(LPARAM)&colmn);
				colmn.pszText = "名称";
				colmn.cx = 150;
				::SendMessageA(hList,LVM_INSERTCOLUMN,1,(LPARAM)&colmn);
				colmn.pszText = "路径";
				colmn.cx = rect.right-rect.left-80-150;
				::SendMessageA(hList,LVM_INSERTCOLUMN,2,(LPARAM)&colmn);

				::SendMessageA(hList,WM_SETFONT,(WPARAM)m_Fontx,MAKELONG(0,0));

				ListView_SetExtendedListViewStyle(hList,LVS_EX_FULLROWSELECT);

		/*		SendMessage(hList, LVM_SETTEXTCOLOR, 0, 0xffffff);
				SendMessage(hList, LVM_SETBKCOLOR, 0, 0x646464);
				SendMessage(hList, LVM_SETTEXTBKCOLOR, 0, 0x000000);*/
			}
			break;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDCANCEL)
			{
				::ShowWindow(hDlg,0);
				return (INT_PTR)TRUE;
			}
			if(LOWORD(wParam) == IDOK)
			{
				::GetDlgItemTextA(hDlg,IDC_EDIT1,szKey,32);
				SearchItem(hDlg,hList,szKey);				
				return (INT_PTR)TRUE;
			}
			break;
		case WM_NOTIFY:
			{
				  if(2001 == LOWORD(wParam))
				  {
					if( ((LPNMHDR)lParam)->code == NM_DBLCLK )
					{
						int iItem = ((LPNMITEMACTIVATE)lParam)->iItem;
						//int iSubItem=((LPNMITEMACTIVATE)lParam)->iSubItem;

						char sz[512];
						//sprintf(sz,"%d-%d",iItem,iSubItem);
						//::MessageBoxA(NULL,sz,"",0);
						
						LV_ITEM _macro_lvi;
						_macro_lvi.iSubItem = 2;
						_macro_lvi.cchTextMax = 512;
						_macro_lvi.pszText = sz;											
						::SendMessageA(hList,LVM_GETITEMTEXT,iItem,(LPARAM)(LV_ITEM *)&_macro_lvi);		

						//::MessageBoxA(NULL,sz,"",0);
						Expand_Node(sz);

					}
				  }
				  break;
			}
		default:
			break;
	}
	return (INT_PTR)FALSE;
}
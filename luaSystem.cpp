#pragma warning(disable : 4996)
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <D3DX8.h>
#include <DXErr8.h>
#include <tchar.h>
#include <dinput.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "DIUtil.h"
#include "DMUtil.h"
#include "DSUtil.h"
#include "DXUtil.h"
#include "D3Dtypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>
#include "string.h"
#include "ctype.h"
#include "readData.hpp"
#include "luaSystem.hpp"
#include "luaSub.hpp"
#include "GDPlay.hpp"
#include "GPlayers.h"

//#include "c99_snprintf.h"

//メモリリーク検出用
#include <crtdbg.h>  
#ifdef _DEBUG 
#ifdef __MEMLEAKCHECK 
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__) 
#define malloc(p1) _malloc_dbg((p1),_NORMAL_BLOCK,__FILE__,__LINE__) 
#define __MEMLEAKCHECK
#endif
#endif 
//--メモリリーク検出用

extern CMyD3DApplication* g_pApp;
extern char szSystemFileName[];
extern char szSystemFileName0[];
extern GFloat luaL3dx,luaL3dy,luaL3dz;
extern int luaGraColor;
extern GVector CompassTarget;
extern int ViewUpdate;
extern DWORD LoadlibDummy;


char SystemOutput[GOUTPUTMAX][GOUTPUTMAXCHAR];
char *SystemSource=NULL;
int SystemErrorCode;
char SystemErrorStr[GOUTPUTMAXCHAR];
lua_State *SystemL=NULL;

extern int scenarioCode;


extern bool GravityFlag;
extern bool AirFlag;
extern bool TorqueFlag;
extern bool JetFlag;
extern bool UnbreakableFlag;
extern bool ScriptFlag;
extern bool CCDFlag;
extern bool EfficientFlag;

extern GRing Ring[];

extern int ChipCount;
extern GFloat Zoom;

extern GDPlay *DPlay;
extern GPLAYERDATA PlayerData[];
extern GMYDATA MyPlayerData;



#define FILEMAX 32
FILE *FpTable[32];
extern bool ControlKeysLock[];//0:Init,1:Reset,2:Open,3:Update,4:OpenLand,5:OpenGame,6:YForce,7:Title

int luaControlKeyLock(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int f=(int)lua_tonumber(L, 2);
	if(n>=8) return 0;
	if(n<0) {
		for(int i=0;i<8;i++) ControlKeysLock[i]=(f!=0);
	}
	else {
		ControlKeysLock[n]=(f!=0);
	}
	return 0;
}
int OpenFp(char *name,char *mode){
	for(int i=0;i<FILEMAX;i++) {
		if(FpTable[i]==NULL) {
			FpTable[i]=fopen(name,mode);return i;
		}
	}
	return -1;
}
int CloseFp(int n){
	if(n<0 || n>=FILEMAX) return 2;
	if(FpTable[n]) {
		fclose(FpTable[n]);
		FpTable[n]=NULL;
		return 0;
	}
	return 1;
}

// Luaに登録するCの関数は、intを返し、lua_State*を受け取るものである必要がある。
// 返す値は戻り値の数。この場合数値を1個返す（スタックに積む）ので1。

int luaSendAllMessage(lua_State *L)
{
	char *str=(char *)lua_tostring(L, 1);
	size_t len=lua_strlen(L, 1);
	if(len>MESSAGEMAX){len=MESSAGEMAX;}
	memcpy(MessageData,str,len);
	MessageData[len]=0x00; //len>MESSAGEMAXの時終端文字が切れるため  終端文字自体いらない気はする
	MessageDataLen=len;
	return 0;
}
int luaReceiveMessage(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=DPlay->GetNumPlayers() || scenarioCode!=RecieaveMessageCode[no]) {
		lua_pushstring(L,"");
		return 1;
	}
	lua_pushlstring(L,RecieaveMessageData[no],RecieaveMessageDataLen[no]);
	return 1;
}
int luaReceiveMessageClear(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=DPlay->GetNumPlayers()) return 0;
	RecieaveMessageData[no][0]='\0';
	RecieaveMessageDataLen[no]=0;
	return 0;
}

int luaFileOpen(lua_State *L)
{
	char str[_MAX_PATH];
	char *name=(char *)lua_tostring(L, 1);
	char *mode=(char *)lua_tostring(L, 2);
	char szDrive[_MAX_DRIVE + 1];	// ドライブ名格納領域 
	char szPath [_MAX_PATH + 1];	// パス名格納領域 
	char szTitle[_MAX_FNAME + 1];	// ファイルタイトル格納領域 
	char szExt  [_MAX_EXT + 1];		// ファイル拡張子格納領域 

	// 絶対パスを分解 
	_splitpath ( name, 
				szDrive, szPath, 
				szTitle, szExt);
	lstrcpy(str,CurrScenarioDir);
	lstrcat(str,"\\");
	lstrcat(str,szTitle);
	lstrcat(str,szExt);
	lua_pushnumber(L,OpenFp(str,mode));
	return 1;
}
int luaFileClose(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	lua_pushnumber(L,CloseFp(no));
	return 1;
}
int luaFilePuts(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	char *str=(char *)lua_tostring(L, 2);
	int ret=-1;
	if(no>=0 && no<FILEMAX && FpTable[no]) ret=fputs(str,FpTable[no]);
	lua_pushnumber(L,ret);
	return 1;
}
int luaFileGets(lua_State *L)
{
	char buf[1026];
	buf[0]='\0';
	int no=(int)lua_tonumber(L, 1);
	if(no>=0 && no<FILEMAX && FpTable[no]) fgets(buf,1024,FpTable[no]);
	lua_pushstring(L,buf);
	return 1;
}

int luaReset(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) return 0;
	World->RestoreLink(Chip[no],Chip[no]->Top);
	Chip[no]->CalcTotalCenter();
	return 0;
}
int luaSetTarget(lua_State *L)
{
	CompassTarget.x=(GFloat)lua_tonumber(L, 1);
	CompassTarget.y=(GFloat)lua_tonumber(L, 2);
	CompassTarget.z=(GFloat)lua_tonumber(L, 3);
	return 0;
}
int luaKeyLock(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int f=(int)lua_tonumber(L, 2);
	if(n>=GKEYMAX) return 0;
	if(n<0) {
		for(int i=0;i<GKEYMAX;i++) KeyList[i].Lock=f;
	}
	else {
		KeyList[n].Lock=f;
	}
	return 0;
}
int luaGetSystemTickCount(lua_State *L)
{
    lua_pushnumber(L, SystemTickCount );
    return 1;
}
int luaSetRegulationFlag(lua_State *L)
{
	char*str=(char *)lua_tostring(L, 1);
	bool b=lua_tonumber(L, 2)==0?false:true;
	switch(str[0]) {
		case 'G':
			if(GravityFlag!=b) {
				GravityFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
		case 'A':
			if(AirFlag!=b) {
				AirFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
		case 'T':
			if(TorqueFlag!=b) {
				TorqueFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
		case 'J':
			if(JetFlag!=b) {
				JetFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
		case 'U':
			if(UnbreakableFlag!=b) {
				UnbreakableFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
		case 'C':
			if(CCDFlag!=b) {
				CCDFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
		case 'S':
			if(ScriptFlag!=b) {
				ScriptFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
		case 'E':
			if(EfficientFlag!=b) {
				EfficientFlag=b;
				g_pApp->SetRegulationMenu();
			}
			break;
	}

	return 0;
}
int luaLoadLand(lua_State *L)
{
	char *str=(char*)lua_tostring(L, 1);
	int r=0;
	char st[_MAX_PATH];
	//if(strcmp(szLandFileName0,str)!=0) {
		//char *s=SearchFolder(CurrDataDir,str,st);
		//if(s==NULL) {
			char *s=SearchFolder(DataDir,str,st);
			if(s==NULL) {
		//		s=SearchFolder(ResourceDir,str,st);
		//		if(s==NULL) {
					lua_pushnumber(L,0);
					return 1;
		//		}
			}
		//}

  
		r=LoadLand(G3dDevice, s);
		if(r==0) {
			char szDrive[_MAX_DRIVE + 1];	// ドライブ名格納領域 
			char szPath [_MAX_PATH + 1];	// パス名格納領域 
			char szTitle[_MAX_FNAME + 1];	// ファイルタイトル格納領域 
			char szExt  [_MAX_EXT + 1];		// ファイル拡張子格納領域 

			// 絶対パスを分解 
			_splitpath ( str, 
						szDrive, szPath, 
						szTitle, szExt);
			lstrcpy(szLandFileName,str);
			lstrcpy(szLandFileName0,szTitle);
			lstrcat(szLandFileName0,szExt);
			GFloat y=World->Land->GetY(0,0);
			Chip[0]->CalcTotalCenter();
			Chip[0]->X=GVector(0,Chip[0]->Top->TotalRadius*2+2+y,0);
			Chip[0]->R=GMatrix33();
			World->RestoreLink(Chip[0],Chip[0]);
			if(Chip[0]->X.y<=-100000.0f)Chip[0]->X.y=0.0f;
			m_pLandMesh->InvalidateDeviceObjects();
			m_pLandMesh->RestoreDeviceObjects(G3dDevice);
		}
	//}
	World->MainStepCount=-1;

	lua_pushnumber(L,r);
	return 1;
}
int luaSaveChips(lua_State *L)
{
	char *fn=(char *)lua_tostring(L, 1);
	char str[512];
	lstrcpy(str,CurrDataDir);
	lstrcat(str,TEXT("\\"));
	lstrcat(str,fn);
	int e=saveData(str);
	lua_pushnumber(L,e);
	return 1;
}
int luaUpdateChips(lua_State *L)
{
	GFloat x=Chip[0]->X.x,z=Chip[0]->X.z;
	int errCode=0;
	if((errCode=readData(szUpdateFileName,true))==0) {
		readData(szUpdateFileName,false);
		if(ChipCount==0) errCode=-1;
//		if(SystemL!=NULL) luaSystemRun("OnOpenChips");
		if(m_pLandMesh) g_pApp->ResetChips(x,z,0);
	}
	lua_pushnumber(L,errCode);
	return 1;
}
int luaWind(lua_State *L)
{
	GFloat x=(GFloat)lua_tonumber(L, 1);
	GFloat y=(GFloat)lua_tonumber(L, 2);
	GFloat z=(GFloat)lua_tonumber(L, 3);

	AirSpeed=GVector(x,y,z);
	return 0;
}
int luaGetWind(lua_State *L)
{
	lua_pushnumber(L,AirSpeed.x);
	lua_pushnumber(L,AirSpeed.y);
	lua_pushnumber(L,AirSpeed.z);;
	return 3;
}
int luaAddBall(lua_State *L)
{
	GFloat r=(GFloat)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	GFloat d=(GFloat)lua_tonumber(L, 5);

	GRigid *rg=World->AddObject(GTYPE_BALL,false,r*2,r*2,r*2,d);
	if(rg) { 
		rg->X.x=x;
		rg->X.y=y;
		rg->X.z=z;
		rg->Ux=0.002f;
		rg->RSet();
		rg->CalcTotalCenter();
	//	ObjectBallFlag=TRUE;
		lua_pushnumber(L,rg->ID);
	}
	else lua_pushnumber(L,-1);
	return 1;
}
int luaSetObjFix(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	bool b=lua_tonumber(L, 2)==0?false:true;
	if(n<0 || n>=GOBJMAX) return 0;
	
	if(World->Object[n]) World->Object[n]->Fixed=b;
	return 0;
}
int luaSetObjColor(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat r=(GFloat)lua_tonumber(L, 2);
	GFloat g=(GFloat)lua_tonumber(L, 3);
	GFloat b=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		World->Object[n]->Color=GFloat((int)(r*255)*256*256+(int)(g*255)*256+(int)(b*255));
	}
	return 0;
}
int luaSetRingState(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int s=(int)lua_tonumber(L, 2);

	if(n>=0 && n<100) Ring[n].State=s;
	return 0;
}
int luaGetRingState(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int s=0;
	if(n>=0 && n<GRINGMAX) s=Ring[n].State;
	lua_pushnumber(L,s);
	return 1;
}
int luaSetRingColor(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat r=(GFloat)lua_tonumber(L, 2);
	GFloat g=(GFloat)lua_tonumber(L, 3);
	GFloat b=(GFloat)lua_tonumber(L, 4);
	if(n>=0 && n<GRINGMAX) {
		Ring[n].Color.x=r;
		Ring[n].Color.y=g;
		Ring[n].Color.z=b;
	}
	return 0;
}
int luaCheckRingArea(lua_State *L)
{
	int rn=(int)lua_tonumber(L, 1);	//リング番号
	int n=(int)lua_tonumber(L, 2);	//チップ番号
	int b=0;
	if((n>=0 && n<World->ChipCount) && (rn>=0 && rn<GRINGMAX)) {
		if((Ring[rn].Point-World->Rigid[n]->X).abs()<Ring[rn].Scale) b=1;
	}
	lua_pushnumber(L,b);
	return 1;
}
int luaCheckObjRingArea(lua_State *L)
{
	int rn=(int)lua_tonumber(L, 1);	//リング番号
	int n=(int)lua_tonumber(L, 2);	//オブジェクト番号
	int b=0;
	if((n>=0 && n<GOBJMAX) && (rn>=0 && rn<GRINGMAX) && World->Object[n]!=NULL) {
		if((Ring[rn].Point-World->Object[n]->X).abs()<Ring[rn].Scale) b=1;
	}
	lua_pushnumber(L,b);
	return 1;
}
int luaCollisionRingArea(lua_State *L)
{
	int rn=(int)lua_tonumber(L, 1);	//リング番号
	int n=(int)lua_tonumber(L, 2);	//チップ番号
	int b=0;
	if((n>=0 && n<World->ChipCount) && (rn>=0 && rn<GRINGMAX)) {
		GMatrix m=GMatrix().rotateX(Ring[rn].Dir.x*(GFloat)M_PI/180.0f).rotateY(Ring[rn].Dir.y*(GFloat)M_PI/180.0f).rotateZ(Ring[rn].Dir.z*(GFloat)M_PI/180.0f);
		GVector norm;
		norm.x=m.elem[2][0];
		norm.y=m.elem[2][1];
		norm.z=m.elem[2][2];
		GFloat t=World->Rigid[n]->preX.distanceOnFaceAndLine(norm,Ring[rn].Point,(World->Rigid[n]->X-World->Rigid[n]->preX));
		if(t>=0 && t<=1.0f) {
			GVector p=World->Rigid[n]->preX+(World->Rigid[n]->X-World->Rigid[n]->preX)*t;
			if((Ring[rn].Point-p).abs()<=Ring[rn].Scale) b=1;
		}
	}
	lua_pushnumber(L,b);
	return 1;
}
int luaCollisionObjRingArea(lua_State *L)
{
	int rn=(int)lua_tonumber(L, 1);	//リング番号
	int n=(int)lua_tonumber(L, 2);	//オブジェクト番号
	int b=0;
	if((n>=0 && n<GOBJMAX) && (rn>=0 && rn<GRINGMAX) && World->Object[n]!=NULL) {
		GMatrix m=GMatrix().rotateX(Ring[rn].Dir.x*(GFloat)M_PI/180.0f).rotateY(Ring[rn].Dir.y*(GFloat)M_PI/180.0f).rotateZ(Ring[rn].Dir.z*(GFloat)M_PI/180.0f);
		GVector norm;
		norm.x=m.elem[2][0];
		norm.y=m.elem[2][1];
		norm.z=m.elem[2][2];
		GFloat t=World->Object[n]->preX.distanceOnFaceAndLine(norm,Ring[rn].Point,(World->Object[n]->X-World->Object[n]->preX));
		if(t>=0 && t<=1.0f) {
			GVector p=World->Object[n]->preX+(World->Object[n]->X-World->Object[n]->preX)*t;
			if((Ring[rn].Point-p).abs()<=Ring[rn].Scale) b=1;
		}
	}
	lua_pushnumber(L,b);
	return 1;
}
int luaGetRing(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n>=0 && n<GRINGMAX) {
		lua_pushnumber(L,Ring[n].Point.x);
		lua_pushnumber(L,Ring[n].Point.y);
		lua_pushnumber(L,Ring[n].Point.z);;
		lua_pushnumber(L,Ring[n].Dir.x);
		lua_pushnumber(L,Ring[n].Dir.y);
		lua_pushnumber(L,Ring[n].Dir.z);
		lua_pushnumber(L,Ring[n].Scale);
		return 7;
	}
	return 0;
}
int luaSetRing(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	GFloat ax=(GFloat)lua_tonumber(L, 5);
	GFloat ay=(GFloat)lua_tonumber(L, 6);
	GFloat az=(GFloat)lua_tonumber(L, 7);
	GFloat r=(GFloat)lua_tonumber(L, 8);
	if(n>=0 && n<GRINGMAX) {
		Ring[n].Point.x=x;
		Ring[n].Point.y=y;
		Ring[n].Point.z=z;
		Ring[n].Dir.x=ax;
		Ring[n].Dir.y=ay;
		Ring[n].Dir.z=az;
		Ring[n].Scale=(float)r;
	}
	return 0;
}
int luaHitCountObj(lua_State *L)
{
	int hit=0;
	int n=(int)lua_tonumber(L, 1);
	char *type=(char*)lua_tostring(L, 2);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		if(strcmp(type,"OBJ")==0) hit=World->Object[n]->HitObj;
		if(strcmp(type,"CHIP")==0) hit=World->Object[n]->HitChip;
		if(strcmp(type,"BULLET")==0) hit=World->Object[n]->HitBullet;
		if(strcmp(type,"LAND")==0) hit=World->Object[n]->HitLand;
	}
	lua_pushnumber(L,hit);
	return 1;
}
int luaHitCount(lua_State *L)
{
	int hit=0;
	int n=(int)lua_tonumber(L, 1);
	char *type=(char*)lua_tostring(L, 2);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		if(strcmp(type,"OBJ")==0) hit=World->Rigid[n]->HitObj;
		if(strcmp(type,"BULLET")==0) hit=World->Rigid[n]->HitBullet;
		if(strcmp(type,"CHIP")==0) hit=World->Rigid[n]->HitChip;
		if(strcmp(type,"LAND")==0) hit=World->Rigid[n]->HitLand;
	}
	lua_pushnumber(L,hit);
	return 1;
}
int luaWarp(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		GVector v=GVector(x,y,z)-World->Rigid[n]->Top->X;
		World->Rigid[n]->Top->TranslateWithChild(v);
		World->Rigid[n]->Top->ResetXfWithChild();
		World->Rigid[n]->CalcTotalCenter();
		World->MainStepCount=-1;
	}
	return 0;
}
int luaWarpObj(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		GVector v=GVector(x,y,z)-World->Object[n]->X;
		World->Object[n]->TranslateWithChild(v);
		World->Object[n]->ResetXfWithChild();
		World->Object[n]->CalcTotalCenter();
		World->MainStepCount=-1;
	}
	return 0;
}
int luaRotate(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		GMatrix m=GMatrix().rotateX(x*(GFloat)M_PI/180.0f).rotateY(y*(GFloat)M_PI/180.0f).rotateZ(z*(GFloat)M_PI/180.0f);
		World->Rigid[n]->Top->RotateWithChild(m,World->Rigid[n]->Top->X);
		World->Rigid[n]->Top->ResetXfWithChild();
	}
	return 0;
}
int luaRotateObj(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		GMatrix m=GMatrix().rotateX(x*(GFloat)M_PI/180.0f).rotateY(y*(GFloat)M_PI/180.0f).rotateZ(z*(GFloat)M_PI/180.0f);
		World->Object[n]->RotateWithChild(m,World->Object[n]->X);
		World->Object[n]->ResetXfWithChild();
	}
	return 0;
}
int luaDirect(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		GMatrix m=GMatrix().rotateX(x*(GFloat)M_PI/180.0f).rotateY(y*(GFloat)M_PI/180.0f).rotateZ(z*(GFloat)M_PI/180.0f);
		World->Rigid[n]->Top->RotateWithChildAbs(m,World->Rigid[n]->Top->X);
		World->Rigid[n]->Top->ResetXfWithChild();
	}
	return 0;
}
int luaDirectObj(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		GMatrix m=GMatrix().rotateX(x*(GFloat)M_PI/180.0f).rotateY(y*(GFloat)M_PI/180.0f).rotateZ(z*(GFloat)M_PI/180.0f);
		World->Object[n]->RotateWithChildAbs(m,World->Object[n]->X);
		World->Object[n]->ResetXfWithChild();
	}
	return 0;
}
int luaEnervate(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		World->Rigid[n]->Top->EnervateWithChild();
	}
	return 0;
}
int luaEnervateObj(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		World->Object[n]->EnervateWithChild();
	}
	return 0;
}
int luaPause(lua_State *L)
{
	World->Stop=true;
	for(int i=0;i<ChipCount;i++) World->Rigid[i]->preX=World->Rigid[i]->X;
	for(int i=0;i<GOBJMAX;i++) if(World->Object[i]) World->Object[i]->preX=World->Object[i]->X;
	return 0;
}
int luaPlay(lua_State *L)
{
	World->Stop=false;
	return 0;
}
int luaGetSystemKey(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int s=0;
	if(n>=0 && n<GSYSKEYMAX) s=(int)SystemKeys[n];
	lua_pushnumber(L,s);
	return 1;
}
int luaGetSystemKeyDown(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int s=0;
	if(n>=0 && n<GSYSKEYMAX) s=(int)SystemKeysDown[n];
	lua_pushnumber(L,s);
	return 1;
}
int luaGetSystemKeyUp(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int s=0;
	if(n>=0 && n<GSYSKEYMAX) s=(int)SystemKeysUp[n];
	lua_pushnumber(L,s);
	return 1;
}
int luaApplyTorqueObj(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		World->Object[n]->ApplyTorque(GVector(x,y,z));
	}
	return 0;
}
int luaApplyForceObj(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=GOBJMAX) return 0;
	if(World->Object[n]) {
		World->Object[n]->ApplyForce(GVector(x,y,z),World->Object[n]->X);
	}
	return 0;
}
int luaApplyTorque(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		World->Rigid[n]->ApplyTorque(GVector(x,y,z));
	}
	return 0;
}
int luaApplyForce(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	GFloat x=(GFloat)lua_tonumber(L, 2);
	GFloat y=(GFloat)lua_tonumber(L, 3);
	GFloat z=(GFloat)lua_tonumber(L, 4);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		World->Rigid[n]->ApplyForce(GVector(x,y,z),World->Rigid[n]->X);
	}
	return 0;
}
int luaGetH(lua_State *L)
{
	D3DXVECTOR3 v1,v2;
	v1.x=(FLOAT)lua_tonumber(L, 1);
	v1.z=(FLOAT)lua_tonumber(L, 2);
	v1.y=100000.0f;
	v2.x=0;v2.y=-1;v2.z=0;
	
	int n=lua_gettop(L);
	if(n>2){
		v1.y=(FLOAT)lua_tonumber(L, 2);
		v1.z=(FLOAT)lua_tonumber(L, 3);
		v2.x=(FLOAT)lua_tonumber(L, 4);
		v2.y=(FLOAT)lua_tonumber(L, 5);
		v2.z=(FLOAT)lua_tonumber(L, 6);
	}
	
	BOOL hit;
	FLOAT dist;
	//LPDIRECT3DVERTEXBUFFER8 pVB;
	//LPDIRECT3DINDEXBUFFER8  pIB;
	//WORD*            pIndices;
	//D3DVERTEX*    pVertices;
	if(m_pLandMesh==NULL) {
		lua_pushnumber(L,-100000.0f);
		return 1;
	}
	//m_pLandMesh->GetSysMemMesh()->GetVertexBuffer( &pVB );
	//m_pLandMesh->GetSysMemMesh()->GetIndexBuffer( &pIB );
	//pIB->Lock( 0, 0, (BYTE**)&pIndices, D3DLOCK_READONLY );
	//pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_READONLY );
	D3DXIntersect(m_pLandMesh->GetSysMemMesh(),&v1,&v2,&hit,NULL,NULL,NULL,&dist,NULL,NULL);
	if(!hit) dist=-100000.0f;
	else if(n<=2) dist=100000.0f-dist;
	//pVB->Unlock();
	//pIB->Unlock();
	//pVB->Release();
	//pIB->Release();
	
	lua_pushnumber(L,dist);
	return 1;

}
int luaAddChip(lua_State *L)
{
	int rn,an;
	int dirCode=0;
	GVector offA,offB,axis[3];
	GLink *link1=NULL,*link2=NULL;
	axis[1]=GVector(0,1,0);
	
	int parentNo=(int)lua_tonumber(L, 1);
	char *type=(char*)lua_tostring(L, 2);
	char *news=(char*)lua_tostring(L, 3);
	GFloat angle=(GFloat)lua_tonumber(L, 4);

	if(parentNo<0 || parentNo>=ChipCount) {
		lua_pushnumber(L,-1);
		return 1;
	}

	if(strcmp(news,"N")==0) {
		offA=GVector(0,0,-CHIPSIZE/2);
		offB=GVector(-0,0,CHIPSIZE/2);
		axis[0]=GVector(-1,0,0);
		axis[2]=GVector(0,0,1);
		rn=2;
		dirCode=0x01;
	}
	else if(strcmp(news,"E")==0) {
		offA=GVector(-CHIPSIZE/2,0,0);
		offB=GVector(CHIPSIZE/2,0,0);
		axis[0]=GVector(0,0,1);
		axis[2]=GVector(1,0,0);
		rn=3;
		dirCode=0x02;
	}
	else if(strcmp(news,"W")==0) {
		offA=GVector(CHIPSIZE/2,0,0);
		offB=GVector(-CHIPSIZE/2,0,0);
		axis[0]=GVector(0,0,-1);
		axis[2]=GVector(-1,0,0);
		rn=1;
		dirCode=0x04;
	}
	else  {	//"S"
		offA=GVector(0,0,CHIPSIZE/2);
		offB=GVector(0,0,-CHIPSIZE/2);
		axis[0]=GVector(1,0,0);
		axis[2]=GVector(0,0,-1);
		rn=0;
		dirCode=0x08;
	}

	int flag=0;
	if(strcmp(type,"CHIP")==0) {
		MakeChip(GT_CHIP,rn);
		an=0;
		flag=2;
	}
	else if(strcmp(type,"FRAME")==0) {
		MakeChip(GT_CHIP2,rn);
		an=0;
		flag=2;
	}
	else if(strcmp(type,"WEIGHT")==0) {
		MakeChip(GT_CHIPH,rn);
		an=0;
		flag=2;
	}
	else if(strcmp(type,"COWL")==0) {
		MakeChip(GT_COWL,rn);
		an=0;
		flag=2;
	}
	else if(strcmp(type,"RUDDER")==0) {
		MakeChip(GT_RUDDER,rn);
		an=1;
		flag=1;
	}
	else if(strcmp(type,"RUDDERF")==0) {
		MakeChip(GT_RUDDER2,rn);
		an=1;
		flag=1;
	}
	else if(strcmp(type,"TRIM")==0) {
		MakeChip(GT_TRIM,rn);
		an=2;
		flag=1;
	}
	else if(strcmp(type,"TRIMF")==0) {
		MakeChip(GT_TRIM2,rn);
		an=2;
		flag=1;
	}
	else if(strcmp(type,"JET")==0) {
		MakeChip(GT_JET,rn);
		an=0;
		flag=1;
	}
	else if(strcmp(type,"ARM")==0) {
		MakeChip(GT_ARM,rn);
		an=0;
		flag=1;
	}
	if(flag>0) {
		int cno=ChipCount;
		Chip[parentNo]->DirCode|=dirCode;
		if(flag==2) {
			if(dirCode==0x01) {
				Chip[cno]->CheckShape.PointN=6;
				Chip[cno]->CheckShape.Point[0]=Chip[cno]->Shape.Point[1];
				Chip[cno]->CheckShape.Point[1]=Chip[cno]->Shape.Point[0];
				Chip[cno]->CheckShape.Point[2]=Chip[cno]->Shape.Point[2];
				Chip[cno]->CheckShape.Point[3]=Chip[cno]->Shape.Point[5];
				Chip[cno]->CheckShape.Point[4]=Chip[cno]->Shape.Point[6];
				Chip[cno]->CheckShape.Point[5]=Chip[cno]->Shape.Point[8];
			}
			else if(dirCode==0x02) {
				Chip[cno]->CheckShape.PointN=6;
				Chip[cno]->CheckShape.Point[0]=Chip[cno]->Shape.Point[2];
				Chip[cno]->CheckShape.Point[1]=Chip[cno]->Shape.Point[1];
				Chip[cno]->CheckShape.Point[2]=Chip[cno]->Shape.Point[3];
				Chip[cno]->CheckShape.Point[3]=Chip[cno]->Shape.Point[6];
				Chip[cno]->CheckShape.Point[4]=Chip[cno]->Shape.Point[7];
				Chip[cno]->CheckShape.Point[5]=Chip[cno]->Shape.Point[8];
			}
			else if(dirCode==0x04) {
				Chip[cno]->CheckShape.PointN=6;
				Chip[cno]->CheckShape.Point[0]=Chip[cno]->Shape.Point[0];
				Chip[cno]->CheckShape.Point[1]=Chip[cno]->Shape.Point[3];
				Chip[cno]->CheckShape.Point[2]=Chip[cno]->Shape.Point[1];
				Chip[cno]->CheckShape.Point[3]=Chip[cno]->Shape.Point[4];
				Chip[cno]->CheckShape.Point[4]=Chip[cno]->Shape.Point[5];
				Chip[cno]->CheckShape.Point[5]=Chip[cno]->Shape.Point[8];
			}
			else if(dirCode==0x08) {
				Chip[cno]->CheckShape.PointN=6;
				Chip[cno]->CheckShape.Point[0]=Chip[cno]->Shape.Point[3];
				Chip[cno]->CheckShape.Point[1]=Chip[cno]->Shape.Point[2];
				Chip[cno]->CheckShape.Point[2]=Chip[cno]->Shape.Point[0];
				Chip[cno]->CheckShape.Point[3]=Chip[cno]->Shape.Point[4];
				Chip[cno]->CheckShape.Point[4]=Chip[cno]->Shape.Point[7];
				Chip[cno]->CheckShape.Point[5]=Chip[cno]->Shape.Point[8];
			}
			Chip[cno]->SaveShape=Chip[cno]->CheckShape;
		}
		else {
			Chip[cno]->CheckShape=Chip[cno]->Shape;
			Chip[cno]->SaveShape=Chip[cno]->Shape;
		}
		if(strcmp(type,"COWL")==0) link2=World->AddCowl(Chip[parentNo],offA,Chip[ChipCount],offB,axis[an],angle);
		else link2=World->AddHinge(Chip[parentNo],offA,Chip[ChipCount],offB,axis[an],angle,1,0.5);
		ChipCount++;if(ChipCount>=GCHIPMAX) ChipCount=GCHIPMAX-1;
	}
	else {
		if(strcmp(type,"WHEEL")==0) {
			int cno2=ChipCount;
			MakeChip(GT_DUMMY,rn);
			Chip[cno2]->CheckShape=Chip[cno2]->Shape;
			Chip[cno2]->SaveShape=Chip[cno2]->Shape;
			link2=World->AddHinge(Chip[parentNo],offA,Chip[cno2],offB,axis[0],angle,1.0,0.5);
			ChipCount++;if(ChipCount>=GCHIPMAX) ChipCount=GCHIPMAX-1;
			int cno=ChipCount;
			MakeChip(GT_WHEEL,rn);
			Chip[cno]->CheckShape=Chip[cno]->Shape;
			Chip[cno]->SaveShape=Chip[cno]->Shape;
			Chip[parentNo]->DirCode|=dirCode;
			link1=World->AddShaft(Chip[cno2],GVector(0,0,0),Chip[cno],GVector(0,0,0),axis[1],0);
			ChipCount++;if(ChipCount>=GCHIPMAX) ChipCount=GCHIPMAX-1;
			an=2;
		}
		else if(strcmp(type,"RLW")==0) {
			int cno2=ChipCount;
			MakeChip(GT_DUMMY,rn);
			Chip[cno2]->CheckShape=Chip[cno2]->Shape;
			Chip[cno2]->SaveShape=Chip[cno2]->Shape;
			link2=World->AddHinge(Chip[parentNo],offA,Chip[cno2],offB,axis[0],angle,1.0,0.5);
			ChipCount++;if(ChipCount>=GCHIPMAX) ChipCount=GCHIPMAX-1;
			int cno=ChipCount;
			MakeChip(GT_RLW,rn);
			Chip[cno]->CheckShape=Chip[cno]->Shape;
			Chip[cno]->SaveShape=Chip[cno]->Shape;
			Chip[parentNo]->DirCode|=dirCode;
			link1=World->AddShaft(Chip[cno2],GVector(0,0,0),Chip[cno],GVector(0,0,0),axis[1],0);
			ChipCount++;if(ChipCount>=GCHIPMAX) ChipCount=GCHIPMAX-1;
			an=2;
		}
	}
	//World->CalcLink(Chip[parentNo]);
	World->RestoreLink(Chip[0],Chip[0]);
	lua_pushnumber(L,ChipCount-1);
	return 1;
}
int luaGetChild(lua_State *L)
{	
	int n=(int)lua_tonumber(L, 1);
	int cn=(int)lua_tonumber(L, 2);
	double value=-1;
	if(n>=0 && n<GCHILDMAX) {
		if(World->Rigid[n]->Child[n].RigidB) value=World->Rigid[n]->Child[n].RigidB->ID;
	}
	lua_pushnumber(L,value);
	return 1;
}
int luaGetChip(lua_State *L)
{	
	int n=(int)lua_tonumber(L, 1);
	char *name=(char*)lua_tostring(L, 2);
	double value=0.0;
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		if(strcmp(name,"DIR")==0) {
			value=World->Rigid[n]->Dir;
		}
		else if(strcmp(name,"EFFECT")==0) {
			value=World->Rigid[n]->Effect;
		}
		else if(strcmp(name,"OPTION")==0) {
			value=World->Rigid[n]->Option;			
		}
		else if(strcmp(name,"ANGLE")==0) {
			if(World->Rigid[n]->LinkInfo) value=World->Rigid[n]->LinkInfo->Angle;
		}
		else if(strcmp(name,"POWER")==0) {
			value=World->Rigid[n]->Power;
		}
		else if(strcmp(name,"SPRING")==0) {
			if(World->Rigid[n]->LinkInfo) value=World->Rigid[n]->LinkInfo->SpringK;
		}
		else if(strcmp(name,"DAMPER")==0 || strcmp(name,"DUMPER")==0) {
			if(World->Rigid[n]->LinkInfo) value=World->Rigid[n]->LinkInfo->DamperK;
		}
		else if(strcmp(name,"BRAKE")==0) {
			if(World->Rigid[n]->LinkInfo) value=World->Rigid[n]->LinkInfo->FrictionK;
		}
		else if(strcmp(name,"COLOR")==0) {
			value=World->Rigid[n]->Color;
		}
		else if(strcmp(name,"PARENT")==0) {
			if(World->Rigid[n]->Parent) value=World->Rigid[n]->Parent->ID;
			else value=-1;
		}
		else if(strcmp(name,"TOP")==0) {
			if(World->Rigid[n]->Top) value=World->Rigid[n]->Top->ID;
			else value=-1;
		}
		else if(strcmp(name,"USER1")==0) {
			value=World->Rigid[n]->UserEffect;
		}
		else if(strcmp(name,"USER2")==0) {
			value=World->Rigid[n]->UserOption;
		}
		else if(strcmp(name,"FUEL")==0) {
			value=World->Rigid[n]->Fuel;
		}
		else if(strcmp(name,"FUELMAX")==0) {
			value=World->Rigid[n]->FuelMax;
		}
		else if(strcmp(name,"Density")==0) {
			value=World->Rigid[n]->Density;
		}
		else if(strcmp(name,"T")==0) {
			value=World->Rigid[n]->Tolerant;
		}
		else if(strcmp(name,"E")==0) {
			value=World->Rigid[n]->Energy;
		}
	}
	lua_pushnumber(L,value);
	return 1;
}
int luaSetChip(lua_State *L)
{	
	int n=(int)lua_tonumber(L, 1);
	char *name=(char*)lua_tostring(L, 2);
	GFloat value=(GFloat)lua_tonumber(L, 3);
	if(n<0 || n>=World->ChipCount) return 0;
	if(World->Rigid[n]) {
		if(strcmp(name,"EFFECT")==0) {
			World->Rigid[n]->Effect=value;
		}
		else if(strcmp(name,"OPTION")==0) {
			setOption(World->Rigid[n],value);			
			if(World->Rigid[n]->ChipType==GT_WHEEL || World->Rigid[n]->ChipType==GT_RLW){
				int nn=World->Rigid[n]->Top->ID;
				for(int i=0;i<VarCount;i++) {
					ValList[i].Val=ValList[i].Def;
					if(ValList[i].Val>ValList[i].Max) ValList[i].Val=ValList[i].Max;
					if(ValList[i].Val<ValList[i].Min) ValList[i].Val=ValList[i].Min;
					ValList[i].Updated=true;
					for(int j=0;j<ValList[i].RefCount;j++) {
						if(ValList[i].Flag[j])
							*(ValList[i].Ref[j])=-ValList[i].Val;
						else *(ValList[i].Ref[j])=ValList[i].Val;
					}
				}
				ResetChip2(nn,0);
			}
		}
		else if(strcmp(name,"ANGLE")==0) {
			if(World->Rigid[n]->LinkInfo) World->Rigid[n]->LinkInfo->Angle=value;
		}
		else if(strcmp(name,"POWER")==0) {
			World->Rigid[n]->Power=value;
		}
		else if(strcmp(name,"SPRING")==0) {
			if(World->Rigid[n]->LinkInfo) World->Rigid[n]->LinkInfo->SpringK=value;
		}
		else if(strcmp(name,"DAMPER")==0||strcmp(name,"DUMPER")==0) {
			if(World->Rigid[n]->LinkInfo) World->Rigid[n]->LinkInfo->DamperK=value;
		}
		else if(strcmp(name,"BRAKE")==0) {
			if(World->Rigid[n]->LinkInfo) World->Rigid[n]->LinkInfo->FrictionK=value;
		}
		else if(strcmp(name,"COLOR")==0) {
			World->Rigid[n]->Color=value;
		}
		else if(strcmp(name,"USER1")==0) {
			World->Rigid[n]->UserEffect=value;
		}
		else if(strcmp(name,"USER2")==0) {
			World->Rigid[n]->UserOption=value;
		}
		else if(strcmp(name,"FUEL")==0) {
			World->Rigid[n]->Fuel=value;
		}
		else if(strcmp(name,"FUELMAX")==0) {
			World->Rigid[n]->FuelMax=value;
		}
		else if(strcmp(name,"Density")==0) {
			World->Rigid[n]->Density=value;
			World->Rigid[n]->SetTensor();
		}
		else if(strcmp(name,"T")==0) {
			World->Rigid[n]->Tolerant=value;
		}
		else if(strcmp(name,"E")==0) {
			World->Rigid[n]->Energy=value;
		}
	}
	return 0;
}

int luaSystemPrint(lua_State *L)
{
	int n=lua_gettop(L);
	if(n<1) return 0;
	int a=(int)lua_tonumber(L, 1);
	if(a<0 || a>=GOUTPUTMAX) return 0;

	__luaPrintSub(L, 2, n, SystemOutput[a], GOUTPUTMAXCHAR);
	return 0;
}
int LoadSystem(char *fileName) {
	FILE *fp;
	if((fp=fopen(fileName,"r"))!=NULL) {
		/*　ファイルの長さを調べる　*/
		fseek( fp, 0, SEEK_SET );
		int start_pos = ftell( fp );
		fseek( fp, 0, SEEK_END );
		int end_pos = ftell( fp );

		fseek( fp, 0, SEEK_SET );

		if(SystemSource) delete SystemSource;
		SystemSource= new char[end_pos-start_pos+1];
		SystemSource[0]='\0';
		int i=0;
		scenarioCode=0;
		while(feof( fp ) == 0) {
			SystemSource[i]=fgetc(fp);
			scenarioCode+=SystemSource[i];
			i++;
		}
		SystemSource[i-1]='\0';
		SystemSource[i]='\0';
		fclose(fp);
		return 0;
	}
	return 1;
}
int luaSystemInit() {
	SystemErrorStr[0]='\0';
	SystemErrorCode=0;
	if(SystemL) luaSystemEnd();
	for(int i=0;i<FILEMAX;i++) FpTable[i]=NULL;
	setlocale(LC_ALL, "");
    //
	SystemL = lua_open();  /* create state */
	
	// 関数を登録する(C13)
    lua_register(SystemL, "_CCD32", luaGetCCD32);
    lua_register(SystemL, "_GETFOGCOLOR", luaGetFogColor);
    lua_register(SystemL, "_SETFOGCOLOR", luaGetFogColor);
	// 関数を登録する(v1.5C)
    lua_register(SystemL, "_GETVIEWUP", luaGetViewUp);
    lua_register(SystemL, "_GETSPEEDLIM", luaGetSpeedLimit);
    lua_register(SystemL, "_SETSPEEDLIM", luaSetSpeedLimit);
    lua_register(SystemL, "_GETSPPEDLIM", luaGetSpeedLimit); //誤字ってたけどそのまま残す
    lua_register(SystemL, "_SETSPPEDLIM", luaSetSpeedLimit);
    lua_register(SystemL, "_GETFOGRANGE", luaGetFogRange);
    lua_register(SystemL, "_SETFOGRANGE", luaSetFogRange);
    lua_register(SystemL, "_GETNAMESIZE", luaGetNameSize);
    lua_register(SystemL, "_SETNAMESIZE", luaSetNameSize);
    lua_register(SystemL, "_GETMAKERSIZE", luaGetMakerSize);
    lua_register(SystemL, "_SETMAKERSIZE", luaSetMakerSize);
    lua_register(SystemL, "_FACEDATA", luaGetFaceData);
	lua_register(SystemL, "_RANGE",luaGetRange);
	lua_register(SystemL, "_NTICKS",luaGetNoiseTicks);
	lua_register(SystemL, "_SETSCLFUNC",luaSetScriptFunction);
    lua_register(SystemL, "_PLAYEREXTTIME", luaGetPlayerExtTime);
	lua_register(SystemL, "_PLAYERPOS",luaGetPlayerPos);
	//ｽﾋﾟ互換用
    lua_register(SystemL, "InitMTRand", luaRandInit);
    lua_register(SystemL, "MTRand", luaRand);
    lua_register(SystemL, "GetTickCount", luaGettimeGetTime);
    lua_register(SystemL, "GetHostName", luaGetHostName);
    lua_register(SystemL, "GetPortNumber", luaGetPortNumber);
    lua_register(SystemL, "_SETLIMITVELOCITY", luaSetSpeedLimit);
	//ﾀﾞﾐｰ関数
	if(LoadlibDummy ) lua_register(SystemL, "loadlib",luaDummyFuncFunc);
	lua_register(SystemL, "_EXFOPEN",luaDummyFunc1);
	lua_register(SystemL, "_EXFCLOSE",luaDummyFunc1);
	lua_register(SystemL, "_EXFPUTS",luaDummyFunc1);
	lua_register(SystemL, "_EXFGETS",luaDummyFunc1);
	// 関数を登録する(v1.5)
    lua_register(SystemL, "_CTRLLOCK", luaControlKeyLock);
    lua_register(SystemL, "_SENDALL", luaSendAllMessage);
    lua_register(SystemL, "_RECEIVE", luaReceiveMessage);
    lua_register(SystemL, "_RECEIVECLEAR", luaReceiveMessageClear);
    lua_register(SystemL, "_FOPEN", luaFileOpen);
    lua_register(SystemL, "_FCLOSE", luaFileClose);
    lua_register(SystemL, "_FPUTS", luaFilePuts);
    lua_register(SystemL, "_FGETS", luaFileGets);
	lua_register(SystemL, "_CHAT", luaGetLastChat);
	// 関数を登録する(v1.4)
    lua_register(SystemL, "_RESET", luaReset);
    lua_register(SystemL, "_COMPASS", luaSetTarget);
    lua_register(SystemL, "_SETVIEWUP", luaSetViewUp);
	// 関数を登録する(v1.3)
    lua_register(SystemL, "_KEYLOCK", luaKeyLock);
    lua_register(SystemL, "_STICKS", luaGetSystemTickCount);
    // 関数を登録する(v1.2)
    lua_register(SystemL, "_SETVIEW", luaSetView);
    lua_register(SystemL, "_GETVIEW", luaGetView);
    lua_register(SystemL, "_SETVIEWTYPE", luaSetViewType);
    lua_register(SystemL, "_GETVIEWTYPE", luaGetViewType);
    lua_register(SystemL, "_SETVIEWZOOM", luaSetViewZoom);
    lua_register(SystemL, "_GETVIEWZOOM", luaGetViewZoom);

    // 関数を登録する(v1.1)
    lua_register(SystemL, "_GET", luaGetChip);
    lua_register(SystemL, "_GETCHILD", luaGetChild);

    // 関数を登録する(v1.0)
    lua_register(SystemL, "out", luaSystemPrint);

    lua_register(SystemL, "_SETREG", luaSetRegulationFlag);

    lua_register(SystemL, "_LOADLAND", luaLoadLand);

    lua_register(SystemL, "_SETWIND", luaWind);
    lua_register(SystemL, "_GETWIND", luaGetWind);

	lua_register(SystemL, "_ADDBALL", luaAddBall);
    lua_register(SystemL, "_SETOBJFIX", luaSetObjFix);
    lua_register(SystemL, "_SETOBJCOLOR", luaSetObjColor);

    lua_register(SystemL, "_SETRINGSTATE", luaSetRingState);
    lua_register(SystemL, "_GETRINGSTATE", luaGetRingState);
    lua_register(SystemL, "_SETRING", luaSetRing);
    lua_register(SystemL, "_GETRING", luaGetRing);
    lua_register(SystemL, "_SETRINGCOLOR", luaSetRingColor);
    lua_register(SystemL, "_CHECK", luaCheckRingArea);
    lua_register(SystemL, "_CHECKOBJ", luaCheckObjRingArea);
    lua_register(SystemL, "_CHECK2", luaCollisionRingArea);
    lua_register(SystemL, "_CHECK2OBJ", luaCollisionObjRingArea);

    lua_register(SystemL, "_WARP", luaWarp);
    lua_register(SystemL, "_WARPOBJ", luaWarpObj);

    lua_register(SystemL, "_FORCE", luaApplyForce);
    lua_register(SystemL, "_FORCEOBJ", luaApplyForceObj);

    lua_register(SystemL, "_TORQUE", luaApplyTorque);
    lua_register(SystemL, "_TORQUEOBJ", luaApplyTorqueObj);

    lua_register(SystemL, "_GETHIT", luaHitCount);
    lua_register(SystemL, "_GETHITOBJ", luaHitCountObj);
	

    lua_register(SystemL, "_SET", luaSetChip);
	
    lua_register(SystemL, "_ROTATE", luaRotate);
    lua_register(SystemL, "_ROTATEOBJ", luaRotateObj);
    lua_register(SystemL, "_DIRECT", luaDirect);
    lua_register(SystemL, "_DIRECTOBJ", luaDirectObj);

    lua_register(SystemL, "_ENERVATE", luaEnervate);
    lua_register(SystemL, "_ENERVATEOBJ", luaEnervateObj);

    lua_register(SystemL, "_PAUSE", luaPause);
    lua_register(SystemL, "_PLAY", luaPlay);

    lua_register(SystemL, "_SKEY", luaGetSystemKey);
    lua_register(SystemL, "_SKEYDOWN", luaGetSystemKeyDown);
    lua_register(SystemL, "_SKEYUP", luaGetSystemKeyUp);

    lua_register(SystemL, "_GETY", luaGetH);

    lua_register(SystemL, "_ADDCHIP", luaAddChip);
    lua_register(SystemL, "_RELOAD", luaUpdateChips);
    lua_register(SystemL, "_SAVE", luaSaveChips);

	lua_register(SystemL, "_MX", luaGetSMouseX);
	lua_register(SystemL, "_MY", luaGetSMouseY);
	lua_register(SystemL, "_ML", luaGetSMouseL);
	lua_register(SystemL, "_MR", luaGetSMouseR);
	lua_register(SystemL, "_MM", luaGetSMouseM);

	lua_register(SystemL, "_DT", luaGetDt);
    lua_register(SystemL, "_FPS", luaGetFps);
    lua_register(SystemL, "_BASE", luaGetBase);
    lua_register(SystemL, "_TICKS", luaGetTickCount);
    lua_register(SystemL, "_SETTICKS", luaSetTicks);
    lua_register(SystemL, "_KEY", luaKey2);
    lua_register(SystemL, "_KEYDOWN", luaKeyDown2);
    lua_register(SystemL, "_KEYUP", luaKeyUp2);
    lua_register(SystemL, "_ANALOG", luaAnalog);
    lua_register(SystemL, "_HAT", luaHat);
    lua_register(SystemL, "_CHIPS", luaChips);
    lua_register(SystemL, "_WEIGHT", luaWeight);
    lua_register(SystemL, "_WIDTH", luaGetWidth);
    lua_register(SystemL, "_HEIGHT", luaGetHeight);
    lua_register(SystemL, "_FACE", luaGetFaces);
    lua_register(SystemL, "_ZOOM", luaSetCCDZoom);
    lua_register(SystemL, "_OX", luaObjPosx);
    lua_register(SystemL, "_OY", luaObjPosy);
    lua_register(SystemL, "_OZ", luaObjPosz);
    lua_register(SystemL, "_X", luaPosx);
    lua_register(SystemL, "_Y", luaPosy);
    lua_register(SystemL, "_Z", luaPosz);
    lua_register(SystemL, "_H", luaGetY);
    lua_register(SystemL, "_AX", luaAx);
    lua_register(SystemL, "_AY", luaAy);
    lua_register(SystemL, "_AZ", luaAz);
    lua_register(SystemL, "_EX", luaEx);
    lua_register(SystemL, "_EY", luaEy);
    lua_register(SystemL, "_EZ", luaEz);
    lua_register(SystemL, "_GX", luaGx);
    lua_register(SystemL, "_GY", luaGy);
    lua_register(SystemL, "_GZ", luaGz);
    lua_register(SystemL, "_XX", luaXx);
    lua_register(SystemL, "_XY", luaXy);
    lua_register(SystemL, "_XZ", luaXz);
    lua_register(SystemL, "_YX", luaYx);
    lua_register(SystemL, "_YY", luaYy);
    lua_register(SystemL, "_YZ", luaYz);
    lua_register(SystemL, "_ZX", luaZx);
    lua_register(SystemL, "_ZY", luaZy);
    lua_register(SystemL, "_ZZ", luaZz);
    lua_register(SystemL, "_QX", luaQx);
    lua_register(SystemL, "_QY", luaQy);
    lua_register(SystemL, "_QZ", luaQz);
    lua_register(SystemL, "_QW", luaQw);
    lua_register(SystemL, "_RX", luaRx);
    lua_register(SystemL, "_RY", luaRy);
    lua_register(SystemL, "_RZ", luaRz);
    lua_register(SystemL, "_LX", luaLx);
    lua_register(SystemL, "_LY", luaLy);
    lua_register(SystemL, "_LZ", luaLz);
    lua_register(SystemL, "_VX", luaVx);
    lua_register(SystemL, "_VY", luaVy);
    lua_register(SystemL, "_VZ", luaVz);
    lua_register(SystemL, "_FX", luaFx);
    lua_register(SystemL, "_FY", luaFy);
    lua_register(SystemL, "_FZ", luaFz);
    lua_register(SystemL, "_WX", luaWx);
    lua_register(SystemL, "_WY", luaWy);
    lua_register(SystemL, "_WZ", luaWz);
    lua_register(SystemL, "_CCD", luaGetCCD);
    lua_register(SystemL, "_RED", luaGetCCDRed);
    lua_register(SystemL, "_GREEN", luaGetCCDGreen);
    lua_register(SystemL, "_BLUE", luaGetCCDBlue);
    lua_register(SystemL, "_BYE", luaUnLinkBye);
    lua_register(SystemL, "_SPLIT", luaUnLink);
    lua_register(SystemL, "_RND", luaRnd);
    lua_register(SystemL, "_TODEG", luaToDeg);
    lua_register(SystemL, "_TORAD", luaToRad);
    lua_register(SystemL, "_TYPE", luaGetType);
    lua_register(SystemL, "_OPTION", luaGetOption);
    lua_register(SystemL, "_EFFECT", luaGetEffect);
    lua_register(SystemL, "_USER1", luaGetUserEffect);
    lua_register(SystemL, "_USER2", luaGetUserOption);
    lua_register(SystemL, "_DIR", luaGetDir);
    lua_register(SystemL, "_ANGLE", luaGetAngle);
    lua_register(SystemL, "_POWER", luaGetPower);
    lua_register(SystemL, "_SPRING", luaGetSpring);
    lua_register(SystemL, "_DAMPER", luaGetDamper);
    lua_register(SystemL, "_BRAKE", luaGetBrake);
    lua_register(SystemL, "_COLOR", luaGetColor);
    lua_register(SystemL, "_PARENT", luaGetParent);
    lua_register(SystemL, "_TOP", luaGetTop);
    lua_register(SystemL, "_M", luaGetM);
    lua_register(SystemL, "_I", luaGetI);
    lua_register(SystemL, "_MOBJ", luaGetObjM);
    lua_register(SystemL, "_IOBJ", luaGetObjI);
    lua_register(SystemL, "_E", luaGetEnergy);
	lua_register(SystemL, "_T",luaGetTolerant);
	lua_register(SystemL, "_MOVE3D",luaMove3D);
	lua_register(SystemL, "_LINE3D",luaLine3D);
	lua_register(SystemL, "_MOVE2D",luaMove2D);
	lua_register(SystemL, "_LINE2D",luaLine2D);
	lua_register(SystemL, "_SETCOLOR",luaSetColor);
	lua_register(SystemL, "_PLAYERS",luaGetPlayers);
	lua_register(SystemL, "_PLAYERHOSTID",luaGetPlayerHostID);
	lua_register(SystemL, "_PLAYERMYID",luaGetPlayerMyID);
	lua_register(SystemL, "_PLAYERID",luaGetPlayerID);
	lua_register(SystemL, "_PLAYERCHIPS",luaGetPlayerChips);
	lua_register(SystemL, "_PLAYERCRUSHES",luaGetPlayerCrushes);
	lua_register(SystemL, "_PLAYERRESETS",luaGetPlayerResets);
	lua_register(SystemL, "_PLAYERINITS",luaGetPlayerInits);
	lua_register(SystemL, "_PLAYERCOLOR",luaGetPlayerColor);
	lua_register(SystemL, "_PLAYERX",luaGetPlayerX);
	lua_register(SystemL, "_PLAYERY",luaGetPlayerY);
	lua_register(SystemL, "_PLAYERZ",luaGetPlayerZ);
	lua_register(SystemL, "_PLAYERARMS",luaGetPlayerArms);
	lua_register(SystemL, "_PLAYEYFORCES",luaGetPlayerYForces);
	lua_register(SystemL, "_PLAYERNAME",luaGetPlayerName);
	lua_register(SystemL, "_FUEL",luaGetFuel);
	lua_register(SystemL, "_FUELMAX",luaGetFuelMax);


	//---LUA_PATHをLua側へ登録
	const char* lua_path = getenv("LUA_PATH_5_0"); //この辺の定数がｿｰｽに残ってないので仕方なくﾏｼﾞｯｸﾅﾝﾊﾞｰで検索
	if (!lua_path) lua_path = getenv("LUA_PATH");
	if (lua_path) {
		lua_pushstring(SystemL, lua_path); lua_setglobal(SystemL, "LUA_PATH");

	}
	//--------
	// ｼﾅﾘｵﾌｧｲﾙﾊﾟｽをLua側へ登録
	{
		char* itr = szSystemFileName;
		while (strcmp(itr, szSystemFileName0) && *itr++) {}

		lua_pushlstring(SystemL, szSystemFileName, itr-szSystemFileName); lua_setglobal(SystemL, "SCENARIO_PATH");
		lua_pushlstring(SystemL, szSystemFileName0, strlen(szSystemFileName0)); lua_setglobal(SystemL, "SCENARIO_NAME");
	}
	//--------
	// EXEﾊﾟｽをLua側へ登録
	{
		//----------------------------
		//ファイルパス登録 (ホストアプリケーション,DLL)  FATとかWin32APIの仕様としては、_MAX_PATH"文字"制限なので_MAX_PATH"byte"のバッファじゃまずいんでは…?   ということで全部*2しとこ
		char tmpBuff[_MAX_PATH*2];   //パス組立用一時バッファ

		char szDrive[_MAX_DRIVE*2];	// ドライブ名格納領域 
		char szPath[_MAX_DIR*2];		// パス名格納領域 
		char szTitle[_MAX_FNAME*2];	// ファイルタイトル格納領域 
		char szExt[_MAX_EXT*2];		// ファイル拡張子格納領域 

		//------------
		GetModuleFileNameA(NULL, tmpBuff, sizeof(tmpBuff)); //ここで返るパスはUNCパスだったり8.3だったり  モジュール呼び出し時に使った文字列依存らしい
		_splitpath_s(tmpBuff, szDrive, szPath, szTitle, szExt);

		lua_pushfstring(SystemL, "%s%s", szDrive, szPath); lua_setglobal(SystemL, "EXE_PATH");
		lua_pushfstring(SystemL, "%s%s", szTitle, szExt); lua_setglobal(SystemL, "EXE_NAME");
		//------------
	}

	//--------
	luaL3dx=luaL3dy=luaL3dz=0.0f;
	luaGraColor=0xffffff;
	for(int i=0;i<8;i++) ControlKeysLock[i]=false;
	//グローバル変数の登録
	for(int i=0;i<VarCount;i++) {
		lua_pushnumber(SystemL,ValList[i].Val);
		lua_setglobal(SystemL,ValList[i].Name);
	}
	//スクリプトをセットする
      luaopen_string(SystemL);
      luaopen_base(SystemL);
      luaopen_table(SystemL);
      luaopen_math(SystemL);
      luaopen_io(SystemL);

	//----------------------------
	  { //SCENARIO_INIT.lua実行
		  lua_State* L=SystemL;
		  //L={}
		  lua_getglobal(L, "EXE_PATH");
		  lua_pushstring(L, "SCENARIO_INIT.lua");
		  lua_concat(L, 2);
		  //L={filepath}
		  int err;
		  if ((err = luaL_loadfile(L, lua_tostring(L, -1)))) {
			  if (err==LUA_ERRFILE) {
				  lua_pop(L, 1); //ｴﾗｰﾒｯｾｰｼﾞを捨てて
				  err=luaL_loadstring(L, //無害な関数を代わりに積む
					  "io=nil;"
					  "os=nil;"
				  );
			  }
			  if(err){ //LUA_ERRFILE以外ならこの時点で抜ける
				  SystemErrorCode = -1;
				  snprintf(SystemErrorStr, GOUTPUTMAXCHAR, "%s\n", lua_tostring(L, -1));
				  luaSystemEnd();
				  return 1;
			  }
		  } else {
			  if(!LoadlibDummy) luaopen_loadlib(L); //loadlibはﾀﾞﾐｰでなくLUA_INIT有効な経路でのみ有効に

		  }
		  lua_remove(L, -2);
		  //L={SCENARIO_INIT}

		  //fenvの作成と一部の値の移動
		  lua_newtable(L);
		  luaUL_movetablefield(L, -1, LUA_GLOBALSINDEX, "LUA_PATH");

		  luaUL_movetablefield(L, -1, LUA_GLOBALSINDEX, "os");
		  luaUL_movetablefield(L, -1, LUA_GLOBALSINDEX, "io");
		  luaUL_movetablefield(L, -1, LUA_GLOBALSINDEX, "loadlib");
		  //----
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "string");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "math");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "table");

		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "assert");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "error");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "pairs");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "ipairs");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "next");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "print");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "tonumber");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "tostring");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "type");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "unpack");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "pcall");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "xpcall");

		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "EXE_PATH");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "EXE_NAME");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "SCENARIO_PATH");
		  luaUL_copytablefield(L, -1, LUA_GLOBALSINDEX, "SCENARIO_NAME");
		  //L={SCENARIO_INIT,newtbl}

		  //fenvのｾｯﾄ
		  lua_pushvalue(L, -1); //L={SCENARIO_INIT,newtbl,newtbl}
		  lua_insert(L, -3); //L={newtbl,SCENARIO_INIT,newtbl}
		  lua_setfenv(L,-2);
		  //L={newtbl,SCENARIO_INIT(fenv=newtbl)}


		  lua_pushcfunction(L, luaErrMsgHandler);
		  lua_insert(L, -2);
		  //L={newtbl,msgh,SCENARIO_INIT}
		  if (err || (err = lua_pcall(L, 0, 0, -2))) {
			  SystemErrorCode = -1;
			  snprintf(SystemErrorStr, GOUTPUTMAXCHAR, "%s\n", lua_tostring(L, -1));
			  luaSystemEnd();
			  return 1;
		  }
		  lua_pop(L, 1);
		  //L={newtbl}

		  //fenvからｸﾞﾛｰﾊﾞﾙへの転送
		  lua_pushnil(L);
		  while(lua_next(L, -2)){
			  //L={newtbl,key,value}
			  lua_pushvalue(L, -2);
			  lua_insert(L, -2); //L={newtbl,key,key,value}
			  lua_settable(L, LUA_GLOBALSINDEX);
		  }

	  }//L={}
	  //----------------------------
	//L={}
	lua_pushcfunction(SystemL, luaErrMsgHandler);
	if (luaL_loadbuffer(SystemL, SystemSource, strlen(SystemSource), "System.rcs") || lua_pcall(SystemL, 0, 0, -2)) {
		SystemErrorCode=-1;
		sprintf(SystemErrorStr,"%s\n",lua_tostring(SystemL,-1));
		luaSystemEnd();
		return 1;
	}
	lua_pop(SystemL, 1);
	//L={}
	return 0;
}
void luaSystemEnd() {
	if(SystemL==NULL) return;
	for(int i=0;i<FILEMAX;i++) if(FpTable[i]) fclose(FpTable[i]);
	lua_close(SystemL);  /* create state */
	SystemL=NULL;
	return;
}
int luaSystemRun (char *funcName) {
	if(SystemL==NULL) return 1;
	for (int i=0;i<GOUTPUTMAX;i++) SystemOutput[i][0]='\0';
	if(strcmp(funcName,"OnInit")==0) {
		World->Stop=false;
		World->MainStepCount=-1;
		for(int i=0;i<GKEYMAX;i++) KeyList[i].Lock=0;
	}
//	int status;
//  struct Smain s;
	//グローバル変数の登録
	for(int i=0;i<VarCount;i++) {
		lua_pushnumber(SystemL,ValList[i].Val);
		lua_setglobal(SystemL,ValList[i].Name);
	}
	// グローバルテーブルからmain関数を拾ってスタックに積む
	lua_pushstring(SystemL, funcName);
	lua_gettable(SystemL, LUA_GLOBALSINDEX);
	if(lua_topointer(SystemL,-1)==NULL) {//もし関数がないなら
		lua_pop ( SystemL, 1 );//スタックを戻す
		if(strcmp(funcName,"OnFrame")==0) {
			lua_pushstring(SystemL, "main");
			lua_gettable (SystemL, LUA_GLOBALSINDEX);
		}
		else return 0;
	}
    // 関数を呼ぶ。lua_callの第2引数は渡す引数の数、第3引数は戻り値の数。
    // 関数とその引数はスタックから取り除かれ、戻り値がスタックに残る。
	lua_pushcfunction(SystemL, luaErrMsgHandler);
	lua_insert(SystemL, -2);
    SystemErrorCode=lua_pcall(SystemL, 0, 0 ,-2);
	if(SystemErrorCode){
		sprintf(SystemErrorStr,"%s\n",lua_tostring(SystemL,-1));
		lua_pop(SystemL,1);//使い終わったｴﾗｰﾒｯｾｰｼﾞを捨てる
	}
	lua_pop(SystemL, 1); //ﾒｯｾｰｼﾞﾊﾝﾄﾞﾗを捨てる
	for(int i=0;i<VarCount;i++) {
		lua_pushstring( SystemL , ValList[i].Name ); // (1) Luaの変数名toCを指定
		lua_gettable( SystemL , LUA_GLOBALSINDEX ); // (2)と(3)の動作
		double v = lua_tonumber( SystemL , -1 ); // (4) 仮想スタックのトップ内容(toCの中身)を数値型で取り出す
		if(ValList[i].Val!=v) ValList[i].Updated=true;
		ValList[i].Val=(GFloat)v;
		lua_pop( SystemL , 1 ); // (5) 取り出したら仮想スタックを1個popする
		if(ValList[i].Val>ValList[i].Max) ValList[i].Val=ValList[i].Max;
		if(ValList[i].Val<ValList[i].Min) ValList[i].Val=ValList[i].Min;
	}
 	luaUpdateVal();

	return 0;
}


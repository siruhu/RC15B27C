#include "GVector.hpp"
#include "GRigid.hpp"
//メモリリーク検出用
#include <crtdbg.h>  
#ifdef _DEBUG 
#ifdef __MEMLEAKCHECK 
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__) 
#define malloc(p1) _malloc_dbg((p1),_NORMAL_BLOCK,__FILE__,__LINE__) 
#define __MEMLEAKCHECK
#endif
#endif 
#include "stdlib.h"

#define GPARTMAX 10000
extern GFloat ARMSPEED;
extern int LIMITFPS;

extern GFloat WaterLine;

int myrand();
class GParticleVertex {// ここのVec,Acc,SizeD,LifeSpanの時間単位は1/30s  例えばVecはm/(1/30s)  まともにすると修正箇所が多くなるからこんな妙な形に･･･
public:
	int Net;
	int Type;
	GVector Pos;
	GVector Vec;
	GVector Vec2;
	GVector Acc;
	GFloat Size;
	GFloat SizeD;
	GFloat Life;
	GFloat Power;
	GFloat LifeSpan;
	DWORD  dpnid;
	GVector Color;
};
class GParticle {
public:
	GParticleVertex *Vertex;
	int NextVertex;
	int MaxVertex;
	GParticle(int n) {
		Vertex= new GParticleVertex[n];
		MaxVertex=n;
		NextVertex=0;
		Clear();
	}
	~GParticle() {
		delete []Vertex;
	}
	GParticleVertex *Add(GVector pos,GVector vec,GVector acc,GFloat sizeD,GFloat life,GFloat lifeSpan,GVector col){
		GParticleVertex *ret=&Vertex[NextVertex];
		Vertex[NextVertex].Net=0;
		Vertex[NextVertex].Type=0;
		Vertex[NextVertex].Pos=pos;
		Vertex[NextVertex].Vec=vec;//速度,寿命の時間単位は1/30s
		Vertex[NextVertex].Acc=acc;
		Vertex[NextVertex].Size=0.4f+(myrand()%100*sizeD*4)/100.0f-sizeD*2;
		Vertex[NextVertex].SizeD=sizeD;
		Vertex[NextVertex].Life=life;
		Vertex[NextVertex].Power=life;
		Vertex[NextVertex].LifeSpan=lifeSpan;
		Vertex[NextVertex].Color=col;
		Vertex[NextVertex].dpnid=0;
		NextVertex++;
		if(NextVertex>=MaxVertex) NextVertex=0;
		return ret;
	}
	GParticleVertex *Add(int type,GVector pos,GVector vec,GVector acc,GFloat sizeD,GFloat life,GFloat lifeSpan,GVector col,DWORD  dpnid,bool net){
		GParticleVertex *ret=&Vertex[NextVertex];
		Vertex[NextVertex].Type=type;//Type=0:Jet煙&土煙,1:爆炎(黄),2:爆炎(白)
		Vertex[NextVertex].Net=(type!=0 && net);
		Vertex[NextVertex].Pos=pos;
		Vertex[NextVertex].Vec=vec;
		Vertex[NextVertex].Acc=acc;
		Vertex[NextVertex].Size=0.4f+(myrand()%100*sizeD*4)/100.0f-sizeD*2;
		Vertex[NextVertex].SizeD=sizeD;
		Vertex[NextVertex].Life=life;
		if(type==1) Vertex[NextVertex].Life=life*2.0f;
		if(type==2) Vertex[NextVertex].Life=life*1.6f;
		Vertex[NextVertex].Power=life;
		Vertex[NextVertex].LifeSpan=lifeSpan;
		Vertex[NextVertex].Color=col;
		Vertex[NextVertex].dpnid=dpnid;
		NextVertex++;
		if(NextVertex>=MaxVertex) NextVertex=0;
		return ret;
	}
	GParticleVertex *Add(int type,GVector pos,GVector vec,GVector vec2,GVector acc,GFloat sizeD,GFloat life,GFloat lifeSpan,GVector col,DWORD  dpnid,bool net){
		GParticleVertex *ret=&Vertex[NextVertex];
		Vertex[NextVertex].Type=type;//Type=0:Jet煙&土煙,1:爆炎(黄),2:爆炎(白)
		Vertex[NextVertex].Net=(type!=0 && net);
		Vertex[NextVertex].Pos=pos;
		Vertex[NextVertex].Vec=vec;
		Vertex[NextVertex].Vec2=vec2;//長さ  現状JetEffect5専用 単位はm
		Vertex[NextVertex].Acc=acc;
		Vertex[NextVertex].Size=0.4f+(myrand()%100*sizeD*4)/100.0f-sizeD*2;
		Vertex[NextVertex].SizeD=sizeD;
		Vertex[NextVertex].Life=life;
		if(type==1) Vertex[NextVertex].Life=life*2.0f;
		if(type==2) Vertex[NextVertex].Life=life*1.6f;
		Vertex[NextVertex].Power=life;
		Vertex[NextVertex].LifeSpan=lifeSpan;
		Vertex[NextVertex].Color=col;
		Vertex[NextVertex].dpnid=dpnid;
		NextVertex++;
		if(NextVertex>=MaxVertex) NextVertex=0;
		return ret;
	}
	void Clear(){
		for(int i=0;i<MaxVertex;i++) {
			Vertex[i].Life=-1;
			Vertex[i].Net=0;
		}
		NextVertex=0;
	}
	void Move(){
		for(int i=0;i<MaxVertex;i++) {
			if(Vertex[i].Life>0) {
				GFloat y=Vertex[i].Pos.y;
				GFloat FPS_mag=(GFloat)LIMITFPS/30.0f;
				if(y>WaterLine && y+Vertex[i].Vec.y/FPS_mag<WaterLine)  {
					GFloat y_temp=WaterLine+Vertex[i].Size/2;
					if(y_temp>y) y_temp=y;//元の高度より高くならないように
					GFloat dist=(y-y_temp)/(-Vertex[i].Vec.y/FPS_mag);//上の条件式より、Vec.yは必ず負数なんでabsじゃなく-でいい
					Vertex[i].Pos+=Vertex[i].Vec/FPS_mag*dist;
					
					Vertex[i].Vec.x=(GFloat)(Vertex[i].Vec.x+((myrand()%100)/100.0-0.5)*Vertex[i].Vec.y);
					Vertex[i].Vec.z=(GFloat)(Vertex[i].Vec.z+((myrand()%100)/100.0-0.5)*Vertex[i].Vec.y);
					Vertex[i].Vec.y=(GFloat)(-Vertex[i].Vec.y/10.0);
					Vertex[i].Vec2=-Vertex[i].Vec/FPS_mag*2;//ﾊﾞﾗける分密度が下がるので尾の長さを伸ばしてごまかす  ･･･誤魔化したい  だいたい誤魔化せるけど水面の散らばりだけはﾑﾘ
					Vertex[i].Size+=Vertex[i].Vec.abs()/10;
				}
				Vertex[i].Pos+=Vertex[i].Vec/FPS_mag;
				Vertex[i].Vec=Vertex[i].Vec*(1-0.005/FPS_mag)+Vertex[i].Acc/FPS_mag;
				Vertex[i].Vec2+=Vertex[i].Vec2*Vertex[i].SizeD/Vertex[i].Size/FPS_mag/2;
				Vertex[i].Size+=Vertex[i].SizeD/FPS_mag;
				Vertex[i].Life-=Vertex[i].LifeSpan/FPS_mag;
				if(Vertex[i].Net>0) Vertex[i].Net++;
				
				GFloat y_wl=Vertex[i].Pos.y-WaterLine;
				if(y_wl*(y_wl+Vertex[i].Vec2.y)<0 && y_wl>0){ //尾が水面を跨ぐ時Vec2縮める
					GFloat dist=y_wl/(-Vertex[i].Vec2.y);
					Vertex[i].Vec2*=dist;
					
				}
			}
		}
	}
};

class GBulletVertex {
public:
	int Net;
	GVector Pos;
	GVector Pos2;
	GVector Vec;
	GFloat Power;
	GFloat Life;
	GFloat Size;
	GVector Tar;
	GFloat Dist;
	GFloat Dist2;
	DWORD  dpnid;
	GRigid *Rigid;
};
class GBullet {
public:
	GBulletVertex *Vertex;
	int NextVertex;
	int MaxVertex;
	GBullet(int n) {
		Vertex= new GBulletVertex[n];
		MaxVertex=n;
		NextVertex=0;
		Clear();
	}
	~GBullet() {
		delete []Vertex;
	}
	GBulletVertex *Add(GRigid *rigid,GVector pos,GVector vec,GFloat power,GFloat size,GFloat dist,GVector t,DWORD dpnid,bool Net){
		
		GBulletVertex *ret=&Vertex[NextVertex];
		Vertex[NextVertex].Pos=pos;
		Vertex[NextVertex].Pos2=pos;
		Vertex[NextVertex].Vec=vec;
		Vertex[NextVertex].Power=power;
		Vertex[NextVertex].Size=size;
		Vertex[NextVertex].Life=1200.0f;
		Vertex[NextVertex].Tar=t;
		Vertex[NextVertex].Dist=dist;
		Vertex[NextVertex].Dist2=dist;
		Vertex[NextVertex].Rigid=rigid;
		Vertex[NextVertex].dpnid=dpnid;
		Vertex[NextVertex].Net=Net;
		NextVertex++;
		if(NextVertex>=MaxVertex) NextVertex=0;
		return ret;
	}
	void Clear(){
		for(int i=0;i<MaxVertex;i++) {
			Vertex[i].Life=-1;
			Vertex[i].Net=0;
		}

		NextVertex=0;
	}
	void Move(){
		for(int i=0;i<MaxVertex;i++) {
			if(Vertex[i].Dist<0) {
				Vertex[i].Life=-1;
				//Vertex[i].Net=0; //ここでﾌﾗｸﾞ倒しちゃうと送信前に地形に当たった弾が送信されない
			}
			if(Vertex[i].Life>0) {
				GFloat y=Vertex[i].Pos.y;
				Vertex[i].Pos+=Vertex[i].Vec;
				Vertex[i].Life-=Vertex[i].Vec.abs();
				Vertex[i].Dist-=Vertex[i].Vec.abs();
				if(Vertex[i].Net>0) Vertex[i].Net++;
			}
		}
	}
};
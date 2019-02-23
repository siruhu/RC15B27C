#include "GVector.hpp"
#define CHIPSIZE 0.6f

#define CHIPSIZEHOSEI (CHIPSIZE/0.6f)
#define CHIPSIZEHOSEI2 (CHIPSIZEHOSEI*CHIPSIZEHOSEI*CHIPSIZEHOSEI)

//メモリリーク検出用
#include <crtdbg.h>  
#ifdef _DEBUG 
#ifdef __MEMLEAKCHECK 
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__) 
#define malloc(p1) _malloc_dbg((p1),_NORMAL_BLOCK,__FILE__,__LINE__) 
#endif
#endif 

class GLandVertex {
public:
	GVector Pos;
	GVector Normal;
	GVector Normal0;
	
	GLandVertex(){
		Normal.clear();
		Normal0.clear();
	};
};
class GLandFace {
public:
	unsigned int Index[3];
	GVector Vertex[3];
	GVector Normal;
	GVector Center;
	GFloat Radius;
	GFloat Rx,Ry,Rz; //各軸、面の中心から最も遠い頂点の距離(x,y,zが同じ頂点の物とは限らない)
	GFloat D; //原点との距離?
	int		AttribNo;
	GFloat Ud;
	GFloat Us;
	GFloat Ux;	//ころがり係数
	//GVector OutVec[3];
	//GFloat OutD[3];
	GVector OutVec2[3];
	GFloat OutD2[3];
	bool CheckFlag;
	
	GLandFace(){
		AttribNo=0;
		Normal.clear();
		Ud=0.0f;
		Us=0.0f;
		Ux=0.0f;	//ころがり係数
		CheckFlag=false;
	};
	//ポリゴン1枚のあたり判定
	GFloat Check(GVector &p,GFloat r,GVector &n) {
		if(Ux<0) return 100000.0f;
		n=Normal;
		GFloat d=p.distanceFromFace(Normal,D);
		if(d>0 || d<-r) return 100000.0f;
		GVector p2=p.pointOnFaceAndLine(Normal,D,Normal);
		if(p2.dot(OutVec2[0])-OutD2[0]>0.003) return 100000.0f;
		if(p2.dot(OutVec2[1])-OutD2[1]>0.003) return 100000.0f;
		if(p2.dot(OutVec2[2])-OutD2[2]>0.003) return 100000.0f;
		/*
		if(p.dot(OutVec2[0])-OutD2[0]>0) {
		d=p.distanceFromLine2(Vertex[0],Vertex[1]);
		}
		if(p.dot(OutVec2[1])-OutD2[1]>0) {
		d=p.distanceFromLine2(Vertex[1],Vertex[2]);
		}
		if(p.dot(OutVec2[2])-OutD2[2]>0) {
		d=p.distanceFromLine2(Vertex[2],Vertex[0]);
		}
		*/
		return d;
	}
	//ポリゴン1枚のあたり判定
	GFloat Check2(const GVector &p,GVector v,GVector &n) {
		if(Ux<0) return 100000.0f;
		//GFloat d=p.dot(Normal)-D;
		GFloat d=p.distanceFromFace(Normal,D); //法線ﾍﾞｸﾄﾙの大きさが1と確約できなければこっちにすべき
		//if(d>=0) return 100000.0f;
		GFloat vl=v.abs();
		if(vl<0.001f) v= -Normal*0.001f;
		//if(vl<0.001f) v= -Normal*(CHIPSIZE/2.0f);
//		if(vl<0.6) v= v/vl*0.6;
		GVector wp;
		GVector p2=p-v;
		int f=p2.pointOnFaceAndLine2(Normal,D,v,wp);
		if(f!=0) return 100000.0f; 
//		wp=p.pointOnFaceAndLine(Normal,D,Normal);
		if(wp.dot(OutVec2[0])-OutD2[0]>0.0f) return 100000.0f; //ここなんでOutVecとOutDが使われてたんだ?このせいで180度逆向きのﾎﾟﾘが頂点共有してると判定消えたり飛び出たりﾊﾞｸﾞってた 橋ﾏｯﾌﾟみたいな
		if(wp.dot(OutVec2[1])-OutD2[1]>0.0f) return 100000.0f;
		if(wp.dot(OutVec2[2])-OutD2[2]>0.0f) return 100000.0f;
		n=Normal;
		return d;
	}
	GFloat Check3(const GVector &p,GVector v,GFloat dmin1,GFloat dmin2,GVector &n) { //ほぼCheck2のｺﾋﾟｰ あまり綺麗じゃない軽量化用
		if(Ux<0) return 100000.0f;
		//GFloat d=p.dot(Normal)-D;
		GFloat d=p.distanceFromFace(Normal,D); //法線ﾍﾞｸﾄﾙの大きさが1と確約できなければこっちにすべき
		//if(d>=0) return 100000.0f;
		if(d>=dmin2 || 0>d && d>=dmin1){
			return 100000.0f;} //dmin1は最も小さい値 dmin2は最も小さい正の値
		GFloat vl=v.abs();
		if(vl<0.001f) v= -Normal*0.001f;
		//if(vl<0.001f) v= -Normal*(CHIPSIZE/2.0f);
//		if(vl<0.6) v= v/vl*0.6;
		GVector wp;
		GVector p2=p-v;
		int f=p2.pointOnFaceAndLine2(Normal,D,v,wp);
		if(f!=0) return 100000.0f; 
//		wp=p.pointOnFaceAndLine(Normal,D,Normal);
		if(wp.dot(OutVec2[0])-OutD2[0]>0.0f) return 100000.0f; //ここなんでOutVecとOutDが使われてたんだ?このせいで180度逆向きのﾎﾟﾘが頂点共有してると判定消えたり飛び出たりﾊﾞｸﾞってた 橋ﾏｯﾌﾟみたいな
		if(wp.dot(OutVec2[1])-OutD2[1]>0.0f) return 100000.0f;
		if(wp.dot(OutVec2[2])-OutD2[2]>0.0f) return 100000.0f;
		n=Normal;
		return d;
	}
	//ボールとポリゴン1枚のあたり判定
	GFloat BallCheck(GVector &cp,GFloat r,GVector &p) {
		GFloat wd;
		if(Ux<0) return 100000.0f;
		GFloat d=fabs(cp.distanceFromFace(Normal,D));
		if(d>=r) goto lb1;
		p=cp.pointOnFaceAndLine(Normal,D,Normal);
		if(p.dot(OutVec2[0])-OutD2[0]>0.0f) goto lb1;
		if(p.dot(OutVec2[1])-OutD2[1]>0.0f) goto lb1;
		if(p.dot(OutVec2[2])-OutD2[2]>0.0f) goto lb1;
		return d-r;
lb1:
//		return 100000.0f;
		d=100000.0f+r;
		GFloat wr=r;
		wd=cp.distanceFromLine2(Vertex[0],Vertex[1]);
		if(wd<wr) {
			p=cp.pointOnLine(Vertex[0],Vertex[1]);
			wr=d=wd;
		}
		wd=cp.distanceFromLine2(Vertex[1],Vertex[2]);
		if(wd<wr) {
			p=cp.pointOnLine(Vertex[1],Vertex[2]);
			wr=d=wd;
		}
		wd=cp.distanceFromLine2(Vertex[2],Vertex[0]);
		if(wd<wr) {
			p=cp.pointOnLine(Vertex[2],Vertex[0]);
			d=wd;

		}
		return d-r;

	}
	//ポリゴン1枚のあたり判定
	GFloat CircleCheck(GVector &cp,GVector &n,GFloat r,GVector &p) {

		if(Ux<0) return 100000.0f;
		GVector nr=Normal.cross(n).cross(n).normalize2();
		if(nr.dot(Normal)>0) nr=-nr;
		p=cp+nr*r;

		GFloat wd;
		GVector p2;
		GFloat d=p.distanceFromFace(Normal,D);
		if(d>0 || d<-r*2) goto lb2;
//		if(d>=0) return 100000.0f;
		p2=p.pointOnFaceAndLine(Normal,D,Normal);
		if(p2.dot(OutVec2[0])-OutD2[0]>0.00) goto lb2;
		if(p2.dot(OutVec2[1])-OutD2[1]>0.00) goto lb2;
		if(p2.dot(OutVec2[2])-OutD2[2]>0.00) goto lb2;
		return d;
	lb2:
		d=100000.0f+r;
		//円平面と線分との交点をpとする
		GVector wp;
		Vertex[1].pointOnFaceAndLine2(n,cp,(Vertex[0]-Vertex[1]),wp);
		GFloat wr=r;
		wd=(wp-cp).abs();
		if(wd<wr) {
			wr=d=wd;
			p=cp+(wp-cp).normalize2()*r;
		}
		Vertex[2].pointOnFaceAndLine2(n,cp,(Vertex[1]-Vertex[2]),wp);
		wd=(wp-cp).abs();
		if(wd<wr) {
			wr=d=wd;
			p=cp+(wp-cp).normalize2()*r;
		}
		Vertex[0].pointOnFaceAndLine2(n,cp,(Vertex[2]-Vertex[0]),wp);
		wd=(wp-cp).abs();
		if(wd<wr) {
			d=wd;
			p=cp+(wp-cp).normalize2()*r;
		}
		return d-r;
	}
};

class GLand {
public:
	int VertexCount;
	int FaceCount;
	GLandVertex *Vertex;
	GLandFace *Face;
	int *List0;
	int List0Count;
	int *List1;
	GVector List1P;
	GFloat List1R;
	int List1Count;
	int *List2;
	int List2Count;
	int *List3;
	int List3Count;
	int *HitList;
	GFloat *HitD;
	unsigned long AttribTableSize;

	GVector *HitNormal;
	int HitListCount;
	GLand(int v,int f) {
		Vertex = new GLandVertex[v];
		Face = new GLandFace[f];
		List0=new int[f]; //ﾁｯﾌﾟごと 用に追加
		List0Count=0;
		List1=new int[f]; //おそらく当たり判定ﾘｽﾄ1系
		List1P=GVector(0,-100000,0);
		List1R=-1;
		List1Count=0;
		List2=new int[f]; //おそらく当たり判定ﾘｽﾄ1subStep
		List2Count=0;
		List3=new int[f]; //おそらく当たり判定ﾘｽﾄ1F
		List3Count=0;
		HitList=new int[f];
		HitD=new GFloat[f];
		HitNormal=new GVector[f];
		HitListCount=0;
		VertexCount=v;
		FaceCount=f;
	};
	~GLand() {
		if(Vertex) {delete [] Vertex;Vertex=NULL;}
		if(Face) {delete [] Face;Face=NULL;}
		if(List0) {delete [] List0;List0=NULL;}
		if(List1) {delete [] List1;List1=NULL;}
		if(List2) {delete [] List2;List2=NULL;}
		if(List3) {delete [] List3;List3=NULL;}
		if(HitList) {delete [] HitList;HitList=NULL;}
		if(HitD) {delete [] HitD;HitD=NULL;}
		if(HitNormal) {delete [] HitNormal;HitNormal=NULL;}
		HitListCount=0;
		VertexCount=0;
		FaceCount=0;
	}
	void Set() {
		int i,f;
		GVector v;
		GFloat r;
		for(f=0;f<FaceCount;f++) {
			for(i=0;i<3;i++) Face[f].Vertex[i]=Vertex[Face[f].Index[i]].Pos;
			Face[f].Center=(Face[f].Vertex[0]+Face[f].Vertex[1]+Face[f].Vertex[2])/3.0;
			Face[f].Normal=((Face[f].Vertex[0]-Face[f].Vertex[2]).cross(Face[f].Vertex[1]-Face[f].Vertex[0])).normalize2();
			for(i=0;i<3;i++) {
				Vertex[Face[f].Index[i]].Normal+=Face[f].Normal*((Face[f].Vertex[(i+2)%3]-Face[f].Vertex[i]).angle(Face[f].Vertex[(i+1)%3]-Face[f].Vertex[i])); //面法線ﾍﾞｸﾄﾙ*cos(2辺の成す角)
				//Vertex[Face[f].Index[i]].Normal=Face[f].Normal; //面が頂点を共有してる場合は上書きしちゃう、けどDirectXの仕様で
				Vertex[Face[f].Index[i]].Normal0=Vertex[Face[f].Index[i]].Normal; //別の法線ﾍﾞｸﾄﾙを持つ面の組が共有する頂点は複製される==>つまり面の組が頂点共有してる場合は同じ法線ﾍﾞｸﾄﾙを持つはず?
			}
			v=Face[f].Vertex[0]-Face[f].Center;
			Face[f].Rx=(GFloat)fabs(v.x);
			Face[f].Ry=(GFloat)fabs(v.y);
			Face[f].Rz=(GFloat)fabs(v.z);
			Face[f].Radius=v.abs();
			v=Face[f].Vertex[1]-Face[f].Center;
			if(fabs(v.x)>Face[f].Rx) Face[f].Rx=(GFloat)fabs(v.x);
			if(fabs(v.y)>Face[f].Ry) Face[f].Ry=(GFloat)fabs(v.y);
			if(fabs(v.z)>Face[f].Rz) Face[f].Rz=(GFloat)fabs(v.z);
			r=v.abs();
			if(r>Face[f].Radius) Face[f].Radius=r;
			v=Face[f].Vertex[2]-Face[f].Center;
			if(fabs(v.x)>Face[f].Rx) Face[f].Rx=(GFloat)fabs(v.x);
			if(fabs(v.y)>Face[f].Ry) Face[f].Ry=(GFloat)fabs(v.y);
			if(fabs(v.z)>Face[f].Rz) Face[f].Rz=(GFloat)fabs(v.z);
			r=v.abs();
			if(r>Face[f].Radius) Face[f].Radius=r;
			Face[f].D=(Face[f].Normal).dot(Face[f].Center);
		}
		for(i=0;i<VertexCount;i++) { //n^2の計算量 頂点数100kあたりでもう読み込み時間がかなり辛い ｺﾋﾟｰして座標順にｿｰﾄでも掛けてやったほうがﾏｼかも? 
			/*for(int j=i+1;j<VertexCount;j++) { //別の法線ﾍﾞｸﾄﾙを持つ面で共有していた頂点が複製された分の和を取る 隣接性ﾃﾞｰﾀ使ってﾙｰﾌﾟ数減らせる?
				if(Vertex[i].Pos==Vertex[j].Pos) {
					Vertex[i].Normal+=Vertex[j].Normal0;
					Vertex[j].Normal+=Vertex[i].Normal0;
				}
			}*/ //頂点法線ﾍﾞｸﾄﾙ重いし使い道思いつかないんで始末 関連してOutDとOutVecも始末 辺との当たり判定付けて、辺の反力方向も実装するなら使えるかも?
			Vertex[i].Normal=(Vertex[i].Normal).normalize2();
		}
		for(f=0;f<FaceCount;f++) {
			
			GVector n=Face[f].Normal;
			Face[f].OutVec2[0]=(n).cross(Face[f].Vertex[0]-Face[f].Vertex[1]).normalize2();
			Face[f].OutVec2[1]=(n).cross(Face[f].Vertex[1]-Face[f].Vertex[2]).normalize2();
			Face[f].OutVec2[2]=(n).cross(Face[f].Vertex[2]-Face[f].Vertex[0]).normalize2();
			Face[f].OutD2[0]=(Face[f].OutVec2[0]).dot(Face[f].Vertex[0]);
			Face[f].OutD2[1]=(Face[f].OutVec2[1]).dot(Face[f].Vertex[1]);
			Face[f].OutD2[2]=(Face[f].OutVec2[2]).dot(Face[f].Vertex[2]);
			//Face[f].OutVec[0]=((Vertex[Face[f].Index[0]].Normal+Vertex[Face[f].Index[1]].Normal)/2.0f).cross(Face[f].Vertex[0]-Face[f].Vertex[1]).normalize2();
			//Face[f].OutVec[1]=((Vertex[Face[f].Index[1]].Normal+Vertex[Face[f].Index[2]].Normal)/2.0f).cross(Face[f].Vertex[1]-Face[f].Vertex[2]).normalize2();
			//Face[f].OutVec[2]=((Vertex[Face[f].Index[2]].Normal+Vertex[Face[f].Index[0]].Normal)/2.0f).cross(Face[f].Vertex[2]-Face[f].Vertex[0]).normalize2();
			//Face[f].OutD[0]=(Face[f].OutVec[0]).dot((Face[f].Vertex[0]+Face[f].Vertex[1])/2.0f);
			//Face[f].OutD[1]=(Face[f].OutVec[1]).dot((Face[f].Vertex[1]+Face[f].Vertex[2])/2.0f);
			//Face[f].OutD[2]=(Face[f].OutVec[2]).dot((Face[f].Vertex[2]+Face[f].Vertex[0])/2.0f);
		}
	}
	GFloat  GetY(GFloat x,GFloat z) {
		GVector p,p0;
		GFloat y;
		y=-100000.0;
		p0=GVector(x,0,z);
		for(int f=0;f<FaceCount;f++) {
			p=p0.pointOnFaceAndLine(Face[f].Normal,Face[f].Center,GVector(0,1,0));
			if(p.distanceFromFace(Face[f].OutVec2[0],Face[f].OutD2[0])>0.0) continue;
			if(p.distanceFromFace(Face[f].OutVec2[1],Face[f].OutD2[1])>0.0) continue;
			if(p.distanceFromFace(Face[f].OutVec2[2],Face[f].OutD2[2])>0.0) continue;
			if(p.y>y) y=p.y;
		}
		return y;
	}
	GFloat  GetY2(GFloat x,GFloat y,GFloat z) {
		GVector p,p0;
		GFloat my;
		my=-100000.0;
		p0=GVector(x,0,z);
		for(int i=0;i<List2Count;i++) {
			int f=List2[i];
			p=p0.pointOnFaceAndLine(Face[f].Normal,Face[f].Center,GVector(0,1,0));
			if(p.distanceFromFace(Face[f].OutVec2[0],Face[f].OutD2[0])>0.0) continue;
			if(p.distanceFromFace(Face[f].OutVec2[1],Face[f].OutD2[1])>0.0) continue;
			if(p.distanceFromFace(Face[f].OutVec2[2],Face[f].OutD2[2])>0.0) continue;
			if(p.y>my && p.y<y) my=p.y;
		}
		return my;
	}
	void List3Reset(void) { //実行順に注意 List2Resetに書いた
		for(int f=0;f<FaceCount;f++) {
			Face[f].CheckFlag=false;
		}
		List3Count=0;
	}
	//ListXup、(pIn,pOut)程度を引数に親ﾃｰﾌﾞﾙから子ﾃｰﾌﾞﾙ生成する関数に変更すべき 階層まで固定なのはだいぶよろしくない
	int List3up(GVector &p,GFloat r) { //AABBに書き換えるべき //(おそらく)1秒毎更新の面当たり判定ﾘｽﾄ
		GVector p2;
		for(int f=0;f<FaceCount;f++) {
			if(Face[f].CheckFlag==true) continue;
			if(Face[f].Ux<0) {Face[f].CheckFlag=false;continue;}
			p2=Face[f].Center-p;
			if(fabs(p2.x)>r+Face[f].Rx 
				|| fabs(p2.z)>r+Face[f].Rz 
				|| fabs(p2.y)>r+Face[f].Ry) continue;
			Face[f].CheckFlag=true;
			List3[List3Count++]=f;
			if(List3Count>=FaceCount) List3Count=FaceCount;
		}
		return List3Count;
	}
	void List2Reset(void) { //List2Reset掛けた後はList3upの実行前に必ずList3Reset実行が必要 逆も同じ 実行順ずれた場合面多重登録、添字違反の可能性
		for(int i=0;i<List3Count;i++) {
			Face[List3[i]].CheckFlag=false;
		}
		List2Count=0;
	}
	int List2up(GVector &p,GFloat r) { //(おそらく)毎F更新の面当たり判定ﾘｽﾄ
		GVector p2;
		for(int i=0;i<List3Count;i++) {
			if(Face[List3[i]].CheckFlag==true) continue;
			p2=Face[List3[i]].Center-p;
			if(fabs(p2.x)>r+Face[List3[i]].Rx 
				|| fabs(p2.z)>r+Face[List3[i]].Rz 
				|| fabs(p2.y)>r+Face[List3[i]].Ry) continue;
			Face[List3[i]].CheckFlag=true;
			List2[List2Count++]=List3[i];
			if(List2Count>=FaceCount) List2Count=FaceCount;
		}
		return List2Count;
	}
	int List1up(GVector &p,GFloat r) { //(おそらく)毎StepTimeの系ごとの面当たり判定ﾘｽﾄ
		if(List1P==p && List1R==r) return List1Count;
		List1P=p;List1R=r;
		List1Count=0;
		GVector p2;
		for(int i=0;i<List2Count;i++) {
			p2=Face[List2[i]].Center-p;
			if(fabs(p2.x)>r+Face[List2[i]].Rx 
				|| fabs(p2.z)>r+Face[List2[i]].Rz 
				|| fabs(p2.y)>r+Face[List2[i]].Ry) continue;
			List1[List1Count++]=List2[i];
			
		}
		return List1Count;
	}
	int List0up(GVector &p,GFloat r) { //ﾁｯﾌﾟごとの面当たり判定ﾘｽﾄ
		List0Count=0;
		GVector p2;
		for(int i=0;i<List1Count;i++) {
			p2=Face[List1[i]].Center-p;
			if(fabs(p2.x)>r+Face[List1[i]].Rx 
				|| fabs(p2.z)>r+Face[List1[i]].Rz 
				|| fabs(p2.y)>r+Face[List1[i]].Ry) continue;
			List0[List0Count++]=List1[i];
			
		}
		return List0Count;
	}
	GFloat Check(const GVector &p,const GVector &v,GFloat v0_abs,GVector &n,GFloat &Ud,GFloat &Us) {
		int i;
		GVector norm;
		GFloat dmin=100000.0;
		GVector norm2;
		GFloat dmin2=100000.0; //最近傍裏面ﾎﾟﾘ
		for(i=0;i<List0Count;i++) {
			GFloat d=Face[List0[i]].Check3(p,v,dmin,dmin2,norm);
			if(dmin>d) { //一番初めに当たったﾎﾟﾘだけ判定したいのなら、めり込み量ではなく交点までの距離で計算すべき
				dmin=d;
				n=norm;
				Ud=Face[List0[i]].Ud;
				Us=Face[List0[i]].Us;
			}
			if(dmin2>d && d>0){ //一番近い裏面ﾎﾟﾘ取得
				dmin2=d;
				norm2=norm;
			}
		}
		//ﾎﾟﾘ食い回避
		GVector v_norm=v.normalize2();
		GFloat VNcos=-(v_norm.dot(n));
		if(-dmin > VNcos*v0_abs){ //v0_absじゃﾀﾞﾒっぽい?実際のDtでの移動距離がv0_absではない気がする
			GFloat VN2cos=v_norm.dot(norm2);
			if(VNcos<gEpsilon) VNcos=gEpsilon;
			if(VN2cos<gEpsilon) VN2cos=gEpsilon;
			if((-dmin2/VN2cos-dmin/VNcos)-0.0001f > 0){ //1e-4は両面ﾎﾟﾘ用のﾊﾞｲｱｽ
				dmin=100000.0;
			}
		}
		return dmin;
	}
	/*int Check2(GVector &p,GFloat r) { //これList0up相当の代物じゃねーか!!!なんでCheck2なんて名前ついてんだよ!!!
		HitListCount=0;
		GVector p2;
		for(int i=0;i<List1Count;i++) {
			p2=Face[List1[i]].Center-p;
			if(fabs(p2.x)>r+Face[List1[i]].Rx 
				|| fabs(p2.z)>r+Face[List1[i]].Rz 
				|| fabs(p2.y)>r+Face[List1[i]].Ry) continue;
			HitList[HitListCount++]=List1[i];
		}
		return HitListCount;
	}*/
	void ClipShadow(GVector shadow1[],int sn1,int FaceID,GVector shadow2[],int *sn2)
	{
		int wn1,wn2;
		static GVector work1[64],work2[64];
		ClipFace(&(Face[FaceID].OutVec2[0]),&Face[FaceID].Vertex[0],shadow1,sn1,work1,wn1);
		ClipFace(&(Face[FaceID].OutVec2[1]),&Face[FaceID].Vertex[1],work1,wn1,work2,wn2);
		ClipFace(&(Face[FaceID].OutVec2[2]),&Face[FaceID].Vertex[2],work2,wn2,shadow2,*sn2);
		
	}
	int CheckClip(GVector *A,GVector *P0,GVector *P1,GVector *P2,GVector *P)
	{
		GFloat a1=-A->dot(*P1-*P0);
		GFloat a2=-A->dot(*P2-*P0);
		if(a1<0 && a2<0) return -1; 
		if(a1>=0 && a2>=0) return 0; 
		GVector B=*P2-*P1;
		GFloat t=-a1/(-A->dot(B));
		*P=B*t+(*P1);
		if(a1<0) return 1;
		return 2;
	}
	void ClipFace(GVector *A,GVector *P0,GVector *Pt1,int n1,GVector *Pt2,int &n2)
	{
		GVector p;
		int s;
		n2=0;
		for(int i=0;i<n1;i++) {
			s=CheckClip(A,P0,&Pt1[i],&Pt1[(i+1)%n1],&p);
			if(s==0) {
				Pt2[n2++]=Pt1[i];
			}
			else if(s==1) {
				Pt2[n2++]=p;
			}
			else if(s==2) {
				Pt2[n2++]=Pt1[i];
				Pt2[n2++]=p;
			}
		}
	}
};
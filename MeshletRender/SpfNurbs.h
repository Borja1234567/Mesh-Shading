#include "Punto.h"
#include "Matriz.h"
#include "Configuracion.h"
#include <DirectXMath.h>

using namespace DirectX;

#ifndef SPF_NURBS_H
#define SPF_NURBS_H

//#define NUM_PUNTOS_X 2
//#define NUM_PUNTOS_Y 3
//#define NUM_PUNTOS_X 4
//#define NUM_PUNTOS_Y 4
//
//struct SimpleVertex
//{
//    D3DXVECTOR3 Pos;  
////	 D3DXVECTOR2 Tex; 
//};
class SpfNurbs
{
	
public:

	
	SpfNurbs(int knotsU, int knotsV, int cvsU, int cvsV, int degU, int degV){
		this->numKnotsU=knotsU;
		this->numKnotsV=knotsV;
		this->numCvsU=cvsU;
		this->numCvsV=cvsV;
		this->degreeU=degU;
		this->degreeV=degV;
		this->orderU=this->degreeU+1;
		this->orderV=this->degreeV+1;
		//Los puntos de control son numCvsU x numCvsV
		int tamanho=sizeof(Punto)*this->numCvsU;
		this->weight=(float**)malloc(sizeof(float)*this->numCvsU*2);
		this->puntos=(Punto**)malloc(tamanho);
		for(int i=0; i<this->numCvsU; i++){
			puntos[i]=(Punto *) malloc (sizeof(Punto)*this->numCvsV);
			weight[i]=(float*)malloc(sizeof(float)*this->numCvsV);
		}
		//Los pesos son orderU x orderV
		//this->weight=(float**)malloc(sizeof(float)*this->orderU);
		//for(int i=0;i<this->orderU;i++){
		//	weight[i]=(float*)malloc(sizeof(float)*this->orderV);
		//}

		this->knotsU=(float*)malloc(sizeof(float)*this->numKnotsU);
		this->knotsV=(float*)malloc(sizeof(float)*this->numKnotsV);
	}
	SpfNurbs(Punto** puntos, float* kntsU, float* kntsV, int knotsU, int knotsV, int cvsU, int cvsV, int degU, int degV){
		SpfNurbs(knotsU,knotsV, cvsU,  cvsV,  degU, degV);
		this->puntos=puntos;
		this->knotsU=kntsU;
		this->knotsV=kntsV;

}
	
	void setPunto(Punto punto, int x, int y);
	void setWeight(float peso, int x, int y);
	void setKnotU(float knot, int x);
	void setKnotV(float knot, int x);
	void setPuntos(Punto** punto);
	void setWeights(float** pesos);
	void setKnotsU(float* knotsU);
	void setKnotsV(float* knotsV);
	float* getKnotsU(){return knotsU;}
	float* getKnotsV(){return knotsV;}
	float** getWeights(){return weight;}
	float getWeight(int x, int y);
	Punto** getPuntos(){return puntos;}
	Punto getPunto(int x, int y);
	float getKnotU(int pos);
	float getKnotV(int pos);
	int getNumCvsU(){return numCvsU;}
	int getNumCvsV(){return numCvsV;}
	int getNumKnotsU(){return numKnotsU;}
	int getNumKnotsV(){return numKnotsV;}
	int getOrderU(){return orderU;}
	int getOrderV(){return orderV;}
	int MinU(){
		return this->minU;
		//return knotsU[degreeU];
	}
	int MinV(){
		return this->minV;
		//return knotsV[degreeV];
	}
	int MaxU(){
		return this->maxU;
		//return knotsU[numKnotsU-degreeU];
	}
	int MaxV(){
		return this->maxV;
		//return knotsV[numKnotsV-degreeV];
	}
int getResolucion(const XMFLOAT3* camara, int maxRes);

void setMaxU(float u){this->maxU=u;};
void setMaxV(float v){this->maxV=v;};
void setMinU(float u){this->minU=u;};
void setMinV(float v){this->minV=v;};
	//static Matriz matrizBase;
private:
	/*Punto puntos[NUM_PUNTOS];*/
	Punto** puntos;
	float* knotsU;
	float* knotsV;
	float** weight;
	int numKnotsU;
	int numKnotsV;
	int numCvsU;
	int numCvsV;	
	int degreeU;
	int degreeV;
	int orderU;
	int orderV;
	float maxU, maxV, minU, minV;

};
#endif SPF_NURBS_H
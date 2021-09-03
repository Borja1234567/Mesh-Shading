#include "SpfNurbs.h"
#include "stdafx.h"
#include "Punto.h"
#include "Matriz.h"
#include "Configuracion.h"
#include <DirectXMath.h>
using namespace std;
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


	SpfNurbs(int knotsU, int knotsV, int cvsU, int cvsV, int degU, int degV) {
		this->numKnotsU = knotsU;
		this->numKnotsV = knotsV;
		this->numCvsU = cvsU;
		this->numCvsV = cvsV;
		this->degreeU = degU;
		this->degreeV = degV;
		this->orderU = this->degreeU + 1;
		this->orderV = this->degreeV + 1;
		//Los puntos de control son numCvsU x numCvsV
		int tamanho = sizeof(Punto) * this->numCvsU;
		this->weight = (float**)malloc(sizeof(float) * this->numCvsU);
		this->puntos = (Punto**)malloc(tamanho);
		for (int i = 0; i < this->numCvsU; i++) {
			puntos[i] = (Punto*)malloc(sizeof(Punto) * this->numCvsV);
			weight[i] = (float*)malloc(sizeof(float) * this->numCvsV);
		}
		//Los pesos son orderU x orderV
		//this->weight=(float**)malloc(sizeof(float)*this->orderU);
		//for(int i=0;i<this->orderU;i++){
		//	weight[i]=(float*)malloc(sizeof(float)*this->orderV);
		//}

		this->knotsU = (float*)malloc(sizeof(float) * this->numKnotsU);
		this->knotsV = (float*)malloc(sizeof(float) * this->numKnotsV);
	}
	SpfNurbs(Punto** puntos, float* kntsU, float* kntsV, int knotsU, int knotsV, int cvsU, int cvsV, int degU, int degV) {
		SpfNurbs(knotsU, knotsV, cvsU, cvsV, degU, degV);
		this->puntos = puntos;
		this->knotsU = kntsU;
		this->knotsV = kntsV;

	}

	void setPunto(Punto punto, int x, int y);
	void setWeight(float peso, int x, int y);
	void setKnotU(float knot, int x);
	void setKnotV(float knot, int x);
	void setPuntos(Punto** punto);
	void setWeights(float** pesos);
	void setKnotsU(float* knotsU);
	void setKnotsV(float* knotsV);
	float* getKnotsU() { return knotsU; }
	float* getKnotsV() { return knotsV; }
	float** getWeights() { return weight; }
	float getWeight(int x, int y);
	Punto** getPuntos() { return puntos; }
	Punto getPunto(int x, int y);
	float getKnotU(int pos);
	float getKnotV(int pos);
	int getNumCvsU() { return numCvsU; }
	int getNumCvsV() { return numCvsV; }
	int getNumKnotsU() { return numKnotsU; }
	int getNumKnotsV() { return numKnotsV; }
	int getOrderU() { return orderU; }
	int getOrderV() { return orderV; }
	int MinU() {
		return this->minU;
		//return knotsU[degreeU];
	}
	int MinV() {
		return this->minV;
		//return knotsV[degreeV];
	}
	int MaxU() {
		return this->maxU;
		//return knotsU[numKnotsU-degreeU];
	}
	int MaxV() {
		return this->maxV;
		//return knotsV[numKnotsV-degreeV];
	}
	int getResolucion(const XMFLOAT3* camara, int maxRes);

	void setMaxU(float u) { this->maxU = u; };
	void setMaxV(float v) { this->maxV = v; };
	void setMinU(float u) { this->minU = u; };
	void setMinV(float v) { this->minV = v; };
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

void SpfNurbs::setWeight(float peso, int x, int y) {

	if ((x >= 0) && (x < this->getNumCvsU()) &&
		(y >= 0) && (y < this->getNumCvsV())) {
		weight[x][y] = peso;
	}
}



float SpfNurbs::getWeight(int x, int y) {
	if ((x >= 0) && (x < this->getNumCvsU()) &&
		(y >= 0) && (y < this->getNumCvsV())) {
		return weight[x][y];
	}
}

void SpfNurbs::setWeights(float** pesos) {
	this->weight = pesos;
}


void SpfNurbs::setPunto(Punto punto, int x, int y) {

	if ((x >= 0) && (x < this->getNumCvsU()) &&
		(y >= 0) && (y < this->getNumCvsV())) {
		puntos[x][y] = punto;
	}
}

Punto SpfNurbs::getPunto(int x, int y) {
	int a = 0;
	if ((x >= 0) && (x < this->getNumCvsU()) &&
		(y >= 0) && (y < this->getNumCvsV())) {
		return puntos[x][y];
	}
	//else{
	//	a++;
	//}
//	return NULL;
}

void SpfNurbs::setPuntos(Punto** puntos) {
	this->puntos = puntos;
}

void SpfNurbs::setKnotsU(float* knotsU) {
	this->knotsU = knotsU;
}

void SpfNurbs::setKnotsV(float* knotsV) {
	this->knotsV = knotsV;
}

void SpfNurbs::setKnotU(float knot, int x) {
	if ((x >= 0) && (x < this->numKnotsU)) {
		this->knotsU[x] = knot;
	}
}

void SpfNurbs::setKnotV(float knot, int x) {
	if ((x >= 0) && (x < this->numKnotsV)) {
		this->knotsV[x] = knot;
	}
}


float SpfNurbs::getKnotU(int pos) {
	if ((pos >= 0) && (pos < this->numKnotsU)) {
		return this->knotsU[pos];
	}
}
float SpfNurbs::getKnotV(int pos) {
	if ((pos >= 0) && (pos < this->numKnotsV)) {
		return this->knotsV[pos];
	}
}

int SpfNurbs::getResolucion(const XMFLOAT3* camara, int maxRes) {
	Punto p1;
	Punto p2;
	Punto d;
	Punto l;
	double distD;
	double distL;
	double resX, resY, resZ;
	int cond;
	int resolucion;
	p1 = this->getPunto(0, 0);
	p1 = this->getPunto(0, 1);


	d.setX(pow(p1.getX() - p2.getX(), 2));
	d.setY(pow(p1.getY() - p2.getY(), 2));
	d.setZ(pow(p1.getZ() - p2.getZ(), 2));
	distD = sqrt(d.getX() + d.getY() + d.getZ());
	l.setX(pow(camara->x - p1.getX(), 2));
	l.setY(pow(camara->y - p1.getY(), 2));
	l.setZ(pow(camara->z - p1.getZ(), 2));
	distL = sqrt(l.getX() + l.getY() + l.getZ());
	if (distD < 1) {
		cond = distL;
	}
	else {

		cond = distL / distD;
	}


	cond = cond / 10;
	int sb = 4;
	switch (cond) {

	case 0:

		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 1:
		maxRes -= 1;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 2:
		maxRes -= 2;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 3:
		maxRes -= 3;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 4:
		maxRes -= 4;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 5:
		maxRes -= 5;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 6:
		maxRes -= 6;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 7:
		maxRes -= 7;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;


	case 8:
		maxRes -= 8;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	case 9:
		maxRes -= 9;
		if (maxRes >= 0)
			sb = Configuracion::getConfiguracion().getSubdivX(maxRes);
		resolucion = sb;
		break;

	default:
		resolucion = sb;
		break;

	}

	return resolucion;
}
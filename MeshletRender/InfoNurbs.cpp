#include "InfoNurbs.h"
#include "stdafx.h"
#include "SpfNurbs.h"

#ifndef INFO_NURBS_H
#define INFO_NURBS_H
class InfoNurbs
{
public:
	InfoNurbs(SpfNurbs* nurbs, int nSpf) {
		this->nSpf = nSpf;
		this->nurbs = nurbs;
		contPtos = 0;
		contKnotsU = 0, contKnotsV = 0;
		this->generarInfo();
	}

	float* getPuntos() { return puntos; };
	float* getPuntosX() { return puntosX; };
	float* getPuntosY() { return puntosY; };
	float* getPuntosZ() { return puntosZ; };
	float* getPesos() { return pesos; };
	float* getKnotsU() { return knotsU; };
	float* getKnotsV() { return knotsV; };
	int* getTabla() { return tabla; };
	float* getTablaKnots() { return tablaKnots; }
	float* getTablaPtos() { return tablaPtos; }
	//He separado la informacion de la Tabla en dos, TablaKnots y TablaPtos, 
	//porque la informacion que envio a la GPU la hago asociada a un shader
	//y, en este caso, necesito la informaciion de TablaPtos en el HS y no la de 
	//TablaKnots (para hacer subdivision adaptativa)
	int getNumPtos() { return contPtos; };
	int getNumKnotsU() { return contKnotsU; };
	int getNumKnotsV() { return contKnotsV; };
	void repasar();
private:
	int nSpf;
	SpfNurbs* nurbs;
	float* puntos;
	float* puntosX;
	float* puntosY;
	float* puntosZ;
	int* nPuntosC;
	//int* nSpfT;
	int* nSpfC;
	float* pesos;
	float* knotsU;
	float* knotsV;
	int* tabla;
	float* tablaKnots;
	float* tablaPtos;
	int contPtos;
	int contKnotsU;
	int contKnotsV;

	void generarInfo();

};
#endif INFO_NURBS_H

void InfoNurbs::generarInfo() {

	int tamInicPtos = this->nSpf * 4 * 4 * 3 * 100;
	int tamInicPesos = this->nSpf * 4 * 4 * 100;
	int tamInicKnotsU = this->nSpf * 4 * 4 * 100;
	int tamInicKnotsV = this->nSpf * 4 * 4 * 100;
	int aux = 0;
	int spfExtra = 0;
	int indice = 0;
	int u = 0;
	puntos = (float*)malloc(sizeof(float) * tamInicPtos * 3);
	nPuntosC = (int*)malloc(sizeof(int) * nSpf);
	//nSpfT = (int*)malloc(sizeof(int) * 177);
	nSpfC = (int*)malloc(sizeof(int) * nSpf);//Numero de subsuperficies en cada superficie
	puntosX = (float*)malloc(sizeof(float) * tamInicPtos);
	puntosY = (float*)malloc(sizeof(float) * tamInicPtos);
	puntosZ = (float*)malloc(sizeof(float) * tamInicPtos);
	pesos = (float*)malloc(sizeof(float) * tamInicPesos);
	knotsU = (float*)malloc(sizeof(float) * tamInicKnotsU);
	knotsV = (float*)malloc(sizeof(float) * tamInicKnotsV);
	tabla = (int*)malloc(sizeof(int) * nSpf * 7);
	tablaKnots = (float*)malloc(sizeof(float) * nSpf * 4);
	tablaPtos = (float*)malloc(sizeof(float) * nSpf * 3);
	/*for(int i=0;i <nSpf;i++){
		tabla[i]=(int*)malloc(sizeof(int)*7);
	}
	*/
	int ptosX, ptosY, knotsX, knotsY;
	Punto p;
	int contPesos = 0;
	contKnotsU = 0, contKnotsV = 0;
	int contPtosXYZ = 0;

	for (int i = 0; i < nSpf; i++) {

		ptosX = nurbs[i].getNumCvsU();
		ptosY = nurbs[i].getNumCvsV();
		knotsX = nurbs[i].getNumKnotsU();
		knotsY = nurbs[i].getNumKnotsV();

		for (int j = 0; j < ptosX; j++) {
			for (int k = 0; k < ptosY; k++) {
				if ((contPtosXYZ + 3) >= tamInicPtos) {
					tamInicPtos *= 2;
					puntos = (float*)realloc(puntos, sizeof(float) * tamInicPtos * 3);
					puntosX = (float*)realloc(puntosX, sizeof(float) * tamInicPtos);
					puntosY = (float*)realloc(puntosY, sizeof(float) * tamInicPtos);
					puntosZ = (float*)realloc(puntosZ, sizeof(float) * tamInicPtos);
				}

				p = nurbs[i].getPunto(j, k);
				puntosX[contPtosXYZ] = p.getX();
				puntosY[contPtosXYZ] = p.getY();
				puntosZ[contPtosXYZ] = p.getZ();
				contPtosXYZ++;

				puntos[contPtos] = p.getX();
				contPtos++;
				puntos[contPtos] = p.getY();
				contPtos++;
				puntos[contPtos] = p.getZ();
				contPtos++;

				if ((contPtos) >= tamInicPesos) {
					tamInicPesos *= 2;
					//nPuntosC = (int*)realloc(puntos, sizeof(int) * tamInicPesos);
					pesos = (float*)realloc(pesos, sizeof(float) * tamInicPesos);
				}
				//nPuntosC[contPesos] = nurbs[i].getNumCvsU() * nurbs[i].getNumCvsU();
				pesos[contPesos] = nurbs[i].getWeight(j, k);
				contPesos++;
			}
		}

		for (int j = 0; j < knotsX; j++) {
			if (contKnotsU >= tamInicKnotsU) {
				tamInicKnotsU *= 2;
				knotsU = (float*)realloc(knotsU, sizeof(float) * tamInicKnotsU);
			}
			knotsU[contKnotsU] = nurbs[i].getKnotU(j);
			contKnotsU++;
		}
		for (int k = 0; k < knotsY; k++) {
			if (contKnotsV >= tamInicKnotsV) {
				tamInicKnotsV *= 2;
				knotsV = (float*)realloc(knotsV, sizeof(float) * tamInicKnotsV);
			}
			knotsV[contKnotsV] = nurbs[i].getKnotV(k);
			contKnotsV++;
		}

		if (i == 0) {//primera superficie
		//	tabla[i*7 + INICIO_PTOS]=0;
			tablaPtos[0] = 0; //Inicio ptos
		//	tabla[i*7 + KNOTS_U]=0;
		//	tabla[i*7 + KNOTS_V]=0;
			tablaKnots[0] = 0; //Inicio knotsU
			tablaKnots[1] = 0; //Inicio knotsV
			nPuntosC[i] = nurbs[i].getNumCvsU() * nurbs[i].getNumCvsV();
			nSpfC[i] = (nPuntosC[i] / 128) + 1;
			/*if (nSpfC[i] == 1) {
				nSpfT[i] == 1;
				indice++;
			}
			else {
				for (int j = 0; u < nSpfC[i]; j++) {
					nSpfC[indice + j] = 1;
					u = j;
				}
				indice = u + 1;
			}*/
			aux = nPuntosC[i];
		}
		else {
			//	tabla[i*7 + INICIO_PTOS]=tabla[(i-1)*7+INICIO_PTOS]+tabla[(i-1)*7+DIM_X]*tabla[(i-1)*7+DIM_Y];
			//	int t1,t2,t3,t4;
			//	t1=tablaPtos[(i-1)*3];
			//	t2=tablaPtos[(i-1)*3+1];
			//	t3=tablaPtos[(i-1)*3+2];
			tablaPtos[i * 3] = tablaPtos[(i - 1) * 3] + tablaPtos[(i - 1) * 3 + 1] * tablaPtos[(i - 1) * 3 + 2];
			//t4=tablaPtos[i*3];
			//int p1= tabla[i*7 + INICIO_PTOS];
		//	tabla[i*7 + KNOTS_U]=tabla[(i-1)*7+KNOTS_U]+tabla[(i-1)*7+DIM_KNOTS_X];
			//p1= tabla[i*7 + INICIO_PTOS];
		//	tabla[i*7 + KNOTS_V]=tabla[(i-1)*7+KNOTS_V]+tabla[(i-1)*7+DIM_KNOTS_Y];
			//p1= tabla[i*7 + INICIO_PTOS];
			tablaKnots[i * 4] = tablaKnots[(i - 1) * 4] + tablaKnots[(i - 1) * 4 + 2]; //Inicio knotsU
			tablaKnots[i * 4 + 1] = tablaKnots[(i - 1) * 4 + 1] + tablaKnots[(i - 1) * 4 + 3]; //Inicio knotsV
			nPuntosC[i] = nurbs[i].getNumCvsU() * nurbs[i].getNumCvsV() + aux;
			nSpfC[i] = ((nurbs[i].getNumCvsU() * nurbs[i].getNumCvsV()) / 128) + 1;
			int j = nSpfC[i];
			/*if (nSpfC[i] == 1) {
				nSpfT[indice] = i + 1;
				indice++;
			}
			else {
				for (int j = 0; u < nSpfC[i]; j++) {
					nSpfT[indice + j] = i + 1;
					u = j;
				}
				indice = u + 1;
			}*/
			aux = nPuntosC[i];
		}

		//	tabla[i*7 + DIM_X]= ptosX;
		//	tabla[i*7 + DIM_Y]= ptosY;
		tablaPtos[i * 3 + 1] = ptosX; //Numero de puntos en la direc parametrica U
		tablaPtos[i * 3 + 2] = ptosY;//Numero de puntos en la direc parametrica V

	//	tabla[i*7 + DIM_KNOTS_X]=knotsX;
	//	tabla[i*7 + DIM_KNOTS_Y]=knotsY;
		tablaKnots[i * 4 + 2] = knotsX; //numero de knots en la direccion parametrica U para la spf i
		tablaKnots[i * 4 + 3] = knotsY;//numero de knots en la direccion parametrica V para la spf i

	}
	/*for (int j = 0; j < nSpf; ++j)
	{
		spfExtra = spfExtra + nSpfC[j];
	}
	nSpfT = (int*)realloc(nSpfT, sizeof(int) * spfExtra);*/
	//repasar();
}

//
//void InfoNurbs::repasar(){
//	//float* ptosP=gEscena->getPuntos();
//
//	for(int i=0;i<this->nSpf;i++)
//	{
//		int inic=this->tablaPtos[i*3];
//		int j=this->tablaPtos[i*3+1];
//		int k=this->tablaPtos[i*3+2];
//		int pX, pY,pZ;
//		for(int jj=0;jj<j;jj++)
//		{
//			for(int kk=0;kk<k;kk++)
//			{
//				pX=this->tablaPtos[(inic+jj*k+kk)*3];
//				pY=this->tablaPtos[(inic+jj*k+kk)*3+1];
//				pZ=this->tablaPtos[(inic+jj*k+kk)*3+2];
//			}
//		}
//	}
//}



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
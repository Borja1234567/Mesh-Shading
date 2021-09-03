#include "SpfNurbs.h"
#include <vector>
#include <string>
#include "InfoNurbs.h"
#include <fstream>

using namespace std;
#ifndef ESCENA_NURBS_H
#define ESCENA_NURBS_H
struct IndicesNurbs
{
	int PosInic;
	int PosFin;
};


struct NurbsVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Patch;
	XMFLOAT2 Tex;
};
using namespace std;
class EscenaNurbs
{
public:
	EscenaNurbs() {
		numSuperficies = 0;
		numPtos = 0;
		pos = 0;
		numResolucion = 0;

		numIndices = 0;
		//objeto=new Objeto(4);
		bPtos = false;

		numVert = 0;
		int numPtos;
		numKnotsU = 0;
		numKnotsV = 0;
		bPtos = false;
	}
	int getNumSpf() { return numNurbs; }
	void setNumNurbs(int numNurbs);
	SpfNurbs getNurbs(int pos) {
		if ((pos >= 0) && (pos < numNurbs))
			return nurbs[pos];
	}

	float* getXs();
	float* getYs();
	float* getZs();
	float* getPuntos();
	bool comprobarFinLinea(char* lect);
	int getNumNurbs();
	void leerNurbs(int fch);
	float* calcularResolucion(const XMFLOAT3* camara, int resolucion);
	int getNumPtos() { return this->numVert; };
	int getNumKnotsU() { return this->numKnotsU; };
	int getNumKnotsV() { return this->numKnotsV; };
	NurbsVertex* obtenerVertices();
	SpfNurbs* getNurbs() { return nurbs; };
	float* getPuntoPeso(InfoNurbs* info);
	//Aqui deberia ir la funcion para leer Nurbs (en el .cpp)





protected:
	void setNumPtos(int nPtos) { this->numPtos = nPtos; };
	SpfNurbs* nurbs;
	int numNurbs;
	float* getPunto(int coordenada);
	int nVertices;
	void setNurbs(SpfNurbs n, int pos);

	int numPtos;
	int numKnotsU;
	int numKnotsV;
	bool bPtos;

private:
	float* ptos;
	void reparametrizar();
protected:
	int numVert;
	int numSuperficies;
	int pos;
	int numResolucion;
	int numVertices;
	long numIndices;
	NurbsVertex* nVtx;
};

#endif ESCENA_NURBS_H
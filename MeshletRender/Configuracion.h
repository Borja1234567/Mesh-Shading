#include "Constantes.h"
#include <string>
#include <wtypes.h>

#ifndef	CONFIGURACION_H
#define CONFIGURACION_H

class Configuracion
{

private:
	Configuracion() {
		maxResolucion = 9;
		metodo[0] = CPU;
		metodo[1] = GUTHE;
		metodo[2] = GUTHE_SPF;
		metodo[3] = GPU;
		metodo[4] = GPU_BATCH;
		metodo[5] = GPUGS;
		metodo[6] = GPUGS2PAS;/*Opciones: CPU, GUTHE, GUTHE_SPF, GPU,GPU_BATCH, GPUGS o GPUGS2PAS*/
		subdivisionX[0] = 4;
		subdivisionX[1] = 8;
		subdivisionX[2] = 16;//NUM_PUNTOS_X+0; /*El numero de subdiviones debe ...*/
		subdivisionX[3] = 32;
		subdivisionX[4] = 64;
		subdivisionX[5] = 128;
		subdivisionX[6] = 256;
		subdivisionX[7] = 512;
		subdivisionX[8] = 1024;
		subdivisionY[0] = 4;
		subdivisionY[1] = 8;
		subdivisionY[2] = 16;//NUM_PUNTOS_Y+0; /*...ser mayor o igual a NUM_PUNTOS*/
		subdivisionY[3] = 32;
		subdivisionY[4] = 64;
		subdivisionY[5] = 128;
		subdivisionY[6] = 256;
		subdivisionY[7] = 512;
		subdivisionY[8] = 1024;

		maxSubdiv[0] = 4;
		maxSubdiv[1] = 8;
		maxSubdiv[2] = 16;
		maxSubdiv[3] = 32;
		maxSubdiv[4] = 64;
		maxSubdiv[5] = 128;
		maxSubdiv[6] = 256;
		maxSubdiv[7] = 512;
		maxSubdiv[8] = 1024;
		maxSubdiv[10] = 10; //Caso especial de preprocesado previo de una superficie en la CPU
		topologia[0] = TOP_TRIANGLES; /*Opciones: TOP_TRIANGLES o TOP_POINTS o TOP_LINE*/
		topologia[1] = TOP_POINTS;
		topologia[2] = TOP_LINE;
/*		fichero[0] = L"teacup";
		fichero[1] = L"teapot";
		fichero[2] = L"10Tazas";
		fichero[3] = L"10Teteras";
		fichero[4] = L"20Tazas";
		fichero[5] = L"20Teteras";
		fichero[6] = L"30Tazas";
		fichero[7] = L"30Teteras";
		fichero[8] = L"40Tazas";
		fichero[9] = L"40Teteras";
		fichero[10] = L"50Tazas";
		fichero[11] = L"50Teteras";
		fichero[12] = L"100Tazas";
		fichero[13] = L"100Teteras";
		fichero[14] = L"Elefante";
		fichero[15] = L"10Elefante";
		fichero[16] = L"20Elefante";
		//fichero[17] = L"killeroo.iges";
		fichero[17] = L"headCen.iges";
		//fichero[17] = L"cuadrado.iges";
		//fichero[17] = L"peon.iges";*/
		//fichero[17] = L"AircraftHinge.iges";
		fichero[17] = L"headCen.iges";
		listaFich[0] = "teacup";
		listaFich[1] = "teapot";
		listaFich[2] = "10Tazas";
		listaFich[3] = "10Teteras";
		listaFich[4] = "20Tazas";
		listaFich[5] = "20Teteras";
		listaFich[6] = "30Tazas";
		listaFich[7] = "30Teteras";
		listaFich[8] = "40Tazas";
		listaFich[9] = "40Teteras";
		listaFich[10] = "50Tazas";
		listaFich[11] = "50Teteras";
		listaFich[12] = "100Tazas";
		listaFich[13] = "100Teteras";
		listaFich[14] = "Elefante";
		listaFich[15] = "10Elefante";
		listaFich[16] = "20Elefante";
		listaFich[17] = "CarR.iges";
		batching = 1;//Se van a agrupar las llamadas a draw para
					//hacer unicamente ''batching'' llamadas
					//Solo tiene sentido en gpu.




		estrategia = STRATEGY_EVAL_DIRECTA; /*Evaluacion en la CPU. Opciones: STRATEGY_EVAL_DIRECTA o STRATEGY_DE_CASTELJAU*/
		maxGrid[0] = 4;
		maxGrid[1] = 8;
		maxGrid[2] = 16; /*Metodo de Guthe. Tamanho de la malla enviada a la GPU por superficie=maxGrid x maxGrid*/
		maxGrid[3] = 32;
		maxGrid[4] = 64;
		maxGrid[5] = 128;
		maxGrid[6] = 256;
		maxGrid[7] = 512;
		maxGrid[8] = 1024;
	}



public:
	int metodo[7];
	int subdivisionX[9];
	int subdivisionY[9];
	int maxSubdiv[11];
	int topologia[3];
	int estrategia;
	const wchar_t* fichero[18];
	int maxGrid[9];
	int batching;
	int maxResolucion;
	std::string  listaFich[18];

	Configuracion static getConfiguracion() {
		Configuracion configuracion = Configuracion();
		return configuracion;
	}
	int getMaxSubdiv(int pos) {
		int res = maxSubdiv[pos];
		return res;
	}
	int getMetodo(int pos) { return metodo[pos]; }
	int getSubdivX(int pos) { return subdivisionX[pos]; }
	int getSubdivY(int pos) { return subdivisionY[pos]; }
	int getBatching() { return batching; }
	int getTopologia(int pos) { return topologia[pos]; }
	const wchar_t* getFichero(int pos) { return fichero[pos]; }
	std::string getNombreFich(int pos) { return listaFich[pos]; }
	int getEstrategia() { return estrategia; }
	int getMaxGrid(int pos) { return maxGrid[pos]; }
	int getMaxResolucion() { return maxResolucion; }



};


#endif CONFIGURACION_H
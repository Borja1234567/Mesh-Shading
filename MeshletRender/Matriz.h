#include "Punto.h"

#ifndef	MATRIZ_H
#define MATRIZ_H

class Matriz
{
public:
	Matriz(float p00, float p01, float p02, float p03,
		float p10, float p11, float p12, float p13,
		float p20, float p21, float p22, float p23,
		float p30, float p31, float p32, float p33) {

		matriz[0][0] = p00;
		matriz[0][1] = p01;
		matriz[0][2] = p02;
		matriz[0][3] = p03;
		matriz[1][0] = p10;
		matriz[1][1] = p11;
		matriz[1][2] = p12;
		matriz[1][3] = p13;
		matriz[2][0] = p20;
		matriz[2][1] = p21;
		matriz[2][2] = p22;
		matriz[2][3] = p23;
		matriz[3][0] = p30;
		matriz[3][1] = p31;
		matriz[3][2] = p32;
		matriz[3][3] = p33;
	}

	/**
	 * Specialization for multiplication by a Punto structure.
	 */
	inline Punto mult(Punto b)
	{
		return Punto(
			(b.getX() * this->matriz[0][0] + b.getY() * this->matriz[1][0] + b.getZ() * this->matriz[2][0] + b.getW() * this->matriz[3][0]),
			(b.getX() * this->matriz[0][1] + b.getY() * this->matriz[1][1] + b.getZ() * this->matriz[2][1] + b.getW() * this->matriz[3][1]),
			(b.getX() * this->matriz[0][2] + b.getY() * this->matriz[1][2] + b.getZ() * this->matriz[2][2] + b.getW() * this->matriz[3][2]),
			(b.getX() * this->matriz[0][3] + b.getY() * this->matriz[1][3] + b.getZ() * this->matriz[2][3] + b.getW() * this->matriz[3][3])
		);
	};


private:
	float matriz[4][4];

};

#endif MATRIZ_H
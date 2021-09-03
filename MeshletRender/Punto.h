#include <windows.h>
//#include <d3d10.h>
//Se puede cambiar a 12
#include <d3d12.h>
#include <cmath>

#ifndef	PUNTO_H
#define PUNTO_H

class Punto
{

public:
	Punto() { x = 0; y = 0; z = 0; }
	Punto(float valorX, float valorY, float valorZ) {
		x = valorX;
		y = valorY;
		z = valorZ;
		w = 0;
	}

	Punto(float valorX, float valorY, float valorZ, float valorW) {
		x = valorX;
		y = valorY;
		z = valorZ;
		w = valorW;
	}

	Punto operator += (Punto in) {

		this->setX(this->getX() + in.getX());
		this->setY(this->getY() + in.getY());
		this->setZ(this->getZ() + in.getZ());
		return *this;
	}
	Punto operator + (Punto in) {
		Punto p0;
		p0.setX(this->getX() + in.getX());
		p0.setY(this->getY() + in.getY());
		p0.setZ(this->getZ() + in.getZ());
		return p0;
	}

	Punto operator * (float in) {
		Punto p0;
		p0.setX(this->getX() * in);
		p0.setY(this->getY() * in);
		p0.setZ(this->getZ() * in);
		return p0;
	}

	Punto operator / (float in) {
		Punto p0;
		if (in != 0) {
			p0.setX(this->getX() / in);
			p0.setY(this->getY() / in);
			p0.setZ(this->getZ() / in);
		}
		return p0;
	}

	Punto operator - (Punto in) {
		Punto p0;
		p0.setX(this->getX() - in.getX());
		p0.setY(this->getY() - in.getY());
		p0.setZ(this->getZ() - in.getZ());
		return p0;
	}

	float dotProduct(Punto in) {
		float dot;
		dot = this->getX() * in.getX() +
			this->getY() * in.getY() +
			this->getZ() * in.getZ();
		return dot;
	}

	Punto crossProduct(Punto in) {
		Punto p0;
		p0.setX(this->getY() * in.getZ() - this->getZ() * in.getY());
		p0.setY(this->getZ() * in.getX() - this->getX() * in.getZ());
		p0.setZ(this->getX() * in.getY() - this->getY() * in.getX());
		return p0;
	}

	float length() {
		return sqrt(this->dotProduct(*this));
	}

	Punto normalize() {
		float magnitude = this->length();
		Punto p0;
		p0.setX(this->getX() / magnitude);
		p0.setY(this->getY() / magnitude);
		p0.setZ(this->getZ() / magnitude);
		return p0;
	}

	inline float get(int i) { return v[i]; };
	inline float getX() { return x; };
	inline float getY() { return y; };
	inline float getZ() { return z; };
	inline float getW() { return w; };
	inline void setX(float valorX) { x = valorX; }
	inline void setY(float valorY) { y = valorY; }
	inline void setZ(float valorZ) { z = valorZ; }
	inline void setW(float valorW) { w = valorW; }
	inline void set(int i, float valor) { v[i] = valor; };
private:
	union {
		struct {
			float x, y, z, w;
		};
		float	v[4];// for looping
	};
};
#endif PUNTO_H
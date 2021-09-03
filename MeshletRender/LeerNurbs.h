#include <fstream>
#include <string>
#include "Configuracion.h"
#include <string>

using namespace std;
#ifndef LEER_NURBS_H
#define LEER_NURBS_H

class LeerNurbs
{
public:
	LeerNurbs(int fch) {
		string fc = Configuracion::getConfiguracion().getNombreFich(fch);
		string::size_type loc = fc.find(".iges", 0);
		int n = string::npos;
		if (loc == std::string::npos) {
			//Error. es un fichero de NURBS
			printf("Error. Esto es un fichero de NURBS. No deberia llegar aqui");
		}
		const wchar_t* fichero;
		fichero = Configuracion::getConfiguracion().getFichero(fch);

		in.open(fichero);
		numSpf = 0;
		linea = string(" ");


	}

	void inicializar() {
		std::string lineaPar;
		getline(in, lineaPar);
		getline(in, lineaPar);
		getline(in, lineaPar);
		getline(in, lineaPar);
	}

	int getNumSpf() {
		numSpf = 0;
		while (true) {
			getline(in, linea);
			int pos = linea.find('D');
			if (pos != -1) {
				getline(in, linea);
				numSpf++;
			}
			else {
				//Quitamos 128
			/*	pos=linea.find_first_of(',');
				linea=linea.substr(pos+1, linea.size()-pos-1);*/

				return numSpf;
			}
		}
	}

	double getValor() {
		int posInic = linea.find_first_of(',');
		//int post = 0;
		if (posInic == -1) {
			posInic = linea.find_first_of(';');
		}

		std::string valor;
		if ((posInic == -1)) {
			getline(in, linea);
			posInic = linea.find_first_of(',');
			//Quitamos 128
			/*linea=linea.substr(posInic+1, linea.size()-posInic-1);
			posInic=linea.find_first_of(',');*/
		}
		valor = linea.substr(0, posInic);
		/*posInic++;
		post = posInic;
		posInic--;*/
		linea = linea.substr(posInic+1, linea.size() - posInic - 1);
		double t = strtod(valor.c_str(), NULL);
		return t;

	}
	void iniciarSpf() {
		std::string valor;
		int pos = linea.find(',');
		//int post = 0;
		if (pos == -1) {
			getline(in, linea);
		}
		pos = linea.find_first_of(',');
		valor = linea.substr(0, pos);
		if (valor != "128") {
			//errrorrrrrr
		}
		/*pos++;
		post = pos;
		pos--;*/
		linea = linea.substr(pos+1, linea.size() - pos - 1);

	}

private:
	ifstream in;
	int numSpf;
	std::string linea;
};
#endif LEER_NURBS_H
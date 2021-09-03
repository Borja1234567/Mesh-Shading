#include "SpfNurbs.h"
#include "stdafx.h"
#include "LeerNurbs.h"
#include "EscenaNurbs.h"
#include "InfoNurbs.h"
#include <vector>
#include <string>
#include <fstream>
using namespace std;

float* EscenaNurbs::getXs() {
	return  this->getPunto(COOR_X);
}

float* EscenaNurbs::getYs() {
	return this->getPunto(COOR_Y);
}

float* EscenaNurbs::getZs() {
	return this->getPunto(COOR_Z);
}

float* EscenaNurbs::getPuntos() {

	int orderU, orderV, ptosU, ptosV;
	int nSpf = this->getNumSpf();
	int nPts = nSpf * 4 * 4 * 3; //Partimos de que el
						//objetivo es trabajar
						//con superficies bicubicas
	/*if(bPtos){
		free(ptos);
	}*/
	bPtos = true;
	Punto pto;
	ptos = (float*)malloc(sizeof(float) * nPts);
	float* ptosNew;
	int cont = 0;
	for (int i = 0; i < nSpf; i++) {
		SpfNurbs n = this->getNurbs(i);
		orderU = n.getOrderU();
		orderV = n.getOrderV();
		ptosU = n.getNumCvsU();
		ptosV = n.getNumCvsV();
		for (int j = 0; j < ptosU; j++) {
			for (int k = 0; k < ptosV; k++) {
				if (cont >= nPts) {
					ptos = (float*)realloc(ptos, nPts * 2 * sizeof(float));
					//free(ptos);
					//ptos=ptosNew;
					nPts *= 2;
				}

				pto = n.getPunto(j, k);


				ptos[cont] = pto.getX();
				cont++;
				ptos[cont] = pto.getY();
				cont++;
				ptos[cont] = pto.getZ();
				cont++;


			}
		}


	}

	//float x,y,z;
	//for( int i=0;i<cont/3;i++)
	//{
	//	x=ptos[i*3];
	//	y=ptos[i*3+1];
	//	z=ptos[i*3+2];
	//}

	return ptos;
}



float* EscenaNurbs::getPuntoPeso(InfoNurbs* info) {

	int orderU, orderV;
	int nSpf = this->getNumSpf();
	int nPts = nSpf * 4 * 4 * 3 * 4; //Partimos de que el
						//objetivo es trabajar
						//con superficies bicubicas
	if (bPtos) {
		free(ptos);
	}
	float* pesos = info->getPesos();
	bPtos = true;
	Punto pto;
	ptos = (float*)malloc(sizeof(float) * nPts);
	float* ptosNew;
	int cont = 0;
	for (int i = 0; i < nSpf; i++) {
		SpfNurbs n = this->getNurbs(i);
		orderU = n.getOrderU();
		orderV = n.getOrderV();
		for (int j = 0; j < orderU; j++) {
			for (int k = 0; k < orderV; k++) {
				if (cont >= nPts) {
					ptosNew = (float*)realloc(ptos, nPts * 2 * sizeof(float));
					free(ptos);
					ptos = ptosNew;
					nPts *= 2;
				}

				pto = n.getPunto(j, k);


				ptos[cont] = pto.getX();
				cont++;
				ptos[cont] = pto.getY();
				cont++;
				ptos[cont] = pto.getZ();
				cont++;
				ptos[cont] = pesos[i * orderV + k];


			}
		}


	}
	return ptos;
}

float* EscenaNurbs::getPunto(int coordenada) {

	int orderU, orderV;
	int nSpf = this->getNumSpf();
	int nPts = nSpf * 4 * 4 * 1000; //Partimos de que el
						//objetivo es trabajar
						//con superficies bicubicas

	bPtos = true;
	Punto pto;
	ptos = (float*)malloc(sizeof(float) * nPts);
	float* ptosNew;
	int cont = 0;
	for (int i = 0; i < nSpf; i++) {
		SpfNurbs n = this->getNurbs(i);
		orderU = n.getNumCvsU();//n.getOrderU();
		orderV = n.getNumCvsV();//n.getOrderV();

		int m;
		for (int j = 0; j < orderU; j++) {
			for (int k = 0; k < orderV; k++) {
				if (cont >= nPts) {
					ptos/*New*/ = (float*)realloc(ptos, nPts * 2 * sizeof(float));
					/*free(ptos);
					ptos=ptosNew;*/
					nPts *= 2;
				}

				pto = n.getPunto(j, k);

				switch (coordenada) {
				default:
				case COOR_X:
					ptos[cont] = pto.getX();
					break;
				case COOR_Y:
					ptos[cont] = pto.getY();
					break;
				case COOR_Z:
					ptos[cont] = pto.getZ();
					break;
				}
				cont++;
			}
		}


	}

	return ptos;
}

//void Tokenize(const string & str,
//                      vector<string>& tokens,
//                      const string& delimiters = " ")
//{
//    // Skip delimiters at beginning.
//    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
//    // Find first "non-delimiter".
//    string::size_type pos     = str.find_first_of(delimiters, lastPos);
//
//    while (string::npos != pos || string::npos != lastPos)
//    {
//        // Found a token, add it to the vector.
//        tokens.push_back(str.substr(lastPos, pos - lastPos));
//        // Skip delimiters.  Note the "not_of"
//        lastPos = str.find_first_not_of(delimiters, pos);
//        // Find next "non-delimiter"
//        pos = str.find_first_of(delimiters, lastPos);
//    }
//}


char* getValor(char* lect) {
	std::string s;

	s = string(lect);
	int inic = s.find_first_of("\n");
	int fin = s.length();
	char* res = (char*)malloc(sizeof(char) * (fin - inic + 1));//(new char[fin-inic+1]);
	char* aux = (char*)malloc(sizeof(char) * (fin + 1));//(new char[fin-inic+1]);
	//string res=s.substr(inic);
	//copy(inic, fin, res);
	#pragma warning(suppress : 4996)
	strcpy(aux, s.c_str());
	res = aux + inic + 1;

	for (int i = 0; i < fin - inic; i++) {
		res[i] = aux[inic + i + 1];
	}
	free(aux);
	res[fin - inic] = '\0';
	return res;
}


bool EscenaNurbs::comprobarFinLinea(char* lect) {

	std::string s;
	s = string(lect);


	if (s.find_first_of("P") != -1) {
		return true;
	}

	/*if((lect[1]=='P') ||(lect[2]=='P')||(lect[3]=='P')){
	return true;
	}*/
	return false;
}
//void EscenaNurbs::leerNurbs(int subDivX, int subDivY, int fch, int resolucion){
////nurbs=(Nurbs*)malloc(sizeof(Nurbs)*(this->numNurbs));
//
//	Nurbs n2(8,8,4,4,3,3);
//	bool infoInicial=true;
//		int inic, fin;
//		int info[9];
//		int numSpf=0;
//	/*	IndicesNurbs* indNurbsNew;*/
//		std::string lineaImpar, lineaPar;
//		int aux;
//		std::string *ptrImp;
//		/*char *ptrPar;*/
//
//	std::string fc=Configuracion::getConfiguracion().getNombreFich(fch);
//
//Nurbs n3(8,8,4,4,3,3);
////nurbs=(Nurbs*)malloc(sizeof(Nurbs)*(this->numNurbs));
//std::string::size_type loc = fc.find(".iges",0);
//	int n=std::string::npos;
//	if( loc == std::string::npos ) {
//		   //Error. es un fichero de NURBS
//		printf("Error. Esto es un fichero de NURBS. No deberia llegar aqui");
////		return 0;
//	}
//	//nurbs=(Nurbs*)malloc(sizeof(Nurbs)*(this->numNurbs));
//	fichero=Configuracion::getConfiguracion().getFichero(fch);
//	ifstream in(fichero);
//	/*std::ofstream file;
//	file.open("lectura.txt");*/
//Nurbs n4(8,8,4,4,3,3);
//	/*int aproxSpf=100;*/
////	IndicesNurbs* indNurbs=(IndicesNurbs*) malloc(sizeof(IndicesNurbs) * aproxSpf);
//	int codNurbs=0;
//	getline(in, lineaPar);
//	getline(in, lineaPar);
//	getline(in, lineaPar);
//	getline(in, lineaPar);
//		while(codNurbs!=128){//Mientras no lleguemos al comienzo de la primera spf
//			in>>codNurbs;
//		}
//
//Nurbs n5(8,8,4,4,3,3);		
//int degU, degV,M1,M2, N1,N2, A, B,C;
//char d;
//	char coma;
//	//nurbs=(Nurbs*)malloc(sizeof(Nurbs)*(this->numNurbs));
//	Nurbs n9(8,8,4,4,3,3);
//char* num=(char*)malloc (sizeof(char)*100);
//Nurbs n10(8,8,4,4,3,3);
////file<<"INFO INICIAL\n";
//		std::string blanco=" ";
//		while(infoInicial){
//	
//				in.get(num, 1000, 'D');
//				in>>d;				
//				in.get(num, 1000, 'D');
//				in>>d;
//				in>>aux;
//				in>>codNurbs;
//				in>>coma;
//				if(coma==','){
//					infoInicial=false;
//				}
//				
//				numSpf++;
//				
//		}
//Nurbs n11(8,8,4,4,3,3);
//const char* valor;
//	//	file<<"NUMERO DE SUPERFICIES = ";
//	////	file<<numSpf;
//	//	file<<"\n";
//
//		this->numNurbs=numSpf;
//		//Nurbs* nrbs=(Nurbs*)malloc(sizeof(Nurbs));
//		nurbs=(Nurbs*)malloc(sizeof(Nurbs)*(this->numNurbs));
//		//num=(char*)malloc (sizeof(char)*10);
//		//Nurbs* nurbs[90];
//		//nurbs = malloc(sizeof(Nurbs)*(this->numNurbs));	
//	
//		Nurbs n12(8,8,4,4,3,3);
//		for(int nspf=0;nspf<numSpf;nspf++){
//			num[0]='\0';
//			//num="";
//		/*	file<<"SUPERFICIE =";
//			file<<nspf;*/
//			if(nspf!=0){
//				in>>codNurbs;
//				in >>coma;
//				if(codNurbs!=128){
//					//ERROR
//				}
//			}
//				for(int i=0;i<8;i++){
//					in>>info[i];
//					in >>coma;
//					/*file<<info[i];
//					file<<"\t";*/
//				}
//				/*file<<"\n";*/
//		
//
//		Nurbs n13(8,8,4,4,3,3);
//			degU=info[0];
//			/*file<<info[0];
//			file<<"\t";*/
//			degV=info[1];
//			/*file<<info[1];
//			file<<"\t";*/
//			M1=info[2];
//			M2=info[3];
//			N1=1+degU-M1;
//			N2=1+degV-M2;
//			A= N1 + 2 * M1;
//			B= N2 + 2 * M2;
//			C= (1+degU)*(1+degV);
//	//		Nurbs n2(A, B, degU+1, degV+1, degU, degV);
//			Nurbs n9(8,8,4,4,3,3);//(A, B, degU+1, degV+1, degU, degV);
//			
////			this->nurbs[nspf]=n2;
//			float knot;
//		/*	file<<"\nKNOT U";*/
//			in>>aux;
//			in >>coma;
//			for(int i=0;i<A+1;i++){
//				num[0]='\0';
//				in.get(num, 40, ',');
//				//in>>num;	
//				
//				if(comprobarFinLinea(num)){
//					/*	free(num);*/
//					/*num=(char*)malloc (sizeof(char)*10);*/
//					
//					num=getValor(num);
//				//in>>num;		
//
//				}
//				/*file<<num;
//				file<<"\t";*/
//					in >>coma;
//				
//				knot=atoi(num);
//				
//
//				//in>>knot;
//				n2.setKnotU(knot, i);
//			}
//			/*file<<"\n KNOT V";*/
//			for(int i=0;i<B+1;i++){
//	//			num=(char*)malloc (sizeof(char)*10);
//				in.get(num, 40, ',');
//				//in>>num;	
//				
//				if(comprobarFinLinea(num)){
//						/*free(num);*/
//					/*num=(char*)malloc (sizeof(char)*10);*/
//					num=getValor(num);
//				//in>>num;	
//				}
//				/*file<<num;
//				file<<"\t";*/
//					in >>coma;
//				
//				knot=atoi(num);
//				//in>>knot;
//				n2.setKnotV(knot, i);
//			}
//
//			float peso;
//		/*	file<<"\n PESOS";*/
//			for(int i=0;i<degU+1;i++){
//				for(int j=0;j<degV+1;j++){
//	//				num=(char*)malloc (sizeof(char)*10);	
//	
//					in.get(num, 40, ',');
//				//in>>num;		
////					in >>coma;
//					if(comprobarFinLinea(num)){
//						/*	free(num);*/
//						/*num=(char*)malloc (sizeof(char)*10);*/
//						num=getValor(num);
//				//in>>num;	
//					}
//					/*file<<num;
//						file<<"\t";*/
//						in >>coma;
//					
//					peso=atof(num);
//					n2.setWeight(peso, i, j);
//				}
//			}
//			/*file<<"\n PUNTOS";*/
//			
//			double ptoX, ptoY, ptoZ;
//			for(int i=0;i<degU+1;i++){
//				for(int j=0;j<degV+1;j++){
//					num[0]='\0';//=(char*)malloc (sizeof(char)*10);
//					getline(in, lineaPar);
//					//in.get(num,80, ',');
////					in >>coma;
//					if(comprobarFinLinea(num)){
//							/*free(num);*/
//						/*num=(char*)malloc (sizeof(char)*10);*/
//						num=getValor(num);
//				//in>>num;	
//					}
//					/*file<<"(";
//					file<<num;
//					file<<", ";*/
//						in >>coma;
//					ptoX = strtod(num, '\0');
//					float pX = strtod(num, '\0');
//					//ptoX=atof(num);
//					num[0]='\0';//=(char*)malloc (sizeof(char)*10);
//					
//					//in.get(num,200, ',');
//					getline(in, lineaPar);
//	//				in >>coma;
//					if(comprobarFinLinea(num)){
//							/*free(num);*/
//						/*num=(char*)malloc (sizeof(char)*10);*/
//						num=getValor(num);
//				//in>>num;	
//					}
//					/*file<<num;
//					file<<", ";*/
//						in >>coma;
//					
//					ptoY= strtod(num, '\0');//(atof(num);
//					num[0]='\0';//=(char*)malloc (sizeof(char)*10);
//					getline(in, lineaPar);
//					//in.get(num,80, ',');
//	//				in >>coma;
//					if(comprobarFinLinea(num)){
//						/*	free(num);*/
//						/*num=(char*)malloc (sizeof(char)*10);*/
//						num=getValor(num);
//				//in>>num;	
//					}
//					/*file<<num;
//					file<<")\t";*/
//						in >>coma;
//					
//					ptoZ= strtod(num, '\0');//atof(num);
//					n2.setPunto(Punto(ptoX,ptoY, ptoZ), i, j);
//				}
//					/*file.close();*/
//			}
//		//	nurbs[nspf]=n2;
//			float paremtricUinic, paremtricUfin,paremtricVinic,paremtricVfin;
//			in>>paremtricUinic;
//			in>>paremtricUfin;
//			in>>paremtricVinic;
//			in>>paremtricVfin;
////				free(num);
//			
//		}
//		
//}



void EscenaNurbs::leerNurbs(int fch) {
	//nurbs=(Nurbs*)malloc(sizeof(Nurbs)*(this->numNurbs));

	this->numKnotsU = 0;
	this->numKnotsV = 0;
	this->numPtos = 0;
	int t = 0;
	int pCont = 0;
	int kCont = 0;
	bool infoInicial = true;
	int inic, fin;
	int info[9];
	int numSpf = 0;
	/*	IndicesNurbs* indNurbsNew;*/
	//	std::string lineaImpar, lineaPar;
	int aux;
	//	std::string *ptrImp;
		/*char *ptrPar;*/



	LeerNurbs leer(fch);

	leer.inicializar();
	int M1, M2, N1, N2, A, B, C, K1, K2;

	this->numNurbs = leer.getNumSpf();
	//this->numNurbs=40;
	nurbs = (SpfNurbs*)malloc(sizeof(SpfNurbs) * (this->numNurbs));

	//Acordarse que el uno es this->numNurbs 
	for (int nspf = 0; nspf < this->numNurbs; nspf++) {

		leer.iniciarSpf();
		K1 = leer.getValor();
		K2 = leer.getValor();
		M1 = leer.getValor();
		M2 = leer.getValor();
		N1 = 1 + K1 - M1;
		N2 = 1 + K2 - M2;
		A = N1 + 2 * M1;
		B = N2 + 2 * M2;
		C = (1 + K1) * (1 + K2);

		SpfNurbs n2(A, B, K1 + 1, K2 + 1, M1, M2);
		float knot;

		double aux;
		for (int i = 0; i < 5; i++)
			aux = leer.getValor();

		for (int i = 0; i < A + 1; i++) {

			kCont++;
			knot = leer.getValor();

			n2.setKnotU(knot, i);
			this->numKnotsU++;
		}

		for (int i = 0; i < B + 1; i++) {

			kCont++;
			knot = leer.getValor();//atoi(num);

			n2.setKnotV(knot, i);
			this->numKnotsV++;
		}

		float peso;

		for (int i = 0; i < K1 + 1; i++) {
			for (int j = 0; j < K2 + 1; j++) {

				peso = leer.getValor();
				n2.setWeight(peso, i, j);
				pCont++;

			}
		}

		double ptoX, ptoY, ptoZ;
		for (int i = 0; i < K1 + 1; i++) {
			for (int j = 0; j < K2 + 1; j++) {

				ptoX = leer.getValor();

				ptoY = leer.getValor();

				ptoZ = leer.getValor();
				n2.setPunto(Punto(ptoX, ptoY, ptoZ), i, j);
				this->numPtos++;
				t++;
			}

		}

		float parametricUinic, parametricUfin, parametricVinic, parametricVfin;
		parametricUinic = leer.getValor();
		n2.setMinU(parametricUinic);
		parametricUfin = leer.getValor();
		n2.setMaxU(parametricUfin);
		parametricVinic = leer.getValor();
		n2.setMinV(parametricVinic);
		parametricVfin = leer.getValor();
		n2.setMaxV(parametricVfin);

		this->setNurbs(n2, nspf);

		/*	in>>paremtricUinic;
			in>>paremtricUfin;
			in>>paremtricVinic;
			in>>paremtricVfin;*/
			//				free(num);

	}
	int p = this->getNumPtos();
	this->reparametrizar();
	pCont++;
	kCont++;
	t++;


}

void EscenaNurbs::reparametrizar() {


	/*std::ofstream out;
	out.open("knots.txt");*/
	int nNurbs = this->getNumNurbs();
	int maxU, maxV;
	int numU, numV;
	float f;
	for (int i = 0; i < nNurbs; i++) {
		SpfNurbs n = this->nurbs[i];
		maxU = n.MaxU();
		numU = n.getNumKnotsU();

		for (int j = 0; j < numU; j++) {
			f = ((float)nurbs[i].getKnotU(j) / maxU);
			nurbs[i].setKnotU(f, j);
		}

		numV = n.getNumKnotsV();
		maxV = n.MaxV();
		for (int j = 0; j < numV; j++) {
			f = ((float)(nurbs[i].getKnotV(j) / maxV));
			nurbs[i].setKnotV(f, j);
		}
	}


	/*for(int i=0;i<nNurbs;i++){
		Nurbs n= this->nurbs[i];
		out<< "Knots de U \n";
		maxU=n.MaxU();
		numU=n.getNumKnotsU();

		out<<"\n superficie ";
			out << i;
			out <<"\n";
		for(int j=0;j<numU;j++){
			f=nurbs[i].getKnotU(j);
			out << f;
			out <<", ";
		}
	}



	for(int i=0;i<nNurbs;i++){
		Nurbs n= this->nurbs[i];
		out<< "\n Knots de V \n";

		numV=n.getNumKnotsV();

		out<<"\n superficie ";
			out << i;
			out <<"\n";
		for(int j=0;j<numV;j++){
			f=nurbs[i].getKnotV(j);
			out << f;
			out <<", ";
		}
	}

	out.close();*/
}


void EscenaNurbs::setNumNurbs(int numNurbs) {
	this->numNurbs = numNurbs;
}
int EscenaNurbs::getNumNurbs() {
	return  this->numNurbs;
}

float* EscenaNurbs::calcularResolucion(const XMFLOAT3* camara, int resolucion) {
	float* tess = (float*)malloc(sizeof(float) * this->getNumNurbs());
	for (int i = 0; i < this->getNumNurbs(); i++) {
		tess[i] = ((this->getNurbs())[i]).getResolucion(camara, resolucion);
	}
	return tess;
}
void EscenaNurbs::setNurbs(SpfNurbs n, int pos) {
	if ((pos >= 0) && (pos < numNurbs))
		nurbs[pos] = n;
}


NurbsVertex* EscenaNurbs::obtenerVertices() {

	long tam = this->getNumNurbs() * 1000;//this->getNumSpf()*100;
	numVert = 0;
	nVertices = 0;
	tam++;
	nVtx = (NurbsVertex*)malloc(sizeof(NurbsVertex) * tam);
	int knotsU, knotsV;

	float u1, u2, v1, v2;
	for (int i = 0; i < this->getNumSpf(); i++) {
		int ss = this->getNumSpf();
		SpfNurbs n = this->getNurbs(i);
		knotsU = n.getNumKnotsU();
		knotsV = n.getNumKnotsV();



		/*if(numVert>=tam){
			tam*=2;
			nVtx=(NurbsVertex*)realloc(nVtx, sizeof(NurbsVertex)*tam);
		}
		nVtx[this->numVert].Pos=D3DXVECTOR3(i,knotsU-1,knotsV-1);

		this->numVert++;*/
		for (float k = 0; k < knotsU - 1; k++) {
			u1 = n.getKnotU(k/*-3*/);
			u2 = n.getKnotU(k + 1);
			if (u1 != u2) {
				for (float l = 0; l < knotsV - 1; l++) {

					v1 = n.getKnotV(l);


					v2 = n.getKnotV(l + 1);
					if (v1 != v2) {
						for (float m = 0; m < 1; m++) {
							for (int ii = 0; ii < NUM_TESS_QUAD; ii++) {
								for (int jj = 0; jj < NUM_TESS_QUAD; jj++) {
									if (numVert >= tam - 1) {
										tam *= 2;
										nVtx = (NurbsVertex*)realloc(nVtx, sizeof(NurbsVertex) * tam);
									}
									nVtx[this->numVert].Pos = XMFLOAT3(i, k, l);
									nVtx[this->numVert].Tex = XMFLOAT2(0, 0);
									nVtx[this->numVert].Patch = XMFLOAT3(NUM_TESS_QUAD, ii, jj);

									this->numVert++;
									nVertices++;
								}
							}
						}
					}
				}
			}
		}

	}

	return nVtx;
}

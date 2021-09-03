#ifndef CONSTANTES_H
#define CONSTANTES_H

#define NUM_PUNTOS_X 4
#define NUM_PUNTOS_Y 4

#define NUM_TESS_QUAD 1


#define COOR_X	0	
#define COOR_Y	1
#define COOR_Z	2

#define TOP_TRIANGLES		0
#define TOP_POINTS			1
#define TOP_LINE			2
#define TOP_POINTS_ALL		3

#define STRATEGY_EVAL_DIRECTA	0
#define STRATEGY_DE_CASTELJAU	1
#define STRATEGY_EVALUACION	2

#define CPU 0
#define GPU 1
#define GPUGS 2
#define GUTHE 3
#define GUTHE_SPF 4
#define GPU_BATCH 5
#define GPUGS2PAS 6
#define GPUGSTRIANGLE 7
#define GPUGSCDRD 8
#define GPUGSCDRD_PREPROC 9
#define GPUGSTRIANGLEBZR 10
#define GPUGSTRIANGLEBZRADPT 11
#define NURBSCPU  12
#define NURBSGPUGS  13
#define NURBS2Pasadas 14


#define INICIO_PTOS 0
#define DIM_X		1
#define DIM_Y		2
#define DIM_KNOTS_X	3
#define DIM_KNOTS_Y	4
#define KNOTS_U		5
#define KNOTS_V		6


#define FILE_TEASPOON		0
#define FILE_TEACUP			1
#define FILE_TEAPOT			2
//struct SimpleVertex
//{
//    D3DXVECTOR3 Pos;  
//	// D3DXVECTOR2 Tex; 
//};
#endif CONSTANTES_H
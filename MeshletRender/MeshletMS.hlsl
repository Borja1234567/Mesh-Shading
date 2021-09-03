//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#define ROOT_SIG "CBV(b0), \
                  SRV(t0), \
                  SRV(t1), \
                  SRV(t2), \
                  SRV(t3), \
                  SRV(t4), \
                  SRV(t5), \
                  SRV(t6)"

struct Constants
{
    float4x4 World;
    float4x4 WorldView;
    float4x4 WorldViewProj;
    uint     DrawMeshlets;
};

struct VertexOut
{
    float4 PositionHS   : SV_Position;
    float3 PositionVS   : POSITION0;
    float3 Normal       : NORMAL0;
    uint   MeshletIndex : COLOR0;
};

ConstantBuffer<Constants> Globals             : register(b0);
StructuredBuffer<float3>  ptos				  : register(t2);
StructuredBuffer<float>   pesos				  : register(t3);
StructuredBuffer<float>   knotsU			  : register(t0);
StructuredBuffer<float>   knotsV			  : register(t1);
StructuredBuffer<float3>  tablaPtos			  : register(t5);
StructuredBuffer<float4>  tablaKnots		  : register(t4);
StructuredBuffer<uint>   indiceNurbs			  : register(t6);

float4 BasisFuncU(int i, float u, int p) {
	float left[5];
	float right[5];
	float saved;
	float N[5];
	N[0] = 1.0;
	float temp;
	float4 result;

	for (int j = 1; j <= p; j++) {
		left[j] = u - knotsU[i + 1 - j];
		right[j] = knotsU[i + j] - u;
		saved = 0.0f;
		for (int r = 0; r < j; r++) {
			temp = (float)(N[r] / (right[r + 1] + left[j - r]));
			N[r] = saved + right[r + 1] * temp;
			saved = left[j - r] * temp;
		}
		N[j] = saved;
	}

	result.x = N[0];
	result.y = N[1];
	result.z = N[2];
	result.w = N[3];
	return result;


}


float4 BasisFuncV(int i, float u, int p) {
	float left[5];
	float right[5];
	float saved;
	float N[5];
	N[0] = 1.0;
	float temp;
	float4 result;

	for (int j = 1; j <= p; j++) {
		left[j] = u - knotsV[i + 1 - j];
		right[j] = knotsV[i + j] - u;
		saved = 0.0f;
		for (int r = 0; r < j; r++) {
			temp = (float)(N[r] / (right[r + 1] + left[j - r]));
			N[r] = saved + right[r + 1] * temp;
			saved = left[j - r] * temp;
		}
		N[j] = saved;
	}

	result.x = N[0];
	result.y = N[1];
	result.z = N[2];
	result.w = N[3];
	return result;

}


/*Calculo de la Nurbs segun la segunda aproximacion. Esta es la que usamos*/
float3 NurbsEval2(int spf, float posKnotsU, float posKnotsV, float m, float l) {

	int inicioSpf = tablaPtos[spf].x;
	int dimX = tablaPtos[spf].y;//tablaPtos[spf*3+1];
	int dimY = tablaPtos[spf].z;//tablaPtos[spf*3+2];
	int dimKnotsU = tablaKnots[spf].z;
	int dimKnotsV = tablaKnots[spf].w;
	int inicKnotsU = tablaKnots[spf].x;
	int inicKnotsV = tablaKnots[spf].y;
	int inicPatch;

	int reso = 4;
	float divis, num;
	float3 numer;
	float aux = 0;
	float nik[4];
	float mjl[4];
	float hij;
	float3 bij = float3(0, 0, 0);

	float3 pto;

	float inicU[4], inicV[4], finU[4], finV[4];

	float us[4], vs[4];
	float bijX, bijY, bijZ;
	float aux2;

	float ml;


	float pt = 0;

	divis = 0;
	numer = float3(0, 0, 0);
	num = 0;


	int pos;
	nik[0] = 0;

	if ((m >= knotsU[inicKnotsU + posKnotsU + 1]) && (knotsU[inicKnotsU + posKnotsU + 1] != 1)) {
		posKnotsU++;
		while (knotsU[inicKnotsU + posKnotsU] == knotsU[inicKnotsU + posKnotsU + 1]) {
			posKnotsU++;
			inicioSpf++;
		}
	}
	if ((l >= knotsV[inicKnotsV + posKnotsV + 1]) && (knotsV[inicKnotsV + posKnotsV + 1] != 1)) {
		posKnotsV++;
		while (knotsV[inicKnotsV + posKnotsV] == knotsV[inicKnotsV + posKnotsV + 1]) {
			posKnotsV++;
			inicioSpf += dimX;
		}
	}

	float4 basisU = BasisFuncU(inicKnotsU + posKnotsU, m, 4);
	float4 basisV = BasisFuncV(inicKnotsV + posKnotsV, l, 4);

	nik[3] = basisU.x;
	nik[2] = basisU.y;
	nik[1] = basisU.z;
	nik[0] = basisU.w;
	mjl[3] = basisV.x;
	mjl[2] = basisV.y;
	mjl[1] = basisV.z;
	mjl[0] = basisV.w;

	float  x, y, z;


	int inKntU = 0;
	if (posKnotsU > 3) {
		inKntU = posKnotsU - 3;
	}
	int inKntV = 0;
	if (posKnotsV > 3) {
		inKntV = posKnotsV - 3;
	}
	int ii, jj;
	float3 nurbs = float3(0, 0, 0);;
	ii = 0, x = 0, y = 0, z = 0;
	float dvsr = 0;
	float mtl;

	//[unroll]
	for (int i = posKnotsU; i >= inKntU; i--) {
		jj = 0;
		//[unroll]
		for (int j = posKnotsV; j >= inKntV; j--) {

			inicPatch = inicioSpf + i + dimX * j;	//killeroo							
			//inicPatch=inicioSpf+i*dimX+j;	 //resto?
			bijX = ptos[inicPatch].x;
			bijY = ptos[inicPatch].y;
			bijZ = ptos[inicPatch].z;
			hij = pesos[inicPatch];
			aux = mul(hij, (mul(nik[ii], mjl[jj])));

			dvsr += mul(hij, (mul(nik[ii], mjl[jj])));

			nurbs.x += mul(bijX, aux);//mul(hij,mul(nik,mjl)));
			nurbs.y += mul(bijY, aux);//mul(hij,mul(nik,mjl)));
			nurbs.z += mul(bijZ, aux);//mul(hij,mul(nik,mjl)));	

			jj++;

		}
		ii++;
	}
	pto.x = nurbs.x / dvsr;
	pto.y = nurbs.y / dvsr;
	pto.z = nurbs.z / dvsr;



	if (dvsr == 0) {
		dvsr = 1;
	}

	return pto;
}

float3 calcNormal(float3 v1, float3 v2, float3 v3) {

	float3 normal;
	float3 v2v1;
	float4 p2, p1, p3;
	p1.xyz = v1;
	p2.xyz = v2;
	p3.xyz = v3;

	v2v1.x = p2.x - p1.x;
	v2v1.y = p2.y - p1.y;
	v2v1.z = p2.z - p1.z;

	float3 v3v1;
	v3v1.x = p3.x - p1.x;
	v3v1.y = p3.y - p1.y;
	v3v1.z = p3.z - p1.z;

	float3 cp = cross(v2v1, v3v1);

	normal = normalize(cp);

	return normal;
}

VertexOut GetVertexAttributes(uint meshletIndex, uint indice)
{

	float3 ptoCtrlA, ptoCtrlB, ptoCtrlC, ptoCtrlD;
	float posU, posV;
	VertexOut vout;

	int indiceV = indice % 9;
	int indiceU = indice % 81;
	int posKnotU = 0;
	if (indiceV == 0) {
		posV = 0;
	}
	else if (indiceV == 1) {
		posV = 0.125;
	}
	else if (indiceV == 2) {
		posV = 0.25;
	}
	else if (indiceV == 3) {
		posV = 0.375;
	}
	else if (indiceV == 4) {
		posV = 0.5;
	}
	else if (indiceV == 5) {
		posV = 0.625;
	}
	else if (indiceV == 6) {
		posV = 0.75;
	}
	else if (indiceV == 7) {
		posV = 0.875;
	}
	else if (indiceV == 8) {
		posV = 1;
	}

	if (indiceU < 9) {
		posU = 0;
		posKnotU = 0;
	}
	else if (indiceU < 18 && indiceU >= 9) {
		posU = 0.125;
		posKnotU = 1;
	}
	else if (indiceU < 27 && indiceU >= 18) {
		posU = 0.25;
		posKnotU = 2;
	}
	else if (indiceU < 36 && indiceU >= 27) {
		posU = 0.375;
		posKnotU = 3;
	}
	else if (indiceU < 45 && indiceU >= 36) {
		posU = 0.5;
		posKnotU = 4;
	}
	else if (indiceU < 54 && indiceU >= 45) {
		posU = 0.625;
		posKnotU = 5;
	}
	else if (indiceU < 63 && indiceU >= 54) {
		posU = 0.75;
		posKnotU = 6;
	}
	else if (indiceU < 72 && indiceU >= 63) {
		posU = 0.875;
		posKnotU = 7;
	}
	else if (indiceU < 81 && indiceU >= 72) {
		posU = 1;
		posKnotU = 8;
	}

	int iv = 3 + indiceV/2;
	int iu = 3 + posKnotU/2;
	if (meshletIndex == 0) {
			ptoCtrlA = NurbsEval2(meshletIndex, iu, iv, posU, posV);
			ptoCtrlB = NurbsEval2(meshletIndex, iu, iv, (posU + 0.33), posV);
			ptoCtrlC = NurbsEval2(meshletIndex, iu, iv, posU, (posV + 0.33));
	}
	else {
			ptoCtrlA = NurbsEval2(meshletIndex, iu, iv, posU, posV);
			ptoCtrlB = NurbsEval2(meshletIndex, iu, iv, (posU + 0.33), posV);
			ptoCtrlC = NurbsEval2(meshletIndex, iu, iv, posU, (posV + 0.33));
	}

	vout.PositionVS = mul(float4(ptoCtrlA, 1), Globals.WorldView).xyz;
	vout.PositionHS = mul(float4(ptoCtrlA, 1), Globals.WorldViewProj);
	vout.MeshletIndex = meshletIndex;
	vout.Normal = -mul(float4(1, 1, 100, 0), Globals.World).xyz;
	//vout.Normal = mul(float4(calcNormal(ptoCtrlA, ptoCtrlB, ptoCtrlC), 0), Globals.World).xyz;

    return vout;
}


[RootSignature(ROOT_SIG)]
[NumThreads(128, 1, 1)]
//[NumThreads(2, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    out indices uint3 tris[128],
    out vertices VertexOut verts[81]
)
{

	SetMeshOutputCounts(81, 128);

	if (gtid < 128)
    {
		tris[gtid] = uint3(indiceNurbs[gtid * 3], indiceNurbs[gtid * 3 + 1], indiceNurbs[gtid * 3 + 2]);
    }
	if (gtid < 81)
    {
		verts[gtid] = GetVertexAttributes(gid, gtid);
    }
}

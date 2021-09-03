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

/*#define ROOT_SIG "CBV(b0), \
                  RootConstants(b1, num32bitconstants=2), \
                  SRV(t0), \
                  SRV(t1), \
                  SRV(t2), \
                  SRV(t3), \
                  SRV(t4), \
                  SRV(t5), \
                  SRV(t6), \
                  SRV(t7), \
                  SRV(t8), \
                  SRV(t9), \
                  SRV(t10)"*/
#define ROOT_SIG "CBV(b0), \
                  RootConstants(b1, num32bitconstants=2), \
                  SRV(t0), \
                  SRV(t1), \
                  SRV(t2), \
                  SRV(t3), \
                  SRV(t4), \
                  SRV(t5), \
                  SRV(t6), \
                  SRV(t7), \
                  SRV(t8), \
                  SRV(t9), \
                  SRV(t10), \
                  SRV(t11)"
#ifndef BEZIER_HS_PARTITION
#define BEZIER_HS_PARTITION "integer"
#endif // BEZIER_HS_PARTITION

// The input patch size.  In this sample, it is 16 control points.
// This value should match the call to IASetPrimitiveTopology()
#define INPUT_PATCH_SIZE 1

// The output patch size.  In this sample, it is also 16 control points.
#define OUTPUT_PATCH_SIZE 1

//thresshold
#define NUMPX 1

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame : register(b0)
{
    matrix g_mViewProjection;
    matrix g_mView;
    float3 g_vCameraPosWorld;
    float  g_fTessellationFactor;
};


struct Table
{
    int ptos;
    int2 dim;
    int2 knots;
    int2 knotsDim;
};

//Texture2D tex2D;
//Texture2DArray tex2DA: register(t1);
SamplerState linearSample
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct Constants
{
    float4x4 World;
    float4x4 WorldView;
    float4x4 WorldViewProj;
    uint     DrawMeshlets;
};

struct MeshInfo
{
	float3 Pos;
	float3 Patch;
	float2 Tex;
};

struct Vertex
{
    float3 Position;
    float3 Normal;
};

struct NurbsVertex
{
	float3 Pos;
	float3 Patch;
	float2 Tex;
};

struct VertexOut
{
    float4 PositionHS   : SV_Position;
    float3 PositionVS   : POSITION0;
    float3 Normal       : NORMAL0;
    uint   MeshletIndex : COLOR0;
};

struct Meshlet
{
    uint VertCount;
    uint VertOffset;
    uint PrimCount;
    uint PrimOffset;
};

struct Punto
{
	float valorX;
	float valorY;
	float valorZ;
	float valorW;
};

ConstantBuffer<Constants> Globals             : register(b0);
ConstantBuffer<MeshInfo>  MeshInfo            : register(b1);

StructuredBuffer<Vertex>  Vertices            : register(t0);
StructuredBuffer<Meshlet> Meshlets            : register(t1);
ByteAddressBuffer         UniqueVertexIndices : register(t2);
StructuredBuffer<uint>    PrimitiveIndices    : register(t3);
StructuredBuffer<float3>  ptos				  : register(t6);
StructuredBuffer<float>   pesos				  : register(t7);
StructuredBuffer<float>   knotsU			  : register(t4);
StructuredBuffer<float>   knotsV			  : register(t5);
StructuredBuffer<uint>   numPtos			  : register(t8);
//StructuredBuffer<uint>   numSpf			      : register(t9);
StructuredBuffer<float3>  tablaPtos			  : register(t10);
StructuredBuffer<float4>  tablaKnots		  : register(t9);
StructuredBuffer<NurbsVertex>   infoNurbs			  : register(t11);

/////
// Data Loaders

uint3 UnpackPrimitive(uint primitive)
{
    // Unpacks a 10 bits per index triangle from a 32-bit uint.
    return uint3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}
uint3 GetPrimitive(Meshlet m, uint index)
//uint3 GetPrimitive(Nurbs m, uint index)
{
	//int dist = sqrt(pow((ptos[index+1].x - ptos[index].x),2) + pow((ptos[index + 1].y - ptos[index].y),2) + pow((ptos[index + 1].z - ptos[index].z),2));
	//return UnpackPrimitive(PrimitiveIndices[dist + index]);
	//return UnpackPrimitive(PrimitiveIndices[index]);
    return UnpackPrimitive(PrimitiveIndices[m.PrimOffset + index]);
}
uint GetVertexIndex(Meshlet m, uint localIndex)
//uint GetVertexIndex(Nurbs m, uint localIndex)
{
	//int dist = sqrt(pow((ptos[localIndex + 1].x - ptos[localIndex].x), 2) + pow((ptos[localIndex + 1].y - ptos[localIndex].y), 2) + pow((ptos[localIndex + 1].z - ptos[localIndex].z), 2));
    //localIndex = dist + localIndex;
	localIndex = localIndex;
    //localIndex = m.VertOffset + localIndex;

    if (localIndex == 4) // 32-bit Vertex Indices
    {
        return UniqueVertexIndices.Load(localIndex * 4);
    }
    else // 16-bit Vertex Indices
    {
        // Byte address must be 4-byte aligned.
        uint wordOffset = (localIndex & 0x1);
        uint byteOffset = (localIndex / 2) * 4;

        // Grab the pair of 16-bit indices, shift & mask off proper 16-bits.
        uint indexPair = UniqueVertexIndices.Load(byteOffset);
        uint index = (indexPair >> (wordOffset * 16)) & 0xffff;

        return index;
    }
}

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

static uint3 spfIndices[] = {
	/*
	uint3(4,1,0),
	uint3(4,5,1),
	uint3(5,2,1),
	uint3(5,6,2),
	uint3(6,3,2),
	uint3(6,7,3),
	uint3(8,5,4),
	uint3(8,9,5),
	uint3(9,6,5),
	uint3(9,10,6),
	uint3(10,7,6),
	uint3(10,11,7),
	uint3(12,9,8),
	uint3(12,13,9),
	uint3(13,10,9),
	uint3(13,14,10),
	uint3(14,11,10),
	uint3(14,15,11),*/
	uint3(0,1,5),
	uint3(1,6,5),
	uint3(1,2,6),
	uint3(2,7,6),
	uint3(2,3,7),
	uint3(3,8,7),
	uint3(3,4,8),
	uint3(4,9,8),
	uint3(5,6,10),
	uint3(6,11,10),
	uint3(6,7,11),
	uint3(7,12,11),
	uint3(7,8,12),
	uint3(8,13,12),
	uint3(8,9,13),
	uint3(9,14,13),
	uint3(10,11,15),
	uint3(11,16,15),
	uint3(11,12,16),
	uint3(12,17,16),
	uint3(12,13,17),
	uint3(13,18,17),
	uint3(13,14,18),
	uint3(14,19,18),
	uint3(15,16,20),
	uint3(16,21,20),
	uint3(16,17,21),
	uint3(17,22,21),
	uint3(17,18,22),
	uint3(18,23,22),
	uint3(18,19,23),
	uint3(19,24,23),
};

//VertexOut GetVertexAttributes(uint meshletIndex, uint vertexIndex, uint control, uint indice)
VertexOut GetVertexAttributes(uint meshletIndex, uint vertexIndex, uint indice)
{

	float3 ptoCtrlA, ptoCtrlB, ptoCtrlC, ptoCtrlD;
	uint t = 0;
	uint j = 0;
	float posU, posV;
	int inicKnotsU = tablaKnots[meshletIndex].x;
	int inicKnotsV = tablaKnots[meshletIndex].y;
	VertexOut vout;

	int indiceV = indice % 5;
	int indiceU = indice % 25;
	if (indiceV == 0) {
		posV = 0;
	}
	else if (indiceV == 1) {
		posV = 0.25;
	}
	else if (indiceV == 2) {
		posV = 0.5;
	}
	else if (indiceV == 3) {
		posV = 0.75;
	}
	else if (indiceV == 4) {
		posV = 1;
	}

	if (indiceU <= 4) {
		posU = 0;
	}
	else if (indiceU <= 9 && indiceU > 4) {
		posU = 0.25;
	}
	else if (indiceU <= 14 && indiceU > 9) {
		posU = 0.5;
	}
	else if (indiceU <= 19 && indiceU > 14) {
		posU = 0.75;
	}
	else if (indiceU <= 24 && indiceU > 19) {
		posU = 1;
	}

	if (meshletIndex == 0) {
		if (indice < numPtos[0]) {
			//ptoCtrlA = ptos[indice];
			//float minU = knotsU[inicKnotsU + indiceU + 1];
			//float minV = knotsV[inicKnotsV + indiceV + 1];
			ptoCtrlA = NurbsEval2(MeshInfo.Pos.x, MeshInfo.Pos.y, MeshInfo.Pos.z, posU, posV);
			ptoCtrlB = NurbsEval2(0, 4, 4, (posU + 0.33), posV);
			ptoCtrlC = NurbsEval2(0, 4, 4, posU, (posV + 0.33));
		}
	}
	else {
			t = numPtos[meshletIndex - 1];
			if (indice < numPtos[meshletIndex] - numPtos[meshletIndex - 1]) {
				//float minU = knotsU[inicKnotsU + indiceU + 1];
				//float minV = knotsV[inicKnotsV + indiceV + 1];
				ptoCtrlA = NurbsEval2(MeshInfo.Pos.x, MeshInfo.Pos.y, MeshInfo.Pos.z, posU, posV);
				ptoCtrlB = NurbsEval2(meshletIndex, 4, 4, (posU + 0.33), posV);
				ptoCtrlC = NurbsEval2(meshletIndex, 4, 4, posU, (posV + 0.33));
					//ptoCtrlA = ptos[indice + t];
					//ptoCtrlB = ptos[indice + t + 2];
					//ptoCtrlC = ptos[indice + t + 3];
					//ptoCtrlD = ptos[indice + t + 1];
			}
			else {
				//ptoCtrlA = ptos[indice];
			}
		
	}

	Vertex v = Vertices[vertexIndex];

	

	vout.PositionVS = mul(float4(ptoCtrlA, 1), Globals.WorldView).xyz;
	vout.PositionHS = mul(float4(ptoCtrlA, 1), Globals.WorldViewProj);
	vout.MeshletIndex = meshletIndex;
	vout.Normal = mul(float4(v.Position, 0), Globals.World).xyz;
	//vout.Normal = mul(float4(calcNormal(ptoCtrlA, ptoCtrlB, ptoCtrlC), 0), Globals.World).xyz;

    return vout;
}


[RootSignature(ROOT_SIG)]
[NumThreads(32, 1, 1)]
//[NumThreads(2, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    out indices uint3 tris[32],
    out vertices VertexOut verts[128]
)
{
	Meshlet m = Meshlets[gid];

	SetMeshOutputCounts(numPtos[gid], 128);

	if (gtid < 128)
    {
		//tris[gtid] = GetPrimitive(m, gtid);
        tris[gtid] = spfIndices[gtid];
    }
	if (gtid < numPtos[gid])
    {
        uint vertexIndex = GetVertexIndex(m, gtid);
		verts[gtid] = GetVertexAttributes(gid, vertexIndex, gtid);
    }
}

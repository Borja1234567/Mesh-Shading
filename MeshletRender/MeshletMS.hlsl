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

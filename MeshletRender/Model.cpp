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
#include "stdafx.h"
#include "Model.h"

#include "DXSampleHelper.h"

#include <fstream>
#include <unordered_set>
#include "InfoNurbs.h"
#include "EscenaNurbs.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace
{
    const D3D12_INPUT_ELEMENT_DESC c_elementDescs[Attribute::Count] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
    };
}

HRESULT Model::LoadFromFileN()
{

    // Populate mesh data from binary data and metadata.
    m_meshes.resize(1);
    for (uint32_t i = 0; i < static_cast<uint32_t>(1); ++i)
    {
        auto& mesh = m_meshes[i];

        // Vertex data & layout metadata

        // Determine the number of unique Buffer Views associated with the vertex attributes & copy vertex buffers.
        std::vector<uint32_t> vbMap;

        // Populate the vertex buffer metadata from accessors.
        for (uint32_t j = 0; j < 2; ++j)
        {
            auto it = std::find(vbMap.begin(), vbMap.end(), 2);
            D3D12_INPUT_ELEMENT_DESC desc = c_elementDescs[j];
            desc.InputSlot = static_cast<uint32_t>(std::distance(vbMap.begin(), it));

            mesh.LayoutElems[mesh.LayoutDesc.NumElements++] = desc;
        }

    }

    return S_OK;
}

HRESULT Model::UploadGpuResourcesN(ID3D12Device* device, ID3D12CommandQueue* cmdQueue, ID3D12CommandAllocator* cmdAlloc,
    ID3D12GraphicsCommandList* cmdList, EscenaNurbs* gEscena)
{
    InfoNurbs* info;
    for (uint32_t i = 0; i < m_meshes.size(); ++i)
    {
        auto& m = m_meshes[i];
        info = new InfoNurbs(gEscena->getNurbs(), gEscena->getNumNurbs());
        int u = 0;
        int* indiceSpf = (int*)malloc(sizeof(int) * 64 * 6);
        indiceSpf[0] = 0;
        for (int i = 0; i < (9*9 - 9); i++) {
            if ((i + 1) % 9 != 0) {
                indiceSpf[u] = i;
                indiceSpf[u + 1] = i + 1;
                indiceSpf[u + 2] = i + 9;
                indiceSpf[u + 3] = i + 1;
                indiceSpf[u + 4] = i + 10;
                indiceSpf[u + 5] = i + 9;
                u = u + 6;
            }
        }

        // Create committed D3D resources of proper sizes
        auto knotUDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(float) * info->getNumKnotsU());
        auto knotVDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(float) * info->getNumKnotsV());
        auto ptosDesc = CD3DX12_RESOURCE_DESC::Buffer(info->getNumPtos() * sizeof(float));
        auto weightDesc = CD3DX12_RESOURCE_DESC::Buffer(info->getNumPtos() * sizeof(float));
        auto tablaKnotsDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(float) * gEscena->getNumNurbs() * 4);
        auto tablaPtosDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(float) * gEscena->getNumNurbs() * 3);
        auto infoNurbsVertexDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(int) * gEscena->getNumNurbs() * 64 * 6);

        auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &knotUDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m.KnotUResource)));
        ThrowIfFailed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &knotVDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m.KnotVResource)));
        ThrowIfFailed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &ptosDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m.PtosResource)));
        ThrowIfFailed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &weightDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m.WeightResource)));
        ThrowIfFailed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &tablaKnotsDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m.TablaKnotsResource)));
        ThrowIfFailed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &tablaPtosDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m.TablaPtosResource)));
        ThrowIfFailed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &infoNurbsVertexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m.indiceNurbsResource)));

        // Create upload resources
        ComPtr<ID3D12Resource>              knotUUpload;
        ComPtr<ID3D12Resource>              knotVUpload;
        ComPtr<ID3D12Resource>              ptosUpload;
        ComPtr<ID3D12Resource>              weightUpload;
        ComPtr<ID3D12Resource>              tablaKnotsUpload;
        ComPtr<ID3D12Resource>              tablaPtosUpload;
        ComPtr<ID3D12Resource>              indiceNurbsUpload;

        auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        ThrowIfFailed(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &knotUDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&knotUUpload)));
        ThrowIfFailed(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &knotVDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&knotVUpload)));
        ThrowIfFailed(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &ptosDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&ptosUpload)));
        ThrowIfFailed(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &weightDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&weightUpload)));
        ThrowIfFailed(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &tablaKnotsDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&tablaKnotsUpload)));
        ThrowIfFailed(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &tablaPtosDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&tablaPtosUpload)));
        ThrowIfFailed(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &infoNurbsVertexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indiceNurbsUpload)));

        // Map & copy memory to upload heap
        {
            uint8_t* memory = nullptr;
            knotUUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
            std::memcpy(memory, info->getKnotsU(), info->getNumKnotsU() * sizeof(float));
            knotUUpload->Unmap(0, nullptr);
        }

        {
            uint8_t* memory = nullptr;
            knotVUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
            std::memcpy(memory, info->getKnotsV(), info->getNumKnotsV() * sizeof(float));
            knotVUpload->Unmap(0, nullptr);
        }

        {
            uint8_t* memory = nullptr;
            ptosUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
            std::memcpy(memory, gEscena->getPuntos(), info->getNumPtos() * sizeof(float));
            ptosUpload->Unmap(0, nullptr);
        }

        {
            uint8_t* memory = nullptr;
            weightUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
            std::memcpy(memory, info->getPesos(), sizeof(float) * info->getNumPtos());
            weightUpload->Unmap(0, nullptr);
        }

        {
            uint8_t* memory = nullptr;
            tablaKnotsUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
            std::memcpy(memory, info->getTablaKnots(), gEscena->getNumNurbs() * 4 * sizeof(float));
            tablaKnotsUpload->Unmap(0, nullptr);
        }

        {
            uint8_t* memory = nullptr;
            tablaPtosUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
            std::memcpy(memory, info->getTablaPtos(), gEscena->getNumNurbs() * 3 * sizeof(float));
            tablaPtosUpload->Unmap(0, nullptr);
        }

        {
            uint8_t* memory = nullptr;
            indiceNurbsUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
            std::memcpy(memory, indiceSpf, 64 * 6 * sizeof(int));
            indiceNurbsUpload->Unmap(0, nullptr);
        }

        // Populate our command list
        cmdList->Reset(cmdAlloc, nullptr);

        D3D12_RESOURCE_BARRIER postCopyBarriers[7];

        cmdList->CopyResource(m.KnotUResource.Get(), knotUUpload.Get());
        postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(m.KnotUResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        cmdList->CopyResource(m.KnotVResource.Get(), knotVUpload.Get());
        postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m.KnotVResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        
        cmdList->CopyResource(m.PtosResource.Get(), ptosUpload.Get());
        postCopyBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(m.PtosResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        
        cmdList->CopyResource(m.WeightResource.Get(), weightUpload.Get());
        postCopyBarriers[3] = CD3DX12_RESOURCE_BARRIER::Transition(m.WeightResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        
        cmdList->CopyResource(m.TablaKnotsResource.Get(), tablaKnotsUpload.Get());
        postCopyBarriers[4] = CD3DX12_RESOURCE_BARRIER::Transition(m.TablaKnotsResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        cmdList->CopyResource(m.TablaPtosResource.Get(), tablaPtosUpload.Get());
        postCopyBarriers[5] = CD3DX12_RESOURCE_BARRIER::Transition(m.TablaPtosResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        
        cmdList->CopyResource(m.indiceNurbsResource.Get(), indiceNurbsUpload.Get());
        postCopyBarriers[6] = CD3DX12_RESOURCE_BARRIER::Transition(m.indiceNurbsResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        cmdList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);

        ThrowIfFailed(cmdList->Close());

        ID3D12CommandList* ppCommandLists[] = { cmdList };
        cmdQueue->ExecuteCommandLists(1, ppCommandLists);

        // Create our sync fence
        ComPtr<ID3D12Fence> fence;
        ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

        cmdQueue->Signal(fence.Get(), 1);

        // Wait for GPU
        if (fence->GetCompletedValue() != 1)
        {
            HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            fence->SetEventOnCompletion(1, event);

            WaitForSingleObjectEx(event, INFINITE, false);
            CloseHandle(event);
        }
    }

    return S_OK;
}
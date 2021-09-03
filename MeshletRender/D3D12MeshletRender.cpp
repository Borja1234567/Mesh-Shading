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
#include "D3D12MeshletRender.h"
#include "EscenaNurbs.h"

const DWORD MIN_DIVS = 1;
const DWORD MAX_DIVS = 64;
//L"..\\Assets\\Dragon_LOD0.bin"
//AircraftHinge.iges
const wchar_t* D3D12MeshletRender::c_meshFilename = L"..\\Assets\\Dragon_LOD0.bin";

const wchar_t* D3D12MeshletRender::c_meshShaderFilename = L"MeshletMS.cso";
const wchar_t* D3D12MeshletRender::c_pixelShaderFilename = L"MeshletPS.cso";
EscenaNurbs* gEscena;
struct CB_PER_FRAME_CONSTANTS
{
    XMFLOAT4X4 mViewProjection;
    XMFLOAT4X4 mView;
    XMFLOAT3 vCameraPosWorld;
    float fTessellationFactor;
};

struct CB_KNOTS
{
    float* knotsU;
    float* knotsV;
};
struct CB_TABLA
{
    //int inicioSpf;
    XMFLOAT3 spf;
    //D3DXVECTOR2 dim;
    //D3DXVECTOR2 Knotsdim;
    //D3DXVECTOR2 Knots;
};

struct CB_NURBS
{
    float* ptosX;//[18000];
    float* ptosY;//[18000];
    float* ptosZ;//[18000];
    float* pesos;//[18000];
    float* knotsU;//[2000];
    float* knotsV;//[2000];
};

ID3D12Resource* g_pcbPerFrame = NULL;
ID3D12Resource* g_pcbTabla = NULL;
ID3D12Resource* g_pcbPtosX = NULL;
ID3D12Resource* g_pcbPtosY = NULL;
ID3D12Resource* g_pcbPtosZ = NULL;
ID3D12Resource* g_pcbPesos = NULL;
ID3D12Resource* g_pcbKnotsU = NULL;
ID3D12Resource* g_pcbKnotsV = NULL;
ID3D12Resource* g_pcbPtos = NULL;

ID3D12Resource* g_pcbTablePtos = NULL;
ID3D12Resource* g_pcbTableKnots = NULL;
D3D12_SHADER_RESOURCE_VIEW_DESC* g_ptbPtos = NULL;
D3D12_SHADER_RESOURCE_VIEW_DESC* g_ptbPesos = NULL;
D3D12_SHADER_RESOURCE_VIEW_DESC* g_ptbKnotsU = NULL;
D3D12_SHADER_RESOURCE_VIEW_DESC* g_ptbKnotsV = NULL;
D3D12_SHADER_RESOURCE_VIEW_DESC* g_ptbTablePtos = NULL;
D3D12_SHADER_RESOURCE_VIEW_DESC* g_ptbTableKnots = NULL;

/*
CDXUTDialogResourceManager          g_DialogResourceManager; // manager for shared resources of dialogs
CModelViewerCamera                  g_Camera;                // A model viewing camera
CD3DSettingsDlg                     g_D3DSettingsDlg;        // Device settings dialog
CDXUTDialog                         g_HUD;                   // manages the 3D
CDXUTDialog                         g_SampleUI;              // dialog for sample specific controls
*/

// Control variables

float despl = 1;
int avance = 1;
float curX = -0.0f;
float curY = -0.0f;
float curZ = -0.0f;

int numRepet = 0;
int numFich = 0;
float                               g_fSubdivs = 4;          // Startup subdivisions per side
bool                                g_bDrawWires = false;    // Draw the mesh with wireframe overlay
#define MAXFRAMES 300
#define MAX_ITERACIONES 50000//100000 /**5000**20000*/

float frames[MAX_ITERACIONES];
int iteracion = 0;
int inicFrames = 0;

InfoNurbs* info;
enum E_PARTITION_MODE
{
    PARTITION_INTEGER,
    PARTITION_FRACTIONAL_EVEN,
    PARTITION_FRACTIONAL_ODD
};

E_PARTITION_MODE                    g_iPartitionMode = PARTITION_INTEGER;

int nIteracion = 0;
using namespace std;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN      1
#define IDC_TOGGLEREF             3
#define IDC_CHANGEDEVICE          4

#define IDC_PATCH_SUBDIVS         5
#define IDC_PATCH_SUBDIVS_STATIC  6
#define IDC_TOGGLE_LINES          7
#define IDC_PARTITION_MODE        8
#define IDC_PARTITION_INTEGER     9
#define IDC_PARTITION_FRAC_EVEN   10
#define IDC_PARTITION_FRAC_ODD    11

//void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

long numPrimitives[MAX_ITERACIONES];
long maxNumPrimitives = 0;
long minNumPrimitives = 0;
long numPrimitivesDS[MAX_ITERACIONES];
long maxNumPrimitivesDS = 0;
long minNumPrimitivesDS = 0;
int numSpfExtra = 0;
float* createSamplerPtos();

D3D12MeshletRender::D3D12MeshletRender(UINT width, UINT height, std::wstring name)
    : DXSample(width, height, name)
    , m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
    , m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
    , m_rtvDescriptorSize(0)
    , m_dsvDescriptorSize(0)
    , m_constantBufferData{}
    , m_cbvDataBegin(nullptr)
    , m_frameIndex(0)
    , m_frameCounter(0)
    , m_fenceEvent{}
    , m_fenceValues{}
{ }

void D3D12MeshletRender::OnInit()
{
    //m_camera.Init({ 0, 75, 150 });
    m_camera.Init({ 0.0f, 0.0f, 10.0f });
    //m_camera.SetMoveSpeed(150.0f);
    m_camera.SetMoveSpeed(37.0f);

    LoadPipeline();
    LoadAssets();
}

// Load the rendering pipeline dependencies.
void D3D12MeshletRender::LoadPipeline()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
            ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter, true);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
            ));
    }

    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
    if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
        || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5))
    {
        OutputDebugStringA("ERROR: Shader Model 6.5 is not supported\n");
        throw std::exception("Shader Model 6.5 is not supported");
    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
    if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features)))
        || (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED))
    {
        OutputDebugStringA("ERROR: Mesh Shaders aren't supported!\n");
        throw std::exception("Mesh Shaders aren't supported!");
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        Win32Application::GetHwnd(),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
        ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

        m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV and a command allocator for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);

            ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n])));
        }
    }

    // Create the depth stencil view.
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        const CD3DX12_HEAP_PROPERTIES depthStencilHeapProps(D3D12_HEAP_TYPE_DEFAULT);
        const CD3DX12_RESOURCE_DESC depthStencilTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_width, m_height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        ThrowIfFailed(m_device->CreateCommittedResource(
            &depthStencilHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilTextureDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&m_depthStencil)
        ));

        NAME_D3D12_OBJECT(m_depthStencil);

        m_device->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    }

    // Create the constant buffer.
    {
        const UINT64 constantBufferSize = sizeof(SceneConstantBuffer) * FrameCount;

        const CD3DX12_HEAP_PROPERTIES constantBufferHeapProps(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

        ThrowIfFailed(m_device->CreateCommittedResource(
            &constantBufferHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &constantBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_constantBuffer)));

        // Describe and create a constant buffer view.
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = constantBufferSize;

        // Map and initialize the constant buffer. We don't unmap this until the
        // app closes. Keeping things mapped for the lifetime of the resource is okay.
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvDataBegin)));
    }
}

// Load the sample assets.
void D3D12MeshletRender::LoadAssets()
{
    // Create the pipeline state, which includes compiling and loading shaders.
    {
        struct 
        { 
            byte* data; 
            uint32_t size; 
        } meshShader, pixelShader;

        //ID3DBlob* pBlobPS = NULL;
        //ID3DBlob* pBlobPSSolid = NULL;

        //ID3D10Blob* pErrorBlob = NULL;
        //D3DCompileFromFile(L"Nurbs4.hlsl", NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, NULL, &pBlobPS, &pErrorBlob);
        //D3DCompileFromFile(L"Nurbs4.hlsl", NULL, NULL, "SolidColorPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, NULL, &pBlobPSSolid, &pErrorBlob);

        ReadDataFromFile(GetAssetFullPath(c_meshShaderFilename).c_str(), &meshShader.data, &meshShader.size);
        ReadDataFromFile(GetAssetFullPath(c_pixelShaderFilename).c_str(), &pixelShader.data, &pixelShader.size);

        // Pull root signature from the precompiled mesh shader.
        ThrowIfFailed(m_device->CreateRootSignature(0, meshShader.data, meshShader.size, IID_PPV_ARGS(&m_rootSignature)));

        D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature        = m_rootSignature.Get();
        psoDesc.MS                    = { meshShader.data, meshShader.size };
        psoDesc.PS                    = { pixelShader.data, pixelShader.size };
        psoDesc.NumRenderTargets      = 1;
        psoDesc.RTVFormats[0]         = m_renderTargets[0]->GetDesc().Format;
        psoDesc.DSVFormat             = m_depthStencil->GetDesc().Format;
        CD3DX12_RASTERIZER_DESC l_cWireframeRasterizer(D3D12_DEFAULT);
        l_cWireframeRasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
        //l_cWireframeRasterizer.FrontCounterClockwise = TRUE;
        psoDesc.RasterizerState       = CD3DX12_RASTERIZER_DESC(l_cWireframeRasterizer);    // CW front; cull back
        //psoDesc.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_BACK, false, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, true, false, false, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);    // CW front; cull back
        psoDesc.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
        psoDesc.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
        psoDesc.SampleMask            = UINT_MAX;
        psoDesc.SampleDesc            = DefaultSampleDesc();

        auto psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(psoDesc);

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
        streamDesc.pPipelineStateSubobjectStream = &psoStream;
        streamDesc.SizeInBytes                   = sizeof(psoStream);

        ThrowIfFailed(m_device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&m_pipelineState)));
    }

    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());

    gEscena = new EscenaNurbs();

    std::ofstream file;
    file.open("FPS.txt", ios::app);
    numFich = 17;
    file << Configuracion::getConfiguracion().getNombreFich(numFich);
    //setVertex(pd3dDevice);
    //En setVertex es donde se llama a leerNurbs, pasar esa funcion a directx 12
    gEscena->leerNurbs(numFich);
    gEscena->obtenerVertices();
    info = new InfoNurbs(gEscena->getNurbs(), gEscena->getNumSpf());
    /*int aux = 0;
    int* nSpfAux = info->getNumSpfC();
    for (uint32_t j = 0; j < gEscena->getNumNurbs(); ++j)
    {
        aux = aux + nSpfAux[j];
    }
    numSpfExtra = aux;*/
    m_model.LoadFromFileN(c_meshFilename, gEscena);
    m_model.UploadGpuResourcesN(m_device.Get(), m_commandQueue.Get(), m_commandAllocators[m_frameIndex].Get(), m_commandList.Get(), gEscena);

#ifdef _DEBUG
    // Mesh shader file expects a certain vertex layout; assert our mesh conforms to that layout.
    const D3D12_INPUT_ELEMENT_DESC c_elementDescs[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
    };

    for (auto& mesh : m_model)
    {
        assert(mesh.LayoutDesc.NumElements == 2);

        for (uint32_t i = 0; i < _countof(c_elementDescs); ++i)
            assert(std::memcmp(&mesh.LayoutElems[i], &c_elementDescs[i], sizeof(D3D12_INPUT_ELEMENT_DESC)) == 0);
    }
#endif
    
    //SAFE_RELEASE(pBlobPS);
    //SAFE_RELEASE(pBlobPSSolid);

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValues[m_frameIndex]++;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForGpu();
    }
}

// Update frame-based values.
void D3D12MeshletRender::OnUpdate()
{
    m_timer.Tick(NULL);

    if (m_frameCounter++ % 30 == 0)
    {
        // Update window text with FPS value.
        wchar_t fps[64];
        swprintf_s(fps, L"%ufps", m_timer.GetFramesPerSecond());
        SetCustomWindowText(fps);
    }

    m_camera.Update(static_cast<float>(m_timer.GetElapsedSeconds()));

    XMMATRIX world = XMMATRIX(g_XMIdentityR0, g_XMIdentityR1, g_XMIdentityR2, g_XMIdentityR3);
    XMMATRIX view = m_camera.GetViewMatrix();
    XMMATRIX proj = m_camera.GetProjectionMatrix(XM_PI / 3.0f, m_aspectRatio);
    
    XMStoreFloat4x4(&m_constantBufferData.World, XMMatrixTranspose(world));
    XMStoreFloat4x4(&m_constantBufferData.WorldView, XMMatrixTranspose(world * view));
    XMStoreFloat4x4(&m_constantBufferData.WorldViewProj, XMMatrixTranspose(world * view * proj));
    m_constantBufferData.DrawMeshlets = true;

    memcpy(m_cbvDataBegin + sizeof(SceneConstantBuffer) * m_frameIndex, &m_constantBufferData, sizeof(m_constantBufferData));
}

// Render the scene.
void D3D12MeshletRender::OnRender()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present(1, 0));

    MoveToNextFrame();
}

void D3D12MeshletRender::OnDestroy()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForGpu();

    CloseHandle(m_fenceEvent);
}

void D3D12MeshletRender::OnKeyDown(UINT8 key)
{
    m_camera.OnKeyDown(key);
}

void D3D12MeshletRender::OnKeyUp(UINT8 key)
{
    m_camera.OnKeyUp(key);
}

void D3D12MeshletRender::PopulateCommandList()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocators[m_frameIndex]->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get()));

    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // Indicate that the back buffer will be used as a render target.
    const auto toRenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &toRenderTargetBarrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    m_commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer->GetGPUVirtualAddress() + sizeof(SceneConstantBuffer) * m_frameIndex);

    for (auto& mesh : m_model)
    {
        m_commandList->SetGraphicsRoot32BitConstant(1, mesh.IndexSize, 0);
        m_commandList->SetGraphicsRootShaderResourceView(2, mesh.VertexResources[0]->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(3, mesh.MeshletResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(4, mesh.UniqueVertexIndexResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(5, mesh.PrimitiveIndexResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(6, mesh.KnotUResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(7, mesh.KnotVResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(8, mesh.PtosResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(9, mesh.WeightResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(10, mesh.NumPtosResource->GetGPUVirtualAddress());
        //m_commandList->SetGraphicsRootShaderResourceView(11, mesh.NumSpfResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(11, mesh.TablaKnotsResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(12, mesh.TablaPtosResource->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootShaderResourceView(13, mesh.infoNurbsVertexResource->GetGPUVirtualAddress());

        for (auto& subset : mesh.MeshletSubsets)
        {
            /*m_commandList->SetGraphicsRoot32BitConstant(1, subset.Offset, 1);
            m_commandList->DispatchMesh(subset.Count, 1, 1);*/
            m_commandList->SetGraphicsRoot32BitConstant(1, 0, 1);
            m_commandList->DispatchMesh(gEscena->getNumSpf(), 1, 1);
        }
    }

    // Indicate that the back buffer will now be used to present.
    const auto toPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &toPresentBarrier);

    ThrowIfFailed(m_commandList->Close());
}

// Wait for pending GPU work to complete.
void D3D12MeshletRender::WaitForGpu()
{
    // Schedule a Signal command in the queue.
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

    // Wait until the fence has been processed.
    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

    // Increment the fence value for the current frame.
    m_fenceValues[m_frameIndex]++;
}

// Prepare to render the next frame.
void D3D12MeshletRender::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

    // Update the frame index.
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
        WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

/*
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
    switch (nControlID)
    {
        // Standard DXUT controls
    case IDC_TOGGLEFULLSCREEN:
        DXUTToggleFullScreen(); break;
    case IDC_TOGGLEREF:
        DXUTToggleREF(); break;
    case IDC_CHANGEDEVICE:
        g_D3DSettingsDlg.SetActive(!g_D3DSettingsDlg.IsActive()); break;

        // Custom app controls
    case IDC_PATCH_SUBDIVS:
    {
        g_fSubdivs = g_SampleUI.GetSlider(IDC_PATCH_SUBDIVS)->GetValue() / 10.0f;

        WCHAR sz[100];
        swprintf_s(sz, L"Patch Divisions: %2.1f", g_fSubdivs);
        g_SampleUI.GetStatic(IDC_PATCH_SUBDIVS_STATIC)->SetText(sz);
    }
    break;
    case IDC_TOGGLE_LINES:
        g_bDrawWires = g_SampleUI.GetCheckBox(IDC_TOGGLE_LINES)->GetChecked();
        break;
    case IDC_PARTITION_INTEGER:
        g_iPartitionMode = PARTITION_INTEGER;
        break;
    case IDC_PARTITION_FRAC_EVEN:
        g_iPartitionMode = PARTITION_FRACTIONAL_EVEN;
        break;
    case IDC_PARTITION_FRAC_ODD:
        g_iPartitionMode = PARTITION_FRACTIONAL_ODD;
        break;
    }
}*/

float* createSamplerPtos() {
    int numPtos = info->getNumPtos() / 3;
    float* X = gEscena->getXs();
    float* Y = gEscena->getYs();
    float* Z = gEscena->getZs();
    float* pesos = info->getPesos();
    int cont = 0;
    float* ptos = (float*)malloc(sizeof(float) * numPtos * 4);//malloc(sizeof(float)*300000);
    for (int i = 0; i < numPtos; i++) {
        ptos[cont] = X[i];
        cont++;
        ptos[cont] = Y[i];
        cont++;
        ptos[cont] = Z[i];
        cont++;
        ptos[cont] = pesos[i];
    }
    return ptos;
    //const GUID id={ 0};
    //g_ptbPtos->SetPrivateData(id,sizeof(float)*18000*4, ptos);

}
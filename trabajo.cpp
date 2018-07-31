// trabajo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "BasicShaders.ps.h"
#include "BasicShaders.vs.h"

using namespace DirectX;

inline HRESULT AtlCheck(HRESULT hr) {
  if (SUCCEEDED(hr)) return hr;
  AtlThrow(hr);
}

struct ShaderTransforms {
  XMMATRIX World;
  XMMATRIX View;
  XMMATRIX Projection;
};

struct InfoVertice {
  XMFLOAT3 Pos;
  XMFLOAT4 Color;
};

class RenderData {
public:
  CComPtr<ID3D11Buffer> CBuffer;
  CComPtr<ID3D11Buffer> IndexBuffer;
  CComPtr<ID3D11Buffer> VertexBuffer;
  CComPtr<ID3D11InputLayout> InputLayout;
  CComPtr<ID3D11VertexShader> BasicVS;
  CComPtr<ID3D11PixelShader> BasicPS;
  std::vector<InfoVertice> figura;
  std::vector<WORD> indices;
  ShaderTransforms transforms;
  float RotationY = 0.0f;

  HRESULT LoadSceneAssets();
  HRESULT CopySceneAssetsToGPU(_In_ ID3D11Device* pd3dDevice);

  void reset() {
    RotationY = 0.0f;
    CBuffer = nullptr;
	IndexBuffer = nullptr;
    VertexBuffer = nullptr;
    InputLayout = nullptr;
    BasicVS = nullptr;
    BasicPS = nullptr;
    figura.clear();
	indices.clear();
  }
};

static RenderData g_RenderData;

HRESULT RenderData::LoadSceneAssets() {
  InfoVertice arr[] = {
	  { XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT4(1.f, 0.0f, 0.0f, 1.0f) },
	  { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	  { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
  };
  figura.resize(_countof(arr));
  memcpy(figura.data(), arr, sizeof(arr));

  WORD indices_figura[] = {
	  2,0,1
  };
  indices.resize(_countof(indices_figura));
  memcpy(indices.data(), indices_figura, sizeof(indices_figura));
  return S_OK;
}

HRESULT RenderData::CopySceneAssetsToGPU(_In_ ID3D11Device* pd3dDevice) {
  HRESULT hr = S_OK;

  CD3D11_BUFFER_DESC vbDesc(figura.size() * sizeof(figura[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
  D3D11_SUBRESOURCE_DATA vbData = { figura.data(), 0, 0 };
  V_RETURN(pd3dDevice->CreateBuffer(&vbDesc, &vbData, &VertexBuffer));

  CD3D11_BUFFER_DESC cbDesc(sizeof(transforms), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT);
  V_RETURN(pd3dDevice->CreateBuffer(&cbDesc, nullptr, &CBuffer));

  CD3D11_BUFFER_DESC ibDesc(indices.size() * sizeof(indices[0]), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT);
  D3D11_SUBRESOURCE_DATA ibData = { indices.data(), 0, 0 };
  V_RETURN(pd3dDevice->CreateBuffer(&ibDesc, &ibData, &IndexBuffer));

  // LPCSTR SemanticName; UINT SemanticIndex; DXGI_FORMAT Format; UINT InputSlot;
  // UINT AlignedByteOffset; D3D11_INPUT_CLASSIFICATION InputSlotClass; UINT InstanceDataStepRate;
  D3D11_INPUT_ELEMENT_DESC inputElementDescs[2] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };

  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows 
  // the shaders to be optimized and to run exactly the way they will run in 
  // the release configuration of this program.
  dwShaderFlags |= D3DCOMPILE_DEBUG;

  // Disable optimizations to further improve shader debugging
  dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  // Compile the pixel shader
  ID3DBlob* pPixelShaderBlob = nullptr;
  V_RETURN( DXUTCompileFromFile(L"BasicShaders.hlsl", nullptr, "ps_main", "ps_4_0", dwShaderFlags, 0, &pPixelShaderBlob) );

  // Create the pixel shader
  hr = pd3dDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &BasicPS);
  if (FAILED(hr))
  {
	  SAFE_RELEASE(pPixelShaderBlob);
	  return hr;
  }

  // Compile the vertex shader
  ID3DBlob* pVertexShaderBlob = nullptr;
  V_RETURN(DXUTCompileFromFile(L"BasicShaders.hlsl", nullptr, "vs_main", "vs_4_0", dwShaderFlags, 0, &pVertexShaderBlob));

  // Create the vertex shader
  hr = pd3dDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &BasicVS);
  if (FAILED(hr))
  {
	  SAFE_RELEASE(pVertexShaderBlob);
	  return hr;
  }

  V_RETURN(pd3dDevice->CreateInputLayout(inputElementDescs, _countof(inputElementDescs), g_vs_main, sizeof(g_vs_main), &InputLayout));

  return hr;
}

HRESULT CALLBACK HandleDeviceCreated(_In_ ID3D11Device* pd3dDevice, _In_ const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, _In_opt_ void* pUserContext) {
  RenderData *pRender = &g_RenderData;

  HRESULT hr = S_OK;
  pRender->reset();
  V_RETURN(pRender->LoadSceneAssets());
  V_RETURN(pRender->CopySceneAssetsToGPU(pd3dDevice));
  return hr;
}

void CALLBACK HandleFrameRender(_In_ ID3D11Device* pd3dDevice, _In_ ID3D11DeviceContext* pd3dImmediateContext, _In_ double fTime, _In_ float fElapsedTime, _In_opt_ void* pUserContext) {
  RenderData *pRender = &g_RenderData;
  ID3D11RenderTargetView *rtv = DXUTGetD3D11RenderTargetView();

  pd3dImmediateContext->ClearRenderTargetView(rtv, DirectX::Colors::Coral);

  RECT r = DXUTGetWindowClientRect();
  pRender->RotationY = (float)DXUTGetTime();
  /*pRender->transforms.World = XMMatrixRotationY(pRender->RotationY);*/
  pRender->transforms.World = XMMatrixIdentity();
  XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -3.f, 0.0f) + sin(pRender->RotationY) * XMVectorSet(0.f, 0.f, 2.5f, 0.f);
  XMVECTOR To = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
  XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  pRender->transforms.View = XMMatrixLookToLH(Eye, To, Up);
  pRender->transforms.Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, r.right / (FLOAT)r.bottom, 0.01f, 100.0f);

  // Las matrices en constant buffers vienen column-major, por convencion.
  pRender->transforms.World = XMMatrixTranspose(pRender->transforms.World);
  pRender->transforms.View = XMMatrixTranspose(pRender->transforms.View);
  pRender->transforms.Projection = XMMatrixTranspose(pRender->transforms.Projection);
  pd3dImmediateContext->UpdateSubresource(pRender->CBuffer, 0, nullptr, &pRender->transforms, 0, 0);

  D3D11_VIEWPORT viewports[1] = { 0, 0, (FLOAT)r.right, (FLOAT)r.bottom, 0.0f, 1.0f };
  ID3D11RenderTargetView *rtvViews[1] = { rtv };
  ID3D11Buffer *vertexBuffers[1] = { pRender->VertexBuffer };
  UINT strides[1] = { sizeof(InfoVertice) };
  UINT offsets[1] = { 0 };
  pd3dImmediateContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
  pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  pd3dImmediateContext->IASetIndexBuffer(pRender->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
  pd3dImmediateContext->IASetInputLayout(pRender->InputLayout);
  pd3dImmediateContext->VSSetShader(pRender->BasicVS, nullptr, 0);
  pd3dImmediateContext->VSSetConstantBuffers(0, 1, &pRender->CBuffer.p);
  pd3dImmediateContext->RSSetViewports(1, viewports);
  pd3dImmediateContext->PSSetShader(pRender->BasicPS, nullptr, 0);
  pd3dImmediateContext->OMSetRenderTargets(1, rtvViews, nullptr);
  pd3dImmediateContext->DrawIndexed(pRender->indices.size(), 0, 0);
}

int main()
{
  try {
    AtlCheck(DXUTInit());
	std::cout << "D3D has been initiated correctly" << std::endl;
    AtlCheck(DXUTCreateWindow(L"Trabajo"));
    DXUTDeviceSettings deviceSettings;
    DXUTApplyDefaultDeviceSettings(&deviceSettings);
#if _DEBUG
    deviceSettings.d3d11.CreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
    deviceSettings.d3d11.DriverType = D3D_DRIVER_TYPE_WARP;
#endif
    DXUTSetCallbackD3D11DeviceCreated(HandleDeviceCreated);
    DXUTSetCallbackD3D11FrameRender(HandleFrameRender);
    AtlCheck(DXUTCreateDeviceFromSettings(&deviceSettings));
    AtlCheck(DXUTMainLoop());
    g_RenderData.reset();
  }
  catch (const CAtlException &e) {
    wprintf(L"failed with hr=0x%08x", (HRESULT)e);
  }
  return 0;
}

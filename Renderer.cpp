#include "stdafx.h"
#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer() {
}


Renderer::~Renderer() {
	// TODO: Reset and release all resources
}

inline HRESULT Renderer::AtlCheck(HRESULT hr) {
	if (SUCCEEDED(hr)) return hr;
	AtlThrow(hr);
}

HRESULT Renderer::Render() {
	try {
		AtlCheck(DXUTInit());
		AtlCheck(DXUTCreateWindow(L"Trabajo"));
		DXUTApplyDefaultDeviceSettings(&deviceSettings);
#ifdef _DEBUG
		deviceSettings.d3d11.CreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
		deviceSettings.d3d11.DriverType = D3D_DRIVER_TYPE_WARP;
#endif
		// Set DXUT Callbacks
		DXUTSetCallbackD3D11DeviceCreated(HandleDeviceCreated);
		DXUTSetCallbackD3D11FrameRender(HandleFrameRender);
		DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);
		DXUTSetCallbackKeyboard(OnKeyboard);
		DXUTSetCallbackMsgProc(MsgProc);

		// Create device
		AtlCheck(DXUTCreateDeviceFromSettings(&deviceSettings));

		// Start main loop
		AtlCheck(DXUTMainLoop());

	}
	catch (const CAtlException &e) {
		wprintf(L"failed with hr=0x%08x", (HRESULT)e);
	}

	return S_OK;
}

HRESULT CALLBACK Renderer::HandleDeviceCreated(_In_ ID3D11Device* pd3dDevice, 
	_In_ const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, _In_opt_ void* pUserContext) {

	HRESULT hr = S_OK;
	V_RETURN(LoadSceneAssets());
	V_RETURN(CopySceneAssetsToGPU(pd3dDevice));

	return hr;
}

HRESULT Renderer::LoadSceneAssets() {

	this->camera = Camera(
		XMVectorSet(0.0f, 0.0f, -5.f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// Map sample diffuse light
	/*this->transforms.diffuseLightPosition = XMFLOAT3(0.f,2.f,-4.f);
	this->transforms.diffuseLightColor = XMFLOAT4(0.2f,0.2f,0.2f,1.f);*/

	return S_OK;
}

HRESULT Renderer::CopySceneAssetsToGPU(_In_ ID3D11Device* pd3dDevice) {
	// Return code, necessary for V_RETURN macro
	HRESULT hr = S_OK;

	// Create buffer to hold vertex shader transorms (view, world and projection matrixes)
	CD3D11_BUFFER_DESC cbDesc(sizeof(Renderer::VertexShaderTransforms), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT);
	V_RETURN(pd3dDevice->CreateBuffer(&cbDesc, nullptr, &this->vsTransformsBuffer));

	// Define the input layout
	
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
	V_RETURN(DXUTCompileFromFile(L"BasicShaders.hlsl", nullptr, "PS", "ps_4_0", dwShaderFlags, 0, &pPixelShaderBlob));
	// Compile the vertex shader
	ID3DBlob* pVertexShaderBlob = nullptr;
	V_RETURN(DXUTCompileFromFile(L"BasicShaders.hlsl", nullptr, "VS", "vs_4_0", dwShaderFlags, 0, &pVertexShaderBlob));

	// Create IA Layout object
	V_RETURN(pd3dDevice->CreateInputLayout(Renderer::MESH_IA_LAYOUT, Renderer::MESH_IA_LAYOUT_SIZE, 
		pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &this->pInputLayout));

	// Create shader objects
	V_RETURN(pd3dDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &this->pVertexShader));
	V_RETURN(pd3dDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &this->pPixelShader));

	// Change this, have one mesh only
	V_RETURN(this->sampleMesh.Create(pd3dDevice, L"models\\abstract.sdkmesh"));

	return hr;
}

void CALLBACK Renderer::HandleFrameRender(_In_ ID3D11Device* pd3dDevice, _In_ ID3D11DeviceContext* pd3dImmediateContext, _In_ double fTime, _In_ float fElapsedTime, _In_opt_ void* pUserContext) {
	HRESULT hr = S_OK;

	ID3D11RenderTargetView *rtv = DXUTGetD3D11RenderTargetView();

	pd3dImmediateContext->ClearRenderTargetView(rtv, DirectX::Colors::LightBlue);

	// Clear the depth stencil
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	RECT r = DXUTGetWindowClientRect();

	// VertexShader transforms data
	VertexShaderTransforms transforms;
	transforms.World = XMMatrixIdentity();
	transforms.View = this->camera.cameraMatrix();
	transforms.Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, r.right / (FLOAT)r.bottom, 0.01f, 100.0f);

	// Matrix should be column-major by D3D convention
	transforms.World = XMMatrixTranspose(transforms.World);
	transforms.View = XMMatrixTranspose(transforms.View);
	transforms.Projection = XMMatrixTranspose(transforms.Projection);

	// Update vShader constant buffer
	pd3dImmediateContext->UpdateSubresource(this->vsTransformsBuffer, 0, nullptr, &transforms, 0, 0);

	UINT Strides[1];
	UINT Offsets[1];
	ID3D11Buffer* pVB[1];
	pVB[0] = this->sampleMesh.GetVB11(0, 0);
	Strides[0] = (UINT)this->sampleMesh.GetVertexStride(0, 0);
	Offsets[0] = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
	pd3dImmediateContext->IASetIndexBuffer(this->sampleMesh.GetIB11(0), this->sampleMesh.GetIBFormat11(0), 0);

	D3D11_VIEWPORT viewports[1] = { 0, 0, (FLOAT)r.right, (FLOAT)r.bottom, 0.0f, 1.0f };
	ID3D11RenderTargetView *rtvViews[1] = { rtv };

	pd3dImmediateContext->IASetInputLayout(this->pInputLayout);
	pd3dImmediateContext->VSSetShader(this->pVertexShader, nullptr, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &this->vsTransformsBuffer.p);
	pd3dImmediateContext->RSSetViewports(1, viewports);
	pd3dImmediateContext->PSSetShader(this->pPixelShader, nullptr, 0);
	pd3dImmediateContext->OMSetRenderTargets(1, rtvViews, pDSV);

	for (UINT subset = 0; subset < this->sampleMesh.GetNumSubsets(0); ++subset) {
		auto pSubset = this->sampleMesh.GetSubset(0, subset);
		auto PrimType = this->sampleMesh.GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);

		pd3dImmediateContext->IASetPrimitiveTopology(PrimType);

		// Ignores most of the material information in them mesh to use
		// only a simple shader
		auto pDiffuseRV = this->sampleMesh.GetMaterial(pSubset->MaterialID)->pDiffuseRV11;

		pd3dImmediateContext->PSSetShaderResources(0, 1, &pDiffuseRV);
		pd3dImmediateContext->DrawIndexed((UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart);
	}
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK Renderer::OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext) {
	float angleDisplacement = .02f;
	if (bKeyDown) {
		switch (nChar) {
		case VK_LEFT:
			// Rotate camera in y-axis clockwise
			g_RenderData.CameraToDirection = RotateYAndNormalize(g_RenderData.CameraToDirection, -angleDisplacement);
			g_RenderData.LateralDirection = RotateYAndNormalize(g_RenderData.LateralDirection, -angleDisplacement);
			break;
		case VK_RIGHT:
			// Rotate camera in y-axis counter-clockwise
			g_RenderData.CameraToDirection = RotateYAndNormalize(g_RenderData.CameraToDirection, angleDisplacement);
			g_RenderData.LateralDirection = RotateYAndNormalize(g_RenderData.LateralDirection, angleDisplacement);
			break;
		default:
			break;
		}
	}
}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK Renderer::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext) {
	float keyDeltaMove = .1f;
	if (uMsg == WM_CHAR) {
		switch (wParam) {
		case 'w':
			g_RenderData.Eye += g_RenderData.CameraToDirection * keyDeltaMove;
			break;
		case 's':
			g_RenderData.Eye += g_RenderData.CameraToDirection * -keyDeltaMove;
			break;
		case 'a':
			g_RenderData.Eye += g_RenderData.LateralDirection * -keyDeltaMove;
			break;
		case 'd':
			g_RenderData.Eye += g_RenderData.LateralDirection * keyDeltaMove;
			break;
		default:
			break;
		}
	}

	return 0;
}

// Handler called on d3d device destruction. Should release resources

void CALLBACK Renderer::OnD3D11DestroyDevice(void* pUserContext) {
	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_RenderData.sampleMesh.Destroy();
}


// Math helper function

XMVECTOR RotateYAndNormalize(XMVECTOR aVector, float anAngle) {
	return XMVector3Normalize(XMVector3Transform(aVector, XMMatrixRotationY(anAngle)));
}
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

HRESULT CALLBACK Renderer::HandleDeviceCreated(_In_ ID3D11Device* pd3dDevice, _In_ const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, _In_opt_ void* pUserContext) {

	HRESULT hr = S_OK;
	V_RETURN(LoadSceneAssets());
	V_RETURN(CopySceneAssetsToGPU(pd3dDevice));

	return hr;
}

HRESULT Renderer::LoadSceneAssets() {
	this->camera = Camera(
		XMVectorSet(0.0f, 0.0f, -5.f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)
		);
	// Lateral direction. Used for camera X Axis displacement, and keep track of the rotated X axis
	this->LateralDirection = XMVector3Normalize(XMVector3Transform(this->CameraToDirection, XMMatrixRotationY(XM_PI * .5)));

	// Map sample diffuse light
	this->transforms.diffuseLightPosition = XMFLOAT3(0.f, 2.f, -4.f);
	this->transforms.diffuseLightColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);

	return S_OK;
}
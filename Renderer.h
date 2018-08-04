#pragma once
class Renderer {
public:
	Renderer();
	~Renderer();

	

	HRESULT Initialize();
	HRESULT Render();


private:
	HRESULT AtlCheck(HRESULT hr);
	DXUTDeviceSettings deviceSettings;
	HRESULT HandleDeviceCreated(ID3D11Device * pd3dDevice, const DXGI_SURFACE_DESC * pBackBufferSurfaceDesc, void * pUserContext);
	HRESULT LoadSceneAssets();
};

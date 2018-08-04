#pragma once

#include "stdafx.h"
#include "Camera.h"


class Renderer {
public:
	Renderer();
	~Renderer();

	

	HRESULT Initialize();
	HRESULT Render();


private:
	HRESULT AtlCheck(HRESULT hr);
	DXUTDeviceSettings deviceSettings;
	HRESULT HandleDeviceCreated(ID3D11Device * pd3dDevice, const DXGI_SURFACE_DESC * pBackBufferSurfaceDesc, 
		void * pUserContext);
	HRESULT LoadSceneAssets();

	Camera camera;


	struct ShaderTransforms {
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

};

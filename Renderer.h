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

	// DXUT Callbacks definition
	HRESULT HandleDeviceCreated(ID3D11Device * pd3dDevice, const DXGI_SURFACE_DESC * pBackBufferSurfaceDesc, 
		void * pUserContext);
	void HandleFrameRender(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pd3dImmediateContext,
		double fTime, float fElapsedTime, void * pUserContext);
	void OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void * pUserContext);
	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
		bool * pbNoFurtherProcessing, void * pUserContext);
	void OnD3D11DestroyDevice(void * pUserContext);

	// D3D needed collaborators
	CComPtr<ID3D11InputLayout> pInputLayout;
	CComPtr<ID3D11VertexShader> pVertexShader;
	CComPtr<ID3D11PixelShader> pPixelShader;

	// TODO: Change this, temporalily only adding one mesh
	CDXUTSDKMesh sampleMesh;

	// Create and load assets helper
	HRESULT LoadSceneAssets();
	HRESULT CopySceneAssetsToGPU(ID3D11Device * pd3dDevice);

	// Scence collaborators
	Camera camera;
	CComPtr<ID3D11Buffer> vsTransformsBuffer;

	// Buffer objects containers
	struct ShaderTransforms {
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

	// IA layout descriptors
	static const D3D11_INPUT_ELEMENT_DESC MESH_IA_LAYOUT[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT MESH_IA_LAYOUT_SIZE= ARRAYSIZE(Renderer::MESH_IA_LAYOUT);

};

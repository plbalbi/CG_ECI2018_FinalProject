#pragma once

#include "stdafx.h"
#include "Camera.h"

#ifndef RENDERER_H
#define RENDERER_H

class Renderer {
public:
	Renderer() {};
	~Renderer();

	HRESULT Initialize();
	HRESULT Render();

	static Renderer* self;

private:
	HRESULT AtlCheck(HRESULT hr);
	DXUTDeviceSettings deviceSettings;


	// DXUT Callbacks definition
	static HRESULT HandleDeviceCreated(ID3D11Device * pd3dDevice, const DXGI_SURFACE_DESC * pBackBufferSurfaceDesc, 
		void * pUserContext);
	static void HandleFrameRender(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pd3dImmediateContext,
		double fTime, float fElapsedTime, void * pUserContext);
	static void OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void * pUserContext);
	static LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
		bool * pbNoFurtherProcessing, void * pUserContext);
	static void OnD3D11DestroyDevice(void * pUserContext);

	// D3D needed collaborators
	CComPtr<ID3D11InputLayout> pInputLayout;
	CComPtr<ID3D11VertexShader> pVertexShader;
	CComPtr<ID3D11PixelShader> pPixelShader;

	// TODO: Change this, temporalily only adding one mesh
	CDXUTSDKMesh sampleMesh;
	std::vector< CDXUTSDKMesh > meshes;
	static void loadMeshIntoVBuffer(ID3D11DeviceContext * pd3dImmediateContext, CDXUTSDKMesh * pAMeshToDraw);

	// Create and load assets helper
	HRESULT LoadSceneAssets();
	HRESULT CopySceneAssetsToGPU(ID3D11Device * pd3dDevice);

	// Scence collaborators
	Camera camera;
	CComPtr<ID3D11Buffer> vsTransformsBuffer;
	// SDL stands for simple diffuse light
	CComPtr<ID3D11Buffer> psSDLBuffer;

	// Buffer objects containers
	struct VertexShaderTransforms {
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

	struct SimpleDiffuseLight {
		XMFLOAT3 position;
		XMFLOAT4 color;
		float __padding;
	};

	// IA layout descriptors
	static const D3D11_INPUT_ELEMENT_DESC MESH_IA_LAYOUT[];
	static const UINT MESH_IA_LAYOUT_SIZE;

};

#endif
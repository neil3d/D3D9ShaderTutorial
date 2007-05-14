#pragma once

#include "inc.h"

class DrawingUtil
{
public:
	DrawingUtil(void);
	~DrawingUtil(void);

	static DrawingUtil* getInst();
	void init(IDirect3DDevice9 *pD3DDevice);
	void destroy();

	ID3DXEffect *loadEffect(IDirect3DDevice9 *pD3DDevice,const char* szFileName);

	void drawLight(D3DXVECTOR3 pos);
	void drawText(const char* szText,int x, int y, D3DCOLOR color = 0xFFFFFFFF);
private:
	ID3DXFont	*m_pD3DFont;
	ID3DXMesh	*m_pLightMesh;
	ID3DXEffect	*m_pSimpleEffect;
};

class SetRenderTarget
{
	IDirect3DSurface9	*m_pLastRT;
	IDirect3DDevice9	*m_pD3DDevice;
public:
	SetRenderTarget(IDirect3DDevice9 *pD3DDevice, IDirect3DTexture9* pTex)
	{
		m_pD3DDevice = pD3DDevice;

		HRESULT hr;
		hr = pD3DDevice->GetRenderTarget(0, &m_pLastRT);

		IDirect3DSurface9 *pSurface = NULL;
		hr = pTex->GetSurfaceLevel(0, &pSurface );
		if(SUCCEEDED(hr))
		{
			hr = pD3DDevice->SetRenderTarget(0, pSurface);
			hr = pSurface->Release();

			hr = pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
		}
	}

	~SetRenderTarget()
	{
		HRESULT hr;
		hr = m_pD3DDevice->SetRenderTarget(0, m_pLastRT);
	}

};
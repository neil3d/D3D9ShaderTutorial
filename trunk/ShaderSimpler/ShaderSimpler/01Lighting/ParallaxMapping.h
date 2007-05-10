#pragma once
#include "..\shadersimpler.h"

class ParallaxMapping :	public ShaderSimpler
{
public:
	ParallaxMapping(void);
	virtual ~ParallaxMapping(void);

	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();

	virtual void onKeyDown(DWORD vk) 
	{
		if(vk == VK_TAB)
		{
			const int MAX_MESH = 3;
			m_activeMesh++;
			m_activeMesh %= MAX_MESH;
		}
		else if(vk == 'P')
		{
			m_bUseParallax = !m_bUseParallax; 
			if(m_bUseParallax)
				m_pEffect->SetTechnique("tech_parallax");
			else
				m_pEffect->SetTechnique("tech_no_parallax");
		}
	}

private:
	void drawEarth();
	void drawWall();

private:
	ID3DXEffect			*m_pEffect;

	ID3DXMesh			*m_pMeshSphere;
	IDirect3DTexture9	*m_pEarthMap;
	IDirect3DTexture9	*m_pEarthNormalMap;
	IDirect3DTexture9	*m_pEarthHeightMap;

	ID3DXMesh			*m_pMeshWall;
	IDirect3DTexture9	*m_pWallMap;
	IDirect3DTexture9	*m_pWallNormalMap;
	IDirect3DTexture9	*m_pWallHeightMap;

	IDirect3DTexture9	*m_pBlisterMap;
	IDirect3DTexture9	*m_pBlisterNormalMap;
	IDirect3DTexture9	*m_pBlisterHeightMap;


	D3DXVECTOR3			m_lightPos;

	bool	m_bUseParallax;
};

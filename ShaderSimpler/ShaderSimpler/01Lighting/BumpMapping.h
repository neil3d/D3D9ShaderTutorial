#pragma once
#include "..\ShaderSimpler.h"

class BumpMapping : public ShaderSimpler
{
public:
	BumpMapping();
	virtual ~BumpMapping();

	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();

	virtual void onKeyDown(DWORD vk) 
	{
		if(vk == VK_TAB)
		{
			const int MAX_MESH = 2;
			m_activeMesh++;
			m_activeMesh %= MAX_MESH;
		}
	}

private:
	void drawEarth();
private:
	ID3DXEffect					*m_pEffect;

	ID3DXMesh			*m_pMeshSphere;
	IDirect3DTexture9	*m_pEarthMap;
	IDirect3DTexture9	*m_pEarthNormalMap;


	D3DXVECTOR3	m_lightPos;
};
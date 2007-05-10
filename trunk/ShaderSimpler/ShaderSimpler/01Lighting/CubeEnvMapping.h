#pragma once
#include "..\shadersimpler.h"

class CubeEnvMapping : public ShaderSimpler
{
public:
	CubeEnvMapping(void);
	virtual ~CubeEnvMapping(void);

	//--
	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();
	virtual void onKeyDown(DWORD vk);

private:
	ID3DXEffect				*m_pEffect;
	IDirect3DCubeTexture9	*m_pCubeMap;
	IDirect3DTexture9		*m_pSphereMap;
};

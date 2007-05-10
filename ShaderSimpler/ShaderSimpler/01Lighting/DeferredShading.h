#pragma once
#include "..\shadersimpler.h"

class DeferredShading :	public ShaderSimpler
{
public:
	DeferredShading(void);
	virtual ~DeferredShading(void);

	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();

private:
	IDirect3DTexture9*	m_posRT;
	IDirect3DTexture9*	m_normalRT;
	IDirect3DTexture9*	m_diffuseRT;

};

#pragma once
#include "..\shadersimpler.h"
#include "..\ScreenQuad.h"

class DS_Basic  : public ShaderSimpler
{
	enum EConst
	{
		NUM_LIGHT = 4
	};
public:
	DS_Basic(void);
	virtual ~DS_Basic(void);

	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();

private:
	void renderScene();
private:
	IDirect3DDevice9*	m_pD3DDevice;

	ID3DXEffect*		m_pEffect;

	IDirect3DTexture9*	m_posRT;
	IDirect3DTexture9*	m_normalRT;
	IDirect3DTexture9*	m_diffuseRT;

	D3DXVECTOR3			m_lightPos[NUM_LIGHT];

	ScreenQuad			m_screenQuad;
};

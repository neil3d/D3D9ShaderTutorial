#pragma once
#include "..\shadersimpler.h"

class BasicLightingSimpler : public ShaderSimpler
{
public:
	BasicLightingSimpler(void);
	virtual ~BasicLightingSimpler(void);

	//--
	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();

private:
	ID3DXEffect	*m_pEffect;
	
	D3DXVECTOR3	m_lightPos;
};

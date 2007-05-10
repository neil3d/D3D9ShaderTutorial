
#include "..\ShaderSimpler.h"

class CelShading : public ShaderSimpler
{
public:
	CelShading();
	~CelShading();

	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();

private:
	ID3DXEffect	*m_pEffect;
	D3DXVECTOR3	m_lightPos;

	IDirect3DTexture9	*m_pCartoonTex;
	IDirect3DTexture9	*m_pCartoonEdgeTex;
};

#include "..\shadersimpler.h"
#include "..\01Lighting\ParallaxMapping.h"
#include "..\ScreenQuad.h"


class PostProcessBloom  : public ShaderSimpler
{
public:
	PostProcessBloom(void);
	virtual ~PostProcessBloom(void);

	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();

	virtual void onKeyDown(DWORD vk)
	{
		if(vk == 'V')
			m_bUsePic = !m_bUsePic;
		else
			m_scene.onKeyDown(vk);
	}

private:
	IDirect3DDevice9	*m_pD3DDevice;

	ID3DXEffect			*m_pEffect;

	IDirect3DTexture9	*m_pRTFull;
	IDirect3DTexture9	*m_pRTQuarterA;
	IDirect3DTexture9	*m_pRTQuarterB;

	IDirect3DTexture9	*m_pPic;
	bool				m_bUsePic;

	ScreenQuad			m_screenQuad;

	ParallaxMapping		m_scene;
};

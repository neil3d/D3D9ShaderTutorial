#include ".\ds_basic_mrt.h"
#include "..\DrawingUtil.h"

DS_Basic_MRT::DS_Basic_MRT(void)
{
}

DS_Basic_MRT::~DS_Basic_MRT(void)
{
	SAFE_RELEASE(m_posRT_Z);
}

bool DS_Basic_MRT::createPosRT()
{
	// mrt的“位数”必须一致，所以使用两个32bit的RT来存储postion
	HRESULT hr1;
	hr1 = m_pD3DDevice->CreateTexture(clientWidth, clientHeight,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_G16R16F,
		D3DPOOL_DEFAULT,
		&m_posRT,
		NULL);

	HRESULT hr2;
	hr2 = m_pD3DDevice->CreateTexture(clientWidth, clientHeight,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_G16R16F,
		D3DPOOL_DEFAULT,
		&m_posRT_Z,
		NULL);
	
	return SUCCEEDED(hr1) && SUCCEEDED(hr2);;
}

void DS_Basic_MRT::buildGBuffers()
{
	HRESULT hr;
	//-- G-Buffsers
	SetRenderTarget SRT(m_pD3DDevice, m_posRT);

	IDirect3DSurface9 *pSurfaceZ = NULL;
	hr = m_posRT_Z->GetSurfaceLevel(0, &pSurfaceZ );

	IDirect3DSurface9 *pSurfaceNormal = NULL;
	hr = m_normalRT->GetSurfaceLevel(0, &pSurfaceNormal );
	
	IDirect3DSurface9 *pSurfaceDiffuse= NULL;
	hr = m_diffuseRT->GetSurfaceLevel(0, &pSurfaceDiffuse );
	
	hr = m_pD3DDevice->SetRenderTarget(1, pSurfaceZ);
	hr = m_pD3DDevice->SetRenderTarget(2, pSurfaceNormal);
	hr = m_pD3DDevice->SetRenderTarget(3, pSurfaceDiffuse);

	pSurfaceZ->Release();
	pSurfaceDiffuse->Release();
	pSurfaceNormal->Release();

	hr = m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);

	hr = m_pEffect->SetTechnique("init_mrt");
	renderScene();

	hr = m_pD3DDevice->SetRenderTarget(1, NULL);
	hr = m_pD3DDevice->SetRenderTarget(2, NULL);
	hr = m_pD3DDevice->SetRenderTarget(3, NULL);

	//hr = D3DXSaveTextureToFile("D:\\test.dds", D3DXIFF_DDS, m_posRT, NULL);
}

const char* DS_Basic_MRT::getShadingTech()
{
	return "shading_fullscreen_mrt";
}

void DS_Basic_MRT::setEffectTexture()
{
	HRESULT hr;

	hr = m_pEffect->SetTexture("posRT_Z", m_posRT_Z);

	DS_Basic::setEffectTexture();
}
#include ".\postprocessbloom.h"
#include "..\inc.h"
#include "..\DrawingUtil.h"

PostProcessBloom::PostProcessBloom(void)
{
	m_pD3DDevice = NULL;

	m_pEffect = NULL;

	m_pRTFull = NULL;
	m_pRTQuarterA = NULL;
	m_pRTQuarterB = NULL;

	m_pPic = NULL;

	m_bUsePic = true;
}

PostProcessBloom::~PostProcessBloom(void)
{
	SAFE_RELEASE(m_pRTFull);
	SAFE_RELEASE(m_pRTQuarterA);
	SAFE_RELEASE(m_pRTQuarterB);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pPic);
}

bool PostProcessBloom::init(IDirect3DDevice9 *pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;

	if(!m_scene.init(pD3DDevice))
		return false;

	// 创建所需的render targets
	HRESULT hr = pD3DDevice->CreateTexture(clientWidth, clientHeight,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pRTFull,
		NULL);

	if(FAILED(hr))
		return false;

	hr = pD3DDevice->CreateTexture(clientWidth/2, clientHeight/2,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pRTQuarterA,
		NULL);
	if(FAILED(hr))
		return false;

	hr = pD3DDevice->CreateTexture(clientWidth/2, clientHeight/2,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pRTQuarterB,
		NULL);
	if(FAILED(hr))
		return false;


	// create effect
	m_pEffect = DrawingUtil::getInst()->loadEffect(pD3DDevice,"Shader\\Bloom.fx");

	// pictue
	hr = D3DXCreateTextureFromFileEx(pD3DDevice, "Media\\gf.jpg",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
		1, //mip lv
		0,//usage
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		NULL,
		NULL,
		&m_pPic);


	if(FAILED(hr))
		return false;
	
	D3DSURFACE_DESC sd;
	hr = m_pPic->GetLevelDesc(0, &sd);


	return m_screenQuad.init(pD3DDevice);
}

void PostProcessBloom::render()
{
	HRESULT hr;

	IDirect3DTexture9 *pScene = NULL;
	
	if(m_bUsePic)
		pScene = m_pPic;
	else
	{
		// 把场景渲染到RT full
		SetRenderTarget SRT(m_pD3DDevice, m_pRTFull);
		m_scene.render();

		pScene = m_pRTFull;
	}


	

	UINT numPass = 0;
	hr = m_pEffect->Begin(&numPass,0);
	assert(numPass == 4);
	// down sample
	{
		hr = m_pEffect->SetTexture("texSource", pScene);
		hr = m_pEffect->BeginPass(0);
		
		SetRenderTarget SRT(m_pD3DDevice, m_pRTQuarterA);
		m_screenQuad.draw();

		hr = m_pEffect->EndPass();
	}

	// H blur
	{
		hr = m_pEffect->SetTexture("texSource", m_pRTQuarterA);
		hr = m_pEffect->BeginPass(1);

		SetRenderTarget SRT(m_pD3DDevice, m_pRTQuarterB);
		m_screenQuad.draw();

		hr = m_pEffect->EndPass();
	}

	// V blur
	{
		hr = m_pEffect->SetTexture("texSource", m_pRTQuarterB);
		hr = m_pEffect->BeginPass(2);

		SetRenderTarget SRT(m_pD3DDevice, m_pRTQuarterA);
		m_screenQuad.draw();

		hr = m_pEffect->EndPass();
	}

	
	// compose
	hr = m_pEffect->SetTexture("texSource", m_pRTQuarterA);
	hr = m_pEffect->SetTexture("texScene", pScene);
	hr = m_pEffect->BeginPass(3);
	m_screenQuad.draw();
	hr = m_pEffect->EndPass();


	hr = m_pEffect->End();

	DrawingUtil::getInst()->drawText("Press V to change view mode", 0, 48);
}
#include ".\postprocessbloom.h"
#include "..\inc.h"
#include "..\DrawingUtil.h"

class SetRenderTarget
{
	IDirect3DSurface9	*m_pLastRT;
	IDirect3DDevice9	*m_pD3DDevice;
public:
	SetRenderTarget(IDirect3DDevice9 *pD3DDevice, IDirect3DTexture9* pTex)
	{
		m_pD3DDevice = pD3DDevice;

		HRESULT hr;
		hr = pD3DDevice->GetRenderTarget(0, &m_pLastRT);

		IDirect3DSurface9 *pSurface = NULL;
		hr = pTex->GetSurfaceLevel(0, &pSurface );
		if(SUCCEEDED(hr))
		{
			hr = pD3DDevice->SetRenderTarget(0, pSurface);
			hr = pSurface->Release();

			hr = pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
		}
	}

	~SetRenderTarget()
	{
		HRESULT hr;
		hr = m_pD3DDevice->SetRenderTarget(0, m_pLastRT);
	}

};

PostProcessBloom::PostProcessBloom(void)
{
	m_pD3DDevice = NULL;

	m_pEffect = NULL;

	m_pRTFull = NULL;
	m_pRTQuarterA = NULL;
	m_pRTQuarterB = NULL;

	m_pQuadVB = NULL;
	m_pVertDecl = NULL;

	m_pPic = NULL;

	m_bUsePic = true;
}

PostProcessBloom::~PostProcessBloom(void)
{
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

	// create vert buffer
	UINT vbSize = sizeof(Vert)*4;
	hr = m_pD3DDevice->CreateVertexBuffer(vbSize,
		D3DUSAGE_WRITEONLY,
		0,//fvf
		D3DPOOL_DEFAULT,
		&m_pQuadVB,
		NULL);
	if(FAILED(hr))
		return false;


	Vert *pVert = NULL;
	hr = m_pQuadVB->Lock(0, vbSize, (void**)&pVert, 0);
	if(FAILED(hr))
		return false;

	float w = clientWidth;
	float h = clientHeight;
	D3DXVECTOR4 offsetP(-0.5f/w, -0.5f/h, 0, 0);
	pVert[0].pos = D3DXVECTOR4(-1,	1,	0, 1) + offsetP;
	pVert[1].pos = D3DXVECTOR4(1,	1,	0, 1) + offsetP;
	pVert[2].pos = D3DXVECTOR4(1,	-1,	0, 1) + offsetP;
	pVert[3].pos = D3DXVECTOR4(-1,	-1,	0, 1) + offsetP;

	pVert[0].uv = D3DXVECTOR2(0,0);
	pVert[1].uv = D3DXVECTOR2(1,0);
	pVert[2].uv = D3DXVECTOR2(1,1);
	pVert[3].uv = D3DXVECTOR2(0,1);


	hr = m_pQuadVB->Unlock();
	if(FAILED(hr))
		return false;

	const D3DVERTEXELEMENT9 elements[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	hr = pD3DDevice->CreateVertexDeclaration(elements, &m_pVertDecl);
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


	return true;
}

void PostProcessBloom::render()
{
	HRESULT hr;

	UINT stride = sizeof(Vert);

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


	hr = m_pD3DDevice->SetStreamSource(0, m_pQuadVB, 0, stride);
	hr = m_pD3DDevice->SetVertexDeclaration(m_pVertDecl);


	UINT numPass = 0;
	hr = m_pEffect->Begin(&numPass,0);
	assert(numPass == 4);
	// down sample
	{
		hr = m_pEffect->SetTexture("texSource", pScene);
		hr = m_pEffect->BeginPass(0);
		
		SetRenderTarget SRT(m_pD3DDevice, m_pRTQuarterA);
		hr = m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,0,2);

		hr = m_pEffect->EndPass();
	}

	// H blur
	{
		hr = m_pEffect->SetTexture("texSource", m_pRTQuarterA);
		hr = m_pEffect->BeginPass(1);

		SetRenderTarget SRT(m_pD3DDevice, m_pRTQuarterB);
		hr = m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,0,2);

		hr = m_pEffect->EndPass();
	}

	// V blur
	{
		hr = m_pEffect->SetTexture("texSource", m_pRTQuarterB);
		hr = m_pEffect->BeginPass(2);

		SetRenderTarget SRT(m_pD3DDevice, m_pRTQuarterA);
		hr = m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,0,2);

		hr = m_pEffect->EndPass();
	}

	
	// compose
	hr = m_pEffect->SetTexture("texSource", m_pRTQuarterA);
	hr = m_pEffect->SetTexture("texScene", pScene);
	hr = m_pEffect->BeginPass(3);
	hr = m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,0,2);
	hr = m_pEffect->EndPass();


	hr = m_pEffect->End();

	DrawingUtil::getInst()->drawText("Press V to change view mode", 0, 48);
}
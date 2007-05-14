#include ".\screenquad.h"

ScreenQuad::ScreenQuad(void)
{
	m_pD3DDevice = NULL;

	m_pQuadVB = NULL;
	m_pVertDecl = NULL;
}

ScreenQuad::~ScreenQuad(void)
{
	SAFE_RELEASE(m_pQuadVB);
	SAFE_RELEASE(m_pVertDecl);
}

bool ScreenQuad::init(IDirect3DDevice9 *pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
		 
	HRESULT hr;
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

	return true;
}

void ScreenQuad::draw()
{
	HRESULT hr;

	UINT stride = sizeof(Vert);

	hr = m_pD3DDevice->SetStreamSource(0, m_pQuadVB, 0, stride);
	hr = m_pD3DDevice->SetVertexDeclaration(m_pVertDecl);

	hr = m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,0,2);
}
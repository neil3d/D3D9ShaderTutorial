#include "CelShading.h"
#include "..\CameraCtrl.h"
#include "..\DrawingUtil.h"

CelShading::CelShading() : m_lightPos(0,0,0)
{
	m_pEffect = NULL;

	m_pCartoonTex = NULL;
	m_pCartoonEdgeTex = NULL;
}

CelShading::~CelShading()
{
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pCartoonTex);
}

bool CelShading::init(IDirect3DDevice9 *pD3DDevice)
{
	m_pEffect = DrawingUtil::getInst()->loadEffect(pD3DDevice,"Shader\\CelShading.fx");

	HRESULT hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\Cartoon.jpg",&m_pCartoonTex);
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\edge.jpg",&m_pCartoonEdgeTex);
	
	return ShaderSimpler::init(pD3DDevice);
}

void CelShading::render()
{
	if(m_pEffect == NULL)
		return;

	HRESULT hr;

	float t = timeGetTime()/4000.0f;
	float r = 150;
	m_lightPos.x = sinf(t) * r;
	m_lightPos.z = cosf(t) * r;
	m_lightPos.y = sinf(t) * r;;

	hr = m_pEffect->SetFloatArray("lightPos",(float*)&m_lightPos,3);
	hr = m_pEffect->SetFloatArray("eyePos",(float*)&g_camera.getEyePos(),3);
	hr = m_pEffect->SetTexture("texCartoon",m_pCartoonTex);
	hr = m_pEffect->SetTexture("texCartoonEdge",m_pCartoonEdgeTex);
	
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	D3DXMATRIX matWorldViewProj = matWorld * g_camera.getViewMat() * g_camera.getProjectMat();
	hr = m_pEffect->SetMatrix("matWorldViewPrj",&matWorldViewProj);
	hr = m_pEffect->SetMatrix("matWorld",&matWorld);

	//-- äÖÈ¾
	UINT numPass = 0;
	hr = m_pEffect->Begin(&numPass,0);

	for(UINT i=0;i<numPass;i++)
	{
		hr = m_pEffect->BeginPass(i);
		drawMesh();
		hr = m_pEffect->EndPass();
	}

	hr = m_pEffect->End();

	DrawingUtil::getInst()->drawLight(m_lightPos);

	ShaderSimpler::render();
}
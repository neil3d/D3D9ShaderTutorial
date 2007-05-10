#include ".\basiclightingsimpler.h"
#include "..\CameraCtrl.h"
#include "..\DrawingUtil.h"

BasicLightingSimpler::BasicLightingSimpler(void) : m_lightPos(0,0,0)
{
	m_pEffect = NULL;
}

BasicLightingSimpler::~BasicLightingSimpler(void)
{
	SAFE_RELEASE(m_pEffect);
}

bool BasicLightingSimpler::init(IDirect3DDevice9 *pD3DDevice)
{
	m_pEffect = DrawingUtil::getInst()->loadEffect(pD3DDevice,"Shader\\BasicLighting.fx");
		
	return ShaderSimpler::init(pD3DDevice);
}

void BasicLightingSimpler::render()
{
	if(m_pEffect == NULL)
		return;
	
	float t = timeGetTime()/1000.0f;
	float r = 150;
	m_lightPos.x = sinf(t) * r;
	m_lightPos.z = cosf(t) * r;
	m_lightPos.y = sinf(t) * r;;

	m_pEffect->SetFloatArray("lightPos",(float*)&m_lightPos,3);

	//-- 设置effect参数
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	D3DXMATRIX matWorldViewProj = matWorld * g_camera.getViewMat() * g_camera.getProjectMat();

	m_pEffect->SetMatrix("matWorldViewPrj",&matWorldViewProj);
	m_pEffect->SetMatrix("matWorld",&matWorld);
		
	//-- 渲染球体
	HRESULT hr;
	UINT numPass = 0;
	hr = m_pEffect->Begin(&numPass,0);
	
	for(UINT i=0;i<numPass;i++)
	{
		m_pEffect->BeginPass(i);
		drawMesh();
		m_pEffect->EndPass();
	}

	hr = m_pEffect->End();

	//--画出灯光的位置
	DrawingUtil::getInst()->drawLight(m_lightPos);

	ShaderSimpler::render();
}
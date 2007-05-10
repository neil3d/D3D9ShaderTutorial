#include "CubeEnvMapping.h"
#include "..\CameraCtrl.h"
#include "..\DrawingUtil.h"

CubeEnvMapping::CubeEnvMapping(void)
{
	m_pEffect = NULL;
	m_pCubeMap = NULL;
	m_pSphereMap = NULL;
}

CubeEnvMapping::~CubeEnvMapping(void)
{
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pCubeMap);
	SAFE_RELEASE(m_pSphereMap);
}

bool CubeEnvMapping::init(IDirect3DDevice9 *pD3DDevice)
{
	m_pEffect = DrawingUtil::getInst()->loadEffect(pD3DDevice,"Shader\\CubeEnvMapping.fx");

	m_pEffect->SetTechnique("myCubeEnvMap");
	
	HRESULT hr = D3DXCreateCubeTextureFromFile(pD3DDevice,"Media\\LobbyCube.dds",&m_pCubeMap);
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\spheremap.jpg",&m_pSphereMap);

	return ShaderSimpler::init(pD3DDevice);
}

void CubeEnvMapping::render()
{
	if(m_pEffect == NULL)
		return;
	HRESULT hr;

	//-- 设置effect参数
	hr = m_pEffect->SetTexture("cubeMapTexture",m_pCubeMap);
	hr = m_pEffect->SetTexture("sphereMapTexture",m_pSphereMap);
	hr = m_pEffect->SetFloatArray("eyePos",(float*)&g_camera.getEyePos(),3);

	D3DXMATRIX matWorld;
	float t = timeGetTime()/4000.0f;
	float r = D3DX_PI;
	float yaw = sinf(t) * r;
	float pitch = cosf(t) * r;
	D3DXMatrixRotationYawPitchRoll(&matWorld,yaw,pitch,(yaw+pitch)*0.5f);

	D3DXMATRIX matTmp;
	D3DXMatrixTranslation(&matTmp,sinf(t)*100,0,0);
	matWorld = matWorld*matTmp;

	D3DXMATRIX matWorldViewProj = matWorld * g_camera.getViewMat() * g_camera.getProjectMat();

	m_pEffect->SetMatrix("matWorldViewPrj",&matWorldViewProj);
	hr = m_pEffect->SetMatrix("matWorld",&matWorld);
	hr = m_pEffect->SetMatrix("matView",&g_camera.getViewMat());
	
	//-- 渲染球体
	UINT numPass = 0;
	hr = m_pEffect->Begin(&numPass,0);

	for(UINT i=0;i<numPass;i++)
	{
		m_pEffect->BeginPass(i);
		drawMesh();
		m_pEffect->EndPass();
	}

	hr = m_pEffect->End();

	DrawingUtil::getInst()->drawText("Key S : sphere, Key C : cube", 0, 32);
	ShaderSimpler::render();
}

void CubeEnvMapping::onKeyDown(DWORD vk)
{
	switch(vk) 
	{
	case 'S':
		m_pEffect->SetTechnique("mySphereEnvMap");
		break;
	case 'C':
		m_pEffect->SetTechnique("myCubeEnvMap");
		break;
	}

	ShaderSimpler::onKeyDown(vk);
}
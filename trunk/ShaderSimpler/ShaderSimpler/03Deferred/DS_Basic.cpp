#include ".\ds_basic.h"
#include "..\CameraCtrl.h"
#include "..\DrawingUtil.h"

DS_Basic::DS_Basic(void)
{
	m_pD3DDevice = NULL;

	m_pEffect = NULL;

	m_posRT = NULL;
	m_normalRT = NULL;
	m_diffuseRT = NULL;
}

DS_Basic::~DS_Basic(void)
{
	SAFE_RELEASE(m_pEffect);

	SAFE_RELEASE(m_posRT);
	SAFE_RELEASE(m_normalRT);
	SAFE_RELEASE(m_diffuseRT);

}

bool DS_Basic::createPosRT()
{
	HRESULT hr;
	// create G-Buffers
	hr = m_pD3DDevice->CreateTexture(clientWidth, clientHeight,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A16B16G16R16F,
		D3DPOOL_DEFAULT,
		&m_posRT,
		NULL);
	
	return SUCCEEDED(hr);
}

bool DS_Basic::init(IDirect3DDevice9 *pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;

	//-- init light pos
	for(int i=0; i<NUM_LIGHT; i++)
	{
		m_lightPos[i].x = 0;
		m_lightPos[i].z = 0;
		m_lightPos[i].y = 0;
	}

	//-- load effect
	m_pEffect = DrawingUtil::getInst()->loadEffect(pD3DDevice,"Shader\\DS_Basic.fx");
	if(m_pEffect == NULL)
		return false;

	HRESULT hr;
	// create G-Buffers
	if(!createPosRT())
		return false;

	hr = pD3DDevice->CreateTexture(clientWidth, clientHeight,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_normalRT,
		NULL);

	if(FAILED(hr))
		return false;

	hr = pD3DDevice->CreateTexture(clientWidth, clientHeight,
		1,//mip-map
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_diffuseRT,
		NULL);

	if(FAILED(hr))
		return false;

	if(!m_screenQuad.init(pD3DDevice))
		return false;

	return ShaderSimpler::init(pD3DDevice);
}

void DS_Basic::buildGBuffers()
{
	HRESULT hr;
	//-- G-Buffsers
	{
		// pos
		SetRenderTarget SRT(m_pD3DDevice, m_posRT);
		hr = m_pEffect->SetTechnique("init_pos");
		renderScene();
	}

	{
		// diffuse
		SetRenderTarget SRT(m_pD3DDevice, m_normalRT);
		hr = m_pEffect->SetTechnique("init_normal");
		renderScene();
	}

	{
		// diffuse
		SetRenderTarget SRT(m_pD3DDevice, m_diffuseRT);
		hr = m_pEffect->SetTechnique("init_diffuse");
		renderScene();
	}
}

const char* DS_Basic::getShadingTech()
{
	return "shading_fullscreen";
}

void DS_Basic::setEffectTexture()
{
	HRESULT hr;

	hr = m_pEffect->SetTexture("posRT", m_posRT);
	hr = m_pEffect->SetTexture("normalRT", m_normalRT);
	hr = m_pEffect->SetTexture("diffuseRT", m_diffuseRT);
}

void DS_Basic::render()
{
	// 为了能在Geforce 5600上测试，没有使用Multi-Render targets

	HRESULT hr;
	//-- 设置effect参数
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	D3DXMATRIX matWorldViewProj = matWorld * g_camera.getViewMat() * g_camera.getProjectMat();

	m_pEffect->SetMatrix("matWorldViewPrj",&matWorldViewProj);
	m_pEffect->SetMatrix("matWorld",&matWorld);

	buildGBuffers();
	

	//-- shading
	hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET,
		D3DCOLOR_COLORVALUE(0.f,0.f,0.f,1.0f), 1.0f, 0 );

	//-- full screen light
	hr = m_pEffect->SetTechnique(getShadingTech());
	
	hr = m_pEffect->SetFloatArray("eyePos",(float*)&g_camera.getEyePos(),3);

	setEffectTexture();

	float t = timeGetTime()/1000.0f;
	float r = 150;
	for(int i=0; i<NUM_LIGHT; i++)
	{
		float l = (float)i/NUM_LIGHT*3.1415926f;
		
		m_lightPos[i].x = sinf(t - l) * r;
		m_lightPos[i].z = cosf(t + l) * r;
		m_lightPos[i].y = sinf(t + l) * r;;
		m_pEffect->SetFloatArray("lightPos",(float*)&m_lightPos[i],3);

		UINT numPass = 0;
		hr = m_pEffect->Begin(&numPass,0);
		for(UINT p=0;p<numPass;p++)
		{
			m_pEffect->BeginPass(p);
			m_screenQuad.draw();
			m_pEffect->EndPass();
		}
		hr = m_pEffect->End();

		//--画出灯光的位置
		DrawingUtil::getInst()->drawLight(m_lightPos[i]);
	}
}

void DS_Basic::renderScene()
{
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
}
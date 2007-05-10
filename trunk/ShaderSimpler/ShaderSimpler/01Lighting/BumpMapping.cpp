#include "BumpMapping.h"
#include "..\CameraCtrl.h"
#include "..\DrawingUtil.h"

BumpMapping::BumpMapping() : m_lightPos(60,60,60)
{
	m_pEffect = NULL;

	m_pMeshSphere = NULL;
	m_pEarthMap = NULL;
	m_pEarthNormalMap = NULL;
}

BumpMapping::~BumpMapping()
{
	SAFE_RELEASE(m_pEffect);

	SAFE_RELEASE(m_pMeshSphere);
	SAFE_RELEASE(m_pEarthMap);
	SAFE_RELEASE(m_pEarthNormalMap);
}

bool BumpMapping::init(IDirect3DDevice9 *pD3DDevice)
{
	HRESULT hr;
	//-- effect
	m_pEffect = DrawingUtil::getInst()->loadEffect(pD3DDevice,"Shader\\BumpMapping.fx");

	//-- mesh
	D3DVERTEXELEMENT9 decl[]=
	{
		// stream, offset, type, method, semantic type (for example normal), ?
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		D3DDECL_END()
	};

	vector<D3DXMATERIAL> xmtlArray;
	//--earth mesh
	m_pMeshSphere = loadMeshDecl(pD3DDevice,"Media\\sphere.x",decl,xmtlArray);

	struct tagVert
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
	};

	tagVert *pVerts = NULL;
	m_pMeshSphere->LockVertexBuffer(0,(void**)&pVerts);
	for(DWORD i=0;i<m_pMeshSphere->GetNumVertices();i++)
	{
		pVerts->pos *= 50;
		// go to next vertex
		pVerts++;

	}

	m_pMeshSphere->UnlockVertexBuffer();

	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\earth.jpg",&m_pEarthMap);
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\normal_earth.jpg",&m_pEarthNormalMap);

	return true;
}

void BumpMapping::render()
{
	if(m_pEffect == NULL)
		return;

	HRESULT hr;
	//-- ÉèÖÃeffect²ÎÊý
	float t = timeGetTime()/3000.0f;
	float r = 60;
	m_lightPos.x = sinf(t) * r;
	m_lightPos.z = cosf(t) * r;
	m_lightPos.y = sinf(t) * r;

	hr = m_pEffect->SetFloatArray("lightPos",(float*)&m_lightPos,3);
	hr = m_pEffect->SetFloatArray("eyePos",(float*)&g_camera.getEyePos(),3);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	D3DXMATRIX matWorldViewProj = matWorld * g_camera.getViewMat() * g_camera.getProjectMat();

	m_pEffect->SetMatrix("matWorldViewPrj",&matWorldViewProj);
	hr = m_pEffect->SetMatrix("matWorld",&matWorld);

	//--
	drawEarth();

	DrawingUtil::getInst()->drawLight(m_lightPos);

	ShaderSimpler::render();
}

void BumpMapping::drawEarth()
{
	HRESULT hr;
	hr = m_pEffect->SetTexture("texColorMap",m_pEarthMap);
	hr = m_pEffect->SetTexture("texNormalMap",m_pEarthNormalMap);

	UINT numPass = 0;
	hr = m_pEffect->Begin(&numPass,0);
	for(UINT j=0;j<numPass;j++)
	{
		hr = m_pEffect->BeginPass(j);
		hr = m_pMeshSphere->DrawSubset(0);
		hr = m_pEffect->EndPass();
	}
	hr = m_pEffect->End();
}
#include ".\parallaxmapping.h"
#include "..\CameraCtrl.h"
#include "..\DrawingUtil.h"

ParallaxMapping::ParallaxMapping() : m_lightPos(60,60,60)
{
	m_pEffect = NULL;

	m_pMeshSphere = NULL;
	m_pEarthMap = NULL;
	m_pEarthNormalMap = NULL;
	m_pEarthHeightMap = NULL;

	m_pMeshWall = NULL;
	m_pWallMap = NULL;
	m_pWallNormalMap = NULL;
	m_pWallHeightMap = NULL;
	m_bUseParallax = true;

	m_pBlisterMap = NULL;
	m_pBlisterNormalMap = NULL;
	m_pBlisterHeightMap = NULL;
}

ParallaxMapping::~ParallaxMapping()
{
	SAFE_RELEASE(m_pEffect);

	SAFE_RELEASE(m_pMeshSphere);
	SAFE_RELEASE(m_pEarthMap);
	SAFE_RELEASE(m_pEarthNormalMap);
	SAFE_RELEASE(m_pEarthHeightMap);

	SAFE_RELEASE(m_pMeshWall);
	SAFE_RELEASE(m_pWallMap);
	SAFE_RELEASE(m_pWallNormalMap);
	SAFE_RELEASE(m_pWallHeightMap);

	SAFE_RELEASE(m_pBlisterMap);
	SAFE_RELEASE(m_pBlisterNormalMap);
	SAFE_RELEASE(m_pBlisterHeightMap);
}

bool ParallaxMapping::init(IDirect3DDevice9 *pD3DDevice)
{
	HRESULT hr;
	//-- effect
	m_pEffect = DrawingUtil::getInst()->loadEffect(pD3DDevice,"Shader\\ParallaxMapping.fx");
	m_pEffect->SetTechnique("tech_parallax");
	m_bUseParallax = true;

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

	//--earth mesh
	vector<D3DXMATERIAL> xmtlArray;
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
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\height_earth.jpg",&m_pEarthHeightMap);

	//--
	float wallSize = 200;
	hr = D3DXCreateMesh(4, 6, D3DXMESH_MANAGED, &decl[0], pD3DDevice, &m_pMeshWall);
	hr = m_pMeshWall->LockVertexBuffer(0, (void**)&pVerts);

	pVerts[0].pos = D3DXVECTOR3(wallSize, 0, wallSize);
	pVerts[0].uv = D3DXVECTOR2(1, 0);
	pVerts[0].normal = D3DXVECTOR3(0, 1, 0);
	pVerts[0].tangent = D3DXVECTOR3(1, 0, 0);

	pVerts[1].pos = D3DXVECTOR3(wallSize, 0, -wallSize);
	pVerts[1].uv = D3DXVECTOR2(1, 1);
	pVerts[1].normal = D3DXVECTOR3(0, 1, 0);
	pVerts[1].tangent = D3DXVECTOR3(1, 0, 0);

	pVerts[2].pos = D3DXVECTOR3(-wallSize, 0, -wallSize);
	pVerts[2].uv = D3DXVECTOR2(0, 1);
	pVerts[2].normal = D3DXVECTOR3(0, 1, 0);
	pVerts[2].tangent = D3DXVECTOR3(1, 0, 0);

	pVerts[3].pos = D3DXVECTOR3(-wallSize, 0, wallSize);
	pVerts[3].uv = D3DXVECTOR2(0, 0);
	pVerts[3].normal = D3DXVECTOR3(0, 1, 0);
	pVerts[3].tangent = D3DXVECTOR3(1, 0, 0);

	pVerts[4].pos = D3DXVECTOR3(wallSize, wallSize, -wallSize);
	pVerts[4].uv = D3DXVECTOR2(0, 1);
	pVerts[4].normal = D3DXVECTOR3(-1, 0, 0);
	pVerts[4].tangent = D3DXVECTOR3(0, 1, 0);

	pVerts[5].pos = D3DXVECTOR3(wallSize, wallSize, wallSize);
	pVerts[5].uv = D3DXVECTOR2(0, 0);
	pVerts[5].normal = D3DXVECTOR3(-1, 0, 0);
	pVerts[5].tangent = D3DXVECTOR3(0, 1, 0);

	hr = m_pMeshWall->UnlockVertexBuffer();

	WORD *pIndex = NULL;
	hr = m_pMeshWall->LockIndexBuffer(0, (void**)&pIndex);
	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;

	pIndex[3] = 2;
	pIndex[4] = 3;
	pIndex[5] = 0;

	pIndex[6] = 0;
	pIndex[7] = 1;
	pIndex[8] = 4;

	pIndex[9] = 4;
	pIndex[10] = 5;
	pIndex[11] = 0;

	hr = m_pMeshWall->UnlockIndexBuffer();

	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\rockwall.jpg",&m_pWallMap);
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\rockwall_normal.jpg",&m_pWallNormalMap);
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\rockwall_height.jpg",&m_pWallHeightMap);


	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\blister.jpg",&m_pBlisterMap);
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\blister_normal.jpg",&m_pBlisterNormalMap);
	hr = D3DXCreateTextureFromFile(pD3DDevice,"Media\\blister_height.jpg",&m_pBlisterHeightMap);

	return true;
}

void ParallaxMapping::render()
{
	if(m_pEffect == NULL)
		return;

	HRESULT hr;
	//-- ÉèÖÃeffect²ÎÊý
	float t = timeGetTime()/2000.0f;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	if(m_activeMesh == 0)
	{
		float r = 60;
		m_lightPos.x = sinf(t) * r;
		m_lightPos.z = cosf(t) * r;
		m_lightPos.y = sinf(t) * r;
	}
	else
	{
		float r = 160;
		m_lightPos.x = sinf(t) * r;
		m_lightPos.z = cosf(t) * r;
		m_lightPos.y = 40;
	}

	hr = m_pEffect->SetFloatArray("lightPos",(float*)&m_lightPos,3);
	hr = m_pEffect->SetFloatArray("eyePos",(float*)&g_camera.getEyePos(),3);

	D3DXMATRIX matWorldViewProj = matWorld * g_camera.getViewMat() * g_camera.getProjectMat();

	m_pEffect->SetMatrix("matWorldViewPrj",&matWorldViewProj);
	hr = m_pEffect->SetMatrix("matWorld",&matWorld);

	//--
	if(m_activeMesh == 0)
		drawEarth();
	else
		drawWall();

	DrawingUtil::getInst()->drawLight(m_lightPos);

	DrawingUtil::getInst()->drawText("Press P to enable/disable parallax mapping", 0, 32);
	ShaderSimpler::render();
}

void ParallaxMapping::drawEarth()
{
	HRESULT hr;
	hr = m_pEffect->SetTexture("texColorMap",m_pEarthMap);
	hr = m_pEffect->SetTexture("texNormalMap",m_pEarthNormalMap);
	hr = m_pEffect->SetTexture("texHeightMap",m_pEarthHeightMap);

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

void ParallaxMapping::drawWall()
{
	HRESULT hr;
	if(m_activeMesh == 1)
	{
		hr = m_pEffect->SetTexture("texColorMap",m_pWallMap);
		hr = m_pEffect->SetTexture("texNormalMap",m_pWallNormalMap);
		hr = m_pEffect->SetTexture("texHeightMap",m_pWallHeightMap);
	}
	else
	{
		hr = m_pEffect->SetTexture("texColorMap",m_pBlisterMap);
		hr = m_pEffect->SetTexture("texNormalMap",m_pBlisterNormalMap);
		hr = m_pEffect->SetTexture("texHeightMap",m_pBlisterHeightMap);
	}

	UINT numPass = 0;
	hr = m_pEffect->Begin(&numPass,0);
	for(UINT j=0;j<numPass;j++)
	{
		hr = m_pEffect->BeginPass(j);
		hr = m_pMeshWall->DrawSubset(0);
		hr = m_pEffect->EndPass();
	}
	hr = m_pEffect->End();
}
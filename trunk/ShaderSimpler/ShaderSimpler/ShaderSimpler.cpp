#include "ShaderSimpler.h"
#include "DrawingUtil.h"

ShaderSimpler::ShaderSimpler(void)
{
	m_activeMesh = 0;

	m_pMeshTeapot = 0;

	m_numSubMeshTank = 0;
	m_pMeshTank = NULL;

	m_pMeshSphere = NULL;
	m_pMeshBox = NULL;

	m_pMeshTorus = NULL;

	m_numSubMeshKnot = 0;
	m_pMeshKnot = NULL;
}

ShaderSimpler::~ShaderSimpler(void)
{
	SAFE_RELEASE(m_pMeshKnot);

	SAFE_RELEASE(m_pMeshTank);

	SAFE_RELEASE(m_pMeshSphere);
	SAFE_RELEASE(m_pMeshBox);

	SAFE_RELEASE(m_pMeshTorus);
}

bool IfHaveUsage(const D3DVERTEXELEMENT9 *pDeclaration,D3DDECLUSAGE usage)
{
	D3DVERTEXELEMENT9 end = D3DDECL_END();
	int i=0;
	while (memcmp(&pDeclaration[i], &end,sizeof(D3DVERTEXELEMENT9)) != 0)
	{
		if(pDeclaration[i].Usage == usage)
			return true;
		i++;
	}

	return false;
}

ID3DXMesh* ShaderSimpler::loadMeshDecl(IDirect3DDevice9 *pD3DDevice, const char* szFileName, const D3DVERTEXELEMENT9 *pDeclaration, vector<D3DXMATERIAL>& xmtlArray)
{
	xmtlArray.clear();

	//--
	ID3DXMesh* pRet = NULL;

	HRESULT hr;
	ID3DXBuffer *pAdjacency = NULL;
	ID3DXMesh *pMesh = NULL;
	ID3DXBuffer *pXMtlBuffer = NULL;
	DWORD numMtl = 0;

	hr = D3DXLoadMeshFromX(szFileName,
		D3DXMESH_SYSTEMMEM,
		pD3DDevice,
		&pAdjacency,&pXMtlBuffer,NULL,&numMtl,
		&pMesh);
	if(SUCCEEDED(hr))
	{
		//--copy d3dx mtl
		D3DXMATERIAL *pXMtl = (D3DXMATERIAL *)pXMtlBuffer->GetBufferPointer();
		for(DWORD i=0;i<numMtl;i++)
		{
			xmtlArray.push_back(pXMtl[i]);
		}

		//--
		hr = pMesh->CloneMesh(D3DXMESH_MANAGED, pDeclaration, pD3DDevice, &pRet);
		if(SUCCEEDED(hr))
		{
			if(IfHaveUsage(pDeclaration,D3DDECLUSAGE_NORMAL))
				hr = D3DXComputeNormals(pRet,NULL);

			if(IfHaveUsage(pDeclaration,D3DDECLUSAGE_TANGENT))
			{
				hr = D3DXComputeTangent(pRet,0,0,D3DX_DEFAULT,TRUE,NULL);
			}
		}

		pMesh->Release();
	}

	return pRet;
}

ID3DXMesh* ShaderSimpler::loadMeshFVF(IDirect3DDevice9 *pD3DDevice, const char* szFileName, DWORD fvf, vector<D3DXMATERIAL>& xmtlArray)
{
	xmtlArray.clear();
	
	//--
	ID3DXMesh* pRet = NULL;

	HRESULT hr;
	ID3DXBuffer *pAdjacency = NULL;
	ID3DXMesh *pMesh = NULL;
	ID3DXBuffer *pXMtlBuffer = NULL;
	DWORD numMtl = 0;

	hr = D3DXLoadMeshFromX(szFileName,
		D3DXMESH_SYSTEMMEM,
		pD3DDevice,
		&pAdjacency,&pXMtlBuffer,NULL,&numMtl,
		&pMesh);
	if(SUCCEEDED(hr))
	{
		//--copy d3dx mtl
		D3DXMATERIAL *pXMtl = (D3DXMATERIAL *)pXMtlBuffer->GetBufferPointer();
		for(DWORD i=0;i<numMtl;i++)
		{
			xmtlArray.push_back(pXMtl[i]);
		}

		//--
		hr = pMesh->CloneMeshFVF(D3DXMESH_MANAGED, fvf, pD3DDevice, &pRet);
		if(SUCCEEDED(hr))
		{
			if(fvf & D3DFVF_NORMAL)
				hr = D3DXComputeNormals(pRet,(DWORD*)pAdjacency->GetBufferPointer());
		}

		pMesh->Release();
	}

	return pRet;
}

bool ShaderSimpler::init(IDirect3DDevice9 *pD3DDevice)
{
	HRESULT hr;

	vector<D3DXMATERIAL> xmtlArray;
	m_pMeshTank = loadMeshFVF(pD3DDevice, "Media\\Tank.x", D3DFVF_XYZ|D3DFVF_NORMAL,xmtlArray);
	m_numSubMeshTank = (DWORD)xmtlArray.size();
	m_pMeshKnot = loadMeshFVF(pD3DDevice, "Media\\Knot.x", D3DFVF_XYZ|D3DFVF_NORMAL,xmtlArray);
	m_numSubMeshKnot = (DWORD)xmtlArray.size();

	//--
	hr = D3DXCreateTeapot(pD3DDevice,&m_pMeshTeapot,NULL);
	if(SUCCEEDED(hr))
	{
		DWORD numVert = m_pMeshTeapot->GetNumVertices();
		D3DXVECTOR3 *pVB = NULL;
		hr = m_pMeshTeapot->LockVertexBuffer(0,(void**)&pVB);
		if(SUCCEEDED(hr))
		{
			float s = 150;
			for(DWORD i=0;i<numVert;i++)
			{
				(*pVB) *= s;
				pVB += 2;
			}
			m_pMeshTeapot->UnlockVertexBuffer();
		}
	}

	
	//--
	hr = D3DXCreateSphere(pD3DDevice,100,32,32,
		&m_pMeshSphere,NULL);
	hr = D3DXCreateBox(pD3DDevice,400,10,400,&m_pMeshBox,NULL);

	hr = D3DXCreateTorus(pD3DDevice,50,100,32,32,&m_pMeshTorus,NULL);

	return true;
}

void ShaderSimpler::render()
{
	//--show usage
	DrawingUtil::getInst()->drawText("Press TAB to change mesh", 0, 16);
}

void ShaderSimpler::drawMesh()
{
	switch(m_activeMesh)
	{
	case 4:
		if(m_pMeshTank)
		{
			for(DWORD j=0;j<m_numSubMeshTank;j++)
				m_pMeshTank->DrawSubset(j);
		}
		break;
	case 3:
		if(m_pMeshTeapot)
			m_pMeshTeapot->DrawSubset(0);
		break;
	case 2:
		if(m_pMeshKnot)
		{
			for(DWORD j=0;j<m_numSubMeshKnot;j++)
				m_pMeshKnot->DrawSubset(j);
		}
		break;
	case 1:
		if(m_pMeshTorus)
			m_pMeshTorus->DrawSubset(0);
		break;
	default:
		if(m_pMeshSphere)
			m_pMeshSphere->DrawSubset(0);
		if(m_pMeshBox)
			m_pMeshBox->DrawSubset(0);
		break;
	}
}
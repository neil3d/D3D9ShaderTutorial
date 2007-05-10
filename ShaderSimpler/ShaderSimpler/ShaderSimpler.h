#pragma once

#include "inc.h"

#include <vector>
using namespace std;

class ShaderSimpler
{
public:
	ShaderSimpler(void);
	virtual ~ShaderSimpler(void);

	virtual bool init(IDirect3DDevice9 *pD3DDevice) = 0;
	virtual void render() = 0;

	virtual void onKeyDown(DWORD vk) 
	{
		if(vk == VK_TAB)
		{
			const int MAX_MESH = 5;
			m_activeMesh++;
			m_activeMesh %= MAX_MESH;
		}
	}

protected:
	void drawMesh();
	ID3DXMesh* loadMeshFVF(IDirect3DDevice9 *pD3DDevice, const char* szFileName, DWORD fvf, vector<D3DXMATERIAL>& xmtlArray);
	ID3DXMesh* loadMeshDecl(IDirect3DDevice9 *pD3DDevice, const char* szFileName, const D3DVERTEXELEMENT9 *pDeclaration, vector<D3DXMATERIAL>& xmtlArray);
protected:
	int	m_activeMesh;

	ID3DXMesh	*m_pMeshTeapot;

	DWORD		m_numSubMeshKnot;
	ID3DXMesh	*m_pMeshKnot;

	DWORD		m_numSubMeshTank;
	ID3DXMesh	*m_pMeshTank;

	ID3DXMesh	*m_pMeshSphere;
	ID3DXMesh	*m_pMeshBox;

	ID3DXMesh	*m_pMeshTorus;
};

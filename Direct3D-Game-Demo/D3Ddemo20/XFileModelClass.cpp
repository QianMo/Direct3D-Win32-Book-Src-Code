//=============================================================================
// Name: XFileModelClass.h
//	Des: 一个封装了X文件载入与渲染功能的类源文件
// 2013年 4月7日  Create by 浅墨 
//=============================================================================
#include <d3dx9.h>
#include <tchar.h> 
#include "XFileModelClass.h"

//-----------------------------------------------------------------------------
// Desc: 构造函数
//-----------------------------------------------------------------------------
XFileModelClass::XFileModelClass(IDirect3DDevice9* pd3dDevice)
{	
	//给各个成员变量赋初值
	m_pd3dDevice = pd3dDevice;
}

//--------------------------------------------------------------------------------------
// Name: XFileModelClass::LoadModelFromXFile()
// Desc: 从.X文件读取三维模型到内存中
//--------------------------------------------------------------------------------------
HRESULT XFileModelClass::LoadModelFromXFile( WCHAR* strFilename )
{

	LPD3DXBUFFER pAdjacencyBuffer = NULL;  //网格模型邻接信息
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;   //存储网格模型材质的缓存对象

	//从磁盘文件加载网格模型
	D3DXLoadMeshFromX( strFilename, D3DXMESH_MANAGED,  m_pd3dDevice, &pAdjacencyBuffer, 
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, &m_pMesh );

	// 读取材质和纹理数据
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
	m_pTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

	//逐子集提取材质属性和纹理文件名
	for( DWORD i=0; i<m_dwNumMaterials; i++ )
	{
		//获取材质，并设置一下环境光的颜色值
		m_pMaterials[i] = d3dxMaterials[i].MatD3D;
		m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

		//创建一下纹理对象
		m_pTextures[i] = NULL;
		if( d3dxMaterials[i].pTextureFilename != NULL && 
			strlen(d3dxMaterials[i].pTextureFilename) > 0 )
		{
			//创建纹理
			if( FAILED( D3DXCreateTextureFromFileA( m_pd3dDevice,d3dxMaterials[i].pTextureFilename, &m_pTextures[i] ) ) )
			{
				MessageBox(NULL, L"SORRY~!没有找到纹理文件!", L"XFileModelClass类读取文件错误", MB_OK);
			}
		}
	}
	//优化网格模型
	m_pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_STRIPREORDER,
		(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL );

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: XFileModelClass::RenderModel()
// Desc: 渲染三维网格模型
//--------------------------------------------------------------------------------------
HRESULT XFileModelClass::RenderModel( )
{
	for( DWORD i=0; i<m_dwNumMaterials; i++ )
	{

		m_pd3dDevice->SetMaterial( &m_pMaterials[i] );
		m_pd3dDevice->SetTexture( 0, m_pTextures[i] );
		m_pMesh->DrawSubset( i );
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Desc: 析构函数
//-----------------------------------------------------------------------------
XFileModelClass::~XFileModelClass(void)
{
	//释放网格模型材质
	SAFE_DELETE_ARRAY(m_pMaterials);

	//释放网格模型纹理
	if( m_pTextures )
	{
		for( DWORD i = 0; i < m_dwNumMaterials; i++ )
		{
			SAFE_RELEASE(m_pTextures[i]);
		}
		SAFE_DELETE_ARRAY(m_pTextures);
	}

	//释放网格模型对象
	SAFE_RELEASE(m_pMesh);
}

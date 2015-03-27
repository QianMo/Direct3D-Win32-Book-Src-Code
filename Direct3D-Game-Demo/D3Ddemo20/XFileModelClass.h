//=============================================================================
// Name: XFileModelClass.h
//	Des: 一个封装了X文件载入与渲染功能的类头文件
// 2013年 4月7日  Create by 浅墨 
//=============================================================================

#pragma once
#include "D3DUtil.h"


class XFileModelClass
{
private:
	LPDIRECT3DDEVICE9				m_pd3dDevice;			//D3D设备对象
	LPD3DXMESH						m_pMesh;       //网格模型对象
	DWORD								m_dwNumMaterials; //材质的数量
	D3DMATERIAL9*					m_pMaterials;     //模型材质结构体的实例
	LPDIRECT3DTEXTURE9 *			m_pTextures;      //模型纹理结构体的实例

public:
	XFileModelClass(IDirect3DDevice9 *pd3dDevice); //构造函数
	~XFileModelClass(void);     //析构函数   

public:
	HRESULT		LoadModelFromXFile(WCHAR* strFilename );  //从.X文件读取三维模型到内存中
	HRESULT		RenderModel( );  //渲染三维网格模型
	
};
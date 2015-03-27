//=============================================================================
// Name: SkyBoxClass.h
//	Des: 一个封装了三维天空盒系统的类的头文件
// 2013年 3月24日  Create by 浅墨 
//=============================================================================
#pragma once
#include "D3DUtil.h"

//为天空盒类定义一个FVF灵活顶点格式
struct SKYBOXVERTEX
{
	float	x,y,z;
	float	u,v;
};
#define D3DFVF_SKYBOX D3DFVF_XYZ|D3DFVF_TEX1


class SkyBoxClass
{
private:
	LPDIRECT3DDEVICE9				m_pd3dDevice;			//D3D设备对象
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;		//顶点缓存对象
	LPDIRECT3DTEXTURE9			m_pTexture[5];			//5个纹理接口对象
	float										m_Length;					//天空盒边长

public:
	SkyBoxClass( LPDIRECT3DDEVICE9  pDevice );		//构造函数
	virtual ~SkyBoxClass(void);								//析构函数

public:
	BOOL	InitSkyBox( float Length );   //初始化天空盒函数
	BOOL	LoadSkyTextureFromFile(wchar_t *pFrontTextureFile, wchar_t *pBackTextureFile,wchar_t *pLeftTextureFile, wchar_t *pRightTextureFile,wchar_t *pTopTextureFile);  //从文件加载天空盒五个方向上的纹理
	VOID		RenderSkyBox( D3DXMATRIX *pMatWorld, BOOL bRenderFrame ); //渲染天空盒,根据第一个参数设定天空盒世界矩阵，第二个参数选择是否渲染出线框

};


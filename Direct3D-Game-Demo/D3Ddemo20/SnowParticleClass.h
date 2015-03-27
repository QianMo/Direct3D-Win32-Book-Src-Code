//=============================================================================
// Name: SnowParticleClass.h
//	Des: 一个封装了雪花粒子系统系统的类的头文件
// 2013年 3月31日  Create by 浅墨 
//=============================================================================


#pragma once
#include "D3DUtil.h"
#define  PARTICLE_NUMBER  3000   //雪花粒子数量，显卡不好、运行起来卡的童鞋请取小一点。
#define  SNOW_SYSTEM_LENGTH_X	20000   //雪花飞扬区域的长度
#define  SNOW_SYSTEM_WIDTH_Z		20000   //雪花飞扬区域的宽度
#define  SNOW_SYSTEM_HEIGHT_Y     20000   //雪花飞扬区域的高度

//-------------------------------------------------------------------------------------------------
//雪花粒子的FVF顶点结构和顶点格式
//-------------------------------------------------------------------------------------------------
struct POINTVERTEX
{
	float x, y, z;    //顶点位置
	float u,v ;		  //顶点纹理坐标
};
#define D3DFVF_POINTVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)


//-------------------------------------------------------------------------------------------------
// Desc: 雪花粒子结构体的定义
//-------------------------------------------------------------------------------------------------
struct  SNOWPARTICLE
{
	float x, y, z;      //坐标位置
	float RotationY;         //雪花绕自身Y轴旋转角度
	float RotationX;       //雪花绕自身X轴旋转角度
	float FallSpeed;       //雪花下降速度
	float RotationSpeed;       //雪花旋转速度
	int   TextureIndex;     //纹理索引数
};

//-------------------------------------------------------------------------------------------------
// Desc: 粒子系统类的定义
//-------------------------------------------------------------------------------------------------
class SnowParticleClass
{
private:
	LPDIRECT3DDEVICE9				m_pd3dDevice;			//D3D设备对象
	SNOWPARTICLE						m_Snows[PARTICLE_NUMBER];    //雪花粒子数组
	LPDIRECT3DVERTEXBUFFER9   m_pVertexBuffer;      //粒子顶点缓存
	LPDIRECT3DTEXTURE9			m_pTexture[6];  //雪花纹理数组

public:
	SnowParticleClass(LPDIRECT3DDEVICE9 pd3dDevice);   //构造函数
	~SnowParticleClass();					//析构函数
	HRESULT InitSnowParticle();        //粒子系统初始化函数
	HRESULT UpdateSnowParticle( float fElapsedTime);    //粒子系统更新函数
	HRESULT RenderSnowParticle( );   //粒子系统渲染函数
};


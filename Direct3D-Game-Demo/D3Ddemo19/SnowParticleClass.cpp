//=============================================================================
// Name: SnowParticleClass.cpp
//	Des: 一个封装了雪花粒子系统系统的类的源文件
// 2013年 3月31日  Create by 浅墨 
//=============================================================================

#include "d3dx9.h"
#include "SnowParticleClass.h"


//-------------------------------------------------------------------------------------------------
// Desc: 构造函数
//-------------------------------------------------------------------------------------------------
SnowParticleClass::SnowParticleClass(LPDIRECT3DDEVICE9 pd3dDevice)
{
	//给各个参数赋初值
	m_pd3dDevice=pd3dDevice;
	m_pVertexBuffer=NULL;	
	for(int i=0; i<5; i++)
		m_pTexture[i] = NULL; 
}

//-------------------------------------------------------------------------------------------------
// Name:  SnowParticleClass::InitSnowParticle( )
// Desc: 粒子系统初始化函数
//-------------------------------------------------------------------------------------------------
HRESULT SnowParticleClass::InitSnowParticle( )
{
	//初始化雪花粒子数组
	srand(GetTickCount());
	for(int i=0; i<PARTICLE_NUMBER; i++)
	{	
		m_Snows[i].x        = float(rand()%SNOW_SYSTEM_LENGTH_X-SNOW_SYSTEM_LENGTH_X/2);
		m_Snows[i].z        = float(rand()%SNOW_SYSTEM_WIDTH_Z-SNOW_SYSTEM_WIDTH_Z/2);
		m_Snows[i].y        = float(rand()%SNOW_SYSTEM_HEIGHT_Y);
		m_Snows[i].RotationY     = (rand()%100)/50.0f*D3DX_PI;
		m_Snows[i].RotationX   = (rand()%100)/50.0f*D3DX_PI;
		m_Snows[i].FallSpeed   = 300.0f + rand()%500;
		m_Snows[i].RotationSpeed   = 5.0f +  rand()%10/10.0f;
		m_Snows[i].TextureIndex = rand()%6;
	}


	//创建雪花粒子顶点缓存
	m_pd3dDevice->CreateVertexBuffer( 4*sizeof(POINTVERTEX), 0, 
		D3DFVF_POINTVERTEX,D3DPOOL_MANAGED, &m_pVertexBuffer, NULL );

	//填充雪花粒子顶点缓存
	POINTVERTEX vertices[] =
	{
		{ -30.0f, 0.0f, 0.0f,   0.0f, 1.0f, },
		{ -30.0f, 60.0f, 0.0f,   0.0f, 0.0f, },
		{  30.0f, 0.0f, 0.0f,   1.0f, 1.0f, }, 
		{  30.0f, 60.0f, 0.0f,   1.0f, 0.0f, }
	};
	//加锁
	VOID* pVertices;
	m_pVertexBuffer->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 );
	//访问
	memcpy( pVertices, vertices, sizeof(vertices) );
	//解锁
	m_pVertexBuffer->Unlock();

	//创建6种雪花纹理
	D3DXCreateTextureFromFile( m_pd3dDevice, L"GameMedia\\snow1.jpg", &m_pTexture[0] );
	D3DXCreateTextureFromFile( m_pd3dDevice, L"GameMedia\\snow2.jpg", &m_pTexture[1] );
	D3DXCreateTextureFromFile( m_pd3dDevice, L"GameMedia\\snow3.jpg", &m_pTexture[2] );
	D3DXCreateTextureFromFile( m_pd3dDevice, L"GameMedia\\snow4.jpg", &m_pTexture[3] );
	D3DXCreateTextureFromFile( m_pd3dDevice, L"GameMedia\\snow5.jpg", &m_pTexture[4] );
	D3DXCreateTextureFromFile( m_pd3dDevice, L"GameMedia\\snow6.jpg", &m_pTexture[5] );

	return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name:  SnowParticleClass::UpdateSnowParticle( )
// Desc: 粒子系统更新函数
//-------------------------------------------------------------------------------------------------
HRESULT SnowParticleClass::UpdateSnowParticle( float fElapsedTime)
{

	//一个for循环，更新每个雪花粒子的当前位置和角度
	for(int i=0; i<PARTICLE_NUMBER; i++)
	{
		m_Snows[i].y -= m_Snows[i].FallSpeed*fElapsedTime;

		//如果雪花粒子落到地面, 重新将其高度设置为最大
		if(m_Snows[i].y<0)
			m_Snows[i].y = SNOW_SYSTEM_WIDTH_Z;
		//更改自旋角度
		m_Snows[i].RotationY    += m_Snows[i].RotationSpeed * fElapsedTime;
		m_Snows[i].RotationX  += m_Snows[i].RotationSpeed * fElapsedTime;
	}

	return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name:  SnowParticleClass::RenderSnowParticle( )
// Desc: 粒子系统渲染函数
//-------------------------------------------------------------------------------------------------
HRESULT SnowParticleClass::RenderSnowParticle(  )
{
	//禁用照明效果
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );

	//设置纹理状态
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);   //将纹理颜色混合的第一个参数的颜色值用于输出
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );   //纹理颜色混合的第一个参数的值就取纹理颜色值
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );  //缩小过滤状态采用线性纹理过滤
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ); //放大过滤状态采用线性纹理过滤

	//设置Alpha混合系数
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);   //打开Alpha混合
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);   //源混合系数设为1
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);  //目标混合系数设为1

	//设置剔出模式为不剔除任何面
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//渲染雪花
	for(int i=0; i<PARTICLE_NUMBER; i++)
	{
		//构造并设置当前雪花粒子的世界矩阵
		static D3DXMATRIX matYaw, matPitch, matTrans, matWorld;
		D3DXMatrixRotationY(&matYaw, m_Snows[i].RotationY);
		D3DXMatrixRotationX(&matPitch, m_Snows[i].RotationX);
		D3DXMatrixTranslation(&matTrans, m_Snows[i].x, m_Snows[i].y, m_Snows[i].z);
		matWorld = matYaw * matPitch * matTrans;
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld);

		//渲染当前雪花粒子
		m_pd3dDevice->SetTexture( 0, m_pTexture[m_Snows[i].TextureIndex] );    //设置纹理
		m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(POINTVERTEX));  //把包含的几何体信息的顶点缓存和渲染流水线相关联  
		m_pd3dDevice->SetFVF(D3DFVF_POINTVERTEX);    //设置FVF灵活顶点格式
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);   //绘制

	}

	//恢复相关渲染状态：Alpha混合 、剔除状态、光照
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

	return S_OK;
}




//-------------------------------------------------------------------------------------------------
// Desc: 析构函数
//-------------------------------------------------------------------------------------------------
SnowParticleClass::~SnowParticleClass()
{
	SAFE_RELEASE(m_pVertexBuffer);

	for(int i=0;i<3; i++)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}
}
//=============================================================================
// Name: CameraClass.h
//	Des: 一个封装了实现虚拟摄像机的类的头文件
// 2013年 3月10日  Create by 浅墨 
//=============================================================================


#pragma once

#include <d3d9.h>
#include <d3dx9.h>

class CameraClass
{
private:
	//成员变量的声明
    D3DXVECTOR3				m_vRightVector;        // 右分量向量
    D3DXVECTOR3				m_vUpVector;           // 上分量向量
    D3DXVECTOR3				m_vLookVector;         // 观察方向向量
    D3DXVECTOR3				m_vCameraPosition;        // 摄像机位置的向量
	D3DXVECTOR3				m_vTargetPosition;        //目标观察位置的向量
    D3DXMATRIX				m_matView;          // 取景变换矩阵
    D3DXMATRIX				m_matProj;          // 投影变换矩阵   
    LPDIRECT3DDEVICE9		m_pd3dDevice;  //Direct3D设备对象

public:
	//一个计算取景变换的函数
    VOID CalculateViewMatrix(D3DXMATRIX *pMatrix);    //计算取景变换矩阵

	//三个Get系列函数
    VOID GetProjMatrix(D3DXMATRIX *pMatrix)  { *pMatrix = m_matProj; }  //返回当前投影矩阵
    VOID GetCameraPosition(D3DXVECTOR3 *pVector)  { *pVector = m_vCameraPosition; } //返回当前摄像机位置矩阵
    VOID GetLookVector(D3DXVECTOR3 *pVector) { *pVector = m_vLookVector; }  //返回当前的观察矩阵

	//四个Set系列函数，注意他们参数都有默认值NULL的，调用时不写参数也可以
    VOID SetTargetPosition(D3DXVECTOR3 *pLookat = NULL);  //设置摄像机的目标观察位置向量
    VOID SetCameraPosition(D3DXVECTOR3 *pVector = NULL); //设置摄像机所在的位置向量
    VOID SetViewMatrix(D3DXMATRIX *pMatrix = NULL);  //设置取景变换矩阵
    VOID SetProjMatrix(D3DXMATRIX *pMatrix = NULL);  //设置投影变换矩阵

public:
    // 沿各分量平移的三个函数
    VOID MoveAlongRightVec(FLOAT fUnits);   // 沿right向量移动
    VOID MoveAlongUpVec(FLOAT fUnits);      // 沿up向量移动
    VOID MoveAlongLookVec(FLOAT fUnits);    // 沿look向量移动

    // 绕各分量旋转的三个函数
    VOID RotationRightVec(FLOAT fAngle);    // 绕right向量旋转
    VOID RotationUpVec(FLOAT fAngle);       // 绕up向量旋转
    VOID RotationLookVec(FLOAT fAngle);     // 绕look向量旋转

public:
	//构造函数和析构函数
	CameraClass(IDirect3DDevice9 *pd3dDevice);  //构造函数
	virtual ~CameraClass(void);  //析构函数

};

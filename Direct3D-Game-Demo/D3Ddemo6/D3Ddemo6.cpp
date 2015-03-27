//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：D3Ddemo6
//	 2013年4月 Create by 浅墨
//  描述：Direct3D中几种几何体的快捷绘制 示例程序
//	 程序运行说明： 按键盘上数字键“1”键和“2”键可以在线框填充模式和实体填充模式之间切换。默认情况下是线框填充
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>

//-----------------------------------【库文件包含部分】---------------------------------------
//	描述：包含程序所依赖的库文件
//------------------------------------------------------------------------------------------------
#pragma comment(lib,"winmm.lib")  //调用PlaySound函数所需库文件
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

//-----------------------------------【宏定义部分】--------------------------------------------
//	描述：定义一些辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	L"【致我们永不熄灭的游戏开发梦想】Direct3D中几种几何体的快捷绘制 示例程序"	//为窗口标题定义的宏
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //定义一个安全释放宏，便于后面COM接口指针的释放



//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
LPDIRECT3DDEVICE9					g_pd3dDevice = NULL; //Direct3D设备对象
ID3DXFont*								g_pFont=NULL;    //字体COM接口
float											g_FPS = 0.0f;       //一个浮点型的变量，代表帧速率
wchar_t										g_strFPS[50];    //包含帧速率的字符数组
LPD3DXMESH							g_teapot = NULL;		//茶壶对象
LPD3DXMESH 							g_cube = NULL;			//立方体（盒子）对象
LPD3DXMESH 							g_sphere = NULL;		//球面体对象
LPD3DXMESH 							g_torus = NULL;		//圆环对象
D3DXMATRIX 							g_WorldMatrix[4],R;  //定义一些全局的世界矩阵


//-----------------------------------【全局函数声明部分】-------------------------------------
//	描述：全局函数声明，防止“未声明的标识”系列错误
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );//窗口过程函数
HRESULT					Direct3D_Init(HWND hwnd);		 //在这个函数中进行Direct3D的初始化
HRESULT					Objects_Init(HWND hwnd); 		//在这个函数中进行要绘制的物体的资源初始化
VOID							Direct3D_Render(HWND hwnd); 	//在这个函数中进行Direct3D渲染代码的书写
VOID							Direct3D_CleanUp( );					//在这个函数中清理COM资源以及其他资源
float							Get_FPS();									//计算帧数的函数
VOID							Matrix_Set();                              //封装了四大变换的函数

//-----------------------------------【WinMain( )函数】--------------------------------------
//	描述：Windows应用程序的入口函数，我们的程序从这里开始
//------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{
	//【1】窗口创建四步曲之一：开始设计一个完整的窗口类
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra		= 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra		= 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon=(HICON)::LoadImage(NULL,L"icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    //指定窗口类的光标句柄。
	wndClass.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//用一个以空终止的字符串，指定窗口类的名字。

	//【2】窗口创建四步曲之二：注册窗口类
	if( !RegisterClassEx( &wndClass ) )				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;		

	//【3】窗口创建四步曲之三：正式创建窗口
	HWND hwnd = CreateWindow( L"ForTheDreamOfGameDevelop",WINDOW_TITLE,				//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL );

	//Direct3D资源的初始化，调用失败用messagebox予以显示
	if (!(S_OK==Direct3D_Init (hwnd)))
	{
		MessageBox(hwnd, _T("Direct3D初始化失败~！"), _T("浅墨的消息窗口"), 0); //使用MessageBox函数，创建一个消息窗口 
	}

	//【4】窗口创建四步曲之四：窗口的移动、显示与更新
	MoveWindow(hwnd,250,80,WINDOW_WIDTH,WINDOW_HEIGHT,true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow( hwnd, nShowCmd );    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	PlaySound(L"君がいるから (Long Version).wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

	//【5】消息循环过程
	MSG msg = { 0 };  //初始化msg
	while( msg.message != WM_QUIT )			//使用while循环
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage( &msg );		//将虚拟键消息转换为字符消息
			DispatchMessage( &msg );		//该函数分发一个消息给窗口程序。
		}
		else
		{
			Direct3D_Render(hwnd);   //进行渲染
		}
	}
	//【6】窗口类的注销
	UnregisterClass(L"ForTheDreamOfGameDevelop", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;  
}

//-----------------------------------【WndProc( )函数】--------------------------------------
//	描述：窗口过程函数WndProc,对窗口消息进行处理
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )      
{
	switch( message )						//switch语句开始
	{
	case WM_PAINT:						// 若是客户区重绘消息
		Direct3D_Render(hwnd);                 //调用Direct3D渲染函数
		ValidateRect(hwnd, NULL);		// 更新客户区的显示
		break;									//跳出该switch语句

	case WM_KEYDOWN:					// 若是键盘按下消息
		if (wParam == VK_ESCAPE)    // 如果被按下的键是ESC
			DestroyWindow(hwnd);		// 销毁窗口, 并发送一条WM_DESTROY消息
		break;									//跳出该switch语句

	case WM_DESTROY:					//若是窗口销毁消息
		Direct3D_CleanUp();			//调用自定义的资源清理函数Game_CleanUp（）进行退出前的资源清理
		PostQuitMessage( 0 );			//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;									//跳出该switch语句

	default:										//若上述case条件都不符合，则执行该default语句
		return DefWindowProc( hwnd, message, wParam, lParam );		//调用缺省的窗口过程
	}

	return 0;									//正常退出
}

//-----------------------------------【Direct3D_Init( )函数】--------------------------------------
//	描述：Direct3D初始化函数，进行Direct3D的初始化
//------------------------------------------------------------------------------------------------
HRESULT Direct3D_Init(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之一，创接口】：创建Direct3D接口对象, 以便用该Direct3D对象创建Direct3D设备对象
	//--------------------------------------------------------------------------------------
	LPDIRECT3D9  pD3D = NULL; //Direct3D接口对象的创建
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) //初始化Direct3D接口对象，并进行DirectX版本协商
		return E_FAIL;

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之二,取信息】：获取硬件设备信息
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
	{
		return E_FAIL;
	}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //支持硬件顶点运算，我们就采用硬件顶点运算，妥妥的
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //不支持硬件顶点运算，无奈只好采用软件顶点运算

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之三，填内容】：填充D3DPRESENT_PARAMETERS结构体
	//--------------------------------------------------------------------------------------
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth            = WINDOW_WIDTH;
	d3dpp.BackBufferHeight           = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之四，创设备】：创建Direct3D设备接口
	//--------------------------------------------------------------------------------------
	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	SAFE_RELEASE(pD3D) //LPDIRECT3D9接口对象的使命完成，我们将其释放掉

		if(!(S_OK==Objects_Init(hwnd))) return E_FAIL;     //调用一次Objects_Init，进行渲染资源的初始化

	return S_OK;
}



//-----------------------------------【Object_Init( )函数】--------------------------------------
//	描述：渲染资源初始化函数，在此函数中进行要被渲染的物体的资源的初始化
//--------------------------------------------------------------------------------------------------
HRESULT Objects_Init(HWND hwnd)
{
	//创建字体
	if(FAILED(D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pFont)))
		return E_FAIL;
	srand(timeGetTime());      //用系统时间初始化随机种子 

	// 物体的创建
	if(FAILED(D3DXCreateBox(g_pd3dDevice, 2, 2, 2, &g_cube, NULL)))	//立方体的创建
		return false;
	if(FAILED(D3DXCreateTeapot(g_pd3dDevice, &g_teapot, NULL)))		//茶壶的创建
		return false;	
	if(FAILED(D3DXCreateSphere(g_pd3dDevice, 1.5, 25, 25,					//球面体的创建
		&g_sphere, NULL))) return false;
	if(FAILED(D3DXCreateTorus(g_pd3dDevice, 0.5f, 1.2f, 25, 25,				//圆环体的创建
		&g_torus, NULL))) return false;

	// 设置渲染状态
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);   //关闭光照
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);  //设置线框填充模式

	return S_OK;
}


//-----------------------------------【Matrix_Set( )函数】--------------------------------------
//	描述：封装了Direct3D四大变换的函数，即世界变换，取景变换，投影变换，视口变换的设置
//--------------------------------------------------------------------------------------------------
VOID Matrix_Set()
{
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -15.0f);  //摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之三】：投影变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj; //定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //计算投影变换矩阵
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //设置投影变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之四】：视口变换的设置
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了
	vp.X      = 0;		//表示视口相对于窗口的X坐标
	vp.Y      = 0;		//视口相对对窗口的Y坐标
	vp.Width  = WINDOW_WIDTH;	//视口的宽度
	vp.Height = WINDOW_HEIGHT; //视口的高度
	vp.MinZ   = 0.0f; //视口在深度缓存中的最小深度值
	vp.MaxZ   = 1.0f;	//视口在深度缓存中的最大深度值
	g_pd3dDevice->SetViewport(&vp); //视口的设置

}

//-----------------------------------【Direct3D_Render( )函数】-------------------------------
//	描述：使用Direct3D进行渲染
//--------------------------------------------------------------------------------------------------
void Direct3D_Render(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之一】：清屏操作
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之二】：开始绘制
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->BeginScene();                     // 开始绘制

	Matrix_Set();//调用封装了四大变换的函数，对Direct3D世界变换，取景变换，投影变换，视口变换进行设置

	// 获取键盘消息并给予设置相应的填充模式
	if (::GetAsyncKeyState(0x31) & 0x8000f)         // 若数字键1被按下，进行实体填充
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);	
	if (::GetAsyncKeyState(0x32) & 0x8000f)         // 若数字键2被按下，进行线框填充
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之三】：正式绘制，利用顶点缓存绘制图形
	//--------------------------------------------------------------------------------------

	D3DXMatrixRotationY(&R, ::timeGetTime() / 1440.0f);  //设置公转的矩阵

	// 进行立方体的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[0], 3.0f, -3.0f, 0.0f);
	g_WorldMatrix[0]  =  g_WorldMatrix[0]*R; 
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[0]);
	g_cube->DrawSubset(0);

	//进行茶壶的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[1], -3.0f, -3.0f, 0.0f);
	g_WorldMatrix[1]  =  g_WorldMatrix[1]*R; 
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[1]);	
	g_teapot->DrawSubset(0);

	// 进行圆环的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[2], 3.0f, 3.0f, 0.0f);
	g_WorldMatrix[2]  =  g_WorldMatrix[2]*R; 
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[2]);	
	g_torus->DrawSubset(0);

	// 进行球面体的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[3], -3.0f, 3.0f, 0.0f);
	g_WorldMatrix[3]  =  g_WorldMatrix[3]*R; 
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[3]);
	g_sphere->DrawSubset(0);

	//在窗口右上角处，显示每秒帧数
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), Get_FPS() );
	g_pFont->DrawText(NULL, g_strFPS, charCount , &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255,39,136));

	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之四】：结束绘制
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->EndScene();                       // 结束绘制
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之五】：显示翻转
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
}

//-----------------------------------【Get_FPS( )函数】------------------------------------------
//	描述：用于计算每秒帧速率的一个函数
//--------------------------------------------------------------------------------------------------
float Get_FPS()
{
	//定义四个静态变量
	static float  fps = 0; //我们需要计算的FPS值
	static int     frameCount = 0;//帧数
	static float  currentTime =0.0f;//当前时间
	static float  lastTime = 0.0f;//持续时间

	frameCount++;//每调用一次Get_FPS()函数，帧数自增1
	currentTime = timeGetTime()*0.001f;//获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间，所以需要乘以0.001，得到单位为秒的时间

	//如果当前时间减去持续时间大于了1秒钟，就进行一次FPS的计算和持续时间的更新，并将帧数值清零
	if(currentTime - lastTime > 1.0f) //将时间控制在1秒钟
	{
		fps = (float)frameCount /(currentTime - lastTime);//计算这1秒钟的FPS值
		lastTime = currentTime; //将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
		frameCount    = 0;//将本次帧数frameCount值清零
	}

	return fps;
}

//-----------------------------------【Direct3D_CleanUp( )函数】--------------------------------
//	描述：资源清理函数，在此函数中进行程序退出前资源的清理工作
//---------------------------------------------------------------------------------------------------
void Direct3D_CleanUp()
{
	//释放COM接口对象
	SAFE_RELEASE(g_torus)
	SAFE_RELEASE(g_sphere)
	SAFE_RELEASE(g_cube)
	SAFE_RELEASE(g_teapot)
	SAFE_RELEASE(g_pFont)
	SAFE_RELEASE(g_pd3dDevice)
}


//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：D3Ddemo8
//	 2013年4月 Create by 浅墨
//  描述：游戏输入控制利器 : DirectInput 示例程序
//  图标素材： 《仙剑奇侠传五前传》 瑕
//------------------------------------------------------------------------------------------------


//-----------------------------------【宏定义部分】--------------------------------------------
// 描述：定义一些辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	800						//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	_T("【致我们永不熄灭的游戏开发梦想】游戏输入控制利器 : DirectInput") //为窗口标题定义的宏
#define DIRECTINPUT_VERSION 0x0800  //指定DirectInput版本，防止DIRECTINPUT_VERSION undefined警告
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }      //自定义一个SAFE_RELEASE()宏,便于COM资源的释放
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }				//指针的便捷释放宏


//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------                                                                                  
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <time.h> 
#include <dinput.h>                 // 使用DirectInput必须包含的头文件，注意这里没有8


//-----------------------------------【库文件包含部分】---------------------------------------
//	描述：包含程序所依赖的库文件
//------------------------------------------------------------------------------------------------
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dinput8.lib")     // 使用DirectInput必须包含的头文件，注意这里有8
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "winmm.lib") 


//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; //Direct3D设备对象
LPD3DXFONT					 g_pTextFPS				=NULL;    //字体COM接口
LPD3DXFONT					g_pTextAdaperName           = NULL;  // 显卡信息的2D文本
LPD3DXFONT					g_pTextHelper          = NULL;  // 帮助信息的2D文本
LPD3DXFONT					g_pTextInfor           = NULL;  // 绘制信息的2D文本
float								g_FPS								= 0.0f;       //一个浮点型的变量，代表帧速率
wchar_t							g_strFPS[50]={0};    //包含帧速率的字符数组
wchar_t							g_strAdapterName[60]={0};    //包含显卡名称的字符数组
LPDIRECTINPUT8				g_pDirectInput      = NULL; //
LPDIRECTINPUTDEVICE8    g_pMouseDevice      = NULL;
DIMOUSESTATE					g_diMouseState      = {0};
LPDIRECTINPUTDEVICE8    g_pKeyboardDevice   = NULL;
char									g_pKeyStateBuffer[256] = {0};
D3DXMATRIX					g_matWorld;   //世界矩阵
LPD3DXMESH					g_pMesh     = NULL; // 网格的对象
D3DMATERIAL9*				g_pMaterials    = NULL; // 网格的材质信息
LPDIRECT3DTEXTURE9*		g_pTextures     = NULL; // 网格的纹理信息
DWORD							g_dwNumMtrls    = 0;    // 材质的数目



//-----------------------------------【全局函数声明部分】-------------------------------------
//	描述：全局函数声明，防止“未声明的标识”系列错误
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
HRESULT				Direct3D_Init(HWND hwnd,HINSTANCE hInstance);
HRESULT				Objects_Init();
void						Direct3D_Render( HWND hwnd);
void						Direct3D_Update( HWND hwnd);
void						Direct3D_CleanUp( );
float						Get_FPS();
void						Matrix_Set();
BOOL					Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize) ;


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
	if (!(S_OK==Direct3D_Init (hwnd,hInstance)))
	{
		MessageBox(hwnd, _T("Direct3D初始化失败~！"), _T("浅墨的消息窗口"), 0); //使用MessageBox函数，创建一个消息窗口 
	}

	//【4】窗口创建四步曲之四：窗口的移动、显示与更新
	MoveWindow(hwnd,250,80,WINDOW_WIDTH,WINDOW_HEIGHT,true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow( hwnd, nShowCmd );    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	PlaySound(L"刻限.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

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
			Direct3D_Update(hwnd);			//调用更新函数，进行画面的更新
			Direct3D_Render(hwnd);			//调用渲染函数，进行画面的渲染	
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


//-----------------------------------【Direct3D_Init( )函数】----------------------------------
//	描述：Direct3D初始化函数，进行Direct3D的初始化
//------------------------------------------------------------------------------------------------
HRESULT Direct3D_Init(HWND hwnd,HINSTANCE hInstance)
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
	d3dpp.BackBufferCount            = 2;
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


	//获取显卡信息到g_strAdapterName中，并在显卡名称之前加上“当前显卡型号：”字符串
	 wchar_t TempName[60]=L"当前显卡型号：";   //定义一个临时字符串，且方便了把"当前显卡型号："字符串引入我们的目的字符串中
	 D3DADAPTER_IDENTIFIER9 Adapter;  //定义一个D3DADAPTER_IDENTIFIER9结构体，用于存储显卡信息
	 pD3D->GetAdapterIdentifier(0,0,&Adapter);//调用GetAdapterIdentifier，获取显卡信息
	 int len = MultiByteToWideChar(CP_ACP,0, Adapter.Description, -1, NULL, 0);//显卡名称现在已经在Adapter.Description中了，但是其为char类型，我们要将其转为wchar_t类型
	 MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, g_strAdapterName, len);//这步操作完成后，g_strAdapterName中就为当前我们的显卡类型名的wchar_t型字符串了
	 wcscat_s(TempName,g_strAdapterName);//把当前我们的显卡名加到“当前显卡型号：”字符串后面，结果存在TempName中
	 wcscpy_s(g_strAdapterName,TempName);//把TempName中的结果拷贝到全局变量g_strAdapterName中，大功告成~


	 //--------------------------------------------------------------------------------------
	 // 【DirectInput使用五步曲的前三步】：创设备，设格式，拿权力。在为鼠标设备初始化
	 //--------------------------------------------------------------------------------------
	// 创建DirectInput接口和设备
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pMouseDevice, NULL);

	// 设置数据格式和协作级别
	g_pDirectInput->CreateDevice(GUID_SysMouse, &g_pMouseDevice, NULL);
	g_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	g_pMouseDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//获取设备控制权
	g_pMouseDevice->Acquire();

	//--------------------------------------------------------------------------------------
	// 【DirectInput使用五步曲的前三步】：创设备，设格式，拿权力。在为键盘设备初始化
	//--------------------------------------------------------------------------------------
	// 创建DirectInput接口和设备
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pKeyboardDevice, NULL);

	// 设置数据格式和协作级别
	g_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	g_pKeyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//获取设备控制权
	g_pKeyboardDevice->Acquire();


	if(!(S_OK==Objects_Init())) return E_FAIL;

	SAFE_RELEASE(pD3D) //LPDIRECT3D9接口对象的使命完成，我们将其释放掉

	return S_OK;
}


//-----------------------------------【Object_Init( )函数】--------------------------------------
//	描述：渲染资源初始化函数，在此函数中进行要被渲染的物体的资源的初始化
//--------------------------------------------------------------------------------------------------
HRESULT Objects_Init()
{
	//创建字体
	D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS);
	D3DXCreateFont(g_pd3dDevice, 20, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"华文中宋", &g_pTextAdaperName); 
	D3DXCreateFont(g_pd3dDevice, 23, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"微软雅黑", &g_pTextHelper); 
	D3DXCreateFont(g_pd3dDevice, 26, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"黑体", &g_pTextInfor); 

	// 从X文件中加载网格数据
	LPD3DXBUFFER pAdjBuffer  = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(L"loli.x", D3DXMESH_MANAGED, g_pd3dDevice, 
		&pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh);

	// 读取材质和纹理数据
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	g_pMaterials = new D3DMATERIAL9[g_dwNumMtrls];
	g_pTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMtrls];

	for (DWORD i=0; i<g_dwNumMtrls; i++) 
	{
		g_pMaterials[i] = pMtrls[i].MatD3D;
		g_pMaterials[i].Ambient = g_pMaterials[i].Diffuse;
		g_pTextures[i]  = NULL;
		D3DXCreateTextureFromFileA(g_pd3dDevice, pMtrls[i].pTextureFilename, &g_pTextures[i]);
	}
	pAdjBuffer->Release();
	pMtrlBuffer->Release();

		// 设置渲染状态
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐
		g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //设置环境光
		
	return S_OK;
}


//-----------------------------------【Matrix_Set( )函数】--------------------------------------
//	描述：封装了Direct3D四大变换的函数，即世界变换，取景变换，投影变换，视口变换的设置
//--------------------------------------------------------------------------------------------------
void Matrix_Set()
{
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -250.0f);  //摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之三】：投影变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj; //定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f,(float)((double)WINDOW_WIDTH/WINDOW_HEIGHT),1.0f, 1000.0f); //计算投影变换矩阵
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

//-----------------------------------【Direct3D_Update( )函数】--------------------------------
//	描述：不是即时渲染代码但是需要即时调用的，如按键后的坐标的更改，都放在这里
//--------------------------------------------------------------------------------------------------
void	Direct3D_Update( HWND hwnd)
{
	// 获取键盘消息并给予设置相应的填充模式  
	if (g_pKeyStateBuffer[DIK_1]    & 0x80)         // 若数字键1被按下，进行实体填充  
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	if (g_pKeyStateBuffer[DIK_2]    & 0x80)         // 若数字键2被按下，进行线框填充  
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);  

	// 读取鼠标输入
	::ZeroMemory(&g_diMouseState, sizeof(g_diMouseState));
	Device_Read(g_pMouseDevice, (LPVOID)&g_diMouseState, sizeof(g_diMouseState));

	// 读取键盘输入
	::ZeroMemory(g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));
	Device_Read(g_pKeyboardDevice, (LPVOID)g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));


	// 按住鼠标左键并拖动，为平移操作
	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
	if (g_diMouseState.rgbButtons[0] & 0x80) 
	{
		fPosX += g_diMouseState.lX *  0.08f;
		fPosY += g_diMouseState.lY * -0.08f;
	}

	//鼠标滚轮，为观察点收缩操作
	fPosZ += g_diMouseState.lZ * 0.02f;

	// 平移物体
	if (g_pKeyStateBuffer[DIK_A] & 0x80) fPosX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_D] & 0x80) fPosX += 0.005f;
	if (g_pKeyStateBuffer[DIK_W] & 0x80) fPosY += 0.005f;
	if (g_pKeyStateBuffer[DIK_S] & 0x80) fPosY -= 0.005f;


	D3DXMatrixTranslation(&g_matWorld, fPosX, fPosY, fPosZ);


	// 按住鼠标右键并拖动，为旋转操作
	static float fAngleX = 0.15f, fAngleY = -(float)D3DX_PI ;
	if (g_diMouseState.rgbButtons[1] & 0x80) 
	{
		fAngleX += g_diMouseState.lY * -0.01f;
		fAngleY += g_diMouseState.lX * -0.01f;
	}
	// 旋转物体
	if (g_pKeyStateBuffer[DIK_UP]    & 0x80) fAngleX += 0.005f;
	if (g_pKeyStateBuffer[DIK_DOWN]  & 0x80) fAngleX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_LEFT]  & 0x80) fAngleY -= 0.005f;
	if (g_pKeyStateBuffer[DIK_RIGHT] & 0x80) fAngleY += 0.005f;


	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, fAngleX);
	D3DXMatrixRotationY(&Ry, fAngleY);

	//得到最终的矩阵并设置
	g_matWorld = Rx * Ry * g_matWorld;//得到最终的矩阵
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);//设置世界矩阵
	Matrix_Set();
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
	
			//--------------------------------------------------------------------------------------
			// 【Direct3D渲染五步曲之三】：正式绘制
			//--------------------------------------------------------------------------------------

			// 绘制网格
			for (DWORD i = 0; i < g_dwNumMtrls; i++)
			{
				g_pd3dDevice->SetMaterial(&g_pMaterials[i]);
				g_pd3dDevice->SetTexture(0, g_pTextures[i]);
				g_pMesh->DrawSubset(i);
			}

			//在窗口右上角处，显示每秒帧数
			int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), Get_FPS() );
			g_pTextFPS->DrawText(NULL, g_strFPS, charCount , &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0,239,136,255));

			//显示显卡类型名
			g_pTextAdaperName->DrawText(NULL,g_strAdapterName, -1, &formatRect, 
				DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));

			// 输出绘制信息
			 formatRect.top = 30;
			static wchar_t strInfo[256] = {0};
			swprintf_s(strInfo,-1, L"模型坐标: (%.2f, %.2f, %.2f)", g_matWorld._41, g_matWorld._42, g_matWorld._43);
			g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135,239,136,255));

			// 输出帮助信息
			formatRect.left = 0,formatRect.top = 380;
			g_pTextInfor->DrawText(NULL, L"控制说明:", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(235,123,230,255));
			formatRect.top += 35;
			g_pTextHelper->DrawText(NULL, L"    按住鼠标左键并拖动：平移模型", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    按住鼠标右键并拖动：旋转模型", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    滑动鼠标滚轮：拉伸模型", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    W、S、A、D键：平移模型 ", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    上、下、左、右方向键：旋转模型 ", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    ESC键 : 退出程序", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));

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
	static int    frameCount = 0;//帧数
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


//-----------------------------------【Device_Read( )函数】------------------------------------
// Desc: 智能读取设备的输入数据
//--------------------------------------------------------------------------------------------------
BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize) 
{
	HRESULT hr;
	while (true) 
	{
		pDIDevice->Poll();              // 轮询设备
		pDIDevice->Acquire();           // 获取设备的控制权
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
		if (FAILED(pDIDevice->Acquire())) return FALSE;
	}
	return TRUE;
}

//-----------------------------------【Direct3D_CleanUp( )函数】--------------------------------
//	描述：对Direct3D的资源进行清理，释放COM接口对象
//---------------------------------------------------------------------------------------------------
void Direct3D_CleanUp()
{
	//释放COM接口对象
	g_pMouseDevice->Unacquire();
	g_pKeyboardDevice->Unacquire();
	for (DWORD i = 0; i<g_dwNumMtrls; i++) 
		SAFE_RELEASE(g_pTextures[i]);
	SAFE_DELETE(g_pTextures)
	SAFE_DELETE(g_pMaterials)
	SAFE_RELEASE(g_pMesh)
	SAFE_RELEASE(g_pTextFPS)
	SAFE_RELEASE(g_pd3dDevice)	
	SAFE_RELEASE(g_pMouseDevice)
	SAFE_RELEASE(g_pKeyboardDevice)
}


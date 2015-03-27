
//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：GDIdemoCore
//	 2013年3月 Create by 浅墨
//  描述：GDI文字输出示例程序
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------
#include <windows.h>

//-----------------------------------【库文件包含部分】---------------------------------------
//	描述：包含程序所依赖的库文件
//------------------------------------------------------------------------------------------------
#pragma comment(lib,"winmm.lib")  //调用PlaySound函数所需库文件

//-----------------------------------【宏定义部分】--------------------------------------------
//	描述：定义一些辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	L"【致我们永不熄灭的游戏开发梦想】GDI文字输出演示程序"	//为窗口标题定义的宏

//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
HDC			g_hdc=NULL;       //全局设备环境句柄


//-----------------------------------【全局函数声明部分】-------------------------------------
//	描述：全局函数声明，防止“未声明的标识”系列错误
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );//窗口过程函数
BOOL						Game_Init(HWND hwnd);			//在此函数中进行资源的初始化
VOID							Game_Paint( HWND hwnd);		//在此函数中进行绘图代码的书写
BOOL						Game_CleanUp(HWND hwnd );	//在此函数中进行资源的清理

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
	wndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//用一个以空终止的字符串，指定窗口类的名字。

	//【2】窗口创建四步曲之二：注册窗口类
	if( !RegisterClassEx( &wndClass ) )				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;		

	//【3】窗口创建四步曲之三：正式创建窗口
	HWND hwnd = CreateWindow( L"ForTheDreamOfGameDevelop",WINDOW_TITLE,				//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL );

	//【4】窗口创建四步曲之四：窗口的移动、显示与更新
	MoveWindow(hwnd,250,80,WINDOW_WIDTH,WINDOW_HEIGHT,true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow( hwnd, nShowCmd );    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	//游戏资源的初始化，若初始化失败，弹出一个消息框，并返回FALSE
	if (!Game_Init (hwnd)) 
	{
		MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口
		return FALSE;
	}
	PlaySound(L"鸟之诗.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

	//【5】消息循环过程
	MSG msg = { 0 };				//定义并初始化msg
	while( msg.message != WM_QUIT )		//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage( &msg );		//将虚拟键消息转换为字符消息
			DispatchMessage( &msg );			//分发一个消息给窗口程序。
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
	PAINTSTRUCT paintStruct;			//定义一个PAINTSTRUCT结构体来记录一些绘制信息

	switch( message )						//switch语句开始
	{
	case WM_PAINT:						// 若是客户区重绘消息
		g_hdc = BeginPaint( hwnd, &paintStruct );  //指定窗口进行绘图工作的准备，并用将和绘图有关的信息填充到paintStruct结构体中。
		Game_Paint( hwnd);
		EndPaint( hwnd, &paintStruct );			//EndPaint函数标记指定窗口的绘画过程结束
		ValidateRect(hwnd, NULL);		// 更新客户区的显示
		break;									//跳出该switch语句

	case WM_KEYDOWN:					// 若是键盘按下消息
		if (wParam == VK_ESCAPE)    // 如果被按下的键是ESC
			DestroyWindow(hwnd);		// 销毁窗口, 并发送一条WM_DESTROY消息
		break;									//跳出该switch语句

	case WM_DESTROY:					//若是窗口销毁消息
		Game_CleanUp(hwnd);			//调用自定义的资源清理函数Game_CleanUp（）进行退出前的资源清理
		PostQuitMessage( 0 );			//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;									//跳出该switch语句

	default:										//若上述case条件都不符合，则执行该default语句
		return DefWindowProc( hwnd, message, wParam, lParam );		//调用缺省的窗口过程
	}

	return 0;									//正常退出
}

//-----------------------------------【Game_Init( )函数】--------------------------------------
//	描述：初始化函数，进行一些简单的初始化
//------------------------------------------------------------------------------------------------
BOOL Game_Init( HWND hwnd )
{
	g_hdc = GetDC(hwnd);  //获取设备环境句柄
	Game_Paint(hwnd);
	ReleaseDC(hwnd,g_hdc);  //释放设备环境
	return TRUE;
}

//-----------------------------------【Game_Paint( )函数】--------------------------------------
//	描述：绘制函数，在此函数中进行绘制操作
//--------------------------------------------------------------------------------------------------
VOID Game_Paint( HWND hwnd )
{
	HFONT hFont=CreateFont(30,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,L"微软雅黑");  //创建一种字体
	SelectObject(g_hdc,hFont);  //将字体选入设备环境中
	SetBkMode(g_hdc, TRANSPARENT);    //设置输出文字背景色为透明

	//定义三段文字
	wchar_t text1[]=L"我们所有的梦想都可以成真，只要我们有勇气去追求它们。";
	wchar_t text2[]=L"All our dreams can come true, if we have the courage to pursue them. ";
	wchar_t text3[]=L"--------沃尔特 迪斯尼";

	//设置文字颜色并输出第一段文字
	SetTextColor(g_hdc,RGB(50,255,50));
	TextOut(g_hdc,30,150,text1,wcslen(text1));
	//设置文字颜色并输出第二段文字
	SetTextColor(g_hdc,RGB(50,50,255));
	TextOut(g_hdc,30,200,text2,wcslen(text2));
	//设置文字颜色并输出第三段文字
	SetTextColor(g_hdc,RGB(255,150,50));
	TextOut(g_hdc,500,250,text3,wcslen(text3));

	DeleteObject(hFont);//释放字体对象
}

//-----------------------------------【Game_CleanUp( )函数】--------------------------------
//	描述：资源清理函数，在此函数中进行程序退出前资源的清理工作
//---------------------------------------------------------------------------------------------------
BOOL Game_CleanUp( HWND hwnd )
{
	return TRUE;
}
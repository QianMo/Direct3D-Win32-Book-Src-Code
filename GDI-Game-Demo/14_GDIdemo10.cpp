
//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：GDIdemo10
//	 2013年3月 Create by 浅墨
//  描述：Windows消息处理之键盘消息处理 示例程序
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>//使用swprintf_s函数所需的头文件

//-----------------------------------【库文件包含部分】---------------------------------------
//	描述：包含程序所依赖的库文件
//------------------------------------------------------------------------------------------------
#pragma comment(lib,"winmm.lib")			//调用PlaySound函数所需库文件
#pragma  comment(lib,"Msimg32.lib")		//添加使用TransparentBlt函数所需的库文件

//-----------------------------------【宏定义部分】--------------------------------------------
//	描述：定义一些辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE		L"【致我们永不熄灭的游戏开发梦想】Windows消息处理之 键盘消息处理 "	//为窗口标题定义的宏

//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
HDC				g_hdc=NULL,g_mdc=NULL,g_bufdc=NULL;      //全局设备环境句柄与两个全局内存DC句柄
HBITMAP		g_hSprite[4]={NULL},g_hBackGround=NULL;								//定义位图句柄数组用于存储四张方向图，以及定义存储背景图的句柄
DWORD		g_tPre=0,g_tNow=0;					//声明l两个函数来记录时间,g_tPre记录上一次绘图的时间，g_tNow记录此次准备绘图的时间
int					g_iNum=0,g_iX=0,g_iY=0;                //g_iNum用来记录图号，g_iX，g_iY分别表示贴图的横纵坐标
int					g_iDirection=0;//g_iDirection为人物移动方向，这里我们中以0，1，2，3代表人物上，下，左，右方向上的移动

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
	PlaySound(L"仙剑·温慧.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

	//【5】消息循环过程
	MSG msg = { 0 };				//定义并初始化msg
	while( msg.message != WM_QUIT )		//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage( &msg );		//将虚拟键消息转换为字符消息
			DispatchMessage( &msg );			//分发一个消息给窗口程序。
		}
		else
		{
			g_tNow = GetTickCount();   //获取当前系统时间
			if(g_tNow-g_tPre >= 50)        //当此次循环运行与上次绘图时间相差0.05秒时再进行重绘操作
				Game_Paint(hwnd);
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

	case WM_KEYDOWN:	     //按下键盘消息
		//判断按键的虚拟键码
		switch (wParam) 
		{
		case VK_ESCAPE:           //按下【Esc】键
			DestroyWindow(hwnd);    // 销毁窗口, 并发送一条WM_DESTROY消息
			PostQuitMessage( 0 );  //结束程序
			break;
		case VK_UP:				  //按下【↑】键
			//根据按键加入人物移动的量（每次按下一次按键移动10个单位），来决定人物贴图坐标的X与Y值，接着判断坐标是否超出窗口区域，若有则进行修正
			g_iY -= 10;
			g_iDirection = 0;
			if(g_iY < 0)
				g_iY = 0;
			break;
		case VK_DOWN:			  //按下【↓】键
			g_iY += 10;
			g_iDirection = 1;
			if(g_iY > WINDOW_HEIGHT-135)
				g_iY = WINDOW_HEIGHT-135;	
			break;
		case VK_LEFT:			  //按下【←】键			
			g_iX -= 10;
			g_iDirection = 2;
			if(g_iX < 0)
				g_iX = 0;		
			break;
		case VK_RIGHT:			   //按下【→】键
			g_iX += 10;
			g_iDirection = 3;
			if(g_iX > WINDOW_WIDTH-75)
				g_iX = WINDOW_WIDTH-75;
			break;
		}			
		break;								//跳出该switch语句

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
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);  
	g_mdc = CreateCompatibleDC(g_hdc);  //创建一个和hdc兼容的内存DC
	g_bufdc = CreateCompatibleDC(g_hdc);//再创建一个和hdc兼容的缓冲DC
	bmp = CreateCompatibleBitmap(g_hdc,WINDOW_WIDTH,WINDOW_HEIGHT);

	//设定人物贴图初始位置和移动方向
	g_iX = 150;
	g_iY = 350;
	g_iDirection = 3;
	g_iNum = 0;

	SelectObject(g_mdc,bmp);
	//加载各张跑动图及背景图，这里以0,1,2,3来代表人物的上，下，左，右移动
	g_hSprite[0] = (HBITMAP)LoadImage(NULL,L"go1.bmp",IMAGE_BITMAP,480,216,LR_LOADFROMFILE);
	g_hSprite[1] = (HBITMAP)LoadImage(NULL,L"go2.bmp",IMAGE_BITMAP,480,216,LR_LOADFROMFILE);
	g_hSprite[2] = (HBITMAP)LoadImage(NULL,L"go3.bmp",IMAGE_BITMAP,480,216,LR_LOADFROMFILE);
	g_hSprite[3] = (HBITMAP)LoadImage(NULL,L"go4.bmp",IMAGE_BITMAP,480,216,LR_LOADFROMFILE);
	g_hBackGround = (HBITMAP)LoadImage(NULL,L"bg.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);

	Game_Paint(hwnd);
	return TRUE;
}

//-----------------------------------【Game_Paint( )函数】--------------------------------------
//	描述：绘制函数，在此函数中进行绘制操作
//--------------------------------------------------------------------------------------------------
VOID Game_Paint( HWND hwnd )
{
	//先在mdc中贴上背景图
	SelectObject(g_bufdc,g_hBackGround);
	BitBlt(g_mdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_bufdc,0,0,SRCCOPY);

	//按照目前的移动方向取出对应人物的连续走动图，并确定截取人物图的宽度与高度
	SelectObject(g_bufdc,g_hSprite[g_iDirection]);
	BitBlt(g_mdc,g_iX,g_iY,60,108,g_bufdc,g_iNum*60,108,SRCAND);
	BitBlt(g_mdc,g_iX,g_iY,60,108,g_bufdc,g_iNum*60,0,SRCPAINT);
	//将最后的画面显示在窗口中
	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);

	g_tPre = GetTickCount();     //记录此次绘图时间
	g_iNum++;
	if(g_iNum == 8)
		g_iNum = 0;

}

//-----------------------------------【Game_CleanUp( )函数】--------------------------------
//	描述：资源清理函数，在此函数中进行程序退出前资源的清理工作
//---------------------------------------------------------------------------------------------------
BOOL Game_CleanUp( HWND hwnd )
{
	//释放资源对象
	DeleteObject(g_hBackGround);
	for (int i=0;i<4;i++)
	{
		DeleteObject(g_hSprite[i]);
	}
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd,g_hdc);
	return TRUE;
}
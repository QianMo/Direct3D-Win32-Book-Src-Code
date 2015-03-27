
//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：GDIdemo11
//	 2013年3月 Create by 浅墨
//  描述：Windows消息处理之 鼠标消息处理  示例程序
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
#define WINDOW_TITLE		L"【致我们永不熄灭的游戏开发梦想】Windows消息处理之 鼠标消息处理 "	//为窗口标题定义的宏

//-----------------------------------【全局结构体定义部分】-------------------------------------
//	描述：全局结构体定义
//------------------------------------------------------------------------------------------------
struct SwordBullets       //SwordBullets结构体代表剑气（子弹）
{
	int x,y;        //剑气（子弹）坐标
	bool exist;     //剑气（子弹）是否存在
};

//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
HDC				g_hdc=NULL,g_mdc=NULL,g_bufdc=NULL;      //全局设备环境句柄与全局内存DC句柄
HBITMAP		g_hSwordMan=NULL,g_hSwordBlade=NULL,g_hBackGround=NULL;		//定义位图句柄用于存储位图资源
DWORD		g_tPre=0,g_tNow=0;	  //声明l两个函数来记录时间,g_tPre记录上一次绘图的时间，g_tNow记录此次准备绘图的时间
int					g_iX=0,g_iY=0,g_iXnow=0,g_iYnow=0;    //g_iX,g_iY代表鼠标光标所在位置，g_iXnow，g_iYnow代表当前人物坐标，也就是贴图的位置
int					g_iBGOffset=0,g_iBulletNum=0;       //g_iBGOffset为滚动背景所要裁剪的区域宽度，g_iBulletNum记录剑侠现有剑气（子弹）数目
SwordBullets Bullet[30];           //声明一个“SwordBullets”类型的数组，用来存储剑侠发出的剑气（子弹）


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
	PlaySound(L"仙剑三·原版战斗3.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

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
			if(g_tNow-g_tPre >= 5)        //当此次循环运行与上次绘图时间相差0.1秒时再进行重绘操作
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
		}

		break;

	case WM_LBUTTONDOWN:			//单击鼠标左键消息
		for(int i=0;i<30;i++)
		{
			if(!Bullet[i].exist)
			{
				Bullet[i].x = g_iXnow;		//剑气（子弹）x坐标
				Bullet[i].y = g_iYnow + 30; //剑气（子弹）y坐标
				Bullet[i].exist = true;
				g_iBulletNum++;			//累加剑气（子弹）数目
				break;
			}
		}

	case WM_MOUSEMOVE:   //鼠标移动消息
		//对X坐标的处理
		g_iX = LOWORD(lParam);			//取得鼠标X坐标
		if(g_iX > WINDOW_WIDTH-317)	//设置临界坐标
			g_iX = WINDOW_WIDTH-317;
		else if(g_iX < 0)
			g_iX = 0;
		//对Y坐标的处理
		g_iY = HIWORD(lParam);			//取得鼠标Y坐标
		if(g_iY > WINDOW_HEIGHT-283)
			g_iY = WINDOW_HEIGHT-283;
		else if(g_iY < -200)
			g_iY = -200;
		break;

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

	//设定人物贴图初始值，鼠标位置初始值
	g_iX = 300;
	g_iY = 100;
	g_iXnow = 300;
	g_iYnow = 100;

	SelectObject(g_mdc,bmp);
	//加载各张跑动图及背景图
	g_hSwordMan = (HBITMAP)LoadImage(NULL,L"swordman.bmp",IMAGE_BITMAP,317,283,LR_LOADFROMFILE);
	g_hSwordBlade = (HBITMAP)LoadImage(NULL,L"swordblade.bmp",IMAGE_BITMAP,100,26,LR_LOADFROMFILE);
	g_hBackGround = (HBITMAP)LoadImage(NULL,L"bg.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);


	POINT pt,lt,rb;
	RECT rect;
	//设定光标位置
	pt.x = 300;
	pt.y = 100;
	ClientToScreen(hwnd,&pt);
	SetCursorPos(pt.x,pt.y);

	ShowCursor(false);		//隐藏鼠标光标

	//限制鼠标光标移动区域
	GetClientRect(hwnd,&rect);  //取得窗口内部矩形
	//将矩形左上点坐标存入lt中
	lt.x = rect.left;
	lt.y = rect.top;
	//将矩形右下坐标存入rb中
	rb.x = rect.right;
	rb.y = rect.bottom;
	//将lt和rb的窗口坐标转换为屏幕坐标
	ClientToScreen(hwnd,&lt);
	ClientToScreen(hwnd,&rb);
	//以屏幕坐标重新设定矩形区域
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	//限制鼠标光标移动区域
	ClipCursor(&rect);

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
	BitBlt(g_mdc,0,0,g_iBGOffset,WINDOW_HEIGHT,g_bufdc,WINDOW_WIDTH-g_iBGOffset,0,SRCCOPY);
	BitBlt(g_mdc,g_iBGOffset,0,WINDOW_WIDTH-g_iBGOffset,WINDOW_HEIGHT,g_bufdc,0,0,SRCCOPY);

	wchar_t str[20] = {};

	//计算剑侠的贴图坐标，设定每次进行剑侠贴图时，其贴图坐标（g_iXnow，g_iYnow）会以10个单位慢慢向鼠标光标所在的目的点（x，y）接近，直到两个坐标相同为止
	if(g_iXnow < g_iX)//若当前贴图X坐标小于鼠标光标的X坐标
	{
		g_iXnow += 10;
		if(g_iXnow > g_iX)
			g_iXnow = g_iX;
	}
	else   //若当前贴图X坐标大于鼠标光标的X坐标
	{
		g_iXnow -=10;
		if(g_iXnow < g_iX)
			g_iXnow = g_iX;
	}

	if(g_iYnow < g_iY)  //若当前贴图Y坐标小于鼠标光标的Y坐标
	{
		g_iYnow += 10;
		if(g_iYnow > g_iY)
			g_iYnow = g_iY;
	}
	else  //若当前贴图Y坐标大于于鼠标光标的Y坐标
	{
		g_iYnow -= 10;  
		if(g_iYnow < g_iY)
			g_iYnow = g_iY;
	}

	//贴上剑侠图
	SelectObject(g_bufdc,g_hSwordMan);
	TransparentBlt(g_mdc,g_iXnow,g_iYnow,317,283,g_bufdc,0,0,317,283,RGB(0,0,0));

	//剑气（子弹）的贴图，先判断剑气（子弹）数目“g_iBulletNum”的值是否为“0”。若不为0，则对剑气（子弹）数组中各个还存在的剑气（子弹）按照其所在的坐标（b[i].x,b[i].y）循环进行贴图操作
	SelectObject(g_bufdc,g_hSwordBlade);
	if(g_iBulletNum!=0)
		for(int i=0;i<30;i++)
			if(Bullet[i].exist)
			{
				//贴上剑气（子弹）图
				TransparentBlt(g_mdc,Bullet[i].x-70,Bullet[i].y+100,100,33,g_bufdc,0,0,100,26,RGB(0,0,0));

				//设置下一个剑气（子弹）的坐标。剑气（子弹）是从右向左发射的，因此，每次其X轴上的坐标值递减10个单位，这样贴图会产生往左移动的效果。而如果剑气（子弹）下次的坐标已超出窗口的可见范围（h[i].x<0）,那么剑气（子弹）设为不存在，并将剑气（子弹）总数g_iBulletNum变量值减1.
				Bullet[i].x -= 10;
				if(Bullet[i].x < 0)
				{
					g_iBulletNum--;
					Bullet[i].exist = false;
				}
			}

			HFONT hFont;  
			hFont=CreateFont(20,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("微软雅黑"));  //创建字体
			SelectObject(g_mdc,hFont);  //选入字体到g_mdc中
			SetBkMode(g_mdc, TRANSPARENT);    //设置文字背景透明
			SetTextColor(g_mdc,RGB(255,255,0));  //设置文字颜色

			//在左上角进行文字输出
			swprintf_s(str,L"鼠标X坐标为%d    ",g_iX);
			TextOut(g_mdc,0,0,str,wcslen(str));
			swprintf_s(str,L"鼠标Y坐标为%d    ",g_iY);
			TextOut(g_mdc,0,20,str,wcslen(str));

			//贴上背景图
			BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);

			g_tPre = GetTickCount();

			g_iBGOffset += 5; //让背景滚动量+5
			if(g_iBGOffset==WINDOW_WIDTH)//如果背景滚动量达到了背景宽度值，就置零
				g_iBGOffset = 0;  
}

//-----------------------------------【Game_CleanUp( )函数】--------------------------------
//	描述：资源清理函数，在此函数中进行程序退出前资源的清理工作
//---------------------------------------------------------------------------------------------------
BOOL Game_CleanUp( HWND hwnd )
{
	//释放资源对象
	DeleteObject(g_hBackGround);
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd,g_hdc);
	return TRUE;
}


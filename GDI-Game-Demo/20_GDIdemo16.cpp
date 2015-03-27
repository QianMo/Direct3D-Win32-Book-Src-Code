
//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：GDIdemo16
//	 2013年3月 Create by 浅墨
//  描述：粒子系统初步之 星光绽放 示例程序
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>//使用swprintf_s函数所需的头文件
#include  <time.h> //使用获取系统时间time函数需要包含的头文件

//-----------------------------------【库文件包含部分】---------------------------------------
//	描述：包含程序所依赖的库文件
//------------------------------------------------------------------------------------------------
#pragma comment(lib,"winmm.lib")			//调用PlaySound函数所需库文件
#pragma  comment(lib,"Msimg32.lib")		//添加使用TransparentBlt函数所需的库文件

//-----------------------------------【宏定义部分】--------------------------------------------
//	描述：定义一些辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	932							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	700							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE		L"【致我们永不熄灭的游戏开发梦想】粒子系统初步之 星光绽放demo"	//为窗口标题定义的宏
#define FLYSTAR_NUMBER	100							//表示粒子数量的宏，以方便修改粒子数量
#define FLYSTAR_LASTED_FRAME 60                	//表示粒子持续帧数的宏，以方便修改每次星光绽放持续的时间           


//-----------------------------------【全局结构体定义部分】-------------------------------------
//	描述：全局结构体定义
//------------------------------------------------------------------------------------------------
struct FLYSTAR
{
	int x;       //星光所在的x坐标
	int y;       //星光所在的y坐标
	int vx;      //星光x方向的速度
	int vy;      //星光y方向的速度
	int lasted;  //星光存在的时间
	BOOL exist;  //星光是否存在
};


//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
HDC				g_hdc=NULL,g_mdc=NULL,g_bufdc=NULL;      //全局设备环境句柄与全局内存DC句柄
HBITMAP		g_hStar=NULL,g_hBackGround=NULL;  		 //定义位图句柄用于存储位图资源
DWORD		g_tPre=0,g_tNow=0;						//声明l两个函数来记录时间,g_tPre记录上一次绘图的时间，g_tNow记录此次准备绘图的时间
RECT				g_rect;				//定义一个RECT结构体，用于储存内部窗口区域的坐标
FLYSTAR		FlyStars[FLYSTAR_NUMBER];  //粒子数组
int					g_StarNum=0; //定义g_StarNum用于计数

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
	MoveWindow(hwnd,150,20,WINDOW_WIDTH,WINDOW_HEIGHT,true);		//调整窗口显示时的位置，使窗口左上角位于（150,20）处
	ShowWindow( hwnd, nShowCmd );    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	//游戏资源的初始化，若初始化失败，弹出一个消息框，并返回FALSE
	if (!Game_Init (hwnd)) 
	{
		MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口
		return FALSE;
	}
	PlaySound(L"君がいるから (Long Version).wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

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
			if(g_tNow-g_tPre >= 30)        //当此次循环运行与上次绘图时间相差0.03秒时再进行重绘操作
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
	case WM_KEYDOWN:					//按键消息
		if(wParam==VK_ESCAPE)		//按下【Esc】键
			PostQuitMessage(0);
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

	srand((unsigned)time(NULL));      //用系统时间初始化随机种子 
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);  
	g_mdc = CreateCompatibleDC(g_hdc);  //创建一个和hdc兼容的内存DC
	g_bufdc = CreateCompatibleDC(g_hdc);//再创建一个和hdc兼容的缓冲DC
	bmp = CreateCompatibleBitmap(g_hdc,WINDOW_WIDTH,WINDOW_HEIGHT); //建一个和窗口兼容的空的位图对象

	SelectObject(g_mdc,bmp);//将空位图对象放到g_mdc中

	//载入位图背景
	g_hBackGround = (HBITMAP)LoadImage(NULL,L"bg.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);
	g_hStar = (HBITMAP)LoadImage(NULL,L"star.bmp",IMAGE_BITMAP,30,30,LR_LOADFROMFILE);

	GetClientRect(hwnd,&g_rect);		//取得内部窗口区域的大小

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

	//创建粒子
	if(g_StarNum == 0)              //随机设置爆炸点
	{
		int	x=rand()%g_rect.right;
		int	y=rand()%g_rect.bottom;

		for(int i=0;i<FLYSTAR_NUMBER;i++)       //产生星光粒子
		{
			FlyStars[i].x = x;
			FlyStars[i].y = y;
			FlyStars[i].lasted = 0;  //设定该粒子存在的时间为零
			 //按粒子编号i来决定粒子在哪个象限运动，且x，y方向的移动速度随机为1—15之间的一个值，由1+rand()%15来完成。
			if(i%4==0)      
			{
				FlyStars[i].vx =  -(1+rand()%15);
				FlyStars[i].vy =  -(1+rand()%15);
			}
			if(i%4==1)
			{
				FlyStars[i].vx = 1+rand()%15;
				FlyStars[i].vy = 1+rand()%15;
			}
			if(i%4==2)
			{
				FlyStars[i].vx = -(1+rand()%15);
				FlyStars[i].vy = 1+rand()%15;
			}
			if(i%4==3)
			{
				FlyStars[i].vx = 1+rand()%15;
				FlyStars[i].vy = -(1+rand()%15);
			}
			FlyStars[i].exist = true;  //设定粒子存在
		}
		g_StarNum = FLYSTAR_NUMBER;   //全部粒子由for循环设置完成后，我们将粒子数量设为FLYSTAR_NUMBER，代表目前有FLYSTAR_NUMBER颗星光
	}


	//显示粒子并更新下一帧的粒子坐标
	for(int i=0;i<FLYSTAR_NUMBER;i++)
	{
		if(FlyStars[i].exist)   //判断粒子是否还存在,若存在，则根据其坐标（FlyStars[i].x,FlyStars[i].y）进行贴图操作
		{
			SelectObject(g_bufdc,g_hStar);
			TransparentBlt(g_mdc,FlyStars[i].x,FlyStars[i].y,30,30,g_bufdc,0,0,30,30,RGB(0,0,0));

			//计算下一次贴图的坐标
			FlyStars[i].x+=FlyStars[i].vx;
			FlyStars[i].y+=FlyStars[i].vy;

			//在每进行一次贴图后，将粒子的存在时间累加1.
			FlyStars[i].lasted++;
			//进行条件判断，若某粒子跑出窗口区域一定的范围，则将该粒子设为不存在，且粒子数随之递减
			if(FlyStars[i].x<=-10 || FlyStars[i].x>g_rect.right || FlyStars[i].y<=-10 || FlyStars[i].y>g_rect.bottom || FlyStars[i].lasted>FLYSTAR_LASTED_FRAME)
			{
				FlyStars[i].exist = false;  //删除星光粒子 
				g_StarNum--;                    //递减星光总数
			}
		}
	}

	//将mdc中的全部内容贴到hdc中
	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);

	g_tPre = GetTickCount();     //记录此次绘图时间
}

//-----------------------------------【Game_CleanUp( )函数】--------------------------------
//	描述：资源清理函数，在此函数中进行程序退出前资源的清理工作
//---------------------------------------------------------------------------------------------------
BOOL Game_CleanUp( HWND hwnd )
{
	//释放资源对象
	DeleteObject(g_hBackGround);
	DeleteObject(g_hStar);
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd,g_hdc);
	return TRUE;
}
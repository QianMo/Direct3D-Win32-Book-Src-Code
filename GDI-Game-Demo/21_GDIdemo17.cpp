
//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：GDIdemo17
//	 2013年3月 Create by 浅墨
//  描述：小型回合制游戏：《勇者斗恶龙》
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>//使用swprintf_s函数所需的头文件
#include  <time.h> //使用获取系统时间time()函数需要包含的头文件

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
#define WINDOW_TITLE		L"【致我们永不熄灭的游戏开发梦想】勇者斗恶龙"	//为窗口标题定义的宏
#define PARTICLE_NUMBER	50							//表示粒子数量的宏，以方便修改粒子数量


//-----------------------------------【全局结构体与枚举体定义部分】-------------------------
//	描述：全局结构体与枚举的定义
//------------------------------------------------------------------------------------------------
//定义一个角色结构体
struct CHARACTER  
{
	int		NowHp;		//当前生命值
	int		MaxHp;			//最大生命值
	int		NowMp;		//当前魔法值
	int		MaxMp;		//最大魔法值
	int		Level;			//角色等级
	int		Strength;		//角色力量值
	int		Intelligence; //角色智力值
	int		Agility;			//角色敏捷值
};

struct SNOW
{
	int x; //雪花的 X坐标 
	int y; //雪花的Y坐标
	BOOL exist;  //雪花是否存在
};


//定义一个动作枚举体
enum ActionTypes
{
	ACTION_TYPE_NORMAL=0,		//普通攻击
	ACTION_TYPE_CRITICAL=1,		//致命一击
	ACTION_TYPE_MAGIC=2,		//魔法攻击
	ACTION_TYPE_MISS=3,			//攻击落空	
	ACTION_TYPE_RECOVER=4,	//释放恢复魔法 
};

//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------
HDC				g_hdc=NULL,g_mdc=NULL,g_bufdc=NULL;      //全局设备环境句柄与全局内存DC句柄
DWORD		g_tPre=0,g_tNow=0;					//声明l两个函数来记录时间,g_tPre记录上一次绘图的时间，g_tNow记录此次准备绘图的时间
RECT				g_rect;				//定义一个RECT结构体，用于储存内部窗口区域的坐标
int					g_iFrameNum,g_iTxtNum;  //帧数和文字数量
wchar_t			text[8][100];  //存储着输出文字的字符串数组
BOOL			g_bCanAttack,g_bGameOver;   //两个游戏状态标识，角色是否可以攻击，游戏是否结束
SNOW			SnowFlowers[PARTICLE_NUMBER];   //雪花粒子数组
int					g_SnowNum=0; //定义g_SnowNum用于雪花的计数
CHARACTER	Hero,Boss;  //两个角色结构体实例，分别表示英雄和BOSS
ActionTypes	HeroActionType,BossActionType;  //两个动作类型枚举实例，分别表示英雄和BOSS的动作类型
//一系列位图句柄的定义
HBITMAP		g_hBackGround,g_hGameOver,g_hVictory,g_hSnow;  //背景，游戏结束，游戏胜利的位图句柄
HBITMAP		g_hMonsterBitmap,g_hHeroBitmap,g_hRecoverSkill;  //BOSS角色图，英雄角色图，恢复技能图的位图句柄
HBITMAP		g_hSkillButton1,g_hSkillButton2,g_hSkillButton3,g_hSkillButton4;  //4个技能按钮的位图句柄
HBITMAP		g_hHeroSkill1,g_hHeroSkill2,g_hHeroSkill3;  //3个英雄技能效果的位图句柄
HBITMAP		g_hBossSkill1,g_hBossSkill2,g_hBossSkill3;  //3个BOSS技能效果的位图句柄


//-----------------------------------【全局函数声明部分】-------------------------------------
//	描述：全局函数声明，防止“未声明的标识”系列错误
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );//窗口过程函数
BOOL						Game_Init(HWND hwnd);			//在此函数中进行资源的初始化
VOID							Game_Main( HWND hwnd);		//在此函数中进行绘图代码的书写
BOOL						Game_ShutDown(HWND hwnd );	//在此函数中进行资源的清理
VOID							Die_Check(int NowHp,bool isHero);   //死亡检查函数
VOID							Message_Insert(wchar_t* str);  //文字消息处理函数
VOID							HeroAction_Logic();  //英雄动作逻辑判断函数
VOID							HeroAction_Paint();  //英雄动作绘图函数
VOID							BossAction_Logic(); //怪物动作逻辑判断函数
VOID							BossAction_Paint(); //怪物动作绘图函数
VOID							Snow_Paint();
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
	PlaySound(L"GameMedia\\梦幻西游原声-战斗1-森林.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

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
			if(g_tNow-g_tPre >= 60)        //当此次循环运行与上次绘图时间相差0.06秒时再进行重绘操作
				Game_Main(hwnd);
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

	case WM_LBUTTONDOWN:			//鼠标左键消息
		if(!g_bCanAttack)
		{
			int x = LOWORD(lParam);		//X坐标
			int y = HIWORD(lParam);		//Y坐标

			if(x >= 530 && x <= 570 && y >=420 && y <= 470)
			{	g_bCanAttack = true;
				HeroActionType=ACTION_TYPE_NORMAL;
			}
			if(x >= 590 && x <= 640 && y >=420 && y <= 470)
			{
				g_bCanAttack = true;
				HeroActionType=ACTION_TYPE_MAGIC;
			}
			if(x >= 650 && x <= 700 && y >=420 && y <= 470)
			{
				g_bCanAttack = true;
				HeroActionType=ACTION_TYPE_RECOVER;
			}
			
		}
		break;
	case WM_DESTROY:					//若是窗口销毁消息
		Game_ShutDown(hwnd);			//调用自定义的资源清理函数Game_ShutDown（）进行退出前的资源清理
		PostQuitMessage( 0 );			//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;									//跳出该switch语句

	default:										//若上述case条件都不符合，则执行该default语句
		return DefWindowProc( hwnd, message, wParam, lParam );		//调用缺省的窗口过程
	}

	return 0;									//正常退出
}



//-----------------------------------【Game_Init( )函数】--------------------------------------
//	描述：初始化函数，进行游戏资源的初始化
//------------------------------------------------------------------------------------------------
BOOL Game_Init( HWND hwnd )
{
	srand((unsigned)time(NULL));      //用系统时间初始化随机种子 

	HBITMAP bmp;

	//三缓冲体系的创建
	g_hdc = GetDC(hwnd);  
	g_mdc = CreateCompatibleDC(g_hdc);  //创建一个和hdc兼容的内存DC
	g_bufdc = CreateCompatibleDC(g_hdc);//再创建一个和hdc兼容的缓冲DC
	bmp = CreateCompatibleBitmap(g_hdc,WINDOW_WIDTH,WINDOW_HEIGHT); //建一个和窗口兼容的空的位图对象

	SelectObject(g_mdc,bmp);//将空位图对象放到mdc中

	//载入一系列游戏资源图到位图句柄中
	g_hGameOver=(HBITMAP)LoadImage(NULL,L"GameMedia\\gameover.bmp",IMAGE_BITMAP,1086,396,LR_LOADFROMFILE);  //游戏结束位图
	g_hVictory=(HBITMAP)LoadImage(NULL,L"GameMedia\\victory.bmp",IMAGE_BITMAP,800,600,LR_LOADFROMFILE);  //游戏胜利位图
	g_hBackGround = (HBITMAP)LoadImage(NULL,L"GameMedia\\bg.bmp",IMAGE_BITMAP,800,600,LR_LOADFROMFILE);  //背景位图
	g_hMonsterBitmap = (HBITMAP)LoadImage(NULL,L"GameMedia\\monster.bmp",IMAGE_BITMAP,360,360,LR_LOADFROMFILE);  //怪物角色位图
	g_hHeroBitmap = (HBITMAP)LoadImage(NULL,L"GameMedia\\hero.bmp",IMAGE_BITMAP,360,360,LR_LOADFROMFILE);  //英雄角色位图
	g_hHeroSkill1=(HBITMAP)LoadImage(NULL,L"GameMedia\\heroslash.bmp",IMAGE_BITMAP,364,140,LR_LOADFROMFILE);  //英雄1技能位图
	g_hHeroSkill2=(HBITMAP)LoadImage(NULL,L"GameMedia\\heromagic.bmp",IMAGE_BITMAP,374,288,LR_LOADFROMFILE);  //英雄2技能位图
	g_hHeroSkill3=(HBITMAP)LoadImage(NULL,L"GameMedia\\herocritical.bmp",IMAGE_BITMAP,574,306,LR_LOADFROMFILE);  //英雄3技能位图
	g_hSkillButton1 = (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton1.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);  //技能1图标位图
	g_hSkillButton2= (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton2.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);   //技能2图标位图
	g_hSkillButton3= (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton3.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);   //技能3图标位图
	g_hSkillButton4= (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton4.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);   //技能4图标位图
	g_hBossSkill1= (HBITMAP)LoadImage(NULL,L"GameMedia\\monsterslash.bmp",IMAGE_BITMAP,234,188,LR_LOADFROMFILE);   //怪物1技能位图
	g_hBossSkill2= (HBITMAP)LoadImage(NULL,L"GameMedia\\monstermagic.bmp",IMAGE_BITMAP,387,254,LR_LOADFROMFILE);  //怪物2技能位图
	g_hBossSkill3= (HBITMAP)LoadImage(NULL,L"GameMedia\\monstercritical.bmp",IMAGE_BITMAP,574,306,LR_LOADFROMFILE); //怪物3技能位图
	g_hSnow = (HBITMAP)LoadImage(NULL,L"GameMedia\\snow.bmp",IMAGE_BITMAP,30,30,LR_LOADFROMFILE);   //雪花位图
	g_hRecoverSkill = (HBITMAP)LoadImage(NULL,L"GameMedia\\recover.bmp",IMAGE_BITMAP,150,150,LR_LOADFROMFILE);    //恢复技能位图

	GetClientRect(hwnd,&g_rect);		//取得内部窗口区域的大小

	//设定玩家圣殿骑士的各项属性值
	Hero.NowHp = Hero.MaxHp = 1000;	//设定玩家生命值的当前值及上限
	Hero.Level = 6;					//设定玩家角色等级
	Hero.NowMp=Hero.MaxMp=60;    //玩家魔法值的当前值及上限
	Hero.Strength  = 10;			//设定英雄的力量值
	Hero.Agility=20;				//设定英雄的敏捷值   
	Hero.Intelligence=10;		//设定英雄的智力值

	//设定BOSS黄金魔龙君的各项属性值
	Boss.NowHp = Boss.MaxHp = 2000;	//设定BOSS生命值的当前值及上限
	Boss.Level = 10;						//设定BOSS的等级
	Boss.Strength = 10;				//设定BOSS的力量值
	Boss.Agility=10;                   //设定BOSS的敏捷值
	Boss.Intelligence=10;			//设定BOSS的智力值

	g_iTxtNum = 0;		//初始化显示消息数目

	//设置字体
	HFONT hFont;
	hFont=CreateFont(20,0,0,0,700,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("微软雅黑"));
	SelectObject(g_mdc,hFont);
	SetBkMode(g_mdc, TRANSPARENT);    //设置文字显示背景透明

	Game_Main(hwnd);  //调用一次游戏Main函数
	return TRUE;
}

//-----------------------------------【Game_Main( )函数】--------------------------------------
//	描述：游戏Main函数，游戏的心脏，处理游戏大大小小函数的合理调用与运作
//--------------------------------------------------------------------------------------------------
VOID Game_Main( HWND hwnd )
{
	wchar_t str[100];

	//先在mdc中贴上背景图
	SelectObject(g_bufdc,g_hBackGround);
	BitBlt(g_mdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_bufdc,0,0,SRCCOPY);

	//下雪效果的绘制
	if (!g_bGameOver)//如果游戏未结束
	{
		Snow_Paint(); //调用封装了雪花飞舞粒子效果的函数，模拟下雪场景
	}

	//显示对战消息
	SetTextColor(g_mdc,RGB(255,255,255));
	for(int i=0;i<g_iTxtNum;i++)
		TextOut(g_mdc,20,410+i*18,text[i],wcslen(text[i]));

	//贴上怪物图以及显示怪物血量
	if(Boss.NowHp>0)
	{
		//贴上怪物图
		SelectObject(g_bufdc,g_hMonsterBitmap);
		TransparentBlt(g_mdc,0,50,360,360,g_bufdc,0,0,360,360,RGB(0,0,0));//采用TransparentBlt透明贴图函数
		//显示怪物血量
		swprintf_s(str,L"%d / %d",Boss.NowHp,Boss.MaxHp);
		SetTextColor(g_mdc,RGB(255,10,10));
		TextOut(g_mdc,100,370,str,wcslen(str));
	}

	//贴上玩家图以及玩家生命值，魔法值的显示
	if(Hero.NowHp>0)
	{
		//贴上人物图
		SelectObject(g_bufdc,g_hHeroBitmap);
		TransparentBlt(g_mdc,400,50,360,360,g_bufdc,0,0,360,360,RGB(0,0,0));//透明色为RGB(0,0,0)纯黑色
		//显示血量
		swprintf_s(str,L"%d / %d",Hero.NowHp,Hero.MaxHp);
		SetTextColor(g_mdc,RGB(255,10,10));
		TextOut(g_mdc,600,350,str, wcslen(str));
		//显示魔法值
		swprintf_s(str,L"%d / %d",Hero.NowMp,Hero.MaxMp);
		SetTextColor(g_mdc,RGB(10,10,255));
		TextOut(g_mdc,600,370,str, wcslen(str));
	}

	//判断游戏是否结束
	if(g_bGameOver)				
	{
		if(Hero.NowHp <= 0)  //玩家阵亡，就贴出“GameOver”图片
		{
			SelectObject(g_bufdc,g_hGameOver);
			BitBlt(g_mdc,120,50,543,396,g_bufdc,543,0,SRCAND);
			BitBlt(g_mdc,120,50,543,396,g_bufdc,0,0,SRCPAINT);
		}

		else  //怪物阵亡，就贴出“胜利”图片
		{
			SelectObject(g_bufdc,g_hVictory);
			TransparentBlt(g_mdc,0,0,800,600,g_bufdc,0,0,800,600,RGB(0,0,0));//透明色为RGB(0,0,0)
		}
	}
	//如果游戏没结束，且为玩家可攻击状态，便依次贴上技能图标
	else if(!g_bCanAttack)		//贴上技能按钮
	{
		SelectObject(g_bufdc,g_hSkillButton1);
		BitBlt(g_mdc,530,420,50,50,g_bufdc,0,0,SRCCOPY);
		SelectObject(g_bufdc,g_hSkillButton2);
		BitBlt(g_mdc,590,420,50,50,g_bufdc,0,0,SRCCOPY);
		SelectObject(g_bufdc,g_hSkillButton3);
		BitBlt(g_mdc,650,420,50,50,g_bufdc,0,0,SRCCOPY);
		SelectObject(g_bufdc,g_hSkillButton4);
		BitBlt(g_mdc,710,420,50,50,g_bufdc,0,0,SRCCOPY);
	}	
	//游戏没结束，且为玩家不可攻击状态，便开始接下来的绘图
	else
	{
		g_iFrameNum++;

		//第5~10个画面时显示玩家攻击效果图
		if(g_iFrameNum>=5 && g_iFrameNum<=10)
		{
			//第5个画面时根据之前的输入计算出游戏逻辑并进行消息显示
			if(g_iFrameNum == 5)
			{			
				HeroAction_Logic( );
				Die_Check(Boss.NowHp,false);
			}	
			//根据人物的动作类型进行相应贴图
			HeroAction_Paint();
		}

		//第15个画面时判断怪物进行哪项动作
		if(g_iFrameNum == 15)
		{
			BossAction_Logic();
		}

		//第26~30个画面时显示怪物攻击图标
		if(g_iFrameNum>=26  && g_iFrameNum<=30)
		{
			BossAction_Paint();
		}

		if(g_iFrameNum == 30)			//回合结束
		{
			g_bCanAttack = false;
			g_iFrameNum = 0;

			//每回合的魔法自然恢复，6点固定值加上0到智力值之间的一个随机值的三倍
			if(!g_bGameOver)
			{
				int MpRecover=2*(rand()%Hero.Intelligence)+6;
				Hero.NowMp+=MpRecover;

				if (Hero.NowMp>=Hero.MaxMp)
				{
					Hero.NowMp=Hero.MaxMp;
				}
			
			swprintf_s(str,L"回合结束，自动恢复了【%d】点魔法值",MpRecover);
			Message_Insert(str);
			}
		}
	}

	//将mdc中的全部内容贴到hdc中
	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);
	g_tPre = GetTickCount();     //记录此次绘图时间
}


//-----------------------------------【Message_Insert( )函数】----------------------------------
//	描述：对战消息函数，用于插入文字消息
//---------------------------------------------------------------------------------------------------
void Message_Insert(wchar_t* str)
{
	//如果未满8行消息，直接新开一条消息
	if(g_iTxtNum < 8)
	{
		swprintf_s(text[g_iTxtNum],str);
		g_iTxtNum++;
	}
	//如果满了8行消息，只有挤走最上面的一条消息
	else
	{
		for(int i=0;i<g_iTxtNum;i++)
			swprintf_s(text[i],text[i+1]);
			swprintf_s(text[7],str);
	}
}


//-----------------------------------【Die_Check( )函数】----------------------------------------
//	描述：生命值判断函数，检查角色是否死亡，第一个参数填某角色当前生命值
//--------------------------------------------------------------------------------------------------
void Die_Check(int NowHp,bool isHero)
{
	wchar_t str[100];
	
	if(NowHp <= 0)//生命值小于等于0了
	{
		g_bGameOver = true;
		if(isHero)  //如果是英雄的生命值小于零，表示游戏失败
		{
			PlaySound(L"GameMedia\\failure.wav", NULL, SND_FILENAME | SND_ASYNC); //播放失败音乐 
			swprintf_s(str,L": ( 胜败乃兵家常事，大侠请重新来过......");  //输出失败提示语
			Message_Insert(str);  //插入到文字消息中
		}
		else
		{
			PlaySound(L"GameMedia\\victory.wav", NULL, SND_FILENAME | SND_ASYNC); //播放胜利音乐 
			swprintf_s(str,L"少年，你赢了，有两下子啊~~~~~！！！！");  //输出胜利提示语
			Message_Insert(str);  //插入到文字消息中
		}
	}
}

//-----------------------------------【HeroAction_Logic( )函数】------------------------------
//	描述：玩家动作逻辑判断函数
//--------------------------------------------------------------------------------------------------
VOID		HeroAction_Logic( )
{
		int damage=0;
		wchar_t str[100];

		switch( HeroActionType)
		{
			case ACTION_TYPE_NORMAL:

				if (1==rand()%4)                   // 20%几率触发幻影刺客的大招，恩赐解脱，4倍暴击伤害
				{
					HeroActionType=ACTION_TYPE_CRITICAL;
					damage = (int)(4.5f*(float)(3*(rand()%Hero.Agility) + Hero.Level*Hero.Strength+20));
					Boss.NowHp -= (int)damage;

					swprintf_s(str,L"恩赐解脱触发，这下牛逼了，4.5倍暴击...对怪物照成了【%d】点伤害",damage);
				} 
				else       //没有触发”恩赐解脱“，还是用普通攻击”无敌斩“
				{
					damage = 3*(rand()%Hero.Agility) + Hero.Level*Hero.Strength+20;
					Boss.NowHp -= (int)damage;

					swprintf_s(str,L"玩家圣殿骑士使用了普通攻击“无敌斩”，伤害一般般...对怪物照成了【%d】点伤害",damage);
				}

				Message_Insert(str);
				break;

			case ACTION_TYPE_MAGIC:  //释放烈火剑法
				if(Hero.NowMp>=30)
				{
					damage = 5*(2*(rand()%Hero.Agility) + Hero.Level*Hero.Intelligence);
					Boss.NowHp -= (int)damage;
					Hero.NowMp-=30;
					swprintf_s(str,L"玩家圣殿骑士释放烈火剑法...对怪物照成了【%d】点伤害",damage);
				}
				else
				{
					HeroActionType=ACTION_TYPE_MISS;
					swprintf_s(str,L"你傻啊~!，魔法值不足30点，施法失败，这回合白费了~！");
				}
				Message_Insert(str);
				break;

			case ACTION_TYPE_RECOVER:  //使用气疗术

					if(Hero.NowMp>=40)
					{
						Hero.NowMp-=40;
						int HpRecover=5*(5*(rand()%Hero.Intelligence)+40);
						Hero.NowHp+=HpRecover;
						if (Hero.NowHp>=Hero.MaxHp)
						{
							Hero.NowHp=Hero.MaxHp;
						}
						swprintf_s(str,L"玩家圣殿骑士使用了气疗术，恢复了【%d】点生命值，感觉好多了。",HpRecover);
					}
					else
					{
						HeroActionType=ACTION_TYPE_MISS;
						swprintf_s(str,L"你傻啊~!，魔法值不足40点，施法失败，这回合白费了~！");
					}
					Message_Insert(str);
					break;
		}
		
}


//-----------------------------------【HeroAction_Paint( )函数】-------------------------------
//	描述：玩家动作贴图函数
//--------------------------------------------------------------------------------------------------
VOID HeroAction_Paint()
{
	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:   //普通攻击，无敌斩
		SelectObject(g_bufdc,g_hHeroSkill1);
		TransparentBlt(g_mdc,50,170,364,140,g_bufdc,0,0,364,140,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		break;

	case ACTION_TYPE_CRITICAL:  //暴击，恩赐解脱
		SelectObject(g_bufdc,g_hHeroSkill3);
		TransparentBlt(g_mdc,20,60,574,306,g_bufdc,0,0,574,306,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		break;

	case ACTION_TYPE_MAGIC:  //魔法攻击，烈火剑法
		SelectObject(g_bufdc,g_hHeroSkill2);
		TransparentBlt(g_mdc,50,100,374,288,g_bufdc,0,0,374,288,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		break;

	case ACTION_TYPE_RECOVER:   //恢复，气疗术
		SelectObject(g_bufdc,g_hRecoverSkill);
		TransparentBlt(g_mdc,560,170,150,150,g_bufdc,0,0,150,150,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		break;
	}
}

//-----------------------------------【BossAction_Logic( )函数】-------------------------------
//	描述：Boss动作逻辑判断函数
//--------------------------------------------------------------------------------------------------
VOID BossAction_Logic()
{
	srand((unsigned)time(NULL));      //用系统时间初始化随机种子 
	if(Boss.NowHp > (Boss.MaxHp/2))				//生命值大于1/2时
	{
		switch(rand()%3)
		{
		case 0:						//释放普通攻击“幽冥鬼火”
			BossActionType = ACTION_TYPE_NORMAL;
			break;
		case 1:						//释放致命一击
			BossActionType = ACTION_TYPE_CRITICAL;
			break;
		case 2:						//使用嗜血咒
			BossActionType = ACTION_TYPE_MAGIC;
			break;
		}
	}
	else								//生命值小于1/2时
	{
		switch(rand()%3)
		{
		case 0:						//释放嗜血咒
			BossActionType = ACTION_TYPE_MAGIC;
			break;
		case 1:						//释放致命一击
			BossActionType = ACTION_TYPE_CRITICAL;
			break;
		case 2:						//使用梅肯斯姆回复
			BossActionType = ACTION_TYPE_RECOVER;
			break;
		}
	}
}

//-----------------------------------【BossAction_Paint( )函数】-------------------------------
//	描述：Boss动作贴图函数
//--------------------------------------------------------------------------------------------------
VOID		BossAction_Paint()
{
	int damage=0,recover=0;
	wchar_t str[100];

	switch(BossActionType)
	{
	case ACTION_TYPE_NORMAL:							//释放普通攻击“幽冥鬼火”
		SelectObject(g_bufdc,g_hBossSkill1);
		TransparentBlt(g_mdc,500,150,234,188,g_bufdc,0,0,234,188,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		//第30个画面时计算玩家受伤害程度并加入显示消息
		if(g_iFrameNum == 30)
		{
			damage = rand()%Boss.Agility+ Boss.Level*Boss.Strength;
			Hero.NowHp -= (int)damage;

			swprintf_s(str,L"黄金魔龙君释放幽冥鬼火...对玩家照成【 %d】 点伤害",damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp,true);
		}
		break;

	case ACTION_TYPE_MAGIC:							//释放嗜血咒
		SelectObject(g_bufdc,g_hBossSkill2);
		TransparentBlt(g_mdc,450,150,387,254,g_bufdc,0,0,387,254,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		//第30个画面时计算玩家受伤害程度并加入显示消息
		if(g_iFrameNum == 30)
		{
			damage = 2*(2*(rand()%Boss.Agility) + Boss.Strength*Boss.Intelligence);  //嗜血咒的伤害值计算
			Hero.NowHp -= damage;	   //让英雄血量减掉一个计算出来的伤害值
			recover	=(int)((float)damage*0.2f);   //吸血量20%
			Boss.NowHp+=recover;   //怪物恢复吸到的血量
			swprintf_s(str,L"黄金魔龙君释放嗜血咒...对玩家照成【 %d 】点伤害,自身恢复【%d】点生命值",damage,recover);   //将数字格式化到文字中
			Message_Insert(str);   //插入文字消息

			Die_Check(Hero.NowHp,true);
		}
		break;

	case ACTION_TYPE_CRITICAL:							//释放致命一击
		SelectObject(g_bufdc,g_hBossSkill3);
		TransparentBlt(g_mdc,280,100,574,306,g_bufdc,0,0,574,306,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		//第30个画面时计算玩家受伤害程度并加入显示消息
		if(g_iFrameNum == 30)
		{
			damage = 2*(rand()%Boss.Agility+ Boss.Level*Boss.Strength);
			Hero.NowHp -= (int)damage;

			swprintf_s(str,L"黄金魔龙君致命一击...对玩家照成【%d】点伤害.",damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp,true);
		}
		break;

	case ACTION_TYPE_RECOVER:							//使用梅肯斯姆补血
		SelectObject(g_bufdc,g_hRecoverSkill);
		TransparentBlt(g_mdc,150,150,150,150,g_bufdc,0,0,150,150,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		//第30个画面时怪物回复生命值并加入显示消息
		if(g_iFrameNum == 30)
		{
				recover= 2*Boss.Intelligence*Boss.Intelligence;
				Boss.NowHp +=recover;
			swprintf_s(str,L"黄金魔龙君使用梅肯斯姆...恢复了【%d】点生命值",recover);
			Message_Insert(str);
		}
		break;
	}
}

//-----------------------------------【Snow_Paint( )函数】--------------------------------------
//	描述：封装了雪花飞舞粒子效果的函数，模拟下雪场景
//--------------------------------------------------------------------------------------------------
VOID Snow_Paint()
{
	//创建粒子
	if(g_SnowNum< PARTICLE_NUMBER)  //当粒子数小于规定的粒子数时，便产生新的粒子，设定每个粒子的属性值
	{
		SnowFlowers[g_SnowNum].x = rand()%g_rect.right; //将粒子的X坐标设为窗口中水平方向上的任意位置
		SnowFlowers[g_SnowNum].y = 0;    //将每个粒子的Y坐标都设为"0"，即从窗口上沿往下落
		SnowFlowers[g_SnowNum].exist = true; //设定粒子存在
		g_SnowNum++;   //每产生一个粒子后进行累加计数
	}

	//首先判断粒子是否存在，若存在，进行透明贴图操作
	for(int i=0;i<PARTICLE_NUMBER;i++)
	{
		if(SnowFlowers[i].exist)  //粒子还存在
		{
			//贴上粒子图
			SelectObject(g_bufdc,g_hSnow);
			TransparentBlt(g_mdc,SnowFlowers[i].x,SnowFlowers[i].y,30,30,g_bufdc,0,0,30,30,RGB(0,0,0));

			//随机决定横向的移动方向和偏移量
			if(rand()%2==0)
				SnowFlowers[i].x+=rand()%6;  //x坐标加上0~5之间的一个随机值
			else 
				SnowFlowers[i].x-=rand()%6;	 //y坐标加上0~5之间的一个随机值
			//纵方向上做匀速运动
			SnowFlowers[i].y+=10;  //纵坐标加上10
			//如果粒子坐标超出了窗口长度，就让它以随机的x坐标出现在窗口顶部
			if(SnowFlowers[i].y > g_rect.bottom)
			{
				SnowFlowers[i].x = rand()%g_rect.right;
				SnowFlowers[i].y = 0;
			}
		}

	}
}

//-----------------------------------【Game_ShutDown( )函数】---------------------------------
//	描述：资源清理函数，在此函数中进行程序退出前资源的清理工作
//--------------------------------------------------------------------------------------------------
BOOL Game_ShutDown( HWND hwnd )
{
	//释放资源对象
	DeleteObject(g_hBackGround);
	DeleteObject(g_hBackGround);
	DeleteObject(g_hGameOver);
	DeleteObject(g_hVictory);
	DeleteObject(g_hSnow);
	DeleteObject(g_hMonsterBitmap);
	DeleteObject(g_hHeroBitmap);
	DeleteObject(g_hRecoverSkill);
	DeleteObject(g_hSkillButton1);
	DeleteObject(g_hSkillButton2);
	DeleteObject(g_hSkillButton3);
	DeleteObject(g_hSkillButton4);
	DeleteObject(g_hHeroSkill1);
	DeleteObject(g_hHeroSkill2);
	DeleteObject(g_hHeroSkill3);
	DeleteObject(g_hBossSkill1);
	DeleteObject(g_hBossSkill2);
	DeleteObject(g_hBossSkill3);
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd,g_hdc);
	return TRUE;
}

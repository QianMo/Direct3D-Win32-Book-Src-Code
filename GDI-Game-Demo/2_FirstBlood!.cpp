#include <Windows.h>
#pragma comment(lib,"winmm.lib")
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{ 
	PlaySound(L"FirstBlood.wav", NULL, SND_FILENAME | SND_ASYNC);   //播放音效
	MessageBox( NULL, L"First blood! 你好，游戏开发的世界，我们来征服你了！", L"First blood! 消息窗口", 0 );//显示一个消息框
	return 0; 
} 


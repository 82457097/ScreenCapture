// 屏幕截图.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "屏幕截图.h"

#define MAX_LOADSTRING 100

void ScreenCapture();
void CopyBitmapToCipBoard();

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HDC g_srcMemDc;		//源桌面截图
int scrW;			//屏幕的宽
int scrH;			//屏幕的高

RECT rect;			//矩形区域
BOOL isSelect = FALSE;
BOOL isDown = FALSE;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。		//窗口的样子基本由这个函数决定
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;			//窗口过程函数
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_MY);	//菜单
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   //类名称	窗口的标题	表示窗口弹出来的样式	窗口的坐标和大小
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_MAXIMIZE);	//窗口的大小样式 
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//	回调函数 用于给系统调用
//	当有消息的时候，就调用这个函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//改变为透明的画刷
	LOGBRUSH brush;
	brush.lbStyle = BS_NULL;
	HBRUSH hBrush = CreateBrushIndirect(&brush);

	//改变一下笔
	LOGPEN pen;
	POINT pt;
	pt.x = 2;
	pt.y = 2;
	
	pen.lopnColor = 0x0000FFFF;
	pen.lopnStyle = PS_SOLID;
	pen.lopnWidth = pt;

	HPEN hPen = CreatePenIndirect(&pen);

    switch (message)
    {
		//用于处理一些我们感兴趣的消息
	case WM_CREATE:
		ScreenCapture();
		break;

	case WM_LBUTTONDOWN: 
		{	
			if (!isSelect) 
			{	//没有选择才能按下左键
				POINT pt;
				GetCursorPos(&pt);	//得到当前光标的位置
				rect.left = pt.x;
				rect.top = pt.y;
				rect.right = pt.x;
				rect.bottom = pt.y;
				InvalidateRgn(hWnd, 0, true);

				isDown = TRUE;	//按下的时候
			}
		}
		break;

	case WM_LBUTTONUP:
	{

		if (isDown == TRUE && !isSelect)
		{
			POINT pt;
			GetCursorPos(&pt);	//得到当前的位置
			rect.right = pt.x;
			rect.bottom = pt.y;
			InvalidateRgn(hWnd, 0, true);

			isDown = FALSE;
			isSelect = TRUE;	//弹起的时候，表示区域已经选中
		}
	}
	break;

	case WM_LBUTTONDBLCLK:
	{		//鼠标双击，复制截图区域
		if (isSelect)
		{
			CopyBitmapToCipBoard();//拷贝选中区域到粘贴板
			isSelect = FALSE;
		}
	}
	break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:		//屏幕绘制消息
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);	//当前窗口
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			SelectObject(hdc, hBrush);
			SelectObject(hdc, hPen);
			
			BitBlt(hdc, 0, 0, scrW, scrH, g_srcMemDc, 0, 0, SRCCOPY);

			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);	//画矩形的函数

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//获取整个桌面的图像
//DC 设备上下文（可以理解为一直与窗口有关的笔）
void ScreenCapture() {
	HDC disDc = ::CreateDC(L"DISPLAY", 0, 0, 0);	//创建一个DC DISPLAY 桌面
	//得到整个桌面的大小（分辨率）
	scrW = GetDeviceCaps(disDc, HORZRES);			//整个桌面的水平大小
	scrH = GetDeviceCaps(disDc, HORZRES);			//整个桌面的垂直大小

	//获取桌面截图
	g_srcMemDc = CreateCompatibleDC(disDc);			//创建一个DC，兼容的DC（内存DC），与桌面的DC相关
	//模拟一张画布
	HBITMAP hBitmap = CreateCompatibleBitmap(disDc, scrW, scrH);
	SelectObject(g_srcMemDc, hBitmap);				//将画布放进DC
	//将桌面画到画布上
	BitBlt(g_srcMemDc, 0, 0, scrW, scrH, disDc, 0, 0, SRCCOPY);	//整个桌面已经放入内存DC
}

//复制鼠标选中区域到粘贴板
void CopyBitmapToCipBoard() {
	//矩形区域
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	HDC hSrcDc = ::CreateDC(L"DSIPLAY", 0, 0, 0);

	HDC memDc = CreateCompatibleDC(hSrcDc);
	HBITMAP bmp = CreateCompatibleBitmap(hSrcDc, width, height);		//模拟画布
	HBITMAP oldmap = (HBITMAP)SelectObject(memDc, bmp);				//将画布选入画笔

	//将桌面的一部分画到画板上
	BitBlt(memDc, 0, 0, width, height, hSrcDc, rect.left, rect.top, SRCCOPY);

	HBITMAP newmap = (HBITMAP)SelectObject(memDc, oldmap);			//newmap 就是我们的截图 放入粘贴板
	
	if (OpenClipboard(0)) {
		EmptyClipboard();	//清空粘贴板
		SetClipboardData(CF_BITMAP, newmap);
		CloseClipboard();
	}
}
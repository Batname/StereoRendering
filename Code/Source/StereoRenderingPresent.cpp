#include "StereoRendering_precompiled.h"
#include <tchar.h>
#include <sstream>

#include <IConsole.h>

#include "StereoRenderingPresent.h"

#include "StereoRenderingUtils.h"

#define LogMessage(...) CryLogAlways("[HMD][StereoRenderingPresent] - " __VA_ARGS__);


namespace StereoRendering
{

	StereoRenderingPresent::StereoRenderingPresent()
		: mhMainWnd(nullptr)
		, mhAppInst(GetModuleHandle(NULL))
		, mMainWndCaption(_T("App"))
		, bISRunning(false)
	{
	}

	StereoRenderingPresent::~StereoRenderingPresent()
	{
		if (bISRunning)
		{
			DestroyWindow(mhMainWnd);

			m_presentThread.join();
		}
	}

	void StereoRenderingPresent::Init()
	{
		//try
		//{
		//	if (!Initialize())
		//	{

		//	}
		//}
		//catch (DxException& e)
		//{
		//	e.ToString().c_str();
		//	//MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		//	return;
		//}

		if (!bISRunning)
		{
			bISRunning = true;

			m_presentThread = std::thread([&] {
				Initialize();
			});
		}
	}

	bool StereoRenderingPresent::Initialize()
	{
		if (!InitMainWindow())
			return false;


		return Run();
	}

	int StereoRenderingPresent::Run()
	{
		MSG msg = { 0 };

		while (msg.message != WM_QUIT)
		{
			// If there are Window messages then process them.
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// Otherwise, do animation/game stuff.
			else
			{
			}
		}

		return (int)msg.wParam;
	}

	bool StereoRenderingPresent::InitMainWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = 0;
		wc.lpszMenuName = 0;
		wc.lpszClassName = _T("MainWnd");

		if (!RegisterClass(&wc))
		{
			MessageBox(0, "RegisterClass Failed.", 0, 0);
			return false;
		}

		int mClientWidth = 800;
		int mClientHeight = 600;

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, mClientWidth, mClientHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		mhMainWnd = CreateWindow(_T("MainWnd"), mMainWndCaption,
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, GetModuleHandle(NULL), 0);

		if (!mhMainWnd)
		{
			DWORD dwrd = GetLastError();

			std::ostringstream s;
			s << "Error UnregisterClass " << dwrd;

			MessageBoxA(NULL, std::string(s.str()).c_str(),
				"Error", MB_OK | MB_ICONERROR);

			return false;
		}

		ShowWindow(mhMainWnd, SW_SHOW);
		UpdateWindow(mhMainWnd);

		return true;
	}

	LRESULT StereoRenderingPresent::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

}

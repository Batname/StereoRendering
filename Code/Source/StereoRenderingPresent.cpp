#include "StereoRendering_precompiled.h"
#include <tchar.h>

#include <IConsole.h>

#include "StereoRenderingPresent.h"

#include "StereoRenderingUtils.h"

#define LogMessage(...) CryLogAlways("[HMD][StereoRenderingPresent] - " __VA_ARGS__);


namespace StereoRendering
{

	StereoRenderingPresent::StereoRenderingPresent()
		: mhMainWnd(nullptr)
		, mhAppInst(nullptr)
	{
		mMainWndCaption = _T("App");
	}

	StereoRenderingPresent::~StereoRenderingPresent()
	{
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

		Initialize();
	}

	bool StereoRenderingPresent::Initialize()
	{
		if (!InitMainWindow())
			return false;

		return true;
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
		wc.lpszClassName = _T("Error");

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
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
		if (!mhMainWnd)
		{
			MessageBox(0, "CreateWindow Failed.", 0, 0);
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

#include "StereoRendering_precompiled.h"

#include <IConsole.h>

#include "StereoRenderingPresent.h"

#include "StereoRenderingUtils.h"

#define LogMessage(...) CryLogAlways("[HMD][StereoRenderingPresent] - " __VA_ARGS__);


namespace StereoRendering
{

	StereoRenderingPresent::StereoRenderingPresent()
	{

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
		return false;
	}

}

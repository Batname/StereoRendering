#include "SR_precompiled.h"
#include <Windows.h>

#include <tchar.h>
#include <sstream>

#include <IConsole.h>

#include "SRRenderer.h"

#include "SRUtils.h"

#define LogMessage(...) CryLogAlways("[HMD][SRRenderer] - " __VA_ARGS__);

// _T("App")

namespace SR
{

	SRRenderer::SRRenderer()
		: bISRunning(false)
		, d3d11Device(nullptr)
		, d3d11DevCon(nullptr)
		, squareIndexBuffer(nullptr)
		, squareVertBuffer(nullptr)
		, VS(nullptr)
		, PS(nullptr)
		, VS_Buffer(nullptr)
		, PS_Buffer(nullptr)
		, vertLayout(nullptr)
	{
	}

	SRRenderer::~SRRenderer()
	{
		if (bISRunning)
		{

			m_presentThread.join();
		}
	}

	void SRRenderer::Init()
	{
		if (!bISRunning)
		{
			bISRunning = true;

			m_presentThread = std::thread([&] {
				Initialize_thread();
			});
		}
	}

	bool SRRenderer::Initialize_thread()
	{
		return Run();
	}

	int SRRenderer::Run()
	{
		while (true)
		{
			Sleep(100);
		}

		return 0;
	}
	void SRRenderer::CleanUp()
	{
	}
	bool SRRenderer::InitScene()
	{
		return false;
	}
	void SRRenderer::UpdateScene()
	{
	}
	void SRRenderer::DrawScene()
	{
	}
}

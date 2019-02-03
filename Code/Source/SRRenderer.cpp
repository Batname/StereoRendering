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
		if (!InitScene())    //Initialize our scene
		{

			MessageLastError("Scene Initialization - Failed");
			return;
		}


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
	void SRRenderer::MessageLastError(const std::string & Message)
	{
		DWORD dwrd = GetLastError();

		std::ostringstream s;
		s << Message.c_str() << dwrd;

		MessageBoxA(NULL, std::string(s.str()).c_str(), "Error", MB_OK | MB_ICONERROR);
	}

	void SRRenderer::CleanUp()
	{
		//Release the COM Objects we created
		d3d11Device != nullptr &&  d3d11Device->Release();
		d3d11DevCon != nullptr && d3d11DevCon->Release();
		squareVertBuffer != nullptr && squareVertBuffer->Release();
		squareIndexBuffer != nullptr && squareIndexBuffer->Release();
		VS != nullptr && VS->Release();
		PS != nullptr && PS->Release();
		VS_Buffer != nullptr && VS_Buffer->Release();
		PS_Buffer != nullptr && PS_Buffer->Release();
		vertLayout != nullptr && vertLayout->Release();
	}

	bool SRRenderer::InitScene()
	{
		hr = D3DX11CompileFromFile(_T("D:/Amazon/1.17.0.0/dev/Cache/StarterGame/pc/startergame/shaders/stereorendering.cfx"), 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
		if (FAILED(hr)) {
			MessageLastError("Error D3DX11CompileFromFile for vertex shader");
		}

		return true;
	}
	void SRRenderer::UpdateScene()
	{
	}
	void SRRenderer::DrawScene()
	{
	}
}

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

	SRRenderer::SRRenderer(ID3D11Device* d3d11Dev, ID3D11DeviceContext* d3d11Con)
		: bISRunning(false)
		, d3d11Device(d3d11Dev)
		, d3d11DevCon(d3d11Con)
		, squareIndexBuffer(nullptr)
		, squareVertBuffer(nullptr)
		, VS(nullptr)
		, PS(nullptr)
		, VS_Buffer(nullptr)
		, PS_Buffer(nullptr)
		, vertLayout(nullptr)
	{
		fullPathToAssets = gEnv->pFileIO->GetAlias("@assets@");
		fileBase = AZ::IO::FileIOBase::GetInstance();
	}

	SRRenderer::~SRRenderer()
	{
		if (bISRunning)
		{

			m_presentThread.join();
		}

		CleanUp();
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

		LogMessage("%s", std::string(s.str()).c_str());
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
		// Find shader file
		AZStd::string StereoShaderPath = AZStd::string::format("%s\\shaders\\%s", fullPathToAssets.c_str(), "stereorendering.cfx");
		if (!fileBase->Exists(StereoShaderPath.c_str()))
		{
			MessageLastError("stereorendering.cfx not exists");

			return false;
		}

		// Compile vertex shader
		hr = D3DX11CompileFromFile(_T(StereoShaderPath.c_str()), 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer, 0, 0);
		if (FAILED(hr)) {
			MessageLastError("Error D3DX11CompileFromFile vertex shader");
		}

		hr = D3DX11CompileFromFile(_T(StereoShaderPath.c_str()), 0, 0, "PS", "ps_5_0", 0, 0, 0, &PS_Buffer, 0, 0);
		if (FAILED(hr)) {
			MessageLastError("Error D3DX11CompileFromFile pixel shader");
		}

		//Create the Shader Objects
		hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
		if (FAILED(hr)) {
			MessageLastError("Error CreateVertexShader pixel shader");
		}
		hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
		if (FAILED(hr)) {
			MessageLastError("Error CreateVertexShader pixel shader");
		}

		//Create the vertex buffer
		Vertex v[] =
		{
			Vertex(-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
			Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
			Vertex(0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
			Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
		};

		DWORD indices[] = {
			0, 1, 2,
			0, 2, 3,
		};

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = indices;
		d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

		d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = v;
		hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

		//Set the vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

		//Create the Input Layout
		hr = d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), &vertLayout);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(vertLayout);

		return true;
	}
	void SRRenderer::UpdateScene()
	{
	}
	void SRRenderer::DrawScene()
	{
	}
}

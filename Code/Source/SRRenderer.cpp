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

	SRRenderer::SRRenderer(ID3D11Device* d3d11Dev, ID3D11DeviceContext* d3d11Con, ID3D11Texture2D* LeftTex, ID3D11Texture2D* RightTex, uint32 width, uint32 height)
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
		, LeftTexture(LeftTex)
		, RightTexture(RightTex)
		, LeftTextureRV(nullptr)
		, RightTextureRV(nullptr)
		, TexSamplerState(nullptr)
		, cbPerObjectBuffer(nullptr)
	{
		fullPathToAssets = gEnv->pFileIO->GetAlias("@assets@");
		fileBase = AZ::IO::FileIOBase::GetInstance();

		cbPerObj.ScreenSize[0] = width;
		cbPerObj.ScreenSize[1] = height;
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
		squareVertBuffer != nullptr && squareVertBuffer->Release();
		squareIndexBuffer != nullptr && squareIndexBuffer->Release();
		VS != nullptr && VS->Release();
		PS != nullptr && PS->Release();
		VS_Buffer != nullptr && VS_Buffer->Release();
		PS_Buffer != nullptr && PS_Buffer->Release();
		vertLayout != nullptr && vertLayout->Release();

		LeftTextureRV != nullptr && LeftTextureRV->Release();
		RightTextureRV != nullptr && RightTextureRV->Release();

		TexSamplerState != nullptr && TexSamplerState->Release();

		cbPerObjectBuffer != nullptr && cbPerObjectBuffer->Release();
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
		// Square geometry
		Vertex v[] =
		{
			// left texture
			{ -1.0f, 1.0f, 0.0f, 0.0f, 0.0f }, // top left 
			{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f }, // top right
			{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f }, // bottom left
			{ 1.0f, -1.0f, 0.0f, 1.0f, 1.0f },// bottom right
		};

		DWORD indices[] = {
			0,  1,  2,
			2,  1,  3,
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


		//Create the Input Layout
		hr = d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), &vertLayout);

		// Create texture sampler
			// Describe the Generic Sample State
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		//Create the Sample State
		hr = d3d11Device->CreateSamplerState(&sampDesc, &TexSamplerState);
		if (FAILED(hr)) {
			MessageLastError("Error CreateSamplerState");
		}

		// Create textures for shader
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			memset(&SRVDesc, 0, sizeof(SRVDesc));
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			hr = d3d11Device->CreateShaderResourceView(LeftTexture, &SRVDesc, &LeftTextureRV);
			if (FAILED(hr)) {
				MessageLastError("Error CreateShaderResourceView LeftTexture");
			}
		}

		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			memset(&SRVDesc, 0, sizeof(SRVDesc));
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			hr = d3d11Device->CreateShaderResourceView(RightTexture, &SRVDesc, &RightTextureRV);
			if (FAILED(hr)) {
				MessageLastError("Error CreateShaderResourceView RightTexture");
			}
		}

		// Create constant buffer
		{
			D3D11_BUFFER_DESC cbbd;
			ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

			cbbd.Usage = D3D11_USAGE_DEFAULT;
			cbbd.ByteWidth = sizeof(cbPerObject);
			cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbbd.CPUAccessFlags = 0;
			cbbd.MiscFlags = 0;

			hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
			if (FAILED(hr)) {
				MessageLastError("Error CreateBuffer");
			}
		}


		return true;
	}
	void SRRenderer::UpdateScene()
	{
	}
	void SRRenderer::DrawScene()
	{
		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(vertLayout);

		//Set Primitive Topology
		d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Set the spheres index buffer
		d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Set the spheres vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

		//Set the new VS and PS shaders
		d3d11DevCon->VSSetShader(VS, 0, 0);
		d3d11DevCon->PSSetShader(PS, 0, 0);

		// Set texture resources
		d3d11DevCon->PSSetShaderResources(0, 1, &LeftTextureRV);
		d3d11DevCon->PSSetSamplers(0, 1, &TexSamplerState);
		d3d11DevCon->PSSetShaderResources(1, 1, &RightTextureRV);
		d3d11DevCon->PSSetSamplers(1, 1, &TexSamplerState);

		// Update constant buffer
		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		d3d11DevCon->PSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		//Draw the triangle
		d3d11DevCon->DrawIndexed(6, 0, 0);
	}
}


#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <thread>
#include <string>


namespace SR
{
    class SRRenderer
    {
    public:
		SRRenderer(ID3D11Device* d3d11Dev, ID3D11DeviceContext* d3d11Con, ID3D11Texture2D* LeftTex, ID3D11Texture2D* RightTex, uint32 width, uint32 height);
    	~SRRenderer();

	public:
		void Init();

	private:
		bool Initialize_thread();
		int Run();

		void MessageLastError(const std::string& Message);

	private:
		bool bISRunning;
		std::thread m_presentThread;

	private:
		/// Init DirectX and variables  /////////////////////////////////////////////
		ID3D11Device* d3d11Device;
		ID3D11DeviceContext* d3d11DevCon;
		ID3D11Buffer* squareIndexBuffer;
		ID3D11Buffer* squareVertBuffer;
		ID3D11VertexShader* VS;
		ID3D11PixelShader* PS;
		ID3D10Blob* VS_Buffer;
		ID3D10Blob* PS_Buffer;
		ID3D11InputLayout* vertLayout;

		ID3D11Texture2D* LeftTexture;
		ID3D11Texture2D* RightTexture;

		ID3D11ShaderResourceView* LeftTextureRV;
		ID3D11ShaderResourceView* RightTextureRV;
		
		ID3D11SamplerState* TexSamplerState;

	private:
		//Vertex Structure and Vertex Layout (Input Layout)//
		struct Vertex	//Overloaded Vertex Structure
		{
			Vertex() {}
			Vertex(float x, float y, float z,
				float u, float v)
				: pos(x, y, z), texCoord(u, v) {}

			XMFLOAT3 pos;
			XMFLOAT2 texCoord;
		};

		D3D11_INPUT_ELEMENT_DESC layout[2] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);

	private:
		ID3D11Buffer* cbPerObjectBuffer;

		struct cbPerObject
		{
			// 3rd Group of 16 bytes
			int ScreenSize[2] = { 1280, 720 }; // 8 bytes 4 * 2
			float pad1 = 0.f; // 4 bytes
			float pad2 = 0.f; // 4 bytes
		};

		cbPerObject cbPerObj;

		HRESULT hr;

	private:
		AZStd::string fullPathToAssets;
		AZ::IO::FileIOBase* fileBase;

	private:
		bool InitScene();

	public:
		void CleanUp();
		void UpdateScene();
		void DrawScene();

		////////////////////////////////////////////////////////////////////////

	};
}
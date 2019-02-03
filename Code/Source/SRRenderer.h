
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
		SRRenderer();
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

	private:
		//Vertex Structure and Vertex Layout (Input Layout)//
		struct Vertex    //Overloaded Vertex Structure
		{
			Vertex() {}
			Vertex(float x, float y, float z,
				float cr, float cg, float cb, float ca)
				: pos(x, y, z), color(cr, cg, cb, ca) {}

			XMFLOAT3 pos;
			XMFLOAT4 color;
		};

		D3D11_INPUT_ELEMENT_DESC layout[2] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		HRESULT hr;

	private:
		void CleanUp();
		bool InitScene();
		void UpdateScene();
		void DrawScene();

		////////////////////////////////////////////////////////////////////////

	};
}
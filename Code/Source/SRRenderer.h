//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
//#include <d3dx10.h>
//#include <xnamath.h>
#include <thread>


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
		bool Initialize();
		int Run();

		/// Init DirectX and win wintod  /////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////

	private:
		bool bISRunning;
		std::thread m_presentThread;
	};
}
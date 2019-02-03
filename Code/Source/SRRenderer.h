
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
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
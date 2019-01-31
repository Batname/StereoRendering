#include <thread>

namespace StereoRendering
{
    class StereoRenderingPresent
    {
    public:
		StereoRenderingPresent();
    	~StereoRenderingPresent();

	public:
		void Init();

	private:
		bool Initialize();
		int Run();

		/// Init DirectX and win wintod  /////////////////////////////////////////////
		bool InitMainWindow();
		////////////////////////////////////////////////////////////////////////

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		HWND mhMainWnd; // main window handle
		LPCSTR mMainWndCaption;
		HINSTANCE mhAppInst;

	private:
		bool bISRunning;
		std::thread m_presentThread;
	};
}
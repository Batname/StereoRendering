
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

		/// Init DirectX and win wintod  /////////////////////////////////////////////
		bool InitMainWindow();
		////////////////////////////////////////////////////////////////////////
	};
}
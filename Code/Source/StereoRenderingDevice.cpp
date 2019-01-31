
#include "StereoRendering_precompiled.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include <IConsole.h>

#include "StereoRenderingDevice.h"

#include "StereoRenderingPresent.h"

#define LogMessage(...) CryLogAlways("[HMD][StereoRendering] - " __VA_ARGS__);

static const char* kDeviceName = "StereoRendering";

namespace StereoRendering
{
    void StereoRenderingDevice::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<StereoRenderingDevice, AZ::Component>()
                ->Version(1);

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<StereoRenderingDevice>("StereoRendering", "Enables the VR system without a VR device.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "VR")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                    //->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void StereoRenderingDevice::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("HMDDevice"));
        provided.push_back(AZ_CRC("StereoRenderingDevice"));
    }

    void StereoRenderingDevice::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("StereoRenderingDevice"));
    }

	void StereoRenderingDevice::Init()
    {
    }

    void StereoRenderingDevice::Activate()
    {
        AZ::VR::HMDInitRequestBus::Handler::BusConnect();
    }

    void StereoRenderingDevice::Deactivate()
    {
        AZ::VR::HMDInitRequestBus::Handler::BusDisconnect();

		LogMessage("StereoRenderingDevice::Deactivate");
    }

    bool StereoRenderingDevice::AttemptInit()
    {
        AZ::VR::HMDDeviceRequestBus::Handler::BusConnect();
        StereoRenderingRequestBus::Handler::BusConnect();
        
        m_deviceInfo.productName = kDeviceName;
        m_deviceInfo.manufacturer = kDeviceName;

        m_initialized = true;

        using namespace AZ::VR;
        VREventBus::Broadcast(&VREvents::OnHMDInitialized);

        return true;
    }

    void StereoRenderingDevice::Shutdown()
    {
		LogMessage("StereoRenderingDevice::Shutdown");

        using namespace AZ::VR;
        VREventBus::Broadcast(&VREvents::OnHMDShutdown);

        AZ::VR::HMDDeviceRequestBus::Handler::BusDisconnect();
        StereoRenderingRequestBus::Handler::BusDisconnect();

		// destroy custom present
		if (m_stereoRenderingPresent.get() != nullptr)
		{
			m_stereoRenderingPresent.reset();
			m_stereoRenderingPresent = nullptr;
		}
    }

    AZ::VR::HMDInitBus::HMDInitPriority StereoRenderingDevice::GetInitPriority() const
    {
        return HMDInitPriority::kHighest;
    }

	void StereoRenderingDevice::SubmitFrame(const EyeTarget & left, const EyeTarget & right)
	{
		LogMessage(">>>> SubmitFrame");

		d3d11DevCon->Draw(1000, 0); // it works
		//d3d11DevCon->DrawIndexed(300, 0, 0);
	}

    void StereoRenderingDevice::GetPerEyeCameraInfo(const EStereoEye eye, const float nearPlane, const float farPlane, AZ::VR::PerEyeCameraInfo& cameraInfo)
    {
        if (ISystem* system = GetISystem())
        {
            if (IConsole* console = system->GetIConsole())
            {
                if (ICVar* cvar = console->GetCVar("stereorendering_fov"))
                {
                    cameraInfo.fov = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("stereorendering_aspectRatio"))
                {
                    cameraInfo.aspectRatio = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("stereorendering_frustumPlane_horizontalDistance"))
                {
                    cameraInfo.frustumPlane.horizontalDistance = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("stereorendering_frustumPlane_verticalDistance"))
                {
                    cameraInfo.frustumPlane.verticalDistance = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("stereorendering_eyeOffsetX"))
                {
                    cameraInfo.eyeOffset = AZ::Vector3(cvar->GetFVal(), 0.0f, 0.0f);
                }
            }
        }
    }

    bool StereoRenderingDevice::CreateRenderTargets(void* renderDevice, const TextureDesc& desc, size_t eyeCount, AZ::VR::HMDRenderTarget* renderTargets[])
    {
		LogMessage(">>>>>>>>>>>>>>>>>>> CreateRenderTargets");

		d3dDevice = static_cast<ID3D11Device*>(renderDevice);
		d3dDevice->GetImmediateContext(&d3d11DevCon);

		LogMessage(">>>>>>>>>>>>>>>>>>> d3dDevice %p", d3dDevice);
		LogMessage(">>>>>>>>>>>>>>>>>>> d3d11DevCon %p", d3d11DevCon);

		// Create StereoRenderingPresent
		if (m_stereoRenderingPresent.get() == nullptr)
		{
			m_stereoRenderingPresent = AZStd::make_shared<StereoRenderingPresent>();
			m_stereoRenderingPresent->Init();
		}

        for (size_t i = 0; i < eyeCount; ++i)
        {
            D3D11_TEXTURE2D_DESC textureDesc;
            textureDesc.Width = desc.width;
            textureDesc.Height = desc.height;
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;
            textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            textureDesc.CPUAccessFlags = 0;
            textureDesc.MiscFlags = 0;

            ID3D11Texture2D* texture;
            d3dDevice->CreateTexture2D(&textureDesc, nullptr, &texture);

            //renderTargets[i]->deviceSwapTextureSet = texture;
			renderTargets[i]->deviceSwapTextureSet = texture;
            renderTargets[i]->numTextures = 1;
            renderTargets[i]->textures = new void*[1];
            renderTargets[i]->textures[0] = texture;
        }

        return true;
    }

    void StereoRenderingDevice::DestroyRenderTarget(AZ::VR::HMDRenderTarget& renderTarget)
    {
        // Note: The textures created in CreateRenderTarget and added to the renderTarget->textuers array are released in the calling function
		LogMessage("DestroyRenderTarget deviceSwapTextureSet %p", renderTarget.deviceSwapTextureSet);

		SAFE_DELETE_ARRAY(renderTarget.textures);
		renderTarget.textures = nullptr;

		// destroy custom present
		if (m_stereoRenderingPresent.get() != nullptr)
		{
			m_stereoRenderingPresent.reset();
			m_stereoRenderingPresent = nullptr;
		}
    }

    AZ::VR::TrackingState* StereoRenderingDevice::GetTrackingState()
    {
        return &m_trackingState;
    }

	void StereoRenderingDevice::PrepareFrame()
	{
		//LogMessage("StereoRenderingDevice::PrepareFrame");
	}

    void StereoRenderingDevice::OutputHMDInfo()
    {
        LogMessage("Null Device");
    }

    AZ::VR::HMDDeviceInfo* StereoRenderingDevice::GetDeviceInfo()
    {
        if (ISystem* system = GetISystem())
        {
            if (IConsole* console = system->GetIConsole())
            {
                if (ICVar* cvar = console->GetCVar("stereorendering_renderWidth"))
                {
                    m_deviceInfo.renderWidth = cvar->GetIVal();
                }

                if (ICVar* cvar = console->GetCVar("stereorendering_renderHeight"))
                {
                    m_deviceInfo.renderHeight = cvar->GetIVal();
                }

                if (ICVar* cvar = console->GetCVar("stereorendering_fovH"))
                {
                    m_deviceInfo.fovH = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("stereorendering_fovV"))
                {
                    m_deviceInfo.fovV = cvar->GetFVal();
                }
            }
        }

        return &m_deviceInfo;
    }

    bool StereoRenderingDevice::IsInitialized()
    {
        return m_initialized;
    }

    const AZ::VR::Playspace* StereoRenderingDevice::GetPlayspace()
    {
        return nullptr;
    }
}

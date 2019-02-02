
#include "SR_precompiled.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include <IConsole.h>

#include "SRDevice.h"

#include "SRPresent.h"

#define LogMessage(...) CryLogAlways("[HMD][SR] - " __VA_ARGS__);

static const char* kDeviceName = "SR";

namespace SR
{
    void SRDevice::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<SRDevice, AZ::Component>()
                ->Version(1);

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<SRDevice>("SR", "Enables the VR system without a VR device.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "VR")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"));
            }
        }
    }

    void SRDevice::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("HMDDevice"));
        provided.push_back(AZ_CRC("SRDevice"));
    }

    void SRDevice::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("SRDevice"));
    }

	void SRDevice::Init()
    {
    }

    void SRDevice::Activate()
    {
        AZ::VR::HMDInitRequestBus::Handler::BusConnect();
    }

    void SRDevice::Deactivate()
    {
        AZ::VR::HMDInitRequestBus::Handler::BusDisconnect();

		LogMessage("SRDevice::Deactivate");
    }

    bool SRDevice::AttemptInit()
    {
        AZ::VR::HMDDeviceRequestBus::Handler::BusConnect();
        SRRequestBus::Handler::BusConnect();
        
        m_deviceInfo.productName = kDeviceName;
        m_deviceInfo.manufacturer = kDeviceName;

        m_initialized = true;

        using namespace AZ::VR;
        VREventBus::Broadcast(&VREvents::OnHMDInitialized);

        return true;
    }

    void SRDevice::Shutdown()
    {
		LogMessage("SRDevice::Shutdown");

        using namespace AZ::VR;
        VREventBus::Broadcast(&VREvents::OnHMDShutdown);

        AZ::VR::HMDDeviceRequestBus::Handler::BusDisconnect();
        SRRequestBus::Handler::BusDisconnect();

		// destroy custom present
		if (m_SRPresent.get() != nullptr)
		{
			m_SRPresent.reset();
			m_SRPresent = nullptr;
		}
    }

    AZ::VR::HMDInitBus::HMDInitPriority SRDevice::GetInitPriority() const
    {
        return HMDInitPriority::kHighest;
    }

	void SRDevice::SubmitFrame(const EyeTarget & left, const EyeTarget & right)
	{
		LogMessage(">>>> SubmitFrame");

		//d3d11DevCon->Draw(1000, 0); // it works
		//d3d11DevCon->DrawIndexed(300, 0, 0);
	}

    void SRDevice::GetPerEyeCameraInfo(const EStereoEye eye, const float nearPlane, const float farPlane, AZ::VR::PerEyeCameraInfo& cameraInfo)
    {
        if (ISystem* system = GetISystem())
        {
            if (IConsole* console = system->GetIConsole())
            {
                if (ICVar* cvar = console->GetCVar("SR_fov"))
                {
                    cameraInfo.fov = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("SR_aspectRatio"))
                {
                    cameraInfo.aspectRatio = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("SR_frustumPlane_horizontalDistance"))
                {
                    cameraInfo.frustumPlane.horizontalDistance = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("SR_frustumPlane_verticalDistance"))
                {
                    cameraInfo.frustumPlane.verticalDistance = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("SR_eyeOffsetX"))
                {
                    cameraInfo.eyeOffset = AZ::Vector3(cvar->GetFVal(), 0.0f, 0.0f);
                }
            }
        }
    }

    bool SRDevice::CreateRenderTargets(void* renderDevice, const TextureDesc& desc, size_t eyeCount, AZ::VR::HMDRenderTarget* renderTargets[])
    {
		d3dDevice = static_cast<ID3D11Device*>(renderDevice);
		d3dDevice->GetImmediateContext(&d3d11DevCon);

		// Create SRPresent
		if (m_SRPresent.get() == nullptr)
		{
			m_SRPresent = AZStd::make_shared<SRPresent>();
			m_SRPresent->Init();
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

    void SRDevice::DestroyRenderTarget(AZ::VR::HMDRenderTarget& renderTarget)
    {
        // Note: The textures created in CreateRenderTarget and added to the renderTarget->textuers array are released in the calling function
		LogMessage("DestroyRenderTarget deviceSwapTextureSet %p", renderTarget.deviceSwapTextureSet);

		SAFE_DELETE_ARRAY(renderTarget.textures);
		renderTarget.textures = nullptr;

		// destroy custom present
		if (m_SRPresent.get() != nullptr)
		{
			m_SRPresent.reset();
			m_SRPresent = nullptr;
		}
    }

    AZ::VR::TrackingState* SRDevice::GetTrackingState()
    {
        return &m_trackingState;
    }

	void SRDevice::PrepareFrame()
	{
		//LogMessage("SRDevice::PrepareFrame");
	}

    void SRDevice::OutputHMDInfo()
    {
        LogMessage("Null Device");
    }

    AZ::VR::HMDDeviceInfo* SRDevice::GetDeviceInfo()
    {
        if (ISystem* system = GetISystem())
        {
            if (IConsole* console = system->GetIConsole())
            {
                if (ICVar* cvar = console->GetCVar("SR_renderWidth"))
                {
                    m_deviceInfo.renderWidth = cvar->GetIVal();
                }

                if (ICVar* cvar = console->GetCVar("SR_renderHeight"))
                {
                    m_deviceInfo.renderHeight = cvar->GetIVal();
                }

                if (ICVar* cvar = console->GetCVar("SR_fovH"))
                {
                    m_deviceInfo.fovH = cvar->GetFVal();
                }

                if (ICVar* cvar = console->GetCVar("SR_fovV"))
                {
                    m_deviceInfo.fovV = cvar->GetFVal();
                }
            }
        }

        return &m_deviceInfo;
    }

    bool SRDevice::IsInitialized()
    {
        return m_initialized;
    }

    const AZ::VR::Playspace* SRDevice::GetPlayspace()
    {
        return nullptr;
    }
}

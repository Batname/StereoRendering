

#include "StereoRendering_precompiled.h"

#include <platform_impl.h>
#include <IGem.h>
#include <AzFramework/Metrics/MetricsPlainTextNameRegistration.h>
#include <FlowSystem/Nodes/FlowBaseNode.h>

#include "StereoRenderingDevice.h"

namespace StereoRendering
{
    class StereoRenderingGem
        : public CryHooksModule
    {
    public:
        AZ_RTTI(StereoRenderingGem, "{69C02B45-6070-47A9-959D-C591484ACEC8}", AZ::Module);

        StereoRenderingGem()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                StereoRenderingDevice::CreateDescriptor(),
            });


			// This is an internal Amazon gem, so register it's components for metrics tracking, otherwise the name of the component won't get sent back.
			// IF YOU ARE A THIRDPARTY WRITING A GEM, DO NOT REGISTER YOUR COMPONENTS WITH EditorMetricsComponentRegistrationBus
			AZStd::vector<AZ::Uuid> typeIds;
			typeIds.reserve(m_descriptors.size());
			for (AZ::ComponentDescriptor* descriptor : m_descriptors)
			{
				typeIds.emplace_back(descriptor->GetUuid());
			}
			EBUS_EVENT(AzFramework::MetricsPlainTextNameRegistrationBus, RegisterForNameSending, typeIds);
        }

		/**
		 * Add required SystemComponents to the SystemEntity.
		 */
		AZ::ComponentTypeList GetRequiredSystemComponents() const override
		{
			return AZ::ComponentTypeList{
				azrtti_typeid<StereoRenderingDevice>(),
			};
		}

		void OnSystemEvent(ESystemEvent event, UINT_PTR, UINT_PTR) override
		{
			switch (event)
			{
			case ESYSTEM_EVENT_FLOW_SYSTEM_REGISTER_EXTERNAL_NODES:
				RegisterExternalFlowNodes();
				break;
			}
		}

		void OnCrySystemCVarRegistry() override
		{
			// Note: Defaults are based on HTC Vive CV1 as of April 6, 2017
			REGISTER_CVAR(stereorendering_renderWidth, 1512, 0, "The width of the render texture in pixels. Set once on start.");
			REGISTER_CVAR(stereorendering_renderHeight, 1680, 0, "The height of the render texture in pixels. Set once on start.");
			REGISTER_CVAR(stereorendering_fovH, 1.84633982f, 0, "The field of view height in radians. Used on start.");
			REGISTER_CVAR(stereorendering_fovV, 1.94590104f, 0, "The field of view width in radians. Used on start.");
			REGISTER_CVAR(stereorendering_fov, 1.91986f, 0, "The field of view in radians. Can be updated at runtime.");
			REGISTER_CVAR(stereorendering_aspectRatio, 0.9f, 0, "Aspect ratio of the null VR implementation.");
			REGISTER_CVAR(stereorendering_frustumPlane_horizontalDistance, -0.0701720715f, 0, "Frustum plane horizontal distance.");
			REGISTER_CVAR(stereorendering_frustumPlane_verticalDistance, -0.00206005573f, 0, "Frustum plane vertical distance.");
			REGISTER_CVAR(stereorendering_eyeOffsetX, -0.0346999988f, 0, "Eye X offset (Horizontal distance).");
		}

	private:
		int stereorendering_renderWidth;
		int stereorendering_renderHeight;
		float stereorendering_fovH;
		float stereorendering_fovV;
		float stereorendering_fov;
		float stereorendering_aspectRatio;
		float stereorendering_frustumPlane_horizontalDistance;
		float stereorendering_frustumPlane_verticalDistance;
		float stereorendering_eyeOffsetX;
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(StereoRendering_51e1f27c07564114988a0420ad47a3e4, StereoRendering::StereoRenderingGem)

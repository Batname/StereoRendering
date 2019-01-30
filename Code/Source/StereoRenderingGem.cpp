

#include "StereoRendering_precompiled.h"

#include <platform_impl.h>
#include <IGem.h>
#include <AzFramework/Metrics/MetricsPlainTextNameRegistration.h>

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
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(StereoRendering_51e1f27c07564114988a0420ad47a3e4, StereoRendering::StereoRenderingGem)

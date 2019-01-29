

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <StereoRenderingSystemComponent.h>

namespace StereoRendering
{
    class StereoRenderingModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(StereoRenderingModule, "{69C02B45-6070-47A9-959D-C591484ACEC8}", AZ::Module);
        AZ_CLASS_ALLOCATOR(StereoRenderingModule, AZ::SystemAllocator, 0);

        StereoRenderingModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                StereoRenderingSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<StereoRenderingSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(StereoRendering_51e1f27c07564114988a0420ad47a3e4, StereoRendering::StereoRenderingModule)

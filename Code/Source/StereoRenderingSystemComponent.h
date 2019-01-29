
#pragma once

#include <AzCore/Component/Component.h>

#include <StereoRendering/StereoRenderingBus.h>

namespace StereoRendering
{
    class StereoRenderingSystemComponent
        : public AZ::Component
        , protected StereoRenderingRequestBus::Handler
    {
    public:
        AZ_COMPONENT(StereoRenderingSystemComponent, "{A12B254B-3E93-4870-A3B3-BED2AD7FBFF1}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // StereoRenderingRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}

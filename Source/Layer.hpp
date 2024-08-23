#pragma once

#include "Base.hpp"

namespace Factory
{
    class Layer : public Chicane::Layer
    {
    public:
        Layer(Chicane::Window* inWindow);
        ~Layer();

    public:
        void build() override;
        void destroy() override;
        void rebuild() override;

        void render(
            Chicane::Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;

    private:
        void initFrameDescriptorSetLayout();
        void initGraphicsPipeline();
        void initFramebuffers();
        void initFrameResources();
        void initVertexBuffers();

        void refreshViewport();

    private:
        Chicane::Renderer::Internals m_rendererInternals;

        std::unique_ptr<Chicane::GraphicsPipeline::Instance> m_graphicsPipeline;

        vk::DescriptorSet m_descriptorSet;
        Chicane::Descriptor::Bundle m_frameDescriptor;
        Chicane::Buffer::Instance m_vertexBuffer;

        Chicane::Vec<2, std::uint32_t> m_viewport;
    };
}
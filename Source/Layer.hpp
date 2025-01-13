#pragma once

#include "Base.hpp"

namespace Editor
{
    class Layer : public Chicane::Layer::Instance
    {
    public:
        Layer();
        ~Layer();

    public:
        void build() override;
        void destroy() override;
        void rebuild() override;

        void render(void* outData) override;

    private:
        void initFrameDescriptorSetLayout();
        std::vector<Chicane::Vulkan::Shader::StageCreateInfo> getGraphicsPipelineShaders();
        std::vector<vk::DescriptorSetLayout> getGraphicsPipelineDescriptorLayouts();
        std::vector<vk::AttachmentDescription> getGraphicsPipelineAttachments();
        void initGraphicsPipeline();
        void initFramebuffers();
        void initFrameResources();
        void initVertexBuffers();

        void refreshViewport();

    private:
        Chicane::Vulkan::Renderer::Internals                         m_internals;

        std::unique_ptr<Chicane::Vulkan::GraphicsPipeline::Instance> m_graphicsPipeline;

        vk::DescriptorSet                                            m_descriptorSet;
        Chicane::Vulkan::Descriptor::Bundle                          m_frameDescriptor;
        Chicane::Vulkan::Buffer::Instance                            m_vertexBuffer;

        std::vector<vk::ClearValue>                                  m_clearValues;
    };
}
#include "Layer.hpp"

namespace Factory
{
    Layer::Layer(Chicane::Window* inWindow)
        : Chicane::Layer("Editor"),
        m_renderer(inWindow->getRenderer())
    {
        m_isInitialized = true;
    }

    Layer::~Layer()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_renderer->m_logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        // Descriptors
        m_renderer->m_logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );

        // Buffers
        Chicane::Buffer::destroy(
            m_renderer->m_logicalDevice,
            m_vertexBuffer
        );
    }

    void Layer::build()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initFrameDescriptorSetLayout();
        initGraphicsPipeline();
        initFramebuffers();
        initFrameResources();
        initVertexBuffers();
    }

    void Layer::destroy()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_renderer->m_logicalDevice.waitIdle();

        m_renderer->m_logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void Layer::rebuild()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initFramebuffers();
        initFrameResources();
    }

    void Layer::render(
        Chicane::Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!m_isInitialized)
        {
            return;
        }

        // Renderpass
        std::vector<vk::ClearValue> clearValues;
        clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));
        clearValues.push_back(vk::ClearDepthStencilValue(1.f, 0u));

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        // Viewport
        m_renderer->updateViewport(inCommandBuffer);

        // Preparing
        inCommandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline->instance
        );

        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline->layout,
            0,
            outFrame.getDescriptorSet(m_id),
            nullptr
        );

        vk::Buffer vertexBuffers[] = { m_vertexBuffer.instance };
        vk::DeviceSize offsets[]   = { 0 };

        inCommandBuffer.bindVertexBuffers(
            0,
            1,
            vertexBuffers,
            offsets
        );

        inCommandBuffer.draw(
            6,
            1,
            0,
            0
        );

        inCommandBuffer.endRenderPass();

        return;
    }

    void Layer::initFrameDescriptorSetLayout()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Chicane::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 1;
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Chicane::Descriptor::initSetLayout(
            m_frameDescriptor.setLayout,
            m_renderer->m_logicalDevice,
            frameLayoutBidings
        );
    }

    void Layer::initGraphicsPipeline()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Chicane::GraphicsPipeline::CreateInfo createInfo = {};
        createInfo.canOverwrite         = false;
        createInfo.hasVertices          = false;
        createInfo.hasDepth             = true;
        createInfo.logicalDevice        = m_renderer->m_logicalDevice;
        createInfo.vertexShaderPath     = "Assets/Shaders/grid.vert.spv";
        createInfo.fragmentShaderPath   = "Assets/Shaders/grid.frag.spv";
        createInfo.swapChainExtent      = m_renderer->m_swapChain.extent;
        createInfo.swapChainImageFormat = m_renderer->m_swapChain.format;
        createInfo.depthFormat          = m_renderer->m_swapChain.images[0].depthFormat;
        createInfo.descriptorSetLayouts = { m_frameDescriptor.setLayout };

        m_graphicsPipeline = std::make_unique<Chicane::GraphicsPipeline::Instance>(createInfo);
    }

    void Layer::initFramebuffers()
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Chicane::Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            Chicane::Frame::Buffer::CreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_renderer->m_logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_renderer->m_swapChain.extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);
            framebufferCreateInfo.attachments.push_back(frame.depthImageView);

            Chicane::Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void Layer::initFrameResources()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Chicane::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size  = static_cast<uint32_t>(m_renderer->m_swapChain.images.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);

        Chicane::Descriptor::initPool(
            m_frameDescriptor.pool,
            m_renderer->m_logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Chicane::Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            // Scene
            vk::DescriptorSet sceneDescriptorSet;
            Chicane::Descriptor::initSet(
                sceneDescriptorSet,
                m_renderer->m_logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, sceneDescriptorSet);

            vk::WriteDescriptorSet cameraMatrixWriteInfo;
            cameraMatrixWriteInfo.dstSet          = sceneDescriptorSet;
            cameraMatrixWriteInfo.dstBinding      = 0;
            cameraMatrixWriteInfo.dstArrayElement = 0;
            cameraMatrixWriteInfo.descriptorCount = 1;
            cameraMatrixWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraMatrixWriteInfo.pBufferInfo     = &frame.cameraMatrixDescriptorBufferInfo;
            frame.addWriteDescriptorSet(cameraMatrixWriteInfo);
        }
    }

    void Layer::initVertexBuffers()
    {
        std::vector<float> vertices = {
            -1.0f, -1.0f,
            1.0f, 1.0f,
            -1.0f, 1.0f
        };

        Chicane::Buffer::CreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.physicalDevice   = m_renderer->m_physicalDevice;
        stagingBufferCreateInfo.logicalDevice    = m_renderer->m_logicalDevice;
        stagingBufferCreateInfo.size             = sizeof(float) * vertices.size();
        stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                   vk::MemoryPropertyFlagBits::eHostCoherent;

        Chicane::Buffer::Instance stagingBuffer;
        Chicane::Buffer::init(stagingBuffer, stagingBufferCreateInfo);

        void* writeLocation = m_renderer->m_logicalDevice.mapMemory(
            stagingBuffer.memory,
            0,
            stagingBufferCreateInfo.size
        );
        memcpy(
            writeLocation,
            vertices.data(),
            stagingBufferCreateInfo.size
        );
        m_renderer->m_logicalDevice.unmapMemory(stagingBuffer.memory);

        Chicane::Buffer::CreateInfo bufferCreateInfo;
        bufferCreateInfo.physicalDevice   = m_renderer->m_physicalDevice;
        bufferCreateInfo.logicalDevice    = m_renderer->m_logicalDevice;;
        bufferCreateInfo.size             = stagingBufferCreateInfo.size;
        bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                            vk::BufferUsageFlagBits::eVertexBuffer;
        bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

        Chicane::Buffer::init(m_vertexBuffer, bufferCreateInfo);

        Chicane::Buffer::copy(
            stagingBuffer,
            m_vertexBuffer,
            bufferCreateInfo.size,
            m_renderer->m_graphicsQueue,
            m_renderer->m_mainCommandBuffer
        );

        Chicane::Buffer::destroy(
            m_renderer->m_logicalDevice,
            stagingBuffer
        );
    }
}
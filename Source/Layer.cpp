#include "Layer.hpp"

namespace Factory
{
    Layer::Layer(Chicane::Window* inWindow)
        : Chicane::Layer("Editor"),
        m_rendererInternals(inWindow->getRendererInternals())
    {
        m_isInitialized = true;
    }

    Layer::~Layer()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_rendererInternals.logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        // Descriptors
        m_rendererInternals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );

        // Buffers
        Chicane::Buffer::destroy(
            m_rendererInternals.logicalDevice,
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

        refreshViewport();
    }

    void Layer::destroy()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_rendererInternals.logicalDevice.waitIdle();

        m_rendererInternals.logicalDevice.destroyDescriptorPool(
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

        refreshViewport();
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
        clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
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
            m_rendererInternals.logicalDevice,
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
        createInfo.canOverwrite         = true;
        createInfo.hasVertices          = true;
        createInfo.hasDepth             = true;
        createInfo.logicalDevice        = m_rendererInternals.logicalDevice;
        createInfo.vertexShaderPath     = "Content/Shaders/grid.vert.spv";
        createInfo.fragmentShaderPath   = "Content/Shaders/grid.frag.spv";
        createInfo.swapChainExtent      = m_rendererInternals.swapchain->extent;
        createInfo.swapChainImageFormat = m_rendererInternals.swapchain->format;
        createInfo.depthFormat          = m_rendererInternals.swapchain->depthFormat;
        createInfo.descriptorSetLayouts = { m_frameDescriptor.setLayout };

        m_graphicsPipeline = std::make_unique<Chicane::GraphicsPipeline::Instance>(createInfo);
    }

    void Layer::initFramebuffers()
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Chicane::Frame::Instance& frame : m_rendererInternals.swapchain->images)
        {
            Chicane::Frame::Buffer::CreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_rendererInternals.logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_rendererInternals.swapchain->extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);
            framebufferCreateInfo.attachments.push_back(frame.depth.imageView);

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
        descriptorPoolCreateInfo.size  = static_cast<uint32_t>(m_rendererInternals.swapchain->images.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);

        Chicane::Descriptor::initPool(
            m_frameDescriptor.pool,
            m_rendererInternals.logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Chicane::Frame::Instance& frame : m_rendererInternals.swapchain->images)
        {
            // Scene
            vk::DescriptorSet sceneDescriptorSet;
            Chicane::Descriptor::initSet(
                sceneDescriptorSet,
                m_rendererInternals.logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, sceneDescriptorSet);

            vk::WriteDescriptorSet cameraWriteInfo;
            cameraWriteInfo.dstSet          = sceneDescriptorSet;
            cameraWriteInfo.dstBinding      = 0;
            cameraWriteInfo.dstArrayElement = 0;
            cameraWriteInfo.descriptorCount = 1;
            cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraWriteInfo.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
            frame.addWriteDescriptorSet(cameraWriteInfo);
        }
    }

    void Layer::initVertexBuffers()
    {
        std::vector<float> vertices = {
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f
        };

        Chicane::Buffer::CreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.physicalDevice   = m_rendererInternals.physicalDevice;
        stagingBufferCreateInfo.logicalDevice    = m_rendererInternals.logicalDevice;
        stagingBufferCreateInfo.size             = sizeof(float) * vertices.size();
        stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                   vk::MemoryPropertyFlagBits::eHostCoherent;

        Chicane::Buffer::Instance stagingBuffer;
        Chicane::Buffer::init(stagingBuffer, stagingBufferCreateInfo);

        void* writeLocation = m_rendererInternals.logicalDevice.mapMemory(
            stagingBuffer.memory,
            0,
            stagingBufferCreateInfo.size
        );
        memcpy(
            writeLocation,
            vertices.data(),
            stagingBufferCreateInfo.size
        );
        m_rendererInternals.logicalDevice.unmapMemory(stagingBuffer.memory);

        Chicane::Buffer::CreateInfo bufferCreateInfo;
        bufferCreateInfo.physicalDevice   = m_rendererInternals.physicalDevice;
        bufferCreateInfo.logicalDevice    = m_rendererInternals.logicalDevice;;
        bufferCreateInfo.size             = stagingBufferCreateInfo.size;
        bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                            vk::BufferUsageFlagBits::eVertexBuffer;
        bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

        Chicane::Buffer::init(m_vertexBuffer, bufferCreateInfo);

        Chicane::Buffer::copy(
            stagingBuffer,
            m_vertexBuffer,
            bufferCreateInfo.size,
            m_rendererInternals.graphicsQueue,
            m_rendererInternals.mainCommandBuffer
        );

        Chicane::Buffer::destroy(
            m_rendererInternals.logicalDevice,
            stagingBuffer
        );
    }

    void Layer::refreshViewport()
    {
        m_viewport.x = static_cast<std::uint32_t>(Chicane::Grid::getSize("82vw"));
        m_viewport.y = static_cast<std::uint32_t>(Chicane::Grid::getSize("80vh"));

        Chicane::setViewport(m_viewport);
    }
}
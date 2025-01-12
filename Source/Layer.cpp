#include "Layer.hpp"

namespace Editor
{
    Layer::Layer()
        : Chicane::Layer::Instance("Editor"),
        m_rendererInternals(Chicane::Application::getRenderer<Chicane::Vulkan::Renderer>()->getInternals()),
        m_clearValues({})
    {
        m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        m_clearValues.push_back(vk::ClearDepthStencilValue(0.0f, 0));

        Chicane::Loader::getModelManager()->watchChanges(
            [this](Chicane::Manager::EventType inEvent)
            {
                if (inEvent != Chicane::Manager::EventType::Activation || !is(Chicane::Layer::Status::Offline))
                {
                    return;
                }

                setStatus(Chicane::Layer::Status::Initialized);

                build();
            }
        );
    }

    Layer::~Layer()
    {
        m_rendererInternals.logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        // Descriptors
        m_rendererInternals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );

        // Buffers
        Chicane::Vulkan::Buffer::destroy(
            m_rendererInternals.logicalDevice,
            m_vertexBuffer
        );
    }

    void Layer::build()
    {
        if (!is(Chicane::Layer::Status::Initialized))
        {
            return;
        }

        initFrameDescriptorSetLayout();
        initGraphicsPipeline();
        initFramebuffers();
        initFrameResources();
        initVertexBuffers();

        refreshViewport();

        setStatus(Chicane::Layer::Status::Running);
    }

    void Layer::destroy()
    {
        if (!is(Chicane::Layer::Status::Running))
        {
            return;
        }

        m_rendererInternals.logicalDevice.waitIdle();

        m_rendererInternals.logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );

        setStatus(Chicane::Layer::Status::Offline);
    }

    void Layer::rebuild()
    {
        if (!is(Chicane::Layer::Status::Offline))
        {
            return;
        }

        initFramebuffers();
        initFrameResources();

        refreshViewport();

        setStatus(Chicane::Layer::Status::Running);
    }

    void Layer::render(void* outData)
    {
        if (!is(Chicane::Layer::Status::Running))
        {
            return;
        }

        Chicane::Vulkan::Renderer::Data* data = (Chicane::Vulkan::Renderer::Data*) outData;

        vk::CommandBuffer& commandBuffer = data->commandBuffer;
        Chicane::Vulkan::Frame::Instance& frame = data->frame;

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
        renderPassBeginInfo.framebuffer         = frame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = data->swapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(m_clearValues.size());
        renderPassBeginInfo.pClearValues        = m_clearValues.data();

        commandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );
            commandBuffer.bindPipeline(
                vk::PipelineBindPoint::eGraphics,
                m_graphicsPipeline->instance
            );
            commandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                m_graphicsPipeline->layout,
                0,
                frame.getDescriptorSet(m_id),
                nullptr
            );

            vk::Buffer vertexBuffers[] = { m_vertexBuffer.instance };
            vk::DeviceSize offsets[]   = { 0 };

            commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
            commandBuffer.draw(6, 1, 0, 0);
        commandBuffer.endRenderPass();
    }

    void Layer::initFrameDescriptorSetLayout()
    {
        Chicane::Vulkan::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 1;
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Chicane::Vulkan::Descriptor::initSetLayout(
            m_frameDescriptor.setLayout,
            m_rendererInternals.logicalDevice,
            frameLayoutBidings
        );
    }

    void Layer::initGraphicsPipeline()
    {
        std::vector<vk::AttachmentDescription> attachments {};

        Chicane::Vulkan::GraphicsPipeline::Attachment colorAttachment {};
        colorAttachment.format        = m_rendererInternals.swapchain->format;
        colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
        colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
        colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

        Chicane::Vulkan::GraphicsPipeline::Attachment depthAttachment {};
        depthAttachment.format        = m_rendererInternals.swapchain->depthFormat;
        depthAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
        depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        Chicane::Vulkan::GraphicsPipeline::CreateInfo createInfo = {};
        createInfo.bHasVertices         = true;
        createInfo.bHasDepthWrite       = false;
        createInfo.bHasBlending         = true;
        createInfo.logicalDevice        = m_rendererInternals.logicalDevice;
        createInfo.vertexShaderPath     = "Content/Shaders/grid.vert.spv";
        createInfo.fragmentShaderPath   = "Content/Shaders/grid.frag.spv";
        createInfo.extent               = m_rendererInternals.swapchain->extent;
        createInfo.descriptorSetLayouts = { m_frameDescriptor.setLayout };
        createInfo.attachments           = {
            Chicane::Vulkan::GraphicsPipeline::createColorAttachment(colorAttachment),
            Chicane::Vulkan::GraphicsPipeline::createDepthAttachment(depthAttachment)
        };
        createInfo.polygonMode          = vk::PolygonMode::eFill;

        m_graphicsPipeline = std::make_unique<Chicane::Vulkan::GraphicsPipeline::Instance>(createInfo);
    }

    void Layer::initFramebuffers()
    {
        for (Chicane::Vulkan::Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            Chicane::Vulkan::Frame::Buffer::CreateInfo createInfo = {};
            createInfo.id              = m_id;
            createInfo.logicalDevice   = m_rendererInternals.logicalDevice;
            createInfo.renderPass      = m_graphicsPipeline->renderPass;
            createInfo.swapChainExtent = m_rendererInternals.swapchain->extent;
            createInfo.attachments.push_back(frame.imageView);
            createInfo.attachments.push_back(frame.depth.imageView);

            Chicane::Vulkan::Frame::Buffer::init(frame, createInfo);
        }
    }

    void Layer::initFrameResources()
    {
        Chicane::Vulkan::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size  = static_cast<uint32_t>(m_rendererInternals.swapchain->frames.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);

        Chicane::Vulkan::Descriptor::initPool(
            m_frameDescriptor.pool,
            m_rendererInternals.logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Chicane::Vulkan::Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            // Scene
            vk::DescriptorSet sceneDescriptorSet;
            Chicane::Vulkan::Descriptor::allocalteSet(
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

        Chicane::Vulkan::Buffer::CreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.physicalDevice   = m_rendererInternals.physicalDevice;
        stagingBufferCreateInfo.logicalDevice    = m_rendererInternals.logicalDevice;
        stagingBufferCreateInfo.size             = sizeof(float) * vertices.size();
        stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                   vk::MemoryPropertyFlagBits::eHostCoherent;

        Chicane::Vulkan::Buffer::Instance stagingBuffer;
        Chicane::Vulkan::Buffer::init(stagingBuffer, stagingBufferCreateInfo);

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

        Chicane::Vulkan::Buffer::CreateInfo bufferCreateInfo;
        bufferCreateInfo.physicalDevice   = m_rendererInternals.physicalDevice;
        bufferCreateInfo.logicalDevice    = m_rendererInternals.logicalDevice;;
        bufferCreateInfo.size             = stagingBufferCreateInfo.size;
        bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                            vk::BufferUsageFlagBits::eVertexBuffer;
        bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

        Chicane::Vulkan::Buffer::init(m_vertexBuffer, bufferCreateInfo);

        Chicane::Vulkan::Buffer::copy(
            stagingBuffer,
            m_vertexBuffer,
            bufferCreateInfo.size,
            m_rendererInternals.graphicsQueue,
            m_rendererInternals.mainCommandBuffer
        );

        Chicane::Vulkan::Buffer::destroy(
            m_rendererInternals.logicalDevice,
            stagingBuffer
        );
    }

    void Layer::refreshViewport()
    {
        Chicane::Renderer::Viewport viewport {};
        viewport.size.x = static_cast<std::uint32_t>(Chicane::Grid::getSize("82vw"));
        viewport.size.y = static_cast<std::uint32_t>(Chicane::Grid::getSize("80vh"));

        Chicane::Application::getRenderer()->setViewport(viewport);
    }
}
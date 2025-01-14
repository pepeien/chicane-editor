#include "Layer.hpp"

namespace Chicane
{
    namespace Editor
    {
        Layer::Layer()
            : Layer::Instance("Editor"),
            m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
            m_clearValues({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
            m_clearValues.push_back(vk::ClearDepthStencilValue(0.0f, 0));

            Loader::getModelManager()->watchChanges(
                [this](Manager::EventType inEvent)
                {
                    if (inEvent != Manager::EventType::Activation || !is(Chicane::Layer::Status::Offline))
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
            m_internals.logicalDevice.waitIdle();

            // Graphics Pipeline
            m_graphicsPipeline.reset();

            // Descriptors
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_frameDescriptor.setLayout
            );

            // Buffers
            Vulkan::Buffer::destroy(
                m_internals.logicalDevice,
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

            m_internals.logicalDevice.waitIdle();

            m_internals.logicalDevice.destroyDescriptorPool(
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

            Vulkan::Renderer::Data* data = (Vulkan::Renderer::Data*) outData;

            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Vulkan::Frame::Instance& frame = data->frame;

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
            Vulkan::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
            frameLayoutBidings.count = 1;
            frameLayoutBidings.indices.push_back(0);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Vulkan::Descriptor::initSetLayout(
                m_frameDescriptor.setLayout,
                m_internals.logicalDevice,
                frameLayoutBidings
            );
        }

        std::vector<Vulkan::Shader::StageCreateInfo> Layer::getGraphicsPipelineShaders()
        {
            Vulkan::Shader::StageCreateInfo vertexShader {};
            vertexShader.path = "Content/Shaders/grid.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Vulkan::Shader::StageCreateInfo fragmentShader {};
            fragmentShader.path = "Content/Shaders/grid.frag.spv";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<Vulkan::Shader::StageCreateInfo> result {};
            result.push_back(vertexShader);
            result.push_back(fragmentShader);

            return result;    
        }

        std::vector<vk::DescriptorSetLayout> Layer::getGraphicsPipelineDescriptorLayouts()
        {
            std::vector<vk::DescriptorSetLayout> result {};
            result.push_back(m_frameDescriptor.setLayout);

            return result;
        }

        std::vector<vk::AttachmentDescription> Layer::getGraphicsPipelineAttachments()
        {
            Vulkan::GraphicsPipeline::Attachment colorAttachment {};
            colorAttachment.format        = m_internals.swapchain->format;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            Vulkan::GraphicsPipeline::Attachment depthAttachment {};
            depthAttachment.format        = m_internals.swapchain->depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            std::vector<vk::AttachmentDescription> result {};
            result.push_back(Vulkan::GraphicsPipeline::createColorAttachment(colorAttachment));
            result.push_back(Vulkan::GraphicsPipeline::createDepthAttachment(depthAttachment));

            return result;
        }

        void Layer::initGraphicsPipeline()
        {
            Vulkan::GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.bHasVertices         = false;
            createInfo.bHasDepthWrite       = false;
            createInfo.bHasBlending         = true;
            createInfo.logicalDevice        = m_internals.logicalDevice;
            createInfo.shaders              = getGraphicsPipelineShaders();
            createInfo.extent               = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts = getGraphicsPipelineDescriptorLayouts();
            createInfo.attachments          = getGraphicsPipelineAttachments();
            createInfo.polygonMode          = vk::PolygonMode::eFill;

            m_graphicsPipeline = std::make_unique<Vulkan::GraphicsPipeline::Instance>(createInfo);
        }

        void Layer::initFramebuffers()
        {
            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                Vulkan::Frame::Buffer::CreateInfo createInfo = {};
                createInfo.id              = m_id;
                createInfo.logicalDevice   = m_internals.logicalDevice;
                createInfo.renderPass      = m_graphicsPipeline->renderPass;
                createInfo.swapChainExtent = m_internals.swapchain->extent;
                createInfo.attachments.push_back(frame.imageView);
                createInfo.attachments.push_back(frame.depth.imageView);

                Vulkan::Frame::Buffer::init(frame, createInfo);
            }
        }

        void Layer::initFrameResources()
        {
            Vulkan::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(m_internals.swapchain->frames.size());
            descriptorPoolCreateInfo.sizes = {
                { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = descriptorPoolCreateInfo.maxSets }
            };

            Vulkan::Descriptor::initPool(
                m_frameDescriptor.pool,
                m_internals.logicalDevice,
                descriptorPoolCreateInfo
            );

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                // Scene
                vk::DescriptorSet sceneDescriptorSet;
                Vulkan::Descriptor::allocalteSet(
                    sceneDescriptorSet,
                    m_internals.logicalDevice,
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
                cameraWriteInfo.pBufferInfo     = &frame.cameraResource.bufferInfo;
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

            Vulkan::Buffer::CreateInfo stagingBufferCreateInfo;
            stagingBufferCreateInfo.physicalDevice   = m_internals.physicalDevice;
            stagingBufferCreateInfo.logicalDevice    = m_internals.logicalDevice;
            stagingBufferCreateInfo.size             = sizeof(float) * vertices.size();
            stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                       vk::MemoryPropertyFlagBits::eHostCoherent;

            Vulkan::Buffer::Instance stagingBuffer;
            Vulkan::Buffer::init(stagingBuffer, stagingBufferCreateInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                stagingBufferCreateInfo.size
            );
            memcpy(
                writeLocation,
                vertices.data(),
                stagingBufferCreateInfo.size
            );
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            Vulkan::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.physicalDevice   = m_internals.physicalDevice;
            bufferCreateInfo.logicalDevice    = m_internals.logicalDevice;;
            bufferCreateInfo.size             = stagingBufferCreateInfo.size;
            bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                                vk::BufferUsageFlagBits::eVertexBuffer;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            Vulkan::Buffer::init(m_vertexBuffer, bufferCreateInfo);

            Vulkan::Buffer::copy(
                stagingBuffer,
                m_vertexBuffer,
                bufferCreateInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );

            Vulkan::Buffer::destroy(
                m_internals.logicalDevice,
                stagingBuffer
            );
        }

        void Layer::refreshViewport()
        {
            Renderer::Viewport viewport {};
            viewport.size.x = static_cast<std::uint32_t>(Grid::getSize("82vw"));
            viewport.size.y = static_cast<std::uint32_t>(Grid::getSize("80vh"));

            Application::getRenderer()->setViewport(viewport);
        }
    }
}
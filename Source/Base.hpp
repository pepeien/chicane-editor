#pragma once

#include "Chicane/Application.hpp"
#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    class AMesh : public Actor
    {
    public:
        AMesh()
            : Actor(),
            m_meshComponent(nullptr)
        {
            m_meshComponent = Application::getLevel()->createComponent<CMesh>();
            m_meshComponent->attachTo(this);
        }

    public:
        void setMesh(const std::string& inMesh)
        {
            m_meshComponent->setMesh(inMesh);
            m_meshComponent->activate();
        }

    private:
        CMesh* m_meshComponent;
    };
}
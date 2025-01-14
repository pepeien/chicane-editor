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
    class MeshActor : public Actor
    {
    public:
        MeshActor(const std::string& inMesh)
            : Actor(),
            m_meshComponent(new MeshComponent())
        {
            m_meshComponent->attachTo(this);
            m_meshComponent->setMesh(inMesh);
            m_meshComponent->activate();
        }

    private:
        MeshComponent* m_meshComponent;
    };
}
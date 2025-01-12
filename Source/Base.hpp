#pragma once

#include "Chicane/Application.hpp"
#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

class MeshActor : public Chicane::Actor
{
public:
    MeshActor(const std::string& inMesh)
        : Chicane::Actor(),
        m_meshComponent(new Chicane::MeshComponent())
    {
        m_meshComponent->attachTo(this);
        m_meshComponent->setMesh(inMesh);
        m_meshComponent->activate();
    }

private:
    Chicane::MeshComponent* m_meshComponent;
};
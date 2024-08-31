#pragma once

#include <filesystem>

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"

class MeshActor : public Chicane::Actor
{
public:
    MeshActor(const std::string& inMesh)
        : Chicane::Actor(),
        m_mesh(std::make_unique<Chicane::MeshComponent>())
    {
        m_mesh->attachTo(this);
        m_mesh->setMesh(inMesh);
        m_mesh->activate();
    }

private:
    std::unique_ptr<Chicane::MeshComponent> m_mesh;
};
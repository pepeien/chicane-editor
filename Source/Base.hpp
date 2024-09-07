#pragma once

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
        m_mesh(std::make_unique<Chicane::MeshComponent>()),
        m_rotationSpeed(
            Chicane::Vec<3, float>(
                (std::rand() % 10) * 0.05f,
                (std::rand() % 10) * 0.05f,
                (std::rand() % 10) * 0.05f
            )
        )
    {
        setCanTick(true);

        m_mesh->attachTo(this);
        m_mesh->setMesh(inMesh);
        m_mesh->activate();
    }

public:
    void onTick(float inDelta) override
    {
        setRelativeRotation(m_rotationSpeed);
    }

private:
    std::unique_ptr<Chicane::MeshComponent> m_mesh;

    Chicane::Vec<3, float> m_rotationSpeed;
};
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
        m_moveSwitch(1),
        m_rotationSpeed((std::rand() % 10) * 0.1f)
    {
        setCanTick(true);

        m_mesh->attachTo(this);
        m_mesh->setMesh(inMesh);
        m_mesh->activate();
    }

public:
    void onTick(float inDelta) override
    {
        if (m_transform.translation.x > 100.0f || m_transform.translation.x < -100.0f)
        {
            m_moveSwitch *= -1;
        }

        sidePosition.x += 0.01f * m_moveSwitch;

        setAbsoluteTranslation(sidePosition);
        setRelativeRotation(
            Chicane::Vec<3, float>(
                m_rotationSpeed,
                0.0f,
                0.0f
            )
        );
    }

public:
    Chicane::Vec<3, float> sidePosition;

private:
    std::unique_ptr<Chicane::MeshComponent> m_mesh;

    int m_moveSwitch;

    float m_rotationSpeed;
};
#pragma once
#include <Jopnal/Jopnal.hpp>


namespace jd
{
    class Spawner : public jop::Component
    {
        float m_elapsed;
        jop::Renderer& m_rend;
        bool m_spawning;

    public:

        Spawner(jop::Object& obj, jop::Renderer& rend)
            : jop::Component(obj, 0),
              m_elapsed(0.f),
              m_rend(rend),
              m_spawning(false)
        {
            using namespace jop;
            using RM = ResourceManager;

            auto root = getObject();
            root->setActive(false);

            auto& baseMat = RM::getEmpty<Material>("spawnbasemat");
            baseMat.setLightingModel(Material::LightingModel::Gouraud).setMap(Material::Map::Diffuse0, Texture2D::getDefault()).setShininess(128).setReflection(Material::Reflection::Specular, Color::White);

            auto& baseMesh = RM::getNamed<SphereMesh>("spawnbasemesh", 0.35f, 20);

            const glm::vec3 basePos(2.f, 0.f, 2.f);

            for (std::size_t i = 0; i < 4; ++i)
            {
                static const glm::vec2 posMult[] =
                {
                    glm::vec2(-1.f, -1.f),
                    glm::vec2(1.f, -1.f),
                    glm::vec2(1.f, 1.f),
                    glm::vec2(-1.f, 1.f)
                };

                auto child = root->createChild(std::to_string(i));

                child->setPosition(basePos.x * posMult[i].x, basePos.y, basePos.z * posMult[i].y);
                child->setIgnoreTransform(Object::Rotation);

                child->createComponent<Drawable>(rend).setModel(baseMesh, baseMat);
            }
        }

        void update(const float deltaTime) override
        {
            getObject()->rotate(0.f, deltaTime * 0.4f, 0.f);

            if (m_spawning && (m_elapsed += deltaTime) > 0.25f)
            {
                jop::Randomizer r;

                auto& root = getObject()->getChildren()[r.range<int>(0, 3)];
                auto& point = root.getGlobalPosition();

                auto& mat = jop::ResourceManager::getExisting<jop::Material>("spawnbasemat");
                auto& mesh = jop::ResourceManager::getExisting<jop::SphereMesh>("spawnbasemesh");

                auto newObj = root.createChild("asdf");

                newObj->setPosition(point);
                newObj->scale(0.5f);
                newObj->setIgnoreParent(true);
                newObj->createComponent<jop::Drawable>(m_rend).setModel(mesh, mat);

                jop::RigidBody::ConstructInfo info(jop::ResourceManager::getNamed<jop::SphereShape>("asdfshape", 0.35f * newObj->getLocalScale().x), jop::RigidBody::Type::Dynamic, 1.f);
                info.restitution = 1.f;
                newObj->createComponent<jop::RigidBody>(getObject()->getScene().getWorld<3>(), info);

                m_elapsed = 0.f;

                struct Deleter : public jop::Component
                {
                    Deleter(jop::Object& obj)
                        : jop::Component(obj, 0)
                    {}

                    void update(const float)
                    {
                        auto& pos = getObject()->getGlobalPosition();

                        if (std::abs(pos.x) > 25.f || std::abs(pos.z) > 25.f)
                            getObject()->removeSelf();
                    }
                };

                newObj->createComponent<Deleter>();
            }
        }

        void toggleSpawning()
        {
            m_spawning = !m_spawning;
        }
    };
}
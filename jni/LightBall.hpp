#pragma once
#include <Jopnal/Jopnal.hpp>


namespace jd
{
    class LightBall : public jop::Component
    {
        jop::Material& m_mat;
        const jop::SphereMesh& m_mesh;
        jop::Renderer& m_rend;
        float m_elapsed;
        jop::SoundEffect* m_sound;
        bool m_spawning;

    public:

        LightBall(jop::Object& obj, jop::Renderer& rend)
            : jop::Component(obj, 0),
              m_mat(jop::ResourceManager::getEmpty<jop::Material>("lightballmat")),
              m_mesh(jop::ResourceManager::getNamed<jop::SphereMesh>("lightballmesh", 0.5f, 25)),
              m_rend(rend),
              m_elapsed(0.f),
              m_spawning(false)
        {
            using namespace jop;

            auto root = getObject();

            root->createComponent<Drawable>(rend).setModel(Model(m_mesh, m_mat));

            root->createComponent<LightSource>(rend, LightSource::Type::Point).setAttenuation(0.f).setIntensity(Color::Black, Color::Green, Color::Green).setCastShadows(true);

            m_sound = &root->createComponent<SoundEffect>();
            m_sound->setBuffer(ResourceManager::get<SoundBuffer>("flash.ogg"))
                .setSpatialization(true)
                .setAttenuation(0.8f)
                .setVolume(100.f)
                .setMinDistance(2.5f);

            root->setActive(false);
        }

        void update(const float deltaTime) override
        {
            using namespace jop;

            if (Engine::getCurrentScene().getDeltaScale() >= 10.f)
            {
                static float elapse = 0.f;
                static unsigned int currIndex = 1;
                elapse += deltaTime;

                if (elapse > 20.f)
                {
                    if (++currIndex > 2)
                        currIndex = 0;

                    Color col(Color::Black);
                    col.colors[currIndex] = 1.f;
                    col *= 5.f;

                    getObject()->getComponent<LightSource>()->setIntensity(Color::Black, col * 0.2f, col * 0.2f);

                    elapse -= 20.f;
                }
            }
            else
            {
                getObject()->getComponent<LightSource>()->setIntensity(Color::Black, Color::Green, Color::Green);
            }

            if (m_spawning && (m_elapsed += deltaTime) > 1.f)
            {
                Randomizer r;
                auto randPointXZ = r.insideCircle(25.f);
                auto randHeight = r.range(-2.5f, 2.5f);

                getObject()->createChild("asdf")->setScale(0.25f).setIgnoreTransform(Object::Scale).setPosition(randPointXZ.x, randHeight, randPointXZ.y).createComponent<Drawable>(m_rend).setModel(Model(m_mesh, m_mat));

                m_elapsed = 0.f;
            }

            for (auto& i : getObject()->getChildren())
            {
                auto d = i.getComponent<Drawable>();

                auto dist = glm::distance(getObject()->getGlobalPosition(), i.getGlobalPosition());

                i.move(glm::min(glm::normalize(getObject()->getGlobalPosition() - i.getGlobalPosition()) * 1.f * deltaTime * 
                       (25.f / dist * 2.f), dist));

                if (glm::distance(getObject()->getGlobalPosition(), i.getGlobalPosition()) < 0.5f * getObject()->getLocalScale().x)
                {
                    i.removeSelf();
                    getObject()->scale(1.5f);
                    m_sound->playReset();
                }
            }

            const float scaleOffset = std::max(0.f, getObject()->getLocalScale().x - 1.f);
            getObject()->setScale(std::min(4.f, 1.f + (scaleOffset - (0.5f * scaleOffset) * deltaTime * 2.f)));

            getObject()->getComponent<LightSource>()->setAttenuation(50.f * scaleOffset);
        }

        void toggleSpawning()
        {
            m_spawning = !m_spawning;
        }
    };
}
#pragma once
#include <Jopnal/Graphics.hpp>


namespace jd
{
    class LoadingScene : public jop::Scene
    {
    public:

        LoadingScene()
            : jop::Scene(typeid(LoadingScene).name())
        {
            using RM = jop::ResourceManager;
            using MAT = jop::Material::Attribute;

            // Camera
            {
                createChild("cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic);
            }

            // Logo
            {
                createChild("logo")->move(-0.5f, 0.f, 0.f).createComponent<jop::Drawable>(getRenderer(), jop::RenderPass::Pass::AfterPost).getModel()

                    .setMesh(RM::getNamed<jop::RectangleMesh>("logoMesh", 0.5f))
                    .setMaterial(RM::getEmpty<jop::Material>("logoMat", true)
                    .setMap(jop::Material::Map::Diffuse, RM::get<jop::Texture2D>("jop.png"))
                    /*.setReflection(jop::Color(0x222222FF), jop::Color::White, jop::Color::Black, jop::Color::Black)*/);
            }

            // Lights
            {
                auto rotator = findChild("logo")->createChild("lightrot");

                rotator->createChild("left")->setPosition(5.f, 0.f, 0.f).createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Point)
                    .setAttenuation(30.f)
                    .setIntensity(jop::Color::White, jop::Color::White, jop::Color::Black);

                rotator->createChild("right")->setPosition(-5.f, 0.f, 0.f).createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Point)
                    .setAttenuation(10.f)
                    .setIntensity(jop::Color::White, jop::Color(50.f, 50.f, 50.f), jop::Color::Black);
            }

            // Text
            {
                createChild("text")->move(0.f, 0.f, 0.f).setScale(0.001f).createComponent<jop::Text>(getRenderer(), jop::RenderPass::Pass::AfterPost)

                    .setFont(RM::get<jop::Font>("Furore.ttf", 64))
                    .setString("please stand by")
                    .setColor(jop::Color(0x1C8EFFFF));
            }

            // Audio
            {
                //auto& stream = createComponent<jop::SoundStream>();
                //stream.setPath("pulse2.wav");
                //stream.setLoop(true).play().setPitch(0.82f);
            }
        }

        ~LoadingScene() override
        {
        }

        void postUpdate(const float deltaTime) override
        {
            if (jop::Engine::newSceneReady())
            {
                auto text = findChild("text")->getComponent<jop::Text>();

                const jop::Color newColor(text->getColor().colors, text->getColor().alpha - deltaTime * 1.25f);

                //getComponent<jop::SoundStream>()->setVolume(100.f * text->getColor().alpha);

                if (newColor.alpha <= 0.f)
                {
                    jop::Engine::signalNewScene();
                    jop::ResourceManager::endLoadPhase(10);
                    return;
                }
                
                text->setColor(newColor);
                auto& mat = jop::ResourceManager::getExisting<jop::Material>("logoMat");
            }

            findChild("lightrot", true)->rotate(0.f, deltaTime * glm::half_pi<float>(), 0.f);
        }
    };
}
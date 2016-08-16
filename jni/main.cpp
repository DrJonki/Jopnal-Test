#if 1

#include "DemoScene.hpp"
#include "LoadingScene.hpp"

int main(int c, char* v[])
{
#ifdef JOP_OS_ANDROID
    jop::SettingManager::setDefaultDirectory("defconf");
    jop::SettingManager::setOverrideWithDefaults();
#endif
    JOP_ENGINE_INIT("JopTestProject", c, v);
    
    jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Frozen);

    jop::Engine::createScene<jd::LoadingScene>();
    jop::Engine::createScene<jd::DemoScene, true, true>();

    return JOP_MAIN_LOOP;
}

#else

#include <Jopnal/Jopnal.hpp>

class MyScene : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_object;
    jop::Material* newMaterial;

public:

    MyScene()
        : jop::Scene("MyScene"),
        m_object()
    {
        createChild("cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective);

        m_object = createChild("box");
        m_object->createComponent<jop::Drawable>(getRenderer());
        m_object->setPosition(0.f, 0.f, -2.5f);


        // Create an object with a directional light component
        auto light = createChild("light");
        light->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Directional).setIntensity(jop::LightSource::Intensity::Ambient, jop::Color::Gray);

        // Move the light to the right and set it to point to the left
        // Notice that the rotation is expected to be in radians
        light->setPosition(5.f, 0.f, 0.f).setRotation(0.f, glm::radians(90.f), 0.f);

        auto drawable = m_object->getComponent<jop::Drawable>();

        // To modify the drawable's material, we must create a new one to replace the default
        newMaterial = &jop::ResourceManager::getEmpty<jop::Material>("newMaterial", false).setAttributes(jop::Material::Attribute::DefaultLighting);
        drawable->getModel().setMaterial(*newMaterial);//.setMesh(jop::ResourceManager::getNamed<jop::SphereMesh>("ball", 0.5f, 20));

        // Set the diffuse reflection. This will automatically enable lighting
        newMaterial->setReflection(jop::Material::Reflection::Ambient, jop::Color::Gray);
        newMaterial->setReflection(jop::Material::Reflection::Diffuse, jop::Color::White);
        //newMaterial->setReflection(jop::Material::Reflection::Solid, jop::Color::Gray);

    }

    void preUpdate(const float deltaTime) override
    {
        m_object->rotate(0.5f * deltaTime, 1.f * deltaTime, 0.f);

        static float count = 0.f;
        //static bool toggle = true;
        count += deltaTime;

        //if (count >= 1.f)
        //{
            //newMaterial->setReflection(jop::Material::Reflection::Solid, jop::Color::White * ((std::sin(count * 8.f) + 1.f) / 3.f));
            //JOP_DEBUG_INFO(newMaterial->getAttributeField());

            //toggle = !toggle;
            //count = 0.f;
        //}
    }
};

int main(int argc, char* argv[])
{
#ifdef JOP_OS_ANDROID
    jop::SettingManager::setDefaultDirectory("defconf");
    jop::SettingManager::setOverrideWithDefaults();
#endif

    JOP_ENGINE_INIT("MyProject", argc, argv);

    jop::Engine::createScene<MyScene>();

    return JOP_MAIN_LOOP;
}

#endif
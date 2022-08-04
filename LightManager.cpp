#include "LightManager.h"
#include "Light.h"

LightManager::LightManager(QObject *parent)
    : QObject(parent)
    , mActiveLight(nullptr)
{}

void LightManager::addLight(Light *light)
{
    mLights << light;
}

void LightManager::removeLight(Light *light)
{
    if (light)
    {
        if (light == mActiveLight)
        {
            mActiveLight = nullptr;
        }

        mLights.removeAll(light);
        light->deleteLater();
    }
}

Light *LightManager::activeLight() const
{
    return mActiveLight;
}

void LightManager::setActiveLight(Light *newActiveLight)
{
    mActiveLight = newActiveLight;
}

LightManager *LightManager::instance()
{
    static LightManager instance;

    return &instance;
}

const QList<Light *> &LightManager::lights() const
{
    return mLights;
}

#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "Light.h"

#include <QObject>

class LightManager : public QObject
{
    Q_OBJECT

private:
    explicit LightManager(QObject *parent = nullptr);

public:
    void addLight(Light *light);
    void removeLight(Light *light);

    const QList<Light *> &lights() const;

    Light *activeLight() const;
    void setActiveLight(Light *newActiveLight);

    static LightManager *instance();

private:
    QList<Light *> mLights;
    Light *mActiveLight;
};

#endif // LIGHTMANAGER_H

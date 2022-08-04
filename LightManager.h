#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <QObject>

class LightManager : public QObject
{
    Q_OBJECT

private:
    friend class Light;

    explicit LightManager(QObject *parent = nullptr);

    void addLight(Light *light);
    void removeLight(Light *light);

    QList<Light *> mLights;
    Light *mActiveLight;

public:
    static LightManager *instance();
    const QList<Light *> &lights() const;
    Light *activeLight() const;
    void setActiveLight(Light *newActiveLight);
};

#endif // LIGHTMANAGER_H

#ifndef LIGHT_H
#define LIGHT_H

#include "Node.h"

class Light : public Node
{
private:
    friend class LightManager;

    explicit Light(QObject *parent = nullptr);
    virtual ~Light();

public:
    static Light *create();
    void remove();

    const QVector4D &color() const;
    void setColor(const QVector4D &newColor);

    float ambient() const;
    void setAmbient(float newAmbient);

    float diffuse() const;
    void setDiffuse(float newDiffuse);

    float specular() const;
    void setSpecular(float newSpecular);

private:
    QVector4D mColor;
    float mAmbient;
    float mDiffuse;
    float mSpecular;

    static LightManager *LIGHT_MANAGER;
};

#endif // LIGHT_H

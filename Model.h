#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Node.h"

#include <QOpenGLTexture>

class Model : public Node
{
private:
    friend class ModelManager;

    explicit Model(QObject *parent = nullptr);
    virtual ~Model();

public:
    enum Type
    { //
        Capsule,
        Cone,
        Cube,
        Cylinder,
        Dome,
        Plane,
        Pyramid,
        Sphere,
        Tetrahedron,
        Torus,
        TorusKnot,
    };

    Type type() const;
    void setType(Type newType);

    Material &material();
    void setMaterial(const Material &newMaterial);

    static Model *create(Type type);
    void remove();

    static const QVector<Model::Type> ALL_MODEL_TYPES;

protected:
    Type mType;
    Material mMaterial;

    static ModelManager *MODEL_MANAGER;
};

#endif // MODEL_H

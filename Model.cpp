#include "Model.h"

#include "ModelManager.h"

Model::Model(QObject *parent)
    : Node{parent}
{}

Model::~Model() {}

Model::Type Model::type() const
{
    return mType;
}

void Model::setType(Type newType)
{
    mType = newType;
}

Material &Model::material()
{
    return mMaterial;
}

void Model::setMaterial(const Material &newMaterial)
{
    mMaterial = newMaterial;
}

Model *Model::create(Type type)
{
    Model *model = new Model;
    model->setType(type);
    MODEL_MANAGER->addModel(model);

    return model;
}

void Model::remove()
{
    MODEL_MANAGER->removeModel(this);
}

const QVector<Model::Type> Model::ALL_MODEL_TYPES = {
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

ModelManager *Model::MODEL_MANAGER = ModelManager::instance();

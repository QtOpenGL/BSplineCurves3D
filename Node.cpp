#include "Node.h"

Node::Node(QObject *parent)
    : QObject{parent}
    , mScale(1, 1, 1)
{}

Node::~Node() {}

const QVector3D &Node::position() const
{
    return mPosition;
}

void Node::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;
}

const QQuaternion &Node::rotation() const
{
    return mRotation;
}

void Node::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation;
}

const QVector3D &Node::scale() const
{
    return mScale;
}

void Node::setScale(const QVector3D &newScale)
{
    mScale = newScale;
}

QMatrix4x4 Node::transformation() const
{
    QMatrix4x4 transformation;
    transformation.scale(mScale);
    transformation.rotate(mRotation);
    transformation.setColumn(3, QVector4D(mPosition, 1.0f));

    return transformation;
}

#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QObject>
#include <QQuaternion>
#include <QVector3D>

class Node : public QObject
{
    Q_OBJECT
protected:
    explicit Node(QObject *parent = nullptr);
    virtual ~Node();

public:
    virtual const QVector3D &position() const;
    virtual void setPosition(const QVector3D &newPosition);

    virtual const QQuaternion &rotation() const;
    virtual void setRotation(const QQuaternion &newRotation);

    virtual const QVector3D &scale() const;
    virtual void setScale(const QVector3D &newScale);

    virtual QMatrix4x4 transformation() const;

protected:
    QVector3D mPosition;
    QQuaternion mRotation;
    QVector3D mScale;
};

#endif // NODE_H

#ifndef CAMERA_H
#define CAMERA_H

#include "Node.h"

#include <QMatrix4x4>
#include <QMouseEvent>

class Camera : public Node
{
    Q_OBJECT
protected:
    explicit Camera(QObject *parent = nullptr);

public:
    virtual float verticalFov() const;
    virtual void setVerticalFov(float newVerticalFov);

    virtual float horizontalFov() const;
    virtual void setHorizontalFov(float newHorizontalFov);

    virtual float aspectRatio() const;
    virtual void setAspectRatio(float newAspectRatio);

    virtual float zNear() const;
    virtual void setZNear(float newZNear);

    virtual float zFar() const;
    virtual void setZFar(float newZFar);

    virtual QMatrix4x4 transformation() const;
    virtual QMatrix4x4 projection() const;
    virtual QVector3D getViewDirection() const;

    virtual void keyPressed(QKeyEvent *) = 0;
    virtual void keyReleased(QKeyEvent *) = 0;
    virtual void mousePressed(QMouseEvent *) = 0;
    virtual void mouseReleased(QMouseEvent *) = 0;
    virtual void mouseMoved(QMouseEvent *) = 0;
    virtual void update(float) = 0;

protected:
    float mVerticalFov;
    float mHorizontalFov;
    float mAspectRatio;
    float mZNear;
    float mZFar;
};

#endif // CAMERA_H

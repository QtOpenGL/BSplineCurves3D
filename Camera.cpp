#include "Camera.h"

#include <QTransform>
#include <QtMath>

Camera::Camera(QObject *parent)
    : QObject(parent)
    , mPosition(0.0f, 0.0f, 0.0f)
    , mAspectRatio(1.0f)
    , mZNear(0.1f)
    , mZFar(10000.0f)
{
    mTransformation.setToIdentity();
    mProjection.setToIdentity();

    setVerticalFov(60.0f);
}

const QVector3D &Camera::position() const
{
    return mPosition;
}

void Camera::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;
    updateTransformation();
}

const QQuaternion &Camera::rotation() const
{
    return mRotation;
}

void Camera::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation;
    updateTransformation();
}

float Camera::horizontalFov() const
{
    return mHorizontalFov;
}

void Camera::setHorizontalFov(float newHorizontalFov)
{
    mHorizontalFov = newHorizontalFov;

    float halfHorizontalFovRadian = 0.5f * qDegreesToRadians(mHorizontalFov);
    float verticalFovRadian = 2 * atan(tan(halfHorizontalFovRadian) * mAspectRatio);

    mVerticalFov = qRadiansToDegrees(verticalFovRadian);
    updateProjection();
}

float Camera::verticalFov() const
{
    return mVerticalFov;
}

void Camera::setVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;

    float halfVerticalFovRadian = 0.5f * qDegreesToRadians(mVerticalFov);
    float horizontalFovRadian = 2 * atan(tan(halfVerticalFovRadian) / mAspectRatio);

    mHorizontalFov = qRadiansToDegrees(horizontalFovRadian);
    updateProjection();
}

float Camera::aspectRatio() const
{
    return mAspectRatio;
}

void Camera::setAspectRatio(float newAspectRatio)
{
    mAspectRatio = newAspectRatio;
    updateProjection();
}

float Camera::zNear() const
{
    return mZNear;
}

void Camera::setZNear(float newZNear)
{
    mZNear = newZNear;
    updateProjection();
}

float Camera::zFar() const
{
    return mZFar;
}

void Camera::setZFar(float newZFar)
{
    mZFar = newZFar;
    updateProjection();
}

const QMatrix4x4 &Camera::projection() const
{
    return mProjection;
}

QVector3D Camera::getViewDirection()
{
    return mRotation * QVector3D(0, 0, -1);
}

void Camera::onKeyPressed(QKeyEvent *)
{

}

void Camera::onKeyReleased(QKeyEvent *)
{

}

void Camera::onMousePressed(QMouseEvent *)
{

}

void Camera::onMouseReleased(QMouseEvent *)
{

}

void Camera::onMouseMoved(QMouseEvent *)
{

}

void Camera::update(float)
{

}

const QMatrix4x4 &Camera::transformation() const
{
    return mTransformation;
}

void Camera::updateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.rotate(mRotation.conjugated());
    mTransformation.translate(-mPosition);
}

void Camera::updateProjection()
{
    mProjection.setToIdentity();
    mProjection.perspective(mVerticalFov, mAspectRatio, mZNear, mZFar);
}

#include "CameraManager.h"
#include <QtMath>

CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
    , mActiveCamera(nullptr)
{}

void CameraManager::addCamera(Camera *camera)
{
    mCameras << camera;
}

void CameraManager::removeCamera(Camera *camera)
{
    if (camera)
    {
        if (camera == mActiveCamera)
            mActiveCamera = nullptr;

        mCameras.removeAll(camera);
        camera->deleteLater();
    }
}

Camera *CameraManager::activeCamera() const
{
    return mActiveCamera;
}

void CameraManager::setActiveCamera(Camera *newActiveCamera)
{
    mActiveCamera = newActiveCamera;
}

void CameraManager::onKeyPressed(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onKeyPressed(event);
}

void CameraManager::onKeyReleased(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onKeyReleased(event);
}

void CameraManager::onMousePressed(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onMousePressed(event);
}

void CameraManager::onMouseReleased(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onMouseReleased(event);
}

void CameraManager::onMouseMoved(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onMouseMoved(event);
}

void CameraManager::update(float ifps)
{
    if (mActiveCamera)
        mActiveCamera->update(ifps);
}

QVector3D CameraManager::getViewDirection() const
{
    if (mActiveCamera)
        return mActiveCamera->getViewDirection();

    return QVector3D();
}

QVector3D CameraManager::getDirectionFromScreen(int x, int y, int width, int height) const
{
    if (mActiveCamera)
    {
        const float halfVerticalFovRadian = 0.5f * qDegreesToRadians(mActiveCamera->verticalFov());
        const float halfHorizontalFovRadian = atan(tan(halfVerticalFovRadian) * mActiveCamera->aspectRatio());

        const float horizontalRotationAngleRadian = atan(tan(halfHorizontalFovRadian) * (0.5f * width - x) / (0.5f * width));
        const float horizontalRotationAngle = qRadiansToDegrees(horizontalRotationAngleRadian);

        const float verticalRotationAngleRadian = atan(((0.5f * height - y) / (0.5f * width - x)) * sin(horizontalRotationAngleRadian));
        const float verticalRotationAngle = qRadiansToDegrees(verticalRotationAngleRadian);

        QQuaternion left = mActiveCamera->rotation() * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), horizontalRotationAngle);
        QQuaternion leftThenUp = left * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), verticalRotationAngle);

        return leftThenUp * QVector3D(0, 0, -1);
    }

    return QVector3D();
}

CameraManager *CameraManager::instance()
{
    static CameraManager instance;
    return &instance;
}

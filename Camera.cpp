#include "Camera.h"

#include <QTransform>
#include <QtMath>

Camera::Camera(QObject *parent)
    : QObject(parent)
    , mVerticalFov(60.0f)
    , mAspectRatio(1.0f)
    , mZNear(0.1f)
    , mZFar(10000.0f)
    , mMovementSpeed(0.0125f)
    , mAngularSpeed(0.125f)
    , mMousePressed(false)
    , mMousePreviousX(0.0f)
    , mMousePreviousY(0.0f)
    , mMouseDeltaX(0.0f)
    , mMouseDeltaY(0.0f)
    , mUpdateRotation(true)
    , mUpdatePosition(true)
{
    mTransformation.setToIdentity();
    mProjection.setToIdentity();

    connect(&mTimer, &QTimer::timeout, this, &Camera::update);
    mTimer.start(5);
}

void Camera::onKeyPressed(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
    mUpdatePosition = true;
}

void Camera::onKeyReleased(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void Camera::onMousePressed(QMouseEvent *event)
{
    mMousePreviousX = event->x();
    mMousePreviousY = event->y();
    mMousePressed = true;
}

void Camera::onMouseReleased(QMouseEvent *)
{
    mMousePressed = false;
}

void Camera::onMouseMoved(QMouseEvent *event)
{
    if (mMousePressed)
    {
        mMouseDeltaX += mMousePreviousX - event->x();
        mMouseDeltaY += mMousePreviousY - event->y();

        mMousePreviousX = event->x();
        mMousePreviousY = event->y();
        mUpdateRotation = true;
    }
}

QVector3D Camera::getDirectionFromScreen(int x, int y, int width, int height)
{
    const float halfVerticalFovRadian = 0.5f * qDegreesToRadians(mVerticalFov);
    const float halfHorizontalFovRadian = atan(tan(halfVerticalFovRadian) * mAspectRatio);

    const float horizontalRotationAngleRadian = atan(tan(halfHorizontalFovRadian) * (0.5f * width - x) / (0.5f * width));
    const float horizontalRotationAngle = qRadiansToDegrees(horizontalRotationAngleRadian);

    const float verticalRotationAngleRadian = atan(((0.5f * height - y) / (0.5f * width - x)) * sin(horizontalRotationAngleRadian));
    const float verticalRotationAngle = qRadiansToDegrees(verticalRotationAngleRadian);

    QQuaternion rotation = mRotation * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), horizontalRotationAngle);
    rotation = rotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), verticalRotationAngle);

    return rotation * QVector3D(0, 0, -1);
}

QVector3D Camera::getViewDirection()
{
    return mRotation * QVector3D(0, 0, -1);
}

void Camera::update()
{
    // Rotation
    if (mUpdateRotation)
    {
        mRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mAngularSpeed * mMouseDeltaX) * mRotation;
        mRotation = mRotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mAngularSpeed * mMouseDeltaY);

        mMouseDeltaY = 0.0f;
        mMouseDeltaX = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        const QList<Qt::Key> keys = mPressedKeys.keys();

        if (mPressedKeys[Qt::Key_Shift])
            mMovementSpeed = 0.25f;
        else if (mPressedKeys[Qt::Key_Control])
            mMovementSpeed = 0.0125f;
        else
            mMovementSpeed = 0.05f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                mPosition += mMovementSpeed * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0)));
    }

    if (mUpdatePosition | mUpdateRotation)
    {
        mTransformation.setToIdentity();
        mTransformation.rotate(mRotation.conjugated());
        mTransformation.translate(-mPosition);
    }

    if (mPressedKeys.empty())
    {
        mUpdatePosition = false;
    }
}

void Camera::updateProjection()
{
    mProjection.setToIdentity();
    mProjection.perspective(mVerticalFov, mAspectRatio, mZNear, mZFar);
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

float Camera::zNear() const
{
    return mZNear;
}

void Camera::setZNear(float newZNear)
{
    mZNear = newZNear;
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

float Camera::verticalFov() const
{
    return mVerticalFov;
}

void Camera::setVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;
    updateProjection();
}

float Camera::horizontalFov() const
{
    const float halfVerticalFovRadian = 0.5f * qDegreesToRadians(mVerticalFov);
    const float horizontalFovRadian = 2 * atan(tan(halfVerticalFovRadian) / mAspectRatio);
    return qRadiansToDegrees(horizontalFovRadian);
}

const QMatrix4x4 &Camera::projection() const
{
    return mProjection;
}

QVector3D Camera::position() const
{
    return mPosition;
}

void Camera::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;
    mUpdatePosition = true;
}

QQuaternion Camera::rotation() const
{
    return mRotation;
}

void Camera::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation;
    mUpdateRotation = true;
}

const QMatrix4x4 &Camera::transformation()
{
    return mTransformation;
}

const QMap<Qt::Key, QVector3D> Camera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};

#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "Camera.h"

#include <QObject>

class FreeCamera : public Camera
{
    Q_OBJECT

private:
    friend class CameraManager;

    explicit FreeCamera(QObject *parent = nullptr);

    void onKeyPressed(QKeyEvent *event) override;
    void onKeyReleased(QKeyEvent *event) override;
    void onMousePressed(QMouseEvent *event) override;
    void onMouseReleased(QMouseEvent *event) override;
    void onMouseMoved(QMouseEvent *event) override;
    void update(float ifps) override;

    QMap<Qt::Key, bool> mPressedKeys;

    float mMovementSpeed;
    float mAngularSpeed;

    bool mMousePressed;
    float mMousePreviousX;
    float mMousePreviousY;
    float mMouseDeltaX;
    float mMouseDeltaY;

    bool mUpdateRotation;
    bool mUpdatePosition;

    static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;

    static CameraManager *CAMERA_MANAGER;

public:
    static FreeCamera *create();
    void remove();
};

#endif // FREECAMERA_H

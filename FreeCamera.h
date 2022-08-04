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

    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void update(float ifps);

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

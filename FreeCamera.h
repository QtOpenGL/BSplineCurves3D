#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "Camera.h"

#include <QObject>

class FreeCamera : public Camera
{
    Q_OBJECT

public:
    explicit FreeCamera(QObject *parent = nullptr);

    void keyPressed(QKeyEvent *event) override;
    void keyReleased(QKeyEvent *event) override;
    void mousePressed(QMouseEvent *event) override;
    void mouseReleased(QMouseEvent *event) override;
    void mouseMoved(QMouseEvent *event) override;
    void update(float ifps) override;

private:
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
};

#endif // FREECAMERA_H

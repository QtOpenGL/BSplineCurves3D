#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "DummyCamera.h"
#include "FreeCamera.h"
#include <QObject>

class CameraManager : public QObject
{
    Q_OBJECT

private:
    friend class DummyCamera;
    friend class FreeCamera;

    explicit CameraManager(QObject *parent = nullptr);

    void addCamera(Camera *camera);
    void removeCamera(Camera *camera);

    Camera *mActiveCamera;
    QList<Camera *> mCameras;

public:
    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void update(float ifps);
    QVector3D getViewDirection();
    QVector3D getDirectionFromScreen(int x, int y, int width, int height);

    Camera *activeCamera() const;
    void setActiveCamera(Camera *newActiveCamera);

    static CameraManager *instance();
};

#endif // CAMERAMANAGER_H

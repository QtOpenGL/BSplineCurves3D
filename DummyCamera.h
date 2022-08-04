#ifndef DUMMYCAMERA_H
#define DUMMYCAMERA_H

#include "Camera.h"

#include <QObject>

class DummyCamera : public Camera
{
    Q_OBJECT
private:
    friend class CameraManager;

private:
    explicit DummyCamera(QObject *parent = nullptr);
    virtual ~DummyCamera();

    static CameraManager *CAMERA_MANAGER;

public:
    static DummyCamera *create();
    void remove();
};

#endif // DUMMYCAMERA_H

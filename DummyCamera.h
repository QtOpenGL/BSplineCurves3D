#ifndef DUMMYCAMERA_H
#define DUMMYCAMERA_H

#include "Camera.h"

#include <QObject>

class DummyCamera : public Camera
{
    Q_OBJECT
public:
    explicit DummyCamera(QObject *parent = nullptr);
    virtual ~DummyCamera();

    void onKeyPressed(QKeyEvent *);
    void onKeyReleased(QKeyEvent *);
    void onMousePressed(QMouseEvent *);
    void onMouseReleased(QMouseEvent *);
    void onMouseMoved(QMouseEvent *);
    void update(float);
};

#endif // DUMMYCAMERA_H

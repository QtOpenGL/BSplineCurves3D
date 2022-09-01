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

    void keyPressed(QKeyEvent *);
    void keyReleased(QKeyEvent *);
    void mousePressed(QMouseEvent *);
    void mouseReleased(QMouseEvent *);
    void mouseMoved(QMouseEvent *);
    void update(float);
};

#endif // DUMMYCAMERA_H

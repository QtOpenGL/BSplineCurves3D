#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Bezier.h"
#include "Camera.h"
#include "RendererManager.h"
#include "Window.h"

#include <QObject>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    void run();

private slots:
    void init();
    void onWheelMoved(QWheelEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onResized(int w, int h);
    void render(float ifps);

private:
    RendererManager *mRendererManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ModelManager *mModelManager;
    Window *mWindow;

    Camera *mCamera;
    Light *mLight;

    Bezier *mBezierTestCurve;

    Model *mPlane;
    Model *mCube;
    Model *mSphere;
};

#endif // CONTROLLER_H

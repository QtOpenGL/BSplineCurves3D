#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Camera.h"
#include "RendererManager.h"
#include "Window.h"

#include <QObject>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    void show();

private:
    friend class Window;

    void init();
    void onWheelMoved(QWheelEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onResizeReceived(int w, int h);
    void update(float ifps);
    void render(float ifps);

    RendererManager *mRendererManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ModelManager *mModelManager;
    Window *mWindow;

    Camera *mCamera;
    Light *mLight;

    Spline *mTestCurve;

    Model *mPlane;
    Model *mCube;
    Model *mSphere;
};

#endif // CONTROLLER_H

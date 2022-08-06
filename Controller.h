#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Bezier.h"
#include "Camera.h"
#include "RendererManager.h"
#include "Window.h"

#include <QObject>

#include <Eigen/Dense>

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
    CurveManager *mCurveManager;
    Window *mWindow;

    Camera *mCamera;
    Light *mLight;

    Bezier *mBezierTestCurve1;
    Bezier *mBezierTestCurve2;

    Model *mPlane;
    Model *mCube;
    Model *mSphere;

    Curve *mSelectedCurve;
    Point *mSelectedPoint;

    Qt::MouseButton mPressedButton;
    Eigen::Hyperplane<float, 3> mTranslationPlane;
};

#endif // CONTROLLER_H

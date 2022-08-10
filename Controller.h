#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Camera.h"
#include "Enums.h"
#include "RendererManager.h"
#include "Window.h"

#include <QObject>

#include <Dense>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    void run();

signals:
    void modeChanged(Mode newMode);

private slots:
    void init();
    void onWheelMoved(QWheelEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onResized(int w, int h);
    void onMouseDoubleClicked(QMouseEvent *event);
    void render(float ifps);
    void onAction(Action, QVariant variant = QVariant());

private:
    RendererManager *mRendererManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ModelManager *mModelManager;
    CurveManager *mCurveManager;
    Window *mWindow;

    Camera *mCamera;
    Light *mLight;

    Spline *mTestCurve1;
    Spline *mTestCurve2;

    Model *mPlane;
    Model *mCube;
    Model *mSphere;

    Spline *mSelectedCurve;
    KnotPoint *mSelectedKnotPoint;

    Qt::MouseButton mPressedButton;
    Eigen::Hyperplane<float, 3> mTranslationPlane;

    Mode mMode;
};

#endif // CONTROLLER_H

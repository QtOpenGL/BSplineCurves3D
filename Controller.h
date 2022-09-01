#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Camera.h"
#include "Enums.h"
#include "RendererManager.h"

#include <QObject>

#include <Dense>
#include <QFileDialog>

class Window;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    void init();
    void wheelMoved(QWheelEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void keyPressed(QKeyEvent *event);
    void keyReleased(QKeyEvent *event);
    void resize(int w, int h);
    void mouseDoubleClicked(QMouseEvent *event);
    void render(float ifps);
    void onAction(Action, QVariant variant = QVariant());

    void setWindow(Window *newWindow);

signals:
    void modeChanged(Mode newMode);

private:
    RendererManager *mRendererManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ModelManager *mModelManager;
    CurveManager *mCurveManager;
    Window *mWindow;

    Camera *mCamera;
    Light *mLight;

    Model *mPlane;

    Spline *mSelectedCurve;
    KnotPoint *mSelectedKnotPoint;

    Qt::MouseButton mPressedButton;
    Eigen::Hyperplane<float, 3> mTranslationPlane;

    Mode mMode;

    QFileDialog *mFileDialog;
    Action mLastFileAction;
};

#endif // CONTROLLER_H

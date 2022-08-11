#ifndef WINDOW_H
#define WINDOW_H

#include "Enums.h"
#include "Light.h"
#include "LightManager.h"
#include "RendererManager.h"

#include <QOpenGLFunctionsPrivate>
#include <QOpenGLWindow>

class Window : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Window(QWindow *parent = nullptr);

    bool imguiWantCapture() const;

public slots:
    void onModeChanged(Mode newMode);

signals:
    void init();
    void resized(int w, int h);
    void render(float ifps);
    void keyPressed(QKeyEvent *);
    void keyReleased(QKeyEvent *);
    void mousePressed(QMouseEvent *);
    void mouseReleased(QMouseEvent *);
    void mouseMoved(QMouseEvent *);
    void wheelMoved(QWheelEvent *);
    void mouseDoubleClicked(QMouseEvent *);
    void action(Action action, QVariant variant = QVariant());

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;

private:
    long long mPreviousTime;
    long long mCurrentTime;

    bool mImguiWantCapture;

    RendererManager *mRendererManager;
    CurveManager *mCurveManager;
    LightManager *mLightManager;

    Light *mActiveLight;

    Mode mMode;

    bool mRenderPaths;
    bool mRenderPipes;

    Spline *mSelectedCurve;
    KnotPoint *mSelectedKnotPoint;

    float mGlobalPipeRadius;
    int mGlobalPipeSectorCount;
};
#endif // WINDOW_H

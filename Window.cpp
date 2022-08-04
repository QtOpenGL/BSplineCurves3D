#include "Window.h"

#include <imgui.h>
#include <QKeyEvent>
#include <QtImGui.h>

#include "Controller.h"
#include <QDateTime>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
{
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setSamples(16);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void Window::setController(Controller *newController)
{
    mController = newController;
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();
    QtImGui::initialize(this);

    mController->init();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;
}

void Window::resizeGL(int w, int h)
{
    mController->onResizeReceived(w, h);
}

void Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();

    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;

    mPreviousTime = mCurrentTime;

    mController->update(ifps);
    mController->render(ifps);

    QtImGui::newFrame();

    ImGui::Text("Does ImGui work?");

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    mController->onKeyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->onKeyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    mController->onMousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    mController->onMouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    mController->onMouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    mController->onWheelMoved(event);
}

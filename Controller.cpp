#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent}
{}

void Controller::init()
{
    mCamera = new Camera;
    mCamera->setPosition(QVector3D(0, 10, 10));
    mCamera->setVerticalFov(60.0f);
    mCamera->setZNear(0.1f);
    mCamera->setZFar(10000.0f);

    mLight = new Light;
    mLight->setScale(QVector3D(0.005f, 0.005f, 0.005f));
    mLight->setPosition(QVector3D(5, 20, 35));

    mRendererManager = new RendererManager;
    mRendererManager->setCamera(mCamera);
    mRendererManager->setLight(mLight);

    // FIXME
    // Sphere
    {
        mSphere = new Model;
        mSphere->setObjectName("Sphere");
        mSphere->setType(Model::Sphere);
        mSphere->material().setColor(QVector4D(0, 1, 0, 1));
        mSphere->setScale(QVector3D(0.001f, 0.001f, 0.001f));
        mSphere->setPosition(QVector3D(0, 2, 0));
        mRendererManager->addModel(mSphere);
    }

    mWindow = new Window;
    mWindow->setRendererManager(mRendererManager);
    mWindow->resize(800, 800);

    connect(mWindow, &Window::wheelMoved, this, &Controller::onWheelMoved);
    connect(mWindow, &Window::mousePressed, this, &Controller::onMousePressed);
    connect(mWindow, &Window::mouseReleased, this, &Controller::onMouseReleased);
    connect(mWindow, &Window::mouseMoved, this, &Controller::onMouseMoved);
    connect(mWindow, &Window::keyPressed, this, &Controller::onKeyPressed);
    connect(mWindow, &Window::keyReleased, this, &Controller::onKeyReleased);
    connect(mWindow, &Window::resizeReceived, this, &Controller::onResizeReceived);
}

void Controller::show()
{
    mWindow->show();
}

void Controller::onWheelMoved(QWheelEvent *event) {}

void Controller::onMousePressed(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        mCamera->onMousePressed(event);
    }
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        mCamera->onMouseReleased(event);
    }
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    // FIXME
    QVector3D dir = mCamera->getDirectionFromScreen(event->x(), event->y(), mWindow->width(), mWindow->height());
    mSphere->setPosition(mCamera->position() + 10 * dir);
    mCamera->onMouseMoved(event);
}

void Controller::onKeyPressed(QKeyEvent *event)
{
    mCamera->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    mCamera->onKeyReleased(event);
}

void Controller::onResizeReceived(int w, int h)
{
    mCamera->setAspectRatio((float) (w) / h);
}

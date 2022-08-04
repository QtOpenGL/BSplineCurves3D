#include "Controller.h"
#include "Light.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    mRendererManager = RendererManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mModelManager = ModelManager::instance();
    mWindow = new Window;
    mWindow->setController(this);
}

void Controller::init()
{
    // Initialize Managers
    mRendererManager->init();

    mCamera = FreeCamera::create();
    mCamera->setPosition(QVector3D(0, 10, 10));
    mCamera->setVerticalFov(60.0f);
    mCamera->setZNear(0.1f);
    mCamera->setZFar(10000.0f);
    mCameraManager->setActiveCamera(mCamera);

    mLight = Light::create();
    mLight->setPosition(QVector3D(5, 20, 35));
    mLightManager->setActiveLight(mLight);

    // FIXME

    // Sphere
    {
        mSphere = Model::create(Model::Sphere);
        mSphere->setObjectName("Sphere");
        mSphere->setPosition(QVector3D(0, 0, 0));
        mSphere->setScale(QVector3D(0.001f, 0.001f, 0.001f));
        mSphere->material().setColor(QVector4D(0, 1, 0, 1));
    }

    // Plane
    {
        mPlane = Model::create(Model::Plane);
        mPlane->setObjectName("Plane");
        mPlane->setPosition(QVector3D(0, 0, 0));
    }

    // Cube
    {
        mCube = Model::create(Model::Cube);
        mCube->setObjectName("Cube");
        mCube->material().setColor(QVector4D(1, 0, 0, 1));
        mCube->setScale(QVector3D(0.01f, 0.01f, 0.01f));
        mCube->setPosition(QVector3D(0, 2, 0));
    }

    // TestCurve
    {
        mTestCurve = new Spline;
        mTestCurve->addKnotPoint(new KnotPoint(0, 0, 0));

        mTestCurve->addKnotPoint(new KnotPoint(5, 5, 0));

        mTestCurve->addKnotPoint(new KnotPoint(0, 10, 0));

        mTestCurve->addKnotPoint(new KnotPoint(0, 15, 0));

        mTestCurve->updateSpline();

        mRendererManager->addCurve(mTestCurve);
    }

    mWindow->resize(800, 800);
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
        mCameraManager->onMousePressed(event);
    }
    else if (event->button() == Qt::LeftButton)
    {
        QVector3D rayDirection = mCameraManager->getDirectionFromScreen(event->x(), event->y(), mWindow->width(), mWindow->height());

        float distance = mTestCurve->closestDistanceToRay(mCamera->position(), rayDirection);

        qDebug() << distance;
    }
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        mCameraManager->onMouseReleased(event);
    }
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    // FIXME
    QVector3D dir = mCameraManager->getDirectionFromScreen(event->x(), event->y(), mWindow->width(), mWindow->height());
    mSphere->setPosition(mCamera->position() + 10 * dir);

    mCameraManager->onMouseMoved(event);
}

void Controller::onKeyPressed(QKeyEvent *event)
{
    mCameraManager->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    mCameraManager->onKeyReleased(event);
}

void Controller::onResizeReceived(int w, int h)
{
    mCamera->setAspectRatio((float) (w) / h);
}

void Controller::update(float ifps)
{
    mCameraManager->update(ifps);
}

void Controller::render(float ifps)
{
    mRendererManager->render(ifps);
}

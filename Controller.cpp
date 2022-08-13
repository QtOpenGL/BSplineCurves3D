#include "Controller.h"
#include "FreeCamera.h"
#include "Helper.h"
#include "Light.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , mSelectedCurve(nullptr)
    , mSelectedKnotPoint(nullptr)
    , mPressedButton(Qt::NoButton)
    , mMode(Mode::Select)
{
    mRendererManager = RendererManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mModelManager = ModelManager::instance();
    mCurveManager = CurveManager::instance();
    mWindow = new Window;

    mFileDialog = new QFileDialog;

    connect(mWindow, &Window::wheelMoved, this, &Controller::onWheelMoved);
    connect(mWindow, &Window::mousePressed, this, &Controller::onMousePressed);
    connect(mWindow, &Window::mouseReleased, this, &Controller::onMouseReleased);
    connect(mWindow, &Window::mouseMoved, this, &Controller::onMouseMoved);
    connect(mWindow, &Window::keyPressed, this, &Controller::onKeyPressed);
    connect(mWindow, &Window::keyReleased, this, &Controller::onKeyReleased);
    connect(mWindow, &Window::resized, this, &Controller::onResized);
    connect(mWindow, &Window::init, this, &Controller::init);
    connect(mWindow, &Window::render, this, &Controller::render);
    connect(mWindow, &Window::action, this, &Controller::onAction);
    connect(mWindow, &Window::mouseDoubleClicked, this, &Controller::onMouseDoubleClicked);

    connect(mCurveManager, &CurveManager::selectedCurveChanged, this, [=](Spline *selectedCurve) { mSelectedCurve = selectedCurve; });
    connect(mCurveManager, &CurveManager::selectedKnotPointChanged, this, [=](KnotPoint *selectedPoint) { mSelectedKnotPoint = selectedPoint; });

    connect(this, &Controller::modeChanged, mWindow, &Window::onModeChanged);

    connect(mFileDialog, &QFileDialog::fileSelected, this, [=](const QString &path) {
        if (!path.isEmpty())
            switch (mLastFileAction)
            {
            case Action::ShowExportWindow:
                onAction(Action::Export, path);
                break;
            case Action::ShowImportWindow:
                onAction(Action::Import, path);
                break;
            default:
                break;
            }
    });
}

void Controller::init()
{
    // Initialize Managers
    mRendererManager->init();

    mCamera = new FreeCamera;
    mCamera->setPosition(QVector3D(0, 10, 10));
    mCamera->setVerticalFov(60.0f);
    mCamera->setZNear(0.1f);
    mCamera->setZFar(10000.0f);
    mCameraManager->addCamera(mCamera);
    mCameraManager->setActiveCamera(mCamera);

    mLight = new Light;
    mLight->setPosition(QVector3D(5, 20, 35));
    mLightManager->addLight(mLight);
    mLightManager->setActiveLight(mLight);

    // Plane
    {
        mPlane = new Model;
        mPlane->setType(Model::Plane);
        mPlane->setObjectName("Plane");
        mPlane->setPosition(QVector3D(0, 0, 0));
        mPlane->setVisible(true);
        mModelManager->addModel(mPlane);
    }

    mCurveManager->addCurves(Helper::loadCurveDataFromJson(":/Resources/Data/test-curves-light.json"));
}

void Controller::run()
{
    // mWindow->showMaximized();
    // mWindow->showFullScreen();
    mWindow->resize(1024, 800);
    mWindow->show();
}

void Controller::onAction(Action action, QVariant variant)
{
    switch (action)
    {
    case Action::Select: {
        QVector3D rayDirection = mCameraManager->getDirectionFromScreen(variant.toPoint().x(), variant.toPoint().y(), mWindow->width(), mWindow->height());
        QVector3D rayOrigin = mCameraManager->activeCamera()->position();

        if (mSelectedCurve)
            mCurveManager->selectKnotPoint(rayOrigin, rayDirection);

        if (!mSelectedKnotPoint)
            mCurveManager->selectCurve(rayOrigin, rayDirection);

        if (mSelectedKnotPoint)
        {
            float x = mSelectedKnotPoint->position().x();
            float y = mSelectedKnotPoint->position().y();
            float z = mSelectedKnotPoint->position().z();

            QVector3D viewDirection = mCameraManager->activeCamera()->getViewDirection();
            Eigen::Vector3f normal = Eigen::Vector3f(viewDirection.x(), viewDirection.y(), viewDirection.z());
            Eigen::Vector3f eigenControlPointPosition = Eigen::Vector3f(x, y, z);
            normal.normalize();
            mTranslationPlane = Eigen::Hyperplane<float, 3>(normal, -normal.dot(eigenControlPointPosition));
        }

        break;
    }
    case Action::AddKnot: {
        QVector3D rayDirection = mCameraManager->getDirectionFromScreen(variant.toPoint().x(), variant.toPoint().y(), mWindow->width(), mWindow->height());
        QVector3D rayOrigin = mCameraManager->activeCamera()->position();
        QVector3D viewDirection = mCameraManager->activeCamera()->getViewDirection();

        Eigen::Vector3f eigenRayDirection = Eigen::Vector3f(rayDirection.x(), rayDirection.y(), rayDirection.z());
        Eigen::Vector3f eigenRayOrigin = Eigen::Vector3f(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());
        Eigen::Vector3f normal = Eigen::Vector3f(viewDirection.x(), viewDirection.y(), viewDirection.z()).normalized();

        auto line = Eigen::ParametrizedLine<float, 3>(eigenRayOrigin, eigenRayDirection);

        if (mSelectedCurve)
        {
            float x = mSelectedCurve->knotPoints().last()->position().x();
            float y = mSelectedCurve->knotPoints().last()->position().y();
            float z = mSelectedCurve->knotPoints().last()->position().z();
            Eigen::Vector3f lastKnotPosition = Eigen::Vector3f(x, y, z);

            Eigen::Hyperplane<float, 3> plane = Eigen::Hyperplane<float, 3>(normal, -normal.dot(lastKnotPosition));
            float t = line.intersection(plane);
            Eigen::Vector3f intersection = line.pointAt(t);

            if (!isnan(t) && !isinf(t))
            {
                KnotPoint *point = new KnotPoint(intersection.x(), intersection.y(), intersection.z());
                mSelectedCurve->addKnotPoint(point);
                mCurveManager->setSelectedKnotPoint(point);
            }
        } else
        {
            Eigen::Hyperplane<float, 3> plane = Eigen::Hyperplane<float, 3>(normal, -normal.dot(eigenRayOrigin + 20 * normal));
            float t = line.intersection(plane);
            Eigen::Vector3f intersection = line.pointAt(t);

            if (!isnan(t) && !isinf(t))
            {
                Spline *spline = new Spline;
                KnotPoint *point = new KnotPoint(intersection.x(), intersection.y(), intersection.z());
                spline->addKnotPoint(point);
                mCurveManager->addCurve(spline);
                mCurveManager->setSelectedCurve(spline);
                mCurveManager->setSelectedKnotPoint(point);
            }
        }

        break;
    }
    case Action::UpdateMode: {
        mMode = (Mode) variant.toInt();

        if (mSelectedKnotPoint)
        {
            float x = mSelectedKnotPoint->position().x();
            float y = mSelectedKnotPoint->position().y();
            float z = mSelectedKnotPoint->position().z();

            QVector3D viewDirection = mCameraManager->activeCamera()->getViewDirection();
            Eigen::Vector3f normal = Eigen::Vector3f(viewDirection.x(), viewDirection.y(), viewDirection.z()).normalized();
            Eigen::Vector3f knotPosition = Eigen::Vector3f(x, y, z);
            mTranslationPlane = Eigen::Hyperplane<float, 3>(normal, -normal.dot(knotPosition));
        }

        if (mMode == Mode::Select)
        {
            mCurveManager->setSelectedCurve(nullptr);
        }

        emit modeChanged(mMode);

        break;
    }
    case Action::UpdateRenderPaths: {
        mRendererManager->setRenderPaths(variant.toBool());
        break;
    }
    case Action::UpdateRenderPipes: {
        mRendererManager->setRenderPipes(variant.toBool());
        break;
    }
    case Action::UpdateKnotPointPositionFromScreen: {
        if (mSelectedKnotPoint)
        {
            QVector3D rayDirection = mCameraManager->getDirectionFromScreen(variant.toPoint().x(), variant.toPoint().y(), mWindow->width(), mWindow->height());
            QVector3D rayOrigin = mCamera->position();

            Eigen::Vector3f eigenRayDirection = Eigen::Vector3f(rayDirection.x(), rayDirection.y(), rayDirection.z());
            Eigen::Vector3f eigenRayOrigin = Eigen::Vector3f(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());

            auto line = Eigen::ParametrizedLine<float, 3>(eigenRayOrigin, eigenRayDirection);
            float t = line.intersection(mTranslationPlane);
            Eigen::Vector3f intersection = line.pointAt(t);

            if (!isnan(t) && !isinf(t))
            {
                mSelectedKnotPoint->setPosition(QVector3D(intersection.x(), intersection.y(), intersection.z()));
            }
        }
        break;
    }
    case Action::UpdateKnotPointPositionFromGui: {
        if (mSelectedKnotPoint)
        {
            QList<QVariant> coords = variant.toList();
            float x = coords[0].toFloat();
            float y = coords[1].toFloat();
            float z = coords[2].toFloat();
            mSelectedKnotPoint->setPosition(QVector3D(x, y, z));
        }
        break;
    }
    case Action::UpdateSelectedCurvePipeRadius: {
        if (mSelectedCurve)
            mSelectedCurve->setRadius(variant.toFloat());
        break;
    }
    case Action::UpdateSelectedCurvePipeSectorCount: {
        if (mSelectedCurve)
            mSelectedCurve->setSectorCount(variant.toInt());
        break;
    }
    case Action::UpdateGlobalPipeRadius: {
        mCurveManager->setGlobalPipeRadius(variant.toFloat());
        break;
    }
    case Action::UpdateGlobalPipeSectorCount: {
        mCurveManager->setGlobalPipeSectorCount(variant.toInt());
        break;
    }
    case Action::RemoveSelectedCurve: {
        mCurveManager->removeCurve(mSelectedCurve);
        break;
    }
    case Action::RemoveSelectedKnotPoint: {
        if (mSelectedCurve)
        {
            if (mSelectedKnotPoint)
            {
                mSelectedCurve->removeKnotPoint(mSelectedKnotPoint);
                mCurveManager->setSelectedKnotPoint(nullptr);
            }
        }
        break;
    }
    case Action::ClearScene: {
        mCurveManager->removeAllCurves();
        break;
    }
    case Action::ShowImportWindow: {
        mLastFileAction = Action::ShowImportWindow;
        mFileDialog->setFileMode(QFileDialog::ExistingFile);
        mFileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        mFileDialog->setNameFilter("*.json");
        mFileDialog->show();
        break;
    }
    case Action::ShowExportWindow: {
        mLastFileAction = Action::ShowExportWindow;
        mFileDialog->setFileMode(QFileDialog::AnyFile);
        mFileDialog->setAcceptMode(QFileDialog::AcceptSave);
        mFileDialog->setDefaultSuffix(".json");
        mFileDialog->setNameFilter("*.json");
        mFileDialog->show();
        break;
    }
    case Action::Export: {
        Helper::saveCurveDataToJson(mCurveManager->getCurvesNonConst(), variant.toString());
        break;
    }
    case Action::Import: {
        QList<Spline *> curves = Helper::loadCurveDataFromJson(variant.toString());

        if (!curves.isEmpty())
        {
            mCurveManager->setSelectedCurve(nullptr);
            mCurveManager->removeAllCurves();
            mCurveManager->addCurves(curves);
        }
        break;
    }
    }
}

void Controller::onWheelMoved(QWheelEvent *)
{
    if (mWindow->imguiWantCapture())
        return;
}

void Controller::onMousePressed(QMouseEvent *event)
{
    if (mWindow->imguiWantCapture())
        return;

    mPressedButton = event->button();

    if (mPressedButton == Qt::RightButton)
    {
        mCameraManager->onMousePressed(event);
    } else if (mPressedButton == Qt::LeftButton)
    {
        switch (mMode)
        {
        case Mode::Select: {
            onAction(Action::Select, event->pos());
            break;
        }
        case Mode::Add: {
            onAction(Action::AddKnot, event->pos());
            break;
        }
        }
    }
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    if (mWindow->imguiWantCapture())
        return;

    mPressedButton = Qt::NoButton;

    if (event->button() == Qt::RightButton)
    {
        mCameraManager->onMouseReleased(event);
    }
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    if (mWindow->imguiWantCapture())
        return;

    if (mPressedButton == Qt::RightButton)
    {
        mCameraManager->onMouseMoved(event);
    } else if (mPressedButton == Qt::LeftButton)
    {
        switch (mMode)
        {
        case Mode::Select: {
            onAction(Action::UpdateKnotPointPositionFromScreen, event->pos());
            break;
        }
        case Mode::Add: {
            break;
        }
        }
    }
}

void Controller::onKeyPressed(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        if (mSelectedKnotPoint)
            onAction(Action::RemoveSelectedKnotPoint);
        else if (mSelectedCurve)
            onAction(Action::RemoveSelectedCurve);

    } else if (event->key() == Qt::Key_Z)
    {
        onAction(Action::UpdateMode, (int) Mode::Select);
    } else if (event->key() == Qt::Key_C)
    {
        onAction(Action::UpdateMode, (int) Mode::Add);
    }

    mCameraManager->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    mCameraManager->onKeyReleased(event);
}

void Controller::onResized(int w, int h)
{
    mCamera->setAspectRatio((float) (w) / h);
}

void Controller::onMouseDoubleClicked(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        onAction(Action::UpdateMode, (int) Mode::Select);
}

void Controller::render(float ifps)
{
    mCameraManager->update(ifps);
    mRendererManager->render(ifps);
}

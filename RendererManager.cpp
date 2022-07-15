#include "RendererManager.h"
#include "Spline.h"

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mBasicShader(nullptr)
    , mLight(nullptr)
    , mCamera(nullptr)

{}

RendererManager::~RendererManager()
{
    // TODO
}

bool RendererManager::init()
{
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glLineWidth(2.5f);

    qInfo() << Q_FUNC_INFO << "Initializing BasicShader...";
    mBasicShader = new BasicShader;

    if (!mBasicShader->init())
    {
        qWarning() << Q_FUNC_INFO << "BasicShader could not be initialized.";
        return false;
    }

    qInfo() << Q_FUNC_INFO << "Loading and creating all models...";

    for (Model::Type type : Model::ALL_MODEL_TYPES)
    {
        ModelData *data = new ModelData(type);
        if (!data->load() || !data->create())
        {
            data->deleteLater();
            continue;
        }

        mTypeToModelData.insert(type, data);
    }

    if (!mLight)
    {
        qWarning() << Q_FUNC_INFO << "Light is not set.";
        return false;
    }

    if (!mCamera)
    {
        qWarning() << Q_FUNC_INFO << "Camera is not set.";
        return false;
    }

    // FIXME

    // Plane
    {
        mPlane = new Model;
        mPlane->setObjectName("Plane");
        mPlane->setType(Model::Plane);
        mPlane->setPosition(QVector3D(0, 0, 0));
        mModels << mPlane;
    }

    // Cube
    {
        mCube = new Model;
        mCube->setObjectName("Cube");
        mCube->setType(Model::Cube);
        mCube->material().setColor(QVector4D(1, 0, 0, 1));
        mCube->setScale(QVector3D(0.01f, 0.01f, 0.01f));
        mCube->setPosition(QVector3D(0, 2, 0));
        mModels << mCube;
    }

    qInfo() << Q_FUNC_INFO << "Initializing PathShader...";
    mPathShader = new PathShader;

    if (!mPathShader->init())
    {
        qWarning() << Q_FUNC_INFO << "PathShader could not be initialized.";
    }

    mTicks = new Ticks(0, 1, 200);
    mTicks->create();

    return true;
}

void RendererManager::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mBasicShader->bind();

    // Camera

    if (mCamera)
    {
        mBasicShader->setUniformValue("projectionMatrix", mCamera->projection());
        mBasicShader->setUniformValue("viewMatrix", mCamera->transformation());
        mBasicShader->setUniformValue("cameraPosition", mCamera->position());
    }

    // Light

    if (mLight)
    {
        mBasicShader->setUniformValue("light.position", mLight->position());
        mBasicShader->setUniformValue("light.color", mLight->color());
        mBasicShader->setUniformValue("light.ambient", mLight->ambient());
        mBasicShader->setUniformValue("light.diffuse", mLight->diffuse());
        mBasicShader->setUniformValue("light.specular", mLight->specular());
    }

    for (Model *model : qAsConst(mModels))
    {
        ModelData *data = mTypeToModelData.value(model->type(), nullptr);

        if (data)
        {
            data->bind();
            mBasicShader->setUniformValue("node.transformation", model->transformation());
            mBasicShader->setUniformValue("node.color", model->material().color());
            mBasicShader->setUniformValue("node.ambient", model->material().ambient());
            mBasicShader->setUniformValue("node.diffuse", model->material().diffuse());
            mBasicShader->setUniformValue("node.specular", model->material().specular());
            mBasicShader->setUniformValue("node.shininess", model->material().shininess());
            glDrawArrays(GL_TRIANGLES, 0, data->count());
            data->release();
        }
    }

    mBasicShader->release();

    mPathShader->bind();

    if (mCamera)
    {
        mPathShader->setUniformValue("projectionMatrix", mCamera->projection());
        mPathShader->setUniformValue("viewMatrix", mCamera->transformation());
    }

    mPathShader->setUniformValue("color", QVector4D(1, 0, 0, 1));

    Spline spline;

    spline.addKnotPoint(new KnotPoint(0, 0, 0));
    spline.addKnotPoint(new KnotPoint(5, 5, 0));
    spline.addKnotPoint(new KnotPoint(0, 10, 0));
    spline.addKnotPoint(new KnotPoint(5, 15, 0));
    spline.addKnotPoint(new KnotPoint(5, 25, 25));

    QVector<QVector3D> controlPoints = spline.getControlPointPositions();

    mPathShader->setUniformValue("controlPointsCount", controlPoints.size());
    mPathShader->setUniformValueArray("controlPoints", controlPoints);

    mTicks->bind();
    glDrawArrays(GL_LINE_STRIP, 0, mTicks->size());
    mTicks->release();

    mPathShader->release();
}

void RendererManager::addModel(Model *model)
{
    // FIXME
    mModels << model;
}

bool RendererManager::removeModel(Model *model)
{
    // FIXME
    int number = mModels.removeAll(model);
    model->deleteLater();

    return number > 0;
}

Light *RendererManager::light()
{
    return mLight;
}

void RendererManager::setLight(Light *newLight)
{
    mLight = newLight;
}

Camera *RendererManager::camera()
{
    return mCamera;
}

void RendererManager::setCamera(Camera *newCamera)
{
    mCamera = newCamera;
}

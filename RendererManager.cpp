#include "RendererManager.h"
#include "Camera.h"
#include "Light.h"
#include "Spline.h"

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mBasicShader(nullptr)
{
    mModelManager = ModelManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
}

RendererManager *RendererManager::instance()
{
    static RendererManager instance;
    return &instance;
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

    // FIXME
    // Knot and Control Point Models
    {
        mKnotModel = Model::create(Model::Sphere);
        mKnotModel->setObjectName("KnotModel");
        mKnotModel->material().setColor(QVector4D(0, 1, 0, 1));
        mKnotModel->setScale(QVector3D(0.001f, 0.001f, 0.001f));
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

void RendererManager::render(float ifps)
{
    Q_UNUSED(ifps);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mBasicShader->bind();

    // Camera
    Camera *camera = mCameraManager->activeCamera();
    Light *light = mLightManager->activeLight();

    if (camera)
    {
        mBasicShader->setUniformValue("projectionMatrix", camera->projection());
        mBasicShader->setUniformValue("viewMatrix", camera->transformation());
        mBasicShader->setUniformValue("cameraPosition", camera->position());
    }

    // Light

    if (light)
    {
        mBasicShader->setUniformValue("light.position", light->position());
        mBasicShader->setUniformValue("light.color", light->color());
        mBasicShader->setUniformValue("light.ambient", light->ambient());
        mBasicShader->setUniformValue("light.diffuse", light->diffuse());
        mBasicShader->setUniformValue("light.specular", light->specular());
    }

    QList<Model *> models = mModelManager->models();

    for (Model *model : qAsConst(models))
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

    mBasicShader->setUniformValue("node.color", mKnotModel->material().color());
    mBasicShader->setUniformValue("node.ambient", mKnotModel->material().ambient());
    mBasicShader->setUniformValue("node.diffuse", mKnotModel->material().diffuse());
    mBasicShader->setUniformValue("node.specular", mKnotModel->material().specular());
    mBasicShader->setUniformValue("node.shininess", mKnotModel->material().shininess());

    for (Spline *curve : qAsConst(mCurves))
    {
        if (curve)
        {
            ModelData *data = mTypeToModelData.value(mKnotModel->type(), nullptr);

            for (int i = 0; i < curve->mKnotPoints.size(); i++)
            {
                mKnotModel->setPosition(curve->mKnotPoints[i]->position());

                data->bind();
                mBasicShader->setUniformValue("node.transformation", mKnotModel->transformation());
                glDrawArrays(GL_TRIANGLES, 0, data->count());
                data->release();
            }
        }
    }

    mBasicShader->release();

    mPathShader->bind();

    if (camera)
    {
        mPathShader->setUniformValue("projectionMatrix", camera->projection());
        mPathShader->setUniformValue("viewMatrix", camera->transformation());
    }

    mPathShader->setUniformValue("color", QVector4D(1, 0, 0, 1));

    for (Spline *curve : qAsConst(mCurves))
    {
        if (curve)
        {
            QVector<QVector3D> controlPoints = curve->getControlPointPositions();

            mPathShader->setUniformValue("controlPointsCount", controlPoints.size());
            mPathShader->setUniformValueArray("controlPoints", controlPoints);

            mTicks->bind();
            glDrawArrays(GL_LINE_STRIP, 0, mTicks->size());
            mTicks->release();
        }
    }

    mPathShader->release();
}

void RendererManager::addCurve(Spline *curve)
{
    mCurves << curve;
}

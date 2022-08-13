#include "RendererManager.h"
#include "Bezier.h"
#include "Camera.h"
#include "Light.h"

#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mSelectedCurve(nullptr)
    , mSelectedKnotPoint(nullptr)
    , mRenderPaths(true)
    , mRenderPipes(true)
{
    mModelManager = ModelManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mCurveManager = CurveManager::instance();
    mShaderManager = ShaderManager::instance();

    connect(mCurveManager, &CurveManager::selectedCurveChanged, this, [=](Spline *selectedCurve) { mSelectedCurve = selectedCurve; });
    connect(mCurveManager, &CurveManager::selectedKnotPointChanged, this, [=](KnotPoint *selectedPoint) { mSelectedKnotPoint = selectedPoint; });
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

    qInfo() << Q_FUNC_INFO << "Initializing ShaderManager...";

    if (!mShaderManager->init())
    {
        qWarning() << Q_FUNC_INFO << "ShaderManager could not be initialized.";
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

    // Knot Point Model
    {
        mKnotPointModel = new Model;
        mKnotPointModel->setType(Model::Sphere);
        mKnotPointModel->setObjectName("KnotPointModel");
        mKnotPointModel->material().setColor(QVector4D(0, 1, 0, 1));
        mKnotPointModel->setScale(QVector3D(0.005f, 0.005f, 0.005f));
        mKnotPointModel->setVisible(false);

        mKnotPointModelData = mTypeToModelData.value(mKnotPointModel->type(), nullptr);
    }

    mPathTicks = new Ticks(0, 1, 100);
    mPathTicks->create();

    mPipeTicks = new Ticks(0, 1, 100);
    mPipeTicks->create();

    return true;
}

void RendererManager::render(float ifps)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCamera = mCameraManager->activeCamera();
    mLight = mLightManager->activeLight();

    renderModels(ifps);

    if (mRenderPaths)
        renderPaths(ifps);

    if (mRenderPipes)
        renderPipes(ifps);

    if (mRenderPaths || mRenderPipes)
        renderKnotPoints(ifps);
}

void RendererManager::renderModels(float ifps)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::Basic);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight)
    {
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
    }

    QList<Model *> models = mModelManager->models();

    for (Model *model : qAsConst(models))
    {
        if (!model->visible())
            continue;

        ModelData *data = mTypeToModelData.value(model->type(), nullptr);

        if (data)
        {
            mShaderManager->setUniformValue("node.transformation", model->transformation());
            mShaderManager->setUniformValue("node.color", model->material().color());
            mShaderManager->setUniformValue("node.ambient", model->material().ambient());
            mShaderManager->setUniformValue("node.diffuse", model->material().diffuse());
            mShaderManager->setUniformValue("node.specular", model->material().specular());
            mShaderManager->setUniformValue("node.shininess", model->material().shininess());
            data->render();
        }
    }

    mShaderManager->release();
}

void RendererManager::renderKnotPoints(float ifps)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::Basic);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight)
    {
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
    }

    if (mSelectedCurve)
    {
        mShaderManager->setUniformValue("node.ambient", mKnotPointModel->material().ambient());
        mShaderManager->setUniformValue("node.diffuse", mKnotPointModel->material().diffuse());
        mShaderManager->setUniformValue("node.specular", mKnotPointModel->material().specular());
        mShaderManager->setUniformValue("node.shininess", mKnotPointModel->material().shininess());

        for (auto &point : qAsConst(mSelectedCurve->knotPoints()))
        {
            mKnotPointModel->setPosition(point->position());
            mKnotPointModel->material().setColor(point->selected() ? QVector4D(1, 1, 0, 1) : QVector4D(0, 1, 0, 1));

            mShaderManager->setUniformValue("node.color", mKnotPointModel->material().color());
            mShaderManager->setUniformValue("node.transformation", mKnotPointModel->transformation());

            mKnotPointModelData->render();
        }
    }

    mShaderManager->release();
}

void RendererManager::renderPaths(float ifps)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::Path);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->transformation());
    }

    mShaderManager->setUniformValue("color", QVector4D(1, 0, 0, 1));

    for (auto &curve : qAsConst(mCurveManager->curves()))
    {
        if (curve)
        {
            QList<Bezier *> patches = curve->bezierPatches();
            for (auto &patch : qAsConst(patches))
            {
                auto controlPointPositions = patch->getControlPointPositions();

                mShaderManager->setUniformValue("control_points_count", controlPointPositions.size());
                mShaderManager->setUniformValueArray("control_points", controlPointPositions);
                mPathTicks->render();
            }
        }
    }

    mShaderManager->release();
}

void RendererManager::renderPipes(float ifps)
{
    // Pipe

    for (auto &curve : qAsConst(mCurveManager->curves()))
    {
        if (curve)
        {
            QList<Bezier *> patches = curve->bezierPatches();

            for (auto &patch : patches)
            {
                if (!patch->initialized())
                {
                    patch->initializeOpenGLStuff();
                }

                if (patch->vertexGenerationStatus() == Bezier::VertexGenerationStatus::GeneratingVertices)
                {
                    renderUsingDumbShader(ifps, curve, patch);
                } else if (patch->vertexGenerationStatus() == Bezier::VertexGenerationStatus::WaitingForOpenGLUpdate)
                {
                    patch->updateOpenGLStuff();
                    renderUsingSmartShader(ifps, curve, patch);
                } else if (patch->vertexGenerationStatus() == Bezier::VertexGenerationStatus::Dirty)
                {
                    patch->generateVertices();
                    renderUsingDumbShader(ifps, curve, patch);
                } else if (patch->vertexGenerationStatus() == Bezier::VertexGenerationStatus::Ready)
                {
                    renderUsingSmartShader(ifps, curve, patch);
                }
            }
        }
    }
}

void RendererManager::renderUsingDumbShader(float ifps, Spline *curve, Bezier *patch)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::PipeDumb);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight)
    {
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
    }

    mShaderManager->setUniformValue("dt", mPipeTicks->ticksDelta());

    auto controlPointPositions = patch->getControlPointPositions();

    mShaderManager->setUniformValue("control_points_count", controlPointPositions.size());
    mShaderManager->setUniformValueArray("control_points", controlPointPositions);

    mShaderManager->setUniformValue("node.color", curve->material().color());
    mShaderManager->setUniformValue("node.ambient", curve->material().ambient());
    mShaderManager->setUniformValue("node.diffuse", curve->material().diffuse());
    mShaderManager->setUniformValue("node.specular", curve->material().specular());
    mShaderManager->setUniformValue("node.shininess", curve->material().shininess());

    int n = patch->sectorCount();
    float r = patch->radius();

    for (int i = 0; i < n; i++)
    {
        float sectorAngle0 = 2.0f * float(i) * M_PI / n;
        float sectorAngle1 = 2.0f * float(i + 1) * M_PI / n;

        mShaderManager->setUniformValue("r", r);
        mShaderManager->setUniformValue("sector_angle_0", sectorAngle0);
        mShaderManager->setUniformValue("sector_angle_1", sectorAngle1);
        mPipeTicks->render();
    }

    mShaderManager->release();
}

void RendererManager::renderUsingSmartShader(float ifps, Spline *curve, Bezier *patch)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::PipeSmart);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight)
        Q_UNUSED(ifps);
    {
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
    }

    mShaderManager->setUniformValue("node.color", curve->material().color());
    mShaderManager->setUniformValue("node.ambient", curve->material().ambient());
    mShaderManager->setUniformValue("node.diffuse", curve->material().diffuse());
    mShaderManager->setUniformValue("node.specular", curve->material().specular());
    mShaderManager->setUniformValue("node.shininess", curve->material().shininess());

    patch->render();

    mShaderManager->release();
}

void RendererManager::setRenderPipes(bool newRenderPipes)
{
    mRenderPipes = newRenderPipes;
}

void RendererManager::setRenderPaths(bool newRenderPaths)
{
    mRenderPaths = newRenderPaths;
}

bool RendererManager::getRenderPaths() const
{
    return mRenderPaths;
}

bool RendererManager::getRenderPipes() const
{
    return mRenderPipes;
}

#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "CurveManager.h"
#include "Light.h"
#include "LightManager.h"
#include "ModelData.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "Ticks.h"

#include <QMap>
#include <QMatrix4x4>
#include <QObject>

class RendererManager : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    static RendererManager *instance();

    bool init();
    void render(float ifps);

    void setRenderPaths(bool newRenderPaths);
    void setRenderPipes(bool newRenderPipes);

    bool getRenderPaths() const;
    bool getRenderPipes() const;

private slots:
    void renderModels(float ifps);
    void renderKnotPoints(float ifps);
    void renderPaths(float ifps);
    void renderPipes(float ifps);

    void renderUsingDumbShader(float ifps, Spline *curve, Bezier *patch);
    void renderUsingSmartShader(float ifps, Spline *curve, Bezier *patch);

private:
    QMap<Model::Type, ModelData *> mTypeToModelData;

    Ticks *mPathTicks;
    Ticks *mPipeTicks;

    ModelManager *mModelManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    CurveManager *mCurveManager;
    ShaderManager *mShaderManager;

    Spline *mSelectedCurve;
    KnotPoint *mSelectedKnotPoint;

    Camera *mCamera;
    Light *mLight;

    Model *mKnotPointModel;
    ModelData *mKnotPointModelData;

    bool mRenderPaths;
    bool mRenderPipes;
};

#endif // RENDERERMANAGER_H

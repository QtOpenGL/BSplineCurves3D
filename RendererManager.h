#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "BasicShader.h"
#include "Bezier.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "ModelData.h"
#include "ModelManager.h"
#include "PathShader.h"
#include "Ticks.h"

#include <QMap>
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

    void addBezierCurve(Bezier *curve);

private:
    QList<Bezier *> mBezierCurves;
    QMap<Model::Type, ModelData *> mTypeToModelData;
    BasicShader *mBasicShader;

    PathShader *mPathShader;
    Ticks *mTicks;

    ModelManager *mModelManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;

    // FIXME
    Model *mKnotPointModel;
    Model *mControlPointModel;
};

#endif // RENDERERMANAGER_H

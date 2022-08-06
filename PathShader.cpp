#include "PathShader.h"
#include "Helper.h"

PathShader::PathShader(QObject *parent)
    : QObject{parent}
    , mProgram(nullptr)
{}

PathShader::~PathShader()
{
    if (mProgram)
        mProgram->deleteLater();

    mProgram = nullptr;
}

bool PathShader::init()
{
    initializeOpenGLFunctions();

    mProgram = new QOpenGLShaderProgram;

    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Path.vert")))
    {
        qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
        return false;
    }

    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Path.frag")))
    {
        qWarning() << Q_FUNC_INFO << "Could not load fragment shader.";
        return false;
    }

    if (!mProgram->link())
    {
        qWarning() << Q_FUNC_INFO << "Could not link shader program.";
        return false;
    }

    if (!mProgram->bind())
    {
        qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
        return false;
    }

    mLocations.insert("controlPoints", mProgram->uniformLocation("controlPoints"));
    mLocations.insert("controlPointsCount", mProgram->uniformLocation("controlPointsCount"));
    mLocations.insert("color", mProgram->uniformLocation("color"));

    mLocations.insert("viewMatrix", mProgram->uniformLocation("viewMatrix"));
    mLocations.insert("projectionMatrix", mProgram->uniformLocation("projectionMatrix"));

    mProgram->bindAttributeLocation("vertex", 0);
    mProgram->release();

    qInfo() << Q_FUNC_INFO << "PathShader is initialized.";
    qInfo() << Q_FUNC_INFO << "Uniform locations are:" << mLocations;

    return true;
}

bool PathShader::bind()
{
    return mProgram->bind();
}

void PathShader::release()
{
    mProgram->release();
}

void PathShader::setUniformValue(const QString &name, int value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PathShader::setUniformValue(const QString &name, float value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PathShader::setUniformValue(const QString &name, const QVector3D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PathShader::setUniformValue(const QString &name, const QVector4D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PathShader::setUniformValue(const QString &name, const QMatrix4x4 &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PathShader::setUniformValueArray(const QString &name, const QVector<QVector3D> &values)
{
    mProgram->setUniformValueArray(mLocations[name], values.constData(), values.size());
}

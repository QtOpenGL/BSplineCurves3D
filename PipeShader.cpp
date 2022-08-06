#include "PipeShader.h"
#include "Helper.h"

PipeShader::PipeShader(QObject *parent)
    : QObject{parent}
    , mProgram(nullptr)
{}

PipeShader::~PipeShader()
{
    if (mProgram)
        mProgram->deleteLater();

    mProgram = nullptr;
}

bool PipeShader::init()
{
    initializeOpenGLFunctions();

    mProgram = new QOpenGLShaderProgram;

    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Pipe.vert")))
    {
        qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
        return false;
    }

    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Geometry, Helper::getBytes(":/Resources/Shaders/Pipe.geom")))
    {
        qWarning() << Q_FUNC_INFO << "Could not load geometry shader.";
        return false;
    }

    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Pipe.frag")))
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

    mLocations.insert("viewMatrix", mProgram->uniformLocation("viewMatrix"));
    mLocations.insert("projectionMatrix", mProgram->uniformLocation("projectionMatrix"));

    mLocations.insert("controlPoints", mProgram->uniformLocation("controlPoints"));
    mLocations.insert("controlPointsCount", mProgram->uniformLocation("controlPointsCount"));

    mLocations.insert("dt", mProgram->uniformLocation("dt"));
    mLocations.insert("r", mProgram->uniformLocation("r"));
    mLocations.insert("sectorAngle0", mProgram->uniformLocation("sectorAngle0"));
    mLocations.insert("sectorAngle1", mProgram->uniformLocation("sectorAngle1"));

    mLocations.insert("light.color", mProgram->uniformLocation("light.color"));
    mLocations.insert("light.position", mProgram->uniformLocation("light.position"));
    mLocations.insert("light.ambient", mProgram->uniformLocation("light.ambient"));
    mLocations.insert("light.diffuse", mProgram->uniformLocation("light.diffuse"));
    mLocations.insert("light.specular", mProgram->uniformLocation("light.specular"));

    mLocations.insert("node.color", mProgram->uniformLocation("node.color"));
    mLocations.insert("node.ambient", mProgram->uniformLocation("node.ambient"));
    mLocations.insert("node.diffuse", mProgram->uniformLocation("node.diffuse"));
    mLocations.insert("node.specular", mProgram->uniformLocation("node.specular"));
    mLocations.insert("node.shininess", mProgram->uniformLocation("node.shininess"));

    mLocations.insert("cameraPosition", mProgram->uniformLocation("cameraPosition"));

    mProgram->bindAttributeLocation("t", 0);
    mProgram->release();

    qInfo() << Q_FUNC_INFO << "PipeShader is initialized.";
    qInfo() << Q_FUNC_INFO << "Uniform locations are:" << mLocations;

    return true;
}

bool PipeShader::bind()
{
    return mProgram->bind();
}

void PipeShader::release()
{
    mProgram->release();
}

void PipeShader::setUniformValue(const QString &name, int value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PipeShader::setUniformValue(const QString &name, float value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PipeShader::setUniformValue(const QString &name, const QVector3D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PipeShader::setUniformValue(const QString &name, const QVector4D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PipeShader::setUniformValue(const QString &name, const QMatrix4x4 &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PipeShader::setUniformValue(const QString &name, const QMatrix3x3 &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void PipeShader::setUniformValueArray(const QString &name, const QVector<QVector3D> &values)
{
    mProgram->setUniformValueArray(mLocations[name], values.constData(), values.size());
}

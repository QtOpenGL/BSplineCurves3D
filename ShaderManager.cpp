#include "ShaderManager.h"
#include "Helper.h"

ShaderManager::ShaderManager(QObject *parent)
    : QObject{parent} {}

bool ShaderManager::init() {
    initializeOpenGLFunctions();

    // Basic
    {
        QOpenGLShaderProgram *basicShader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::Basic, basicShader);

        if (!basicShader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Basic.vert"))) {
            qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
            return false;
        }

        if (!basicShader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Basic.frag"))) {
            qWarning() << Q_FUNC_INFO << "Could not load fragment shader.";
            return false;
        }

        if (!basicShader->link()) {
            qWarning() << Q_FUNC_INFO << "Could not link shader program.";
            return false;
        }

        if (!basicShader->bind()) {
            qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
            return false;
        }

        QMap<QString, GLuint> locations;

        locations.insert("light.color", basicShader->uniformLocation("light.color"));
        locations.insert("light.position", basicShader->uniformLocation("light.position"));
        locations.insert("light.ambient", basicShader->uniformLocation("light.ambient"));
        locations.insert("light.diffuse", basicShader->uniformLocation("light.diffuse"));
        locations.insert("light.specular", basicShader->uniformLocation("light.specular"));

        locations.insert("node.transformation", basicShader->uniformLocation("node.transformation"));
        locations.insert("node.color", basicShader->uniformLocation("node.color"));
        locations.insert("node.ambient", basicShader->uniformLocation("node.ambient"));
        locations.insert("node.diffuse", basicShader->uniformLocation("node.diffuse"));
        locations.insert("node.specular", basicShader->uniformLocation("node.specular"));
        locations.insert("node.shininess", basicShader->uniformLocation("node.shininess"));

        locations.insert("camera_position", basicShader->uniformLocation("camera_position"));
        locations.insert("view_matrix", basicShader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", basicShader->uniformLocation("projection_matrix"));

        basicShader->bindAttributeLocation("position", 0);
        basicShader->bindAttributeLocation("normal", 1);

        basicShader->release();

        mLocations.insert(Shader::Basic, locations);

        qInfo() << Q_FUNC_INFO << "BasicShader is initialized.";
        qInfo() << Q_FUNC_INFO << "Uniform locations are:" << locations;
    }

    // Path
    {
        QOpenGLShaderProgram *pathShader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::Path, pathShader);

        if (!pathShader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Path.vert"))) {
            qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
            return false;
        }

        if (!pathShader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Path.frag"))) {
            qWarning() << Q_FUNC_INFO << "Could not load fragment shader.";
            return false;
        }

        if (!pathShader->link()) {
            qWarning() << Q_FUNC_INFO << "Could not link shader program.";
            return false;
        }

        if (!pathShader->bind()) {
            qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
            return false;
        }

        QMap<QString, GLuint> locations;

        locations.insert("control_points", pathShader->uniformLocation("control_points"));
        locations.insert("control_points_count", pathShader->uniformLocation("control_points_count"));
        locations.insert("color", pathShader->uniformLocation("color"));

        locations.insert("view_matrix", pathShader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", pathShader->uniformLocation("projection_matrix"));

        pathShader->bindAttributeLocation("t", 0);
        pathShader->release();

        mLocations.insert(Shader::Path, locations);

        qInfo() << Q_FUNC_INFO << "PathShader is initialized.";
        qInfo() << Q_FUNC_INFO << "Uniform locations are:" << locations;
    }

    // Pipe
    {
        QOpenGLShaderProgram *pipeShader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::Pipe, pipeShader);

        if (!pipeShader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Pipe.vert"))) {
            qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
            return false;
        }

        if (!pipeShader->addShaderFromSourceCode(QOpenGLShader::Geometry, Helper::getBytes(":/Resources/Shaders/Pipe.geom"))) {
            qWarning() << Q_FUNC_INFO << "Could not load geometry shader.";
            return false;
        }

        if (!pipeShader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Pipe.frag"))) {
            qWarning() << Q_FUNC_INFO << "Could not load fragment shader.";
            return false;
        }

        if (!pipeShader->link()) {
            qWarning() << Q_FUNC_INFO << "Could not link shader program.";
            return false;
        }

        if (!pipeShader->bind()) {
            qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
            return false;
        }

        QMap<QString, GLuint> locations;

        locations.insert("view_matrix", pipeShader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", pipeShader->uniformLocation("projection_matrix"));
        locations.insert("camera_position", pipeShader->uniformLocation("camera_position"));

        locations.insert("control_points", pipeShader->uniformLocation("control_points"));
        locations.insert("control_points_count", pipeShader->uniformLocation("control_points_count"));

        locations.insert("dt", pipeShader->uniformLocation("dt"));
        locations.insert("r", pipeShader->uniformLocation("r"));
        locations.insert("sector_angle_0", pipeShader->uniformLocation("sector_angle_0"));
        locations.insert("sector_angle_1", pipeShader->uniformLocation("sector_angle_1"));

        locations.insert("light.color", pipeShader->uniformLocation("light.color"));
        locations.insert("light.position", pipeShader->uniformLocation("light.position"));
        locations.insert("light.ambient", pipeShader->uniformLocation("light.ambient"));
        locations.insert("light.diffuse", pipeShader->uniformLocation("light.diffuse"));
        locations.insert("light.specular", pipeShader->uniformLocation("light.specular"));

        locations.insert("node.color", pipeShader->uniformLocation("node.color"));
        locations.insert("node.ambient", pipeShader->uniformLocation("node.ambient"));
        locations.insert("node.diffuse", pipeShader->uniformLocation("node.diffuse"));
        locations.insert("node.specular", pipeShader->uniformLocation("node.specular"));
        locations.insert("node.shininess", pipeShader->uniformLocation("node.shininess"));

        pipeShader->bindAttributeLocation("t", 0);
        pipeShader->release();

        mLocations.insert(Shader::Pipe, locations);

        qInfo() << Q_FUNC_INFO << "PipeShader is initialized.";
        qInfo() << Q_FUNC_INFO << "Uniform locations are:" << locations;
    }

    return true;
}

bool ShaderManager::bind(Shader shader) {
    return mPrograms.value(shader)->bind();
}

void ShaderManager::release(Shader shader) {
    mPrograms.value(shader)->release();
}

void ShaderManager::setUniformValue(Shader shader, const QString &name, int value) {
    mPrograms.value(shader)->setUniformValue(mLocations.value(shader).value(name), value);
}

void ShaderManager::setUniformValue(Shader shader, const QString &name, float value) {
    mPrograms.value(shader)->setUniformValue(mLocations.value(shader).value(name), value);
}

void ShaderManager::setUniformValue(Shader shader, const QString &name, const QVector3D &value) {
    mPrograms.value(shader)->setUniformValue(mLocations.value(shader).value(name), value);
}

void ShaderManager::setUniformValue(Shader shader, const QString &name, const QVector4D &value) {
    mPrograms.value(shader)->setUniformValue(mLocations.value(shader).value(name), value);
}

void ShaderManager::setUniformValue(Shader shader, const QString &name, const QMatrix4x4 &value) {
    mPrograms.value(shader)->setUniformValue(mLocations.value(shader).value(name), value);
}

void ShaderManager::setUniformValue(Shader shader, const QString &name, const QMatrix3x3 &value) {
    mPrograms.value(shader)->setUniformValue(mLocations.value(shader).value(name), value);
}

void ShaderManager::setUniformValueArray(Shader shader, const QString &name, const QVector<QVector3D> &values) {
    mPrograms.value(shader)->setUniformValueArray(mLocations.value(shader).value(name), values.constData(), values.size());
}

ShaderManager *ShaderManager::instance() {
    static ShaderManager instance;

    return &instance;
}

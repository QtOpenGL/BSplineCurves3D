#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class ShaderManager : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit ShaderManager(QObject *parent = nullptr);

    enum class Shader { //
        None,
        Basic,
        Path,
        PipeDumb,
        PipeSmart
    };

    bool init();
    bool bind(Shader shader);
    void release();

    void setUniformValue(const QString &name, int value);
    void setUniformValue(const QString &name, float value);
    void setUniformValue(const QString &name, const QVector3D &value);
    void setUniformValue(const QString &name, const QVector4D &value);
    void setUniformValue(const QString &name, const QMatrix4x4 &value);
    void setUniformValue(const QString &name, const QMatrix3x3 &value);
    void setUniformValueArray(const QString &name, const QVector<QVector3D> &values);

    static ShaderManager *instance();

private:
    Shader mActiveShader;
    QMap<Shader, QOpenGLShaderProgram *> mPrograms;
    QMap<Shader, QMap<QString, GLuint>> mLocations;
};

#endif // SHADERMANAGER_H

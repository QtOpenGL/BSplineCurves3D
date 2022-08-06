#ifndef PIPESHADER_H
#define PIPESHADER_H

#include <QObject>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class PipeShader : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit PipeShader(QObject *parent = nullptr);
    ~PipeShader();

    bool init();
    bool bind();
    void release();

    void setUniformValue(const QString &name, int value);
    void setUniformValue(const QString &name, float value);
    void setUniformValue(const QString &name, const QVector3D &value);
    void setUniformValue(const QString &name, const QVector4D &value);
    void setUniformValue(const QString &name, const QMatrix4x4 &value);
    void setUniformValue(const QString &name, const QMatrix3x3 &value);
    void setUniformValueArray(const QString &name, const QVector<QVector3D> &values);

private:
    QOpenGLShaderProgram *mProgram;
    QMap<QString, GLuint> mLocations;
};

#endif // PIPESHADER_H

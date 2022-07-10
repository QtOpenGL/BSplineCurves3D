#ifndef PATHSHADER_H
#define PATHSHADER_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class PathShader : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit PathShader(QObject *parent = nullptr);
    ~PathShader();

    bool init();
    bool bind();
    void release();

    void setUniformValue(const QString &name, int value);
    void setUniformValue(const QString &name, float value);
    void setUniformValue(const QString &name, const QVector3D &value);
    void setUniformValue(const QString &name, const QVector4D &value);
    void setUniformValue(const QString &name, const QMatrix4x4 &value);
    void setUniformValueArray(const QString &name, const QVector<QVector3D> &values);

private:
    QOpenGLShaderProgram *mProgram;
    QMap<QString, GLuint> mLocations;
};

#endif // PATHSHADER_H

#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLFunctions>
#include <QOpenGLWindow>

class Controller;

class Window : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Window(QWindow *parent = nullptr);

signals:
    void init();
    void resized(int w, int h);
    void render(float ifps);
    void keyPressed(QKeyEvent *);
    void keyReleased(QKeyEvent *);
    void mousePressed(QMouseEvent *);
    void mouseReleased(QMouseEvent *);
    void mouseMoved(QMouseEvent *);
    void wheelMoved(QWheelEvent *);

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

    long long mPreviousTime;
    long long mCurrentTime;
};
#endif // WINDOW_H

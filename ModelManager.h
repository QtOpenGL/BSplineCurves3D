#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "Model.h"
#include <QObject>

class ModelManager : public QObject
{
    Q_OBJECT

private:
    explicit ModelManager(QObject *parent = nullptr);

public:
    void addModel(Model *model);
    void removeModel(Model *model);

    const QList<Model *> &models() const;

    static ModelManager *instance();

private:
    QList<Model *> mModels;
};

#endif // MODELMANAGER_H

#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "Model.h"
#include <QObject>

class ModelManager : public QObject
{
    Q_OBJECT

private:
    friend class Model;

    explicit ModelManager(QObject *parent = nullptr);
    void addModel(Model *model);
    void removeModel(Model *model);

    QList<Model *> mModels;

public:
    const QList<Model *> &models() const;

    static ModelManager *instance();
};

#endif // MODELMANAGER_H

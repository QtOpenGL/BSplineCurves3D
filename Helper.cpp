#include "Helper.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

Helper::Helper() {}

QByteArray Helper::getBytes(QString path) {
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        return file.readAll();
    } else {
        qWarning() << QString("Could not open '%1'").arg(path);
        return QByteArray();
    }
}

QList<Spline *> Helper::loadCurveDataFromJson(const QString &filename) {
    QJsonDocument document;

    // Read the file
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            qCritical() << "Error occured while loading the file:" << filename;
            return QList<Spline *>();
        }

        document = QJsonDocument::fromJson(file.readAll());
        file.close();
    }

    QList<Spline *> curves;
    QJsonArray curvesArray = document.array();

    for (const auto &element : qAsConst(curvesArray)) {
        Spline *curve = new Spline;

        QJsonObject curveObject = element.toObject();
        QJsonArray knotsArray = curveObject["knots"].toArray();
        float r = curveObject["r"].toDouble();
        int sectorCount = curveObject["sector_count"].toInt();
        curve->setRadius(r);
        curve->setSectorCount(sectorCount);

        for (const auto &knotElement : qAsConst(knotsArray)) {
            QJsonObject knotObject = knotElement.toObject();
            QJsonObject positionObject = knotObject["position"].toObject();
            float x = positionObject["x"].toDouble();
            float y = positionObject["y"].toDouble();
            float z = positionObject["z"].toDouble();

            KnotPoint *knot = new KnotPoint(x, y, z);
            curve->addKnotPoint(knot);
        }

        curves << curve;
    }

    return curves;
}

bool Helper::saveCurveDataToJson(const QList<Spline *> &curves, const QString &filename) {
    QJsonArray curvesArray;
    for (auto &curve : curves) {
        QJsonObject curveObject;
        QJsonArray knotsArray;
        QList<KnotPoint *> knots = curve->knotPoints();

        for (auto &knot : knots) {
            QJsonObject knotObject;

            QJsonObject position;
            position.insert("x", knot->position().x());
            position.insert("y", knot->position().y());
            position.insert("z", knot->position().z());

            knotObject.insert("position", position);

            knotsArray << knotObject;
        }

        curveObject.insert("r", curve->radius());
        curveObject.insert("sector_count", curve->sectorCount());
        curveObject.insert("knots", knotsArray);

        curvesArray << curveObject;
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument document;
        document.setArray(curvesArray);
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << document.toJson(QJsonDocument::Indented);
        stream.flush();
        file.close();
        return true;
    } else {
        qCritical() << Q_FUNC_INFO << "Couldn't write to file" << filename;
        return false;
    }
}

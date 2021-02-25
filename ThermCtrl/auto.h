#pragma once
#include <QObject>

class Auto : public QObject {
    Q_OBJECT
public:
    explicit Auto(QObject* parent = nullptr);

signals:
};

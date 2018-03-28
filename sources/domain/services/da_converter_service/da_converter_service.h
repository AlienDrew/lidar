#ifndef DA_CONVERTER_SERVICE_H
#define DA_CONVERTER_SERVICE_H

#include <QObject>
#include "dto_traits.h"

class DAConverterService : public QObject
{
    Q_OBJECT
public:
    explicit DAConverterService(QObject *parent = nullptr);
    ~DAConverterService() override;

signals:

public slots:
};

#endif // DA_CONVERTER_SERVICE_H

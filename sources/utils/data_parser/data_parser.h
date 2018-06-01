#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <QObject>

class QIODevice;

namespace utils
{
    class DataParser : public QObject
    {
        Q_OBJECT
    public:
        DataParser(QObject* parent = nullptr);
        void parse(QByteArray data);
        void setXYIODevice(QIODevice* device);
        ~DataParser() override;
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // DATA_PARSER_H

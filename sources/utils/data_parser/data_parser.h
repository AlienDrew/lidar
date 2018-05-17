#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <QObject>

namespace utils
{
    class DataParser : public QObject
    {
        Q_OBJECT
    public:
        DataParser(QObject* parent = nullptr);
        void parse(QByteArray data);
        ~DataParser() override;
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // DATA_PARSER_H

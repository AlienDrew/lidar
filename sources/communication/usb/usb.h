#ifndef USB_H
#define USB_H

#include <QObject>

namespace communication
{
    class USB : public QObject
    {
        Q_OBJECT
    public:
        USB(QObject* parent = nullptr);
        ~USB() override;

        void initialize(bool debug = false);
        void open();
        bool isOpened() const;
        void close();

        void asyncBulkReadTransfer();
        void cancelAsynchBulkReadTransfer();
        void bulkWriteTransfer(QByteArray& data);

        //---DO NOT USE OUTSIDE---
        void setDataAvailable(bool value);
        QByteArray dataIn() const;
        int packetSize();
        bool isEventThreadEnabled() const;
        //------------------------
    signals:
        void readyRead(QByteArray data);

    private:
        void eventThread();

        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // USB_H

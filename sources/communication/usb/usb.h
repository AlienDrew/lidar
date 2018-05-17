#ifndef USB_H
#define USB_H

#include <QObject>

struct libusb_device_handle;

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
        bool bulkWriteTransfer(QByteArray& data);

        //---DO NOT USE OUTSIDE---
        libusb_device_handle* handle() const;
        void setDataAvailable(bool value);
        QByteArray dataIn() const;
        int packetSize();
        bool isEventThreadEnabled() const;
        //------------------------
    signals:
        void readyRead(QByteArray data);
        void deviceDisconnected();

    private:
        void eventThread();

        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // USB_H

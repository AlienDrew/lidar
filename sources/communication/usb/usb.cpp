#include "usb.h"

#include "settings_provider.h"

#include "libusb.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#define USB_ENDPOINT_IN 0x81
#define USB_ENDPOINT_OUT 0x01
#define DEV_INTF 0

using namespace communication;

class USB::Impl
{
public:
    bool isInitialized = false;
    bool isOpen = false;
    bool enableEventThread = true;
    QFuture<void> future;

    libusb_context *context = nullptr;
    libusb_device_handle* handle = nullptr;
    libusb_transfer* pcToUsbTransfer = nullptr;
    libusb_transfer* usbToPcTransfer = nullptr;

    unsigned char inBuf[64];
    int actualPacketSize;
    bool dataAvailable = false;
};

USB::USB(QObject *parent): QObject(parent),
    d(new Impl)
{
    d->actualPacketSize = settingsProvider->value(settings::communication::usb::packetSize).toInt();
}

USB::~USB()
{
    d->enableEventThread = false;
    d->future.waitForFinished();
}

extern "C" void LIBUSB_CALL asyncBulkReadTransferCallback(struct libusb_transfer* transfer)
{
    USB* cthis = static_cast<USB*>(transfer->user_data);
    switch (transfer->status)
    {
    case LIBUSB_TRANSFER_COMPLETED:
        cthis->setDataAvailable(true);
        if(cthis->isEventThreadEnabled())
        {
            if (cthis->packetSize() == transfer->actual_length)
                cthis->readyRead(cthis->dataIn());
            //libusb_submit_transfer(transfer);
        }
        break;
    case LIBUSB_TRANSFER_CANCELLED:
        qDebug()<<"usb transfer callback: cancelled!";
        break;
    case LIBUSB_TRANSFER_NO_DEVICE:
        qDebug()<<"usb transfer callback: no device!";
        break;
    default:
        qDebug()<<"usb transfer callback: some error occured";
        break;
    }
    libusb_submit_transfer(transfer);
}

extern "C" void LIBUSB_CALL asyncBulkWriteTransferCallback(struct libusb_transfer* transfer)
{

}

void USB::initialize(bool debug)
{
    int status;
    status = libusb_init(&d->context);
    if(status != 0)
    {
        qDebug()<<"Initialization Error! "<<status<<endl; //there was an error
        d->isInitialized = false;
        return;
    }
    if (debug)
        libusb_set_debug(d->context, LIBUSB_LOG_LEVEL_DEBUG);
    d->isInitialized = true;
    //d->future = QtConcurrent::run(this, &USB::eventThread);
}

void USB::open()
{
    d->handle = libusb_open_device_with_vid_pid(d->context, settingsProvider->value(settings::communication::usb::VID).toUInt(),
                                                settingsProvider->value(settings::communication::usb::PID).toUInt());
    if (d->handle == nullptr)
    {
        qCritical("Cannot open usb device. It's probably not connected");
        d->isOpen = false;
        return;
    }
    if (libusb_kernel_driver_active(d->handle, DEV_INTF))
        libusb_detach_kernel_driver(d->handle, DEV_INTF);
    if (libusb_claim_interface(d->handle, DEV_INTF) < 0)
    {
        qCritical("cannot claim interface");
        libusb_close(d->handle);
        d->isOpen = false;
        return;
    }
    d->isOpen = true;
}

bool USB::isOpened() const
{
    return d->isOpen;
}

void USB::close()
{
    if (isOpened())
    {
        if(d->usbToPcTransfer)
        {
            libusb_cancel_transfer(d->usbToPcTransfer);
        }
        if(d->pcToUsbTransfer)
        {
            libusb_cancel_transfer(d->pcToUsbTransfer);
        }
        d->enableEventThread = false;
        //d->future.cancel();
        d->future.waitForFinished();
        //libusb_attach_kernel_driver(d->handle, 0);
        libusb_release_interface(d->handle, DEV_INTF);
        libusb_close(d->handle);
        libusb_exit(d->context);
        d->isOpen = false;
        d->isInitialized = false;
    }
    else if (d->isInitialized)
    {
        d->enableEventThread = false;
        libusb_exit(d->context);
        d->isInitialized = false;
    }
}

void USB::setDataAvailable(bool value)
{
    d->dataAvailable = value;
}

QByteArray USB::dataIn() const
{
    QByteArray data = QByteArray(reinterpret_cast<char*>(d->inBuf), 64);
    return data;
}

int USB::packetSize()
{
    return d->actualPacketSize;
}

bool USB::isEventThreadEnabled() const
{
    return d->enableEventThread;
}

void USB::asyncBulkReadTransfer()
{
    if(!isOpened())
    {
        qDebug()<<"Device not connected";
        return;
    }
    int status;
    d->usbToPcTransfer = libusb_alloc_transfer(0);
    libusb_fill_bulk_transfer(d->usbToPcTransfer, d->handle, USB_ENDPOINT_IN,
                               d->inBuf, packetSize(), asyncBulkReadTransferCallback, this, 0);
    status = libusb_submit_transfer(d->usbToPcTransfer);
    if(status!=0)
    {
        qCritical("Transfer Failed! %d", status);
        return;
    }
    if(!d->future.isRunning())
        d->future = QtConcurrent::run(this,&USB::eventThread);
}

void USB::cancelAsynchBulkReadTransfer()
{
    if(d->usbToPcTransfer)
    {
        libusb_cancel_transfer(d->usbToPcTransfer);
    }
}

bool USB::bulkWriteTransfer(QByteArray& data)
{
    if (!isOpened())
    {
        qCritical("cannot write data. usb device is not opened/connected!");
        return false;
    }
    int status;
    int transferred = 0;

    uint8_t actualSize = data.size();
    data.prepend(actualSize);

    status = libusb_interrupt_transfer(d->handle, USB_ENDPOINT_OUT, (unsigned char*)data.data(), packetSize(), &transferred, 0);
    //status = libusb_interrupt_transfer(d->handle, 0x01, (unsigned char*)data.data(), actual, &transferred, 0);

    if (status < 0)
    {
        qDebug() << "Error sending data!" << libusb_error_name(status);
        return false;
    }
//    if (transferred != packetSize())
//        qDebug()<<"Error sending data! transferred bytes doesn't equal packet size";
    else
    {
        qDebug() << "Sended data" << data;
    }
    return true;
}

void USB::eventThread()
{
    timeval zero_tv;
    libusb_submit_transfer(d->usbToPcTransfer);
    while (d->enableEventThread)
    {
        //libusb_handle_events(d->context);
        if (libusb_handle_events_timeout(d->context, &zero_tv)!= LIBUSB_SUCCESS) break;
        //if (libusb_handle_events(d->context) != LIBUSB_SUCCESS) break;
    }
}

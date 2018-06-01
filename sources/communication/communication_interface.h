#ifndef COMMUNICATION_INTERFACE_H
#define COMMUNICATION_INTERFACE_H

#include <QObject>

namespace communication
{
    class CommunicationInterface : public QObject
    {
        Q_OBJECT
    public:
        CommunicationInterface();
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual bool isOpen() = 0;

        virtual void read() = 0;
        virtual void cancelRead() = 0;
        virtual bool write(QByteArray data) = 0;
    signals:
        void readyRead(QByteArray data);
    protected:
        bool m_isOpened = false;
    private:
    };
}

#endif // COMMUNICATION_INTERFACE_H

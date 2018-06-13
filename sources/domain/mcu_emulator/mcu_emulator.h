#ifndef MCU_EMULATOR_H
#define MCU_EMULATOR_H

#include <QObject>

namespace domain
{
    class MCUEmulator : public QObject
    {
        Q_OBJECT
    public:
        MCUEmulator(QObject* parent = nullptr);
        ~MCUEmulator() override;
    public slots:
        void prepareADCData();
        void sendADCData();
        void sendTemperature();
        void sendPeriphStatus();
    signals:
        void readyRead(QByteArray data);
        void finishedRead();
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // MCU_EMULATOR_H

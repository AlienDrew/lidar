#include "mcu_emulator.h"

#include "settings_provider.h"
#include "command.h"

#include <QVector>
#include <QtMath>
#include <QRandomGenerator>

using namespace domain;

class MCUEmulator::Impl
{
public:
    //===ADC===
    QVector<int> ch0;
    QVector<int> ch1;
    int numOfSamples;
    qreal amplitude;
    int freq;
    qreal time;
    qreal maxTime;
    qreal deltaTime;
    //==============
};

MCUEmulator::MCUEmulator(QObject* parent): QObject(parent),
    d(new Impl)
{
    d->numOfSamples = settingsProvider->value(settings::adc::maxNumberOfSamples).toInt();
    d->freq = settingsProvider->value(settings::adc::samplingFreq).toInt();
    d->maxTime = d->numOfSamples/(qreal)d->freq*1000000;
    d->deltaTime = 1/(qreal)d->freq;
}

MCUEmulator::~MCUEmulator()
{

}

void MCUEmulator::prepareADCData()
{
    d->time = 0;
    d->amplitude = 0.80;
    for (int sample = 0; sample<d->numOfSamples; ++sample)
    {
        d->ch0.append(settingsProvider->value(settings::adc::maxVal).toInt()/settingsProvider->value(settings::adc::vRef).toReal()
                      *d->amplitude);//*qSin(2*M_PI*d->freq*d->time)
        d->time += d->deltaTime;
        if (d->amplitude>=0.80 && d->amplitude<=1.3)
            d->amplitude += 0.10;
        else if (d->amplitude>1.3)
            d->amplitude -= 0.10;
    }

    d->time = 0;
    d->amplitude = 0.80;
    for (int sample = 0; sample<d->numOfSamples; ++sample)
    {
        d->ch1.append(settingsProvider->value(settings::adc::maxVal).toInt()/settingsProvider->value(settings::adc::vRef).toReal()
                      *d->amplitude); //qSin(2*M_PI*d->freq*d->time)
        d->time += d->deltaTime;
        if (d->amplitude>=0.80 && d->amplitude<=1.3)
            d->amplitude += 0.10;
        else if (d->amplitude>1.3)
            d->amplitude -= 0.10;
    }
}

void MCUEmulator::sendADCData()
{
    int dataSize;
    if (d->ch0.size() == 0)
        prepareADCData();
    //===adc data emulator===
    if (d->ch0.size() == d->ch1.size() && d->ch0.size() != 0)
        dataSize = d->ch0.size();
    else
    {
        //d->timer->stop();
        emit finishedRead();
        return;
    }
    int maxLen = 64;
    QByteArray data;
    int i = 0;
    int j = 1;
    data.append(dto::Command::adc_data);
    while (j<maxLen && i<qMin(dataSize, 15))
    {
        data[j++] = (uint8_t)((d->ch0[i] & 0xFF00) >> 8);
        data[j++] = (uint8_t)(d->ch0[i] & 0x00FF);

        data[j++] = (uint8_t)((d->ch1[i] & 0xFF00) >> 8);
        data[j++] = (uint8_t)(d->ch1[i] & 0x00FF);
        i++;
    }
    d->ch0.remove(0, i);
    d->ch1.remove(0, i);
    dataSize = d->ch0.size();
    if (dataSize == 0)
    {
        //d->timer->stop();
        emit finishedRead();
    }
    //================
    emit readyRead(data);
}

void MCUEmulator::sendTemperature()
{
    QByteArray data;
    data.append(dto::Command::temp_sensor_received);
    data.append(0b00011001);
    //add temp data
    emit readyRead(data);
}

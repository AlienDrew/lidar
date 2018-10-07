#include "adc_service.h"

#include "settings_provider.h"

#include <QVector>

using namespace domain;

class ADCService::Impl
{
public:
    QVector<int> ch1Data;
    QVector<int> ch2Data;
};

namespace
{
    uint16_t convertFrom8To16(uint8_t dataFirst, uint8_t dataSecond)
    {
        uint16_t dataBoth = 0x0000;
        dataBoth = dataFirst;
        dataBoth = dataBoth << 8;
        dataBoth |= dataSecond;
        return dataBoth;
    }
}

ADCService::ADCService(QObject* parent):BasePeripheralService(settingsProvider->value(settings::adc::channelCount).toInt(), parent),
    d(new Impl)
{
}

void ADCService::updateADCChannel(const QByteArray& data)
{
    for (int i = 0; i<data.size(); i+=4)
    {
        d->ch1Data.append(convertFrom8To16(data[i], data[i+1]));
        d->ch2Data.append(convertFrom8To16(data[i+2], data[i+3]));
    }
    if (d->ch1Data.size() == d->ch2Data.size() && d->ch1Data.size() == 750)//settingsProvider->value(settings::adc::maxNumberOfSamples).toInt())
    {
        emit adcDataUpdated();
        d->ch1Data.clear();
        d->ch2Data.clear();
    }
}

const QVector<int> ADCService::ch1Data() const
{
    return d->ch1Data;
}

const QVector<int> ADCService::ch2Data() const
{
    return d->ch2Data;
}

ADCService::~ADCService()
{

}

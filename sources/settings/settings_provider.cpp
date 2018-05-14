#include "settings_provider.h"

#include <QMap>

using namespace settings;

namespace
{
    QMap <QString, QVariant> defaultSettings =
    {
        {dac::maxVal, 256},
        {dac::vRef, 3.3},
        {dac::channelCount, 1},
        {freq_generator::channelCount, 2},
        {freq_generator::maxFrequency, 200000000},
        {communication::usb::VID, 1155},
        {communication::usb::PID, 22352},
        {communication::usb::packetSize, 64}
    };
}

Provider* Provider::instance()
{
    static Provider self;
    return &self;
}

Provider::Provider()
{

}

Provider::~Provider()
{

}

QVariant Provider::value(const QString &key)
{
    return defaultSettings.value(key);
}

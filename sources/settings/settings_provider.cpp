#include "settings_provider.h"

#include <QMap>

using namespace settings;

namespace
{
    QMap <QString, QVariant> defaultSettings =
    {
        //ad5302armz used
        {dac::maxVal, 255},
        {dac::vRef, 3.3},
        {dac::channelCount, 2},

        //si5351 used
        {freq_generator::channelCount, 2},
        {freq_generator::minFrequency, 2500},
        {freq_generator::maxFrequency, 200000000},
        {freq_generator::autoUnitConversion, false},

        //mcp41xx used
        {digital_potentiometer::maxVal, 255},
        {digital_potentiometer::vRef, 3.3},
        {digital_potentiometer::channelCount, 1},
        {digital_potentiometer::minK, 3},
        {digital_potentiometer::maxK, 100},

        //internal stm32 adc used
        {adc::maxVal, 4095},
        {adc::vRef, 3.3},
        {adc::samplingFreq, 500000},
        {adc::maxNumberOfSamples, 800},

        {temp_sensor::channelCount, 1},
        {temp_sensor::minTemp, -10},
        {temp_sensor::maxTemp, 50},

        //custom HID device
        {communication::usb::VID, 1155},
        {communication::usb::PID, 22352},
        {communication::usb::packetSize, 64},
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

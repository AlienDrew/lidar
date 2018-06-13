#include "data_parser.h"

#include "service_registry.h"
#include "freq_generator_service.h"
#include "da_converter_service.h"
#include "dcdc_switch_service.h"
#include "digital_potentiometer_service.h"
#include "temperature_service.h"
#include "command.h"

#include <QIODevice>
#include <QDebug>

using namespace utils;

class DataParser::Impl
{
public:
    QIODevice* xyseriesDevice = nullptr;
};

namespace
{
    uint32_t toUint32(const QByteArray& buffer, int index)
    {
        uint32_t converted = 0;
        converted = (buffer[index]&0xFF) | (buffer[index+1]<<8 & 0xFFFF)|(buffer[index+2]<<16 & 0xFFFFFF)|(buffer[index+3]<<24 & 0xFFFFFFFF);
        return converted;
    }
}

DataParser::DataParser(QObject* parent) : QObject(parent),
    d(new Impl)
{

}

void DataParser::parse(QByteArray data)
{
    int command = data.front();
    data.remove(0, 1);
    switch (command)
    {
    case dto::Command::temp_sensor_received:
        serviceRegistry->temperatureService()->updateTemperature(0, (data[0]<<8)|(data[1]&0xFF));
        break;

//    case dto::Command::dac:
//        serviceRegistry->daConverterService()->updateDAC(data[0], (uint8_t)data[2], data[1], false);
//        break;
//    case dto::Command::dcdc_switch:
//        serviceRegistry->dcdcSwitchService()->updateSwitch(data[0], false);
//        break;
//    case dto::Command::digital_pot:
//        serviceRegistry->digitalPotentiometerSerivce()->updatePotentiometer(data[0], (uint8_t)data[2], data[1], false);
//        break;
//    case dto::Command::gen:
//        serviceRegistry->freqGeneratorService()->updateGenerator(data[0], toUint32(data, 2), data[1], false);
//        break;

    case dto::Command::periph_status:
        serviceRegistry->daConverterService()->updateDAC(0, (uint8_t)data[1], data[0], false);
        serviceRegistry->dcdcSwitchService()->updateSwitch(data[2], false);
        serviceRegistry->daConverterService()->updateDAC(1, (uint8_t)data[4], data[3], false);
        serviceRegistry->digitalPotentiometerSerivce()->updatePotentiometer(0, (uint8_t)data[6], data[5], false);
        //data[7] digitalPot ch2_enabled
        //data[8] digitalPot ch2
        serviceRegistry->freqGeneratorService()->updateGenerator(0, toUint32(data, 10), data[9], false);
        serviceRegistry->freqGeneratorService()->updateGenerator(1, toUint32(data, 15), data[14], false);

        break;
    case dto::Command::adc_data:
        data.remove(60, 3);
        if (d->xyseriesDevice)
        {
            if (d->xyseriesDevice->isOpen())
                d->xyseriesDevice->write(data);
        }
        break;
    default:
        break;
    }
}

void DataParser::setXYIODevice(QIODevice* device)
{
    d->xyseriesDevice = device;
}

DataParser::~DataParser()
{

}

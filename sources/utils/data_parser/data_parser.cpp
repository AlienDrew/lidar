#include "data_parser.h"

#include "service_registry.h"
#include "freq_generator_service.h"
#include "da_converter_service.h"
#include "digital_potentiometer_service.h"
#include "temperature_service.h"
#include "command.h"

#include <QIODevice>

using namespace utils;

class DataParser::Impl
{
public:
    struct PeriphState
    {

    };
    PeriphState currPeriphState;
    QIODevice* xyseriesDevice = nullptr;
};

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
        //qDebug()<<"temp:"<<(((data[0]*256)+(data[1]&0xF0))/16)*0.0625<<"C";
        serviceRegistry->temperatureService()->updateTemperature(0, (data[0]<<8)|(data[1]&0xFF));
        break;
    case dto::Command::telemetry_received:
//        serviceRegistry->daConverterService()->updateDAC(0, data[1]);
//        serviceRegistry->daConverterService()->updateDAC(1, data[2]);
//        serviceRegistry->digitalPotentiometerSerivce()->updateChannel(0, data[3]);
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

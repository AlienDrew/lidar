#include "data_parser.h"

#include "service_registry.h"
#include "freq_generator_service.h"
#include "da_converter_service.h"
#include "digital_potentiometer_service.h"

using namespace utils;

class DataParser::Impl
{
public:
    struct PeriphState
    {

    };
    PeriphState currPeriphState;
};

DataParser::DataParser(QObject* parent) : QObject(parent),
    d(new Impl)
{

}

void DataParser::parse(QByteArray data)
{
    int command = data.front();
    switch (command)
    {
    case 0:
//        serviceRegistry->daConverterService()->updateDAC(0, data[1]);
//        serviceRegistry->daConverterService()->updateDAC(1, data[2]);
//        serviceRegistry->digitalPotentiometerSerivce()->updateChannel(0, data[3]);
        break;
    case 1:
        //device->write(data);
        break;
    default:
        break;
    }
}

DataParser::~DataParser()
{

}

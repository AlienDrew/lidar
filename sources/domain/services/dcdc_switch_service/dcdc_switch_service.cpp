#include "dcdc_switch_service.h"

#include "channel.h"

using namespace domain;

DCDCSwitchService::DCDCSwitchService(QObject* parent): BasePeripheralService(1, parent)
{

}

void DCDCSwitchService::updateSwitch(bool isOn, bool toMCU)
{
    BasePeripheralService::updateChannel(0, 0, isOn, toMCU);
}

void DCDCSwitchService::updateSwitch(dto::ChannelPtr channel, bool toMCU)
{
    this->updateChannel(channel, toMCU);
}

DCDCSwitchService::~DCDCSwitchService()
{

}

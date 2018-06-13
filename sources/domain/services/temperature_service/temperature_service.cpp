#include "temperature_service.h"

#include "settings_provider.h"
#include "channel.h"

using namespace domain;

TemperatureService::TemperatureService(QObject* parent) :
    BasePeripheralService(settingsProvider->value(settings::temp_sensor::channelCount).toInt(), parent)
{
    connect(this, &TemperatureService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        uint8_t lower = ((uint16_t)channel->value()) & 0xFF;
        uint8_t higher = ((uint16_t)channel->value())>>8;
        qreal temp = (((higher*256)+(lower&0xF0))/16)*0.0625;
        emit temperatureUpdated(temp);

        if (temp<settingsProvider->value(settings::temp_sensor::minTemp).toInt() &&
                temp>settingsProvider->value(settings::temp_sensor::maxTemp).toInt())
        {
            emit temperatureAlert();
        }
    });
}

TemperatureService::~TemperatureService()
{

}

void TemperatureService::updateTemperature(int chId, quint32 temperature)
{
    if (chId<settingsProvider->value(settings::temp_sensor::channelCount).toInt())
        BasePeripheralService::updateChannel(chId, temperature, true, false);
}

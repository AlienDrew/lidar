#include "settings_provider.h"

#include <QMap>

using namespace settings;

namespace
{
    QMap <QString, QVariant> defaultSettings =
    {
        {DACoverterSettings::bitDepth, 12},
        {FreqGeneratorSettings::channelCount, 3},
        {FreqGeneratorSettings::maxFrequency, 200000000}
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

#ifndef SETTINGS_PROVIDER_H
#define SETTINGS_PROVIDER_H

#include "settings.h"

#include <QVariant>

namespace settings
{
    class Provider : public QObject
    {
        Q_OBJECT
    public:
        static Provider* instance();
        QVariant value(const QString& key);
    private:
        Provider();
        ~Provider() override;

        Q_DISABLE_COPY(Provider)
    };
}

#define settingsProvider (settings::Provider::instance())

#endif

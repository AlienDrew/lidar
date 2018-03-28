#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QWidget>
#include "settings.h"

namespace Ui {
    class SettingsWindow;
}

namespace presentation
{
    class SettingsWindow : public QWidget
    {
        Q_OBJECT

    public:
        explicit SettingsWindow(QWidget* parent = nullptr);
        ~SettingsWindow() override;

    private:
        Ui::SettingsWindow* ui;
    };
}

#endif // SETTINGS_WINDOW_H

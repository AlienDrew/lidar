#ifndef PRESENTATION_MANAGER_H
#define PRESENTATION_MANAGER_H

#include <QObject>

#include "main_window.h"
#include "settings_window.h"
#include "about_dialog.h"
#include "chart_window.h"

namespace presentation
{
    class PresentationManager : public QObject
    {
        Q_OBJECT
    public:
        PresentationManager(QObject *parent = nullptr);
        ~PresentationManager() override;

        presentation::MainWindow* mainWindow() const;
        presentation::SettingsWindow* settingsWindow() const;
        presentation::AboutDialog* aboutDialog() const;
    signals:

    private:
        class Impl;
        const QScopedPointer<Impl> d;
    };
}

#endif // PRESENTATION_MANAGER_H

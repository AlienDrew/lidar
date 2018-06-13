#ifndef PRESENTATION_MANAGER_H
#define PRESENTATION_MANAGER_H

#include <QObject>

#include "main_window.h"
#include "status_window.h"
#include "settings_window.h"
#include "about_dialog.h"
#include "chart_window.h"

namespace presentation
{
    class PresentationManager : public QObject
    {
        Q_OBJECT
    public:
        static PresentationManager* instance();
        PresentationManager(QObject *parent = nullptr);
        ~PresentationManager() override;

        MainWindow* mainWindow() const;
        StatusWindow* statusWindow() const;
        SettingsWindow* settingsWindow() const;
        AboutDialog* aboutDialog() const;
        ChartWindow* chartWindow() const;

    private:
        static PresentationManager* self;

        class Impl;
        const QScopedPointer<Impl> d;

        Q_DISABLE_COPY(PresentationManager)
    };
}

#define presentationManager (presentation::PresentationManager::instance())

#endif // PRESENTATION_MANAGER_H

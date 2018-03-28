#include "presentation_manager.h"

#include <QAction>
#include <QPointer>
#include <QDebug>

using namespace presentation;

namespace presentation
{
    namespace
    {
        const QString settings = "settings";
        const QString about = "about";
    }
}

class PresentationManager::Impl
{
public:
    QPointer< presentation::MainWindow > m_mainWindow;
    QPointer< presentation::SettingsWindow > m_settingsWindow;
    QPointer< presentation::AboutDialog > m_aboutDialog;
    QPointer < presentation::ChartWindow > m_chartWindow;
};

PresentationManager::PresentationManager(QObject *parent) : QObject(parent),
    d(new Impl)
{
    d->m_mainWindow = new presentation::MainWindow();
    d->m_chartWindow = new ChartWindow(d->m_mainWindow);
    d->m_chartWindow->setAttribute(Qt::WA_DeleteOnClose);
    d->m_chartWindow->show();

    connect(d->m_mainWindow->actionMap()[presentation::settings], &QAction::triggered, [this]() {
        if (mainWindow() && !settingsWindow())
        {
            d->m_settingsWindow = new SettingsWindow(d->m_mainWindow);
            d->m_settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
        }
        if (settingsWindow()->isHidden())
            settingsWindow()->show();
    });
    connect(d->m_mainWindow->actionMap()[presentation::about], &QAction::triggered, [this]() {
        if (mainWindow() && !aboutDialog())
        {
            d->m_aboutDialog = new AboutDialog(d->m_mainWindow);
            d->m_aboutDialog->setAttribute(Qt::WA_DeleteOnClose);
        }
        if (aboutDialog()->isHidden())
            aboutDialog()->show();
    });
}

PresentationManager::~PresentationManager()
{
    if (!d->m_mainWindow.isNull())
        delete d->m_mainWindow;
    qDebug()<<"presentation manager destroyed";
}

presentation::MainWindow* PresentationManager::mainWindow() const
{
    return d->m_mainWindow;
}

presentation::SettingsWindow* PresentationManager::settingsWindow() const
{
    return d->m_settingsWindow;
}

presentation::AboutDialog* PresentationManager::aboutDialog() const
{
    return d->m_aboutDialog;
}

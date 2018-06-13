#include "presentation_manager.h"

#include <QAction>
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

PresentationManager* PresentationManager::self = nullptr;

class PresentationManager::Impl
{
public:
    MainWindow m_mainWindow;
    StatusWindow m_statusWindow;
    SettingsWindow m_settingsWindow;
    AboutDialog m_aboutDialog;
    ChartWindow m_chartWindow;
};

PresentationManager* PresentationManager::instance()
{
    return PresentationManager::self;
}

PresentationManager::PresentationManager(QObject *parent) : QObject(parent),
    d(new Impl)
{
    PresentationManager::self = this;

    d->m_statusWindow.setParent(&d->m_mainWindow);
    d->m_statusWindow.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    d->m_aboutDialog.setParent(&d->m_mainWindow);
    d->m_aboutDialog.setWindowFlag(Qt::Window);
    d->m_settingsWindow.setParent(&d->m_mainWindow);
    d->m_settingsWindow.setWindowFlag(Qt::Window);
    d->m_chartWindow.setParent(&d->m_mainWindow);
    d->m_chartWindow.setWindowFlag(Qt::Window);
    //d->m_chartWindow.show();
    d->m_statusWindow.show();

    connect(d->m_mainWindow.actionMap()[presentation::settings], &QAction::triggered, [this]() {
        if (settingsWindow()->isHidden())
            settingsWindow()->show();
    });
    connect(d->m_mainWindow.actionMap()[presentation::about], &QAction::triggered, [this]() {
        if (aboutDialog()->isHidden())
            aboutDialog()->show();
    });
}

PresentationManager::~PresentationManager()
{
    qDebug()<<"presentation manager destroyed";
}

presentation::MainWindow* PresentationManager::mainWindow() const
{
    return &d->m_mainWindow;
}

StatusWindow*PresentationManager::statusWindow() const
{
    return &d->m_statusWindow;
}

presentation::SettingsWindow* PresentationManager::settingsWindow() const
{
    return &d->m_settingsWindow;
}

presentation::AboutDialog* PresentationManager::aboutDialog() const
{
    return &d->m_aboutDialog;
}

ChartWindow*PresentationManager::chartWindow() const
{
    return &d->m_chartWindow;
}

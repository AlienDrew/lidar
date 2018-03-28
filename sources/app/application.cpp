#include "application.h"
#include "application_impl.h"
#include "presentation_manager.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>

using namespace core;

Application::Application(int& argc, char** argv, const QString& appName, QObject* parent) : QObject(parent),
    d(new Impl(this, argc, argv, appName) )
{

}

Application::Application(QApplication &app, domain::ServiceRegistry& serviceReg, QObject* parent): QObject(parent),
    d(new Impl(this, app, serviceReg))
{

}

Application::~Application()
{
    qDebug()<<"app core destroyed";
}

bool core::Application::start()
{
    return d->start();
}

void Application::quit()
{
    d->quit();
}

void Application::exit(int retcode)
{
    d->exit(retcode);
}

int Application::exec()
{
    if (d->start())
        return d->exec();
    else
    {
        qCritical("some problems occurred during application startup!\nclosing...");
        return 1;
    }
}

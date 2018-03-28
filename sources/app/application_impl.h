#ifndef APPLICATION_IMPL_H
#define APPLICATION_IMPL_H

#include "application.h"

#include <QPointer>

class QApplication;
class QCoreApplication;
class QLockFile;

namespace presentation
{
    class PresentationManager;
}

namespace domain
{
    class ServiceRegistry;
}

namespace core
{
    class Application::Impl : public QObject
    {
        Q_OBJECT
    public:
        Impl(Application* parent, QApplication& app, domain::ServiceRegistry& serviceReg);
        Impl(Application* parent, int& argc, char** argv, const QString& appName);
        bool start();
        int exec();
        void quit();
        void exit(int retcode=0);
        ~Impl() override;
    private:
        void init(const QString& appName);
        QPointer< presentation::PresentationManager >m_presentationManager;
        domain::ServiceRegistry* m_serviceRegistry;

        QCoreApplication* m_app = nullptr;
        QLockFile* m_lock = nullptr;
        bool m_isExternalApp;
        bool m_isExternalService;
    };
}

#endif // APPLICATION_IMPL_H

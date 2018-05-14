#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>

class QApplication;

namespace domain
{
    class ServiceRegistry;
}

namespace presentation
{
    class PresentationManager;
}

namespace core
{
    class Application : public QObject
    {
        Q_OBJECT
    public:
        Application(int& argc, char** argv, const QString& appName, QObject* parent = nullptr);
        Application(QApplication& app, domain::ServiceRegistry& serviceReg,
                    presentation::PresentationManager& presentationMgr, QObject* parent = nullptr);
        ~Application() override;
        bool start();
        void quit();
        void exit(int retcode=0);
        int exec();
    private:
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        class Impl;
        const QScopedPointer<Impl> d;
    };
}
#endif // APPLICATION_H

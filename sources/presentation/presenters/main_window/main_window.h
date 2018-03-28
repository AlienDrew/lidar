#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

namespace presentation
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override;
        const QMap<QString, QAction*>& actionMap() const;
    private slots:
        void updateGenerator(double value, int chId);
    private:
        class Impl;
        Ui::MainWindow* ui;
        const QScopedPointer <Impl> d;
    };
}

#endif // MAINWINDOW_H

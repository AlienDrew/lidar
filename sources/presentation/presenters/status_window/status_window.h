#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW_H

#include <QWidget>

namespace Ui {
    class StatusWindow;
}

namespace presentation
{
    class StatusWindow : public QWidget
    {
        Q_OBJECT

    public:
        explicit StatusWindow(QWidget *parent = 0);
        ~StatusWindow() override;

    private:
        Ui::StatusWindow *ui;

        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // STATUS_WINDOW_H

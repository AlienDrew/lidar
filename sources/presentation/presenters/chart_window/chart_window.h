#ifndef CHART_WINDOW_H
#define CHART_WINDOW_H

#include <QWidget>

namespace Ui
{
    class ChartWindow;
}

namespace presentation
{
    class ChartWindow :public QWidget
    {
        Q_OBJECT
    public:
        explicit ChartWindow(QWidget *parent = 0);
        ~ChartWindow() override;
    private:
        Ui::ChartWindow* ui;

        class Impl;
        const QScopedPointer<Impl> d;
    };

}

#endif // CHART_WINDOW_H

#ifndef CHART_WINDOW_H
#define CHART_WINDOW_H

#include <QMainWindow>

namespace Ui {
    class ChartWindow;
}

namespace presentation
{
    class ChartWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit ChartWindow(QWidget *parent = 0);
        ~ChartWindow();
        void initCharts();
        void startReading();

    private slots:
        void saveData();

        void drag(bool checked);
        void resetZoom();
        void zoomOut();
        void zoomIn();

    protected:
        void closeEvent(QCloseEvent *event) override;
    private:
        QToolBar* createToolBar();

        Ui::ChartWindow *ui;
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // CHART_WINDOW_H

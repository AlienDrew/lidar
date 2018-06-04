#ifndef CHART_VIEW_H
#define CHART_VIEW_H

#include <QtCharts>

namespace presentation
{
    class ChartView : public QChartView
    {
        Q_OBJECT
    public:
        explicit ChartView(QWidget *parent = Q_NULLPTR);
        explicit ChartView(QChart *chart, QChart* nextChart, QWidget *parent = Q_NULLPTR);
        ~ChartView() override;
        void setDragable(bool value = true);
    signals:
        void mousePosChanged(QPoint pos);
    protected:
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void keyPressEvent(QKeyEvent *event) override;
        void keyReleaseEvent(QKeyEvent *event) override;
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // CHART_VIEW_H

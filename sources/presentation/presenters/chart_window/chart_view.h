#ifndef CHART_VIEW_H
#define CHART_VIEW_H

#include <QtCharts>

class ChartView : public QChartView
{
    Q_OBJECT
public:
    explicit ChartView(QWidget *parent = Q_NULLPTR);
    explicit ChartView(QChart *chart, QWidget *parent = Q_NULLPTR);
signals:
    void mousePosChanged(QPoint pos);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    QPoint mousePos;
};

#endif // CHART_VIEW_H

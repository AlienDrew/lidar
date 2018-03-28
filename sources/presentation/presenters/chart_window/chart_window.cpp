#include "chart_window.h"
#include "ui_chart_window.h"

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartGlobal>
#include <QtWidgets/QVBoxLayout>

using namespace presentation;
QT_CHARTS_USE_NAMESPACE

class ChartWindow::Impl
{
public:
    QChart* chart;
    QLineSeries* series;
};

ChartWindow::ChartWindow(QWidget* parent) : QWidget(parent),
    d(new Impl)
{
    QWidget::setWindowFlags(Qt::Window);
    d->chart = new QChart;
    QChartView* chartView = new QChartView(d->chart);
    chartView->setMinimumSize(1000, 400);
    d->series = new QLineSeries;
    d->chart->addSeries(d->series);
    QValueAxis* axisX = new QValueAxis(this);
    axisX->setRange(0, 2000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");
    QValueAxis* axisY = new QValueAxis(this);
    axisY->setRange(0, 10);
    axisY->setTitleText("Value level");
    d->chart->setAxisX(axisX, d->series);
    d->chart->setAxisY(axisY, d->series);
    d->chart->legend()->hide();
    d->chart->setTitle("Data from ADC");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);
}

ChartWindow::~ChartWindow()
{
    delete d->series;
    delete d->chart;
}

#include "chart_window.h"
#include "ui_chart_window.h"

#include "service_registry.h"
#include "transfer_service.h"
#include "settings_provider.h"
#include "xyseries_iodevice.h"
#include "chart_view.h"

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartGlobal>
#include <QtWidgets/QVBoxLayout>
#include <QToolBar>
#include <QDebug>

using namespace presentation;
QT_CHARTS_USE_NAMESPACE

class ChartWindow::Impl
{
public:
    QChart* chart;
    ChartView* chartView;
    QLineSeries* series;

    QChart* chart2;
    ChartView* chartView2;
    QLineSeries* ch2Series;

    XYSeriesIODevice* device;
    domain::TransferService* transferService;

    QPoint mousePos;
};

ChartWindow::ChartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChartWindow), d(new Impl)
{
    ui->setupUi(this);
    addToolBar(Qt::TopToolBarArea, createToolBar());

    d->chart = new QChart();
    d->chartView = new ChartView(d->chart);
    //d->chartView->setRubberBand(QChartView::RectangleRubberBand);
    d->chartView->setMinimumSize(1000, 400);
    d->series = new QLineSeries;
    d->series->setName(tr("Channel A"));
    d->series->setPen(QPen(QBrush(Qt::red), 2));
    d->series->setUseOpenGL(true);
    d->chart->addSeries(d->series);


    d->chart2 = new QChart();
    d->chartView2 = new ChartView(d->chart2);
    //d->chartView2->setRubberBand(QChartView::RectangleRubberBand);
    d->chartView2->setMinimumSize(1000, 400);
    d->ch2Series = new QLineSeries;
    d->ch2Series->setName(tr("Channel B"));
    d->ch2Series->setPen(QPen(QBrush(Qt::blue), 2));
    d->ch2Series->setUseOpenGL(true);
    d->chart2->addSeries(d->ch2Series);

    QValueAxis* axisX = new QValueAxis(this);
    axisX->setRange(0, settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Time, us");
    QValueAxis* axisY = new QValueAxis(this);
    axisY->setRange(0, settingsProvider->value(settings::adc::vRef).toReal());
    axisY->setTitleText("Voltage, V");

    QValueAxis* axisX2 = new QValueAxis(this);
    axisX2->setRange(0, settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    axisX2->setLabelFormat("%g");
    axisX2->setTitleText("Time, us");
    QValueAxis* axisY2 = new QValueAxis(this);
    axisY2->setRange(0, settingsProvider->value(settings::adc::vRef).toReal());
    axisY2->setTitleText("Voltage, V");

    d->chart->setAxisX(axisX, d->series);
    d->chart->setAxisY(axisY, d->series);

    d->chart2->setAxisX(axisX2, d->ch2Series);
    d->chart2->setAxisY(axisY2, d->ch2Series);

    //d->chart->legend()->hide();
    d->chart->setTitle("Data from ADC1");
    d->chart2->setTitle("Data from ADC2");

    d->chartView->setRenderHint(QPainter::Antialiasing);
    d->chartView->setRenderHint(QPainter::Antialiasing);
    QVBoxLayout* mainLayout = new QVBoxLayout(ui->centralwidget);

    mainLayout->addWidget(d->chartView);
    mainLayout->addWidget(d->chartView2);

    d->transferService = serviceRegistry->transferService();
    d->device = new XYSeriesIODevice(d->series, d->ch2Series, this);
}

ChartWindow::~ChartWindow()
{
    delete d->series;
    delete d->chart;

    delete d->ch2Series;
    delete d->chart2;

    delete ui;
    qDebug()<<"chart window destroyed";
}

void ChartWindow::clearChart()
{
    d->series->clear();
    d->ch2Series->clear();
    d->series->pointsVector().clear();
    d->ch2Series->pointsVector().clear();
    //d->chart->removeAllSeries();
}

void ChartWindow::startReading()
{
    clearChart();
    d->device->open(QIODevice::WriteOnly);
    d->transferService->getAdcData(d->device);
}

void ChartWindow::drag(bool checked)
{
    if (checked)
        this->installEventFilter(d->chartView);
    else
        this->removeEventFilter(d->chartView);

//    connect(d->chartView, &ChartView::mousePosChanged, this, [=](QPoint pos)
//    {
//        d->chart->mapToValue(pos);
//    });
    //d->chart->scroll(20.0, 0);
}

void ChartWindow::resetZoom()
{
    d->chart->zoomReset();
}

void ChartWindow::zoomOut()
{
    d->chart->zoomOut();
}

void ChartWindow::zoomIn()
{
    //d->chart->zoomIn(QRectF());
    d->chart->zoomIn();
}

void ChartWindow::closeEvent(QCloseEvent* event)
{
    //d->transferService->cancelListen();
    event->accept();
}

QToolBar*ChartWindow::createToolBar()
{
    QToolBar* ptb = new QToolBar("Chart ToolBar");
    ptb->addAction(QIcon(":/icons/hand_drag.png"), "drag", this, SLOT(drag(bool)))->setCheckable(true);
    ptb->addSeparator();
    ptb->addAction(QIcon(":/icons/reset_zoom.png"), "reset zoom", this, SLOT(resetZoom()));
    ptb->addAction(QIcon(":/icons/zoom_out.png"), "zoom out", this, SLOT(zoomOut()));
    ptb->addAction(QIcon(":/icons/zoom_in.png"), "zoom in", this, SLOT(zoomIn()));
    return ptb;
}

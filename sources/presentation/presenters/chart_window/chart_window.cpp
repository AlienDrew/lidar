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
#include <QFileDialog>
#include <QDebug>

using namespace presentation;
QT_CHARTS_USE_NAMESPACE

class ChartWindow::Impl
{
public:
    QChart* chart;
    ChartView* chartView;
    QLineSeries* series = nullptr;

    QChart* chart2;
    ChartView* chartView2;
    QLineSeries* ch2Series = nullptr;

    QValueAxis* axisX;
    QValueAxis* axisY;
    QValueAxis* axisX2;
    QValueAxis* axisY2;

    XYSeriesIODevice* device = nullptr;
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

    d->chart2 = new QChart();
    d->chartView2 = new ChartView(d->chart2);
    //d->chartView2->setRubberBand(QChartView::RectangleRubberBand);
    d->chartView2->setMinimumSize(1000, 400);

    d->axisX = new QValueAxis(this);
    d->axisX->setRange(0, settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    d->axisX->setLabelFormat("%g");
    d->axisX->setTitleText("Time, us");
    d->axisY = new QValueAxis(this);
    d->axisY->setRange(0, settingsProvider->value(settings::adc::vRef).toReal());
    d->axisY->setTitleText("Voltage, V");

    d->axisX2 = new QValueAxis(this);
    d->axisX2->setRange(0, settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    d->axisX2->setLabelFormat("%g");
    d->axisX2->setTitleText("Time, us");
    d->axisY2 = new QValueAxis(this);
    d->axisY2->setRange(0, settingsProvider->value(settings::adc::vRef).toReal());
    d->axisY2->setTitleText("Voltage, V");

//    d->chart->setAxisX(axisX, d->series);
//    d->chart->setAxisY(axisY, d->series);

//    d->chart2->setAxisX(axisX2, d->ch2Series);
//    d->chart2->setAxisY(axisY2, d->ch2Series);

    //d->chart->legend()->hide();
    d->chart->setTitle("Data from ADC1");
    d->chart2->setTitle("Data from ADC2");

    d->chartView->setRenderHint(QPainter::Antialiasing);
    d->chartView->setRenderHint(QPainter::Antialiasing);
    QVBoxLayout* mainLayout = new QVBoxLayout(ui->centralwidget);

    mainLayout->addWidget(d->chartView);
    mainLayout->addWidget(d->chartView2);

    d->transferService = serviceRegistry->transferService();
    //d->device = new XYSeriesIODevice(d->series, d->ch2Series, this);

    QPushButton* saveDataButton = new QPushButton("Save data", this);
    connect(saveDataButton, &QPushButton::clicked, this, &ChartWindow::saveData);
    mainLayout->addWidget(saveDataButton);
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

void ChartWindow::initCharts()
{
    if (d->device)
    {
        d->device->close();
        //delete d->device;
    }
    resetZoom();
    auto slist = d->chart->series();
    if (!slist.isEmpty())
    {
        QLineSeries* old_series = dynamic_cast<QLineSeries*>(slist.at(0));
        old_series->detachAxis(d->axisX);
        old_series->detachAxis(d->axisY);
        d->chart->removeAxis(d->axisX);
        d->chart->removeAxis(d->axisY);
        d->chart->removeSeries(old_series);
    }
    slist = d->chart2->series();
    if (!slist.isEmpty())
    {
        QLineSeries* old_series = dynamic_cast<QLineSeries*>(slist.at(0));
        old_series->detachAxis(d->axisX2);
        old_series->detachAxis(d->axisY2);
        d->chart2->removeAxis(d->axisX2);
        d->chart2->removeAxis(d->axisY2);
        d->chart2->removeSeries(old_series);
    }

    d->series = new QLineSeries;
    d->series->setName(tr("Channel A"));
    d->series->setPen(QPen(QBrush(Qt::red), 2));
    d->series->setUseOpenGL(true);
    d->chart->addSeries(d->series);

    d->ch2Series = new QLineSeries;
    d->ch2Series->setName(tr("Channel B"));
    d->ch2Series->setPen(QPen(QBrush(Qt::blue), 2));
    d->ch2Series->setUseOpenGL(true);
    d->chart2->addSeries(d->ch2Series);

    d->chart->setAxisX(d->axisX, d->series);
    d->chart->setAxisY(d->axisY, d->series);

    d->chart2->setAxisX(d->axisX2, d->ch2Series);
    d->chart2->setAxisY(d->axisY2, d->ch2Series);
    d->device = new XYSeriesIODevice(d->series, d->ch2Series, this);
}

void ChartWindow::startReading()
{
    initCharts();
    d->device->open(QIODevice::WriteOnly);
    d->transferService->setADCXYIODevice(d->device);
}

void ChartWindow::saveData()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save ADC data"), "",
                                                    tr("ADC data (*.txt);;All Files (*)"));
    if (fileName.isEmpty())
        return;
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_10);
        QVector<QPointF> seriesCh1 = d->series->pointsVector();
        QVector<QPointF> seriesCh2 = d->ch2Series->pointsVector();
        out<<"ch 1"<<"\n";
        for (int i = 0; i<seriesCh1.size(); ++i)
            out<<QString::number(seriesCh1[i].x())<<" "<<QString::number(seriesCh1[i].y())<<"\n";
        out<<"\n"<<"ch 2"<<"\n";
        for (int i = 0; i<seriesCh2.size(); ++i)
            out<<QString::number(seriesCh2[i].x())<<" "<<QString::number(seriesCh2[i].y())<<"\n";
        QMessageBox::information(this, qApp->applicationName(), tr("Data written to file!"));
    }
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
    d->chart2->zoomReset();
}

void ChartWindow::zoomOut()
{
    d->chart->zoomOut();
    d->chart2->zoomOut();
}

void ChartWindow::zoomIn()
{
    //d->chart->zoomIn(QRectF());
    d->chart->zoomIn();
    d->chart2->zoomIn();
}

void ChartWindow::closeEvent(QCloseEvent* event)
{
    resetZoom();
    d->device->close();
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

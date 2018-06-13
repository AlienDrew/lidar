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

namespace
{
    bool compareMaxX(const QPointF& a, const QPointF& b)
    {
        return a.x()<b.x();
    }
    bool compareMinX(const QPointF& a, const QPointF& b)
    {
        return a.x()>b.x();
    }

    bool compareMaxY(const QPointF& a, const QPointF& b)
    {
        return a.y()<b.y();
    }
    bool compareMinY(const QPointF& a, const QPointF& b)
    {
        return a.y()>b.y();
    }
}

ChartWindow::ChartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChartWindow), d(new Impl)
{
    ui->setupUi(this);
    addToolBar(Qt::TopToolBarArea, createToolBar());

    d->chart = new QChart();
    d->chart2 = new QChart();
    d->chartView = new ChartView(d->chart, d->chart2);
    d->chartView->setMinimumSize(800, 250);

    d->chartView2 = new ChartView(d->chart2, d->chart);
    d->chartView2->setMinimumSize(800, 250);

    d->axisX = new QValueAxis(this);
    d->axisX->setRange(0, settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    d->axisX->setMax(settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    d->axisX->setLabelFormat("%g");
    d->axisX->setTitleText("Time, us");
    d->axisY = new QValueAxis(this);
    d->axisY->setRange(0, settingsProvider->value(settings::adc::vRef).toReal());
    d->axisY->setMax(settingsProvider->value(settings::adc::vRef).toReal());
    d->axisY->setTitleText("Voltage, V");

    d->axisX2 = new QValueAxis(this);
    d->axisX2->setRange(0, settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    d->axisX2->setMax(settingsProvider->value(settings::adc::maxNumberOfSamples).toInt()/settingsProvider->value(settings::adc::samplingFreq).toReal()*1000000);
    d->axisX2->setLabelFormat("%g");
    d->axisX2->setTitleText("Time, us");
    d->axisY2 = new QValueAxis(this);
    d->axisY2->setRange(0, settingsProvider->value(settings::adc::vRef).toReal());
    d->axisY2->setMax(settingsProvider->value(settings::adc::vRef).toReal());
    d->axisY2->setTitleText("Voltage, V");

    //d->chart->legend()->hide();
    d->chart->setTitle("Data from ADC1");
    d->chart2->setTitle("Data from ADC2");

    QVBoxLayout* mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->addWidget(d->chartView);
    mainLayout->addWidget(d->chartView2);

    d->transferService = serviceRegistry->transferService();
    //d->device = new XYSeriesIODevice(d->series, d->ch2Series, this);
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
    d->series->setName(tr("Reference signal"));
    d->series->setColor(Qt::red);
    d->series->setPen(QPen(Qt::red, 2));
    d->series->setPointsVisible();
    d->chart->addSeries(d->series);

    d->ch2Series = new QLineSeries;
    d->ch2Series->setName(tr("Measured signal"));
    d->ch2Series->setColor(Qt::darkBlue);
    d->ch2Series->setPen(QPen(Qt::darkBlue, 2));
    d->ch2Series->setPointsVisible();
    d->chart2->addSeries(d->ch2Series);

    d->chart->setAxisX(d->axisX, d->series);
    d->chart->setAxisY(d->axisY, d->series);

    d->chart2->setAxisX(d->axisX2, d->ch2Series);
    d->chart2->setAxisY(d->axisY2, d->ch2Series);

    connect(d->series, &QLineSeries::pointsReplaced, this, [this]()
    {
        if (!d->series->pointsVector().isEmpty())
        {
            QPointF minPointY = (*std::max_element(d->series->pointsVector().constBegin(), d->series->pointsVector().constEnd(), compareMinY));
            QPointF maxPointY = (*std::max_element(d->series->pointsVector().constBegin(), d->series->pointsVector().constEnd(), compareMaxY));
            d->chart->axisY()->setRange(minPointY.y()-minPointY.y()/8, maxPointY.y()+minPointY.y()/8);
            d->chart->axisX()->setRange(50, 160);
        }
    });
    connect(d->ch2Series, &QLineSeries::pointsReplaced, this, [this]()
    {
        if (!d->ch2Series->pointsVector().isEmpty())
        {
            QPointF minPointY = (*std::max_element(d->ch2Series->pointsVector().constBegin(), d->ch2Series->pointsVector().constEnd(), compareMinY));
            QPointF maxPointY = (*std::max_element(d->ch2Series->pointsVector().constBegin(), d->ch2Series->pointsVector().constEnd(), compareMaxY));
            d->chart2->axisY()->setRange(minPointY.y()-minPointY.y()/8, maxPointY.y()+minPointY.y()/8);
            d->chart2->axisX()->setRange(50, 160);
        }
    });
    resetZoom();

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
    {
        d->chartView->setCursor(QCursor(Qt::OpenHandCursor));
        d->chartView2->setCursor(QCursor(Qt::OpenHandCursor));
    }
    else
    {
        d->chartView->setCursor(QCursor(Qt::ArrowCursor));
        d->chartView2->setCursor(QCursor(Qt::ArrowCursor));
    }
    d->chartView->setDragable(checked);
    d->chartView2->setDragable(checked);
}

void ChartWindow::resetZoom()
{
    d->chart->zoomReset();
    d->chart2->zoomReset();

    //reset scale
    emit d->series->pointsReplaced();
    emit d->ch2Series->pointsReplaced();
}

void ChartWindow::zoomOut()
{
    d->chart->zoomOut();
    d->chart2->zoomOut();
}

void ChartWindow::zoomIn()
{
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
    ptb->addSeparator();
    ptb->addAction(QIcon(":/icons/save.png"), "save data", this, SLOT(saveData()));
    return ptb;
}

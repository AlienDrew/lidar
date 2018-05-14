#ifndef XYSERIES_IODEVICE_H
#define XYSERIES_IODEVICE_H

#include <QtCore/QIODevice>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QXYSeries;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace presentation
{
    class XYSeriesIODevice : public QIODevice
    {
        Q_OBJECT
    public:
        explicit XYSeriesIODevice(QXYSeries * series, QXYSeries* series2, QObject *parent = 0);

    protected:
        qint64 readData(char * data, qint64 maxSize) override;
        qint64 writeData(const char * data, qint64 maxSize) override;

    private:
        QXYSeries* m_series;
        QXYSeries* m_series2;
    };

}

#endif // XYSERIES_IODEVICE_H

#include "chart_view.h"

using namespace presentation;

class ChartView::Impl
{
public:
    QPoint zeroMousePos;
    bool dragable = false;

    bool ctrlPressed = false;
    QChart* nextChart;
};

ChartView::ChartView(QWidget* parent) : QChartView(parent),
    d(new Impl)
{

}

ChartView::ChartView(QChart* chart, QChart* nextChart, QWidget* parent) : QChartView(chart, parent),
    d(new Impl)
{
    d->nextChart = nextChart;
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing);
    this->setMouseTracking(true);
}

ChartView::~ChartView()
{

}

void ChartView::setDragable(bool value)
{
    d->dragable = value;
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (d->dragable)
    {
        setRubberBand(QChartView::NoRubberBand);
        d->zeroMousePos = QPoint(event->x(), event->y());
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }
    else
    {
        setRubberBand(QChartView::RectangleRubberBand);
    }
    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton && d->dragable)
    {
        chart()->scroll(d->zeroMousePos.x()-event->x(), event->y()-d->zeroMousePos.y());
        if (!d->ctrlPressed)
            d->nextChart->scroll(d->zeroMousePos.x()-event->x(), event->y()-d->zeroMousePos.y());
        d->zeroMousePos = event->pos();
    }
    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (d->dragable)
    {
        this->setCursor(QCursor(Qt::OpenHandCursor));
    }
    QChartView::mouseReleaseEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        if (!d->ctrlPressed)
            d->nextChart->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        if (!d->ctrlPressed)
            d->nextChart->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        if (!d->ctrlPressed)
            d->nextChart->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        if (!d->ctrlPressed)
            d->nextChart->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        if (!d->ctrlPressed)
            d->nextChart->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        if (!d->ctrlPressed)
            d->nextChart->scroll(0, -10);
        break;
    case Qt::Key_Control:
        d->ctrlPressed = true;
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void ChartView::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Control:
        d->ctrlPressed = false;
        break;
    default:
        break;
    }
}

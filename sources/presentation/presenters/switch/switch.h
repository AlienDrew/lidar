#ifndef SWITCH_H
#define SWITCH_H

#include <QtWidgets>

namespace presentation
{
    class Switch : public QAbstractButton {
        Q_OBJECT
        Q_PROPERTY(int offset READ offset WRITE setOffset)
        Q_PROPERTY(QBrush brush READ brush WRITE setBrush)

    public:
        Switch(QWidget* parent = nullptr);
        Switch(const QBrush& brush, QWidget* parent = nullptr);

        QSize sizeHint() const override;

        QBrush brush() const
        {
            return m_brush;
        }
        void setBrush(const QBrush &brsh)
        {
            m_brush = brsh;
        }

        int offset() const
        {
            return m_x;
        }
        void setOffset(int o)
        {
            m_x = o;
            update();
        }
        void setToggle(bool value);

    protected:
        void paintEvent(QPaintEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void enterEvent(QEvent*) override;

    private:
        bool m_switch;
        qreal m_opacity;
        int m_x, m_y, m_height, m_margin;
        QBrush m_thumb, m_track, m_brush;
        QPropertyAnimation *m_anim = nullptr;
    };
}
#endif // SWITCH_H

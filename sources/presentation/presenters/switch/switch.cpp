#include "switch.h"

using namespace presentation;

Switch::Switch(QWidget *parent) : QAbstractButton(parent),
    m_switch(false),
    m_opacity(0.000),
    m_height(16),
    m_margin(3),
    m_thumb("#d5d5d5"),
    m_anim(new QPropertyAnimation(this, "offset", this))
{
    setOffset(m_height / 2);
    m_y = m_height / 2;
    setBrush(QColor("#009688"));
}

Switch::Switch(const QBrush &brush, QWidget *parent) : QAbstractButton(parent),
    m_switch(false),
    m_opacity(0.000),
    m_height(16),
    m_margin(3),
    m_thumb("#d5d5d5"),
    m_anim(new QPropertyAnimation(this, "offset", this))
{
    setOffset(m_height / 2);
    m_y = m_height / 2;
    setBrush(brush);
}

void Switch::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    if (isEnabled()) {
        p.setBrush(m_switch ? brush() : Qt::black);
        p.setOpacity(m_switch ? 0.5 : 0.38);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(QRect(m_margin, m_margin, width() - 2 * m_margin, height() - 2 * m_margin), 8.0, 8.0);
        p.setBrush(m_thumb);
        p.setOpacity(1.0);
        p.drawEllipse(QRectF(offset() - (m_height / 2), m_y - (m_height / 2), height(), height()));
    }
    else
    {
        p.setBrush(Qt::black);
        p.setOpacity(0.12);
        p.drawRoundedRect(QRect(m_margin, m_margin, width() - 2 * m_margin, height() - 2 * m_margin), 8.0, 8.0);
        p.setOpacity(1.0);
        p.setBrush(QColor("#BDBDBD"));
        p.drawEllipse(QRectF(offset() - (m_height / 2), m_y - (m_height / 2), height(), height()));
    }
}

void Switch::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        m_switch = m_switch ? false : true;
        m_thumb = m_switch ? m_brush : QBrush("#d5d5d5");
        if (m_switch)
        {
            m_anim->setStartValue(m_height / 2);
            m_anim->setEndValue(width() - m_height);
            m_anim->setDuration(120);
            m_anim->start();
        }
        else
        {
            m_anim->setStartValue(offset());
            m_anim->setEndValue(m_height / 2);
            m_anim->setDuration(120);
            m_anim->start();
        }
    }
    emit toggled(m_switch);
    QAbstractButton::mouseReleaseEvent(e);
}

void Switch::enterEvent(QEvent *e)
{
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::enterEvent(e);
}

QSize Switch::sizeHint() const
{
    return QSize(2 * (m_height + m_margin), m_height + 2 * m_margin);
}

void Switch::setToggle(bool value)
{
    m_switch = value;
    m_thumb = m_switch ? m_brush : QBrush("#d5d5d5");
    if (m_switch)
    {
        m_anim->setStartValue(m_height / 2);
        m_anim->setEndValue(width() - m_height);
        m_anim->setDuration(120);
        m_anim->start();
    }
    else
    {
        m_anim->setStartValue(offset());
        m_anim->setEndValue(m_height / 2);
        m_anim->setDuration(120);
        m_anim->start();
    }
}

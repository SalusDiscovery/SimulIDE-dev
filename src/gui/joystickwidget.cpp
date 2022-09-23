/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QVariant>
#include <QMouseEvent>
#include <QPainter>

#include "joystickwidget.h"

#define STICK_PERCENTAGE_SIZE 0.6

JoystickWidget::JoystickWidget()
{
}
JoystickWidget::~JoystickWidget() {}

void JoystickWidget::setupWidget()
{
    setObjectName(QString::fromUtf8("joystick"));
    
    m_xValue = 500;
    m_yValue = 500;
    
    m_grabCenter = false;
    
    setAttribute(Qt::WA_NoSystemBackground);
    
    m_movingOffset = center();
    updateOutputValues();
}

void JoystickWidget::updateOutputValues() {
    m_xValue = m_movingOffset.x() * 1000 / width();
    m_yValue = m_movingOffset.y() * 1000 / height();
    
    emit valueChanged(m_xValue, m_yValue);
}

void JoystickWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_grabCenter = centerEllipse().contains(event->pos());
    else QWidget::mousePressEvent(event);
}
 
void JoystickWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_grabCenter)
    {
        QLineF limitLine = QLineF(center(), event->pos());
        int maxDistance = width() / 2;
        if (limitLine.length() > maxDistance) limitLine.setLength(maxDistance);

        m_movingOffset = limitLine.p2();
        update();
        updateOutputValues();
}   }

void JoystickWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_grabCenter = false;
    m_movingOffset = center();
    update();
    updateOutputValues();
}

void JoystickWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int size = width();
    QRectF bounds = QRectF(-size/2, -size/2, size, size).translated(center());
    painter.setBrush(Qt::darkGray);
    painter.drawEllipse(bounds);
    painter.setBrush(Qt::black);
    painter.drawEllipse(centerEllipse());
}

QRectF JoystickWidget::centerEllipse()
{
    int radius = width() * STICK_PERCENTAGE_SIZE / 2;
    QRectF rect = QRectF(-radius, -radius, 2*radius, 2*radius);

    if (m_grabCenter) return rect.translated(m_movingOffset);
    return rect.translated(center());
}

QPointF JoystickWidget::center()
{
    return QPointF(width()/2, height()/2);
}

#include "moc_joystickwidget.cpp"

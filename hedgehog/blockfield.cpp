#include "blockfield.h"
#include "connectnodewidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

BlockField::BlockField(QWidget* parent)
    : QScrollArea(parent)
{

}

void BlockField::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() ==  Qt::LeftButton)
    {
        QPainter pd(this);
        if(select)
        {
            qDebug() << "set pos";
            pos = {event->x(), event->y()};
        }
    }
}

void BlockField::mousePressEvent(QMouseEvent *event)
{
    select = nullptr;
}


void BlockField::paintEvent(QPaintEvent *event)
{
    if (select)
    {
        qDebug() << "ready to draw";
        QPainter p(this);
        p.setBackground(QBrush(Qt::white));
        p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
        p.drawLine(select->pos(), pos);
    }
    QScrollArea::paintEvent(event);
}

void BlockField::on_start(ConnectNodeWidget* start)
{
    qDebug() << "set select";
    select = start;
}

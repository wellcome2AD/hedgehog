#include "blockfield.h"
#include "connectnodewidget.h"

#include <QMouseEvent>
#include <QDebug>

BlockField::BlockField(QWidget* parent)
    : QScrollArea(parent)
{
    setMouseTracking(true);
}

void BlockField::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() ==  Qt::LeftButton)
    {
        //qDebug() << "here";
        if(select)
        {
            // qDebug() << "set pos";
            pos = {event->x(), event->y()};
            repaint();
        }
    }
}

void BlockField::mousePressEvent(QMouseEvent *event)
{
    /*qDebug() << "unset select";
    select = nullptr;*/
}

void BlockField::paintEvent(QPaintEvent *event)
{
    qDebug() << "paint";
    QPainter p(this);
    p.setBackground(QBrush(Qt::white));
    p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    p.drawLine(select->pos(), pos);
}

void BlockField::on_start(ConnectNodeWidget* start)
{
    qDebug() << "set select";
    select = start;
}

#include "blockfield.h"
#include "connectnodewidget.h"

#include <QMouseEvent>
#include <QDebug>

BlockField::BlockField(QWidget* parent)
    : QWidget(parent)
{
    qDebug() << "BlockField::BlockField";
    setMouseTracking(true);
}

void BlockField::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "here";
    if(!select)
        return;
    // qDebug() << "set pos";
    pos = {event->x(), event->y()};
    repaint();

    qDebug() << this->mapFromGlobal(select->mapToGlobal(select->pos()));
    qDebug() << select->mapToParent(select->pos());
}

void BlockField::mousePressEvent(QMouseEvent *event)
{
    select->makeTransparent(true);
    select = nullptr;
    repaint();
}

void BlockField::paintEvent(QPaintEvent *event)
{
    if (!select)
        return;
    QPainter p(this);
    p.setBackground(QBrush(Qt::white));
    p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    select->makeTransparent(false);
    auto parent = select->parentWidget();
    p.drawLine(parent->mapToParent(select->pos()) + QPoint(5, 5), pos);
}

void BlockField::on_start(ConnectNodeWidget* start)
{
    select = start;
    select->makeTransparent(false);
    auto parent = select->parentWidget();
    qDebug() << parent->mapToParent(select->pos()) + QPoint(5, 5);
    pos = parent->mapToParent(select->pos()) + QPoint(5, 5);
    repaint();
}

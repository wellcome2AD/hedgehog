#include "blockfield.h"
#include "connectnodewidget.h"


#include <QPainter>
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
    if(!selected_node)
        return;
    pos = {event->x(), event->y()};
    repaint();
}

static bool isPointOnLine(QLine line, QPoint point)
{
    float x1 = line.p1().x(), y1 = line.p1().y();
    float x2 = line.p2().x(), y2 = line.p2().y();
    float k = (y2 - y1) / (x2 - x1);
    float b = y1 - k * x1;
    float eps = 6;
    return (abs(point.y() - (k * point.x() + b)) < eps);
}

void BlockField::mousePressEvent(QMouseEvent *event)
{
    if(selected_node)
    {
        selected_node->makeTransparent(true);
        selected_node = nullptr;
        repaint();
    }

    QPainter p(this);
    for(auto it = _connection_map.begin(); it != _connection_map.end(); ++it)
    {
        auto start = it.key();
        auto end = it.value();
        auto start_pos = coordToBlockField(start);
        auto end_pos = coordToBlockField(end);
        bool find_line = isPointOnLine(QLine(start_pos, end_pos), event->pos());
        if(find_line)
        {
            _map_of_selected_nodes[start] = end;
            repaint();
            return;
        }
    }
    _map_of_selected_nodes.clear();
    repaint();
}

void BlockField::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.eraseRect(rect());
    p.setBackground(QBrush(Qt::white));
    QVector<QLineF> unselected_lines, selected_lines;
    for(auto it = _connection_map.begin(); it != _connection_map.end(); ++it)
    {
        auto start = it.key();
        auto end = it.value();
        auto start_pos = coordToBlockField(start);
        auto end_pos = coordToBlockField(end);
        start->makeTransparent(false);
        end->makeTransparent(false);
        if(_map_of_selected_nodes.value(start, nullptr) == end)
        {
            selected_lines.append({start_pos, end_pos});
        }
        else
        {
            unselected_lines.append({start_pos, end_pos});
        }
    }

    p.setPen(QPen(Qt::green, 1, Qt::SolidLine));
    p.drawLines(selected_lines.data(), selected_lines.size());

    p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    p.drawLines(unselected_lines.data(), unselected_lines.size());

    if (!selected_node)
        return;

    selected_node->makeTransparent(false);
    p.drawLine(coordToBlockField(selected_node), pos);
}

QPoint BlockField::coordToBlockField(ConnectNodeWidget* n)
{
    if(!n)
    {
        qDebug() << "node is nullptr";
        return QPoint();
    }
    auto parent = n->parentWidget();
    return parent->mapToParent(n->pos()) + QPoint(5, 5);
}

void BlockField::on_start(ConnectNodeWidget* start)
{
    if(selected_node)
    {
        if(selected_node != start && selected_node->parent() != start->parent())
        {
            _connection_map[selected_node] = start;
            selected_node->makeTransparent(false);
            start->makeTransparent(false);
        }        
        selected_node = nullptr;
        repaint();
    }
    else
    {
        if (_connection_map.value(selected_node, nullptr))
        {
            _connection_map.remove(selected_node);
            repaint();
        }
        selected_node = start;
        selected_node->makeTransparent(false);
        pos = coordToBlockField(selected_node);
        repaint();
    }
}

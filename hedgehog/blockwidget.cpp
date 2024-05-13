#include "blockwidget.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QPainter>
#include <QDebug>

BlockWidget::BlockWidget(QWidget *parent)
    : QWidget(parent),
      block_name(new QLabel("block name", this)),
      label(new QLabel(this)),
      resume_pause_button(new QPushButton("||", this))
{
    auto spacing = 2;

    QPixmap pm(11, 11);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    auto radius = 5;
    painter.setBrush(QBrush(Qt::green));
    auto point = QPoint(radius, radius);
    painter.drawEllipse(point, radius, radius);
    label->setPixmap(pm);
    block_name->move(0, resume_pause_button->y() + resume_pause_button->height() + spacing);
    label->setGeometry(block_name->width() - 15, block_name->y() + radius, 11, 11);
    label->setHidden(true);

    auto height = block_name->height() + resume_pause_button->height() + spacing;
    auto width = std::max(block_name->width() + label->width() + spacing, resume_pause_button->width());
    qDebug() << width << height;
    setGeometry(10, 10, width, height);
    block_name->setWordWrap(true);
    block_name->setAlignment(Qt::AlignCenter);
    block_name->setFrameStyle(QFrame::Box | QFrame::Plain);
    auto new_font = block_name->font();
    new_font.setPointSize(12);
    block_name->setFont(new_font);

    setMouseTracking(true);

    resume_pause_button->move((block_name->width() - resume_pause_button->width()) / 2, 0);
}

void BlockWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        oldPos = event->pos();
    }
}

void BlockWidget::mouseMoveEvent(QMouseEvent *event)
{
    label->setVisible(true);
    if (event->buttons() ==  Qt::LeftButton)
    {
        QPoint delta = event->pos() - oldPos;
        move(pos() + delta);
    }
}

void BlockWidget::leaveEvent(QEvent *event)
{
    label->setHidden(true);
}

void BlockWidget::paintEvent(QPaintEvent* event)
{

}

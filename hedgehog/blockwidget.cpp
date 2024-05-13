#include "blockwidget.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QPainter>
#include <QDebug>

BlockWidget::BlockWidget(QWidget *parent)
    : QWidget(parent),
      block_name(new QLabel("block name", this)),
      left_circle(new QLabel(this)),
      right_circle(new QLabel(this)),
      resume_pause_button(new QPushButton("||", this))
{
    block_name->setWordWrap(true);
    block_name->setAlignment(Qt::AlignCenter);
    block_name->setFrameStyle(QFrame::Box | QFrame::Plain);
    auto new_font = block_name->font();
    new_font.setPointSize(12);
    block_name->setFont(new_font);
    resume_pause_button->setFont(new_font);

    block_name->show();
    resume_pause_button->show();

    auto label_width = block_name->width();
    auto label_height = block_name->height();
    auto spacing = 2, spacing2 = 3;
    auto button_width = resume_pause_button->width();
    auto button_height = resume_pause_button->height();
    auto radius = 5, diameter = radius * 2;

    auto width = 2 * (2 * spacing + diameter) + label_width;
    auto height = 2 * spacing + button_height + spacing2 + label_height;
    setGeometry(0,0, width, height);

    resume_pause_button->move((width - button_width) / 2, spacing);
    block_name->move(2 * spacing + diameter, spacing + button_height + spacing2);

    QPixmap pm(diameter + 1, diameter + 1);
    pm.fill(Qt::transparent);
    left_circle->setPixmap(pm);
    right_circle->setPixmap(pm);

    left_circle->move(spacing, spacing + button_height + spacing2 + label_height / 2 - diameter / 2);
    right_circle->move(width - spacing - diameter, spacing + button_height + spacing2 + label_height / 2 - diameter / 2);

    setMouseTracking(true);

    connect(resume_pause_button, &QPushButton::clicked, this, &BlockWidget::on_pushButton_clicked);
}

void BlockWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        oldPos = event->pos();
    }
}

void BlockWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPixmap pm(*left_circle->pixmap());
    QPainter painter(&pm);
    painter.setBrush(QBrush(Qt::green));
    auto radius = 5;
    auto point = QPoint(radius, radius);
    painter.drawEllipse(point, radius, radius);
    left_circle->setPixmap(pm);
    right_circle->setPixmap(pm);
    if (event->buttons() ==  Qt::LeftButton)
    {
        QPoint delta = event->pos() - oldPos;
        move(pos() + delta);
    }
}

void BlockWidget::leaveEvent(QEvent *event)
{
    QPixmap pm(*left_circle->pixmap());
    pm.fill(Qt::transparent);
    left_circle->setPixmap(pm);
    right_circle->setPixmap(pm);
}

void BlockWidget::paintEvent(QPaintEvent* event)
{

}

void BlockWidget::on_pushButton_clicked()
{
    auto text = resume_pause_button->text();
    if(text == "||")
        resume_pause_button->setText("▶");
    else
        resume_pause_button->setText("||");
}

#ifndef BLOCKFIELD_H
#define BLOCKFIELD_H

#include <QWidget>
#include <QPainter>

class ConnectNodeWidget;

class BlockField : public QWidget
{
    Q_OBJECT
public:
    BlockField(QWidget* parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event)  override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void on_start(ConnectNodeWidget* start);

private:
    QPoint pos;
    ConnectNodeWidget* select = nullptr;
};

#endif // BLOCKFIELD_H

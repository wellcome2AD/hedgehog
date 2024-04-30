#ifndef QTAGMODEL_H
#define QTAGMODEL_H

#include <QStandardItemModel>
#include <QTableWidget>
#include <QPair>

class QTagModel : public QStandardItemModel
{
    Q_OBJECT

public:
    QTagModel(int rows, int columns, QTableWidget *atr_widg, QObject *parent = nullptr);

private slots:
    void on_clicked(QStandardItemModel *item, int column);

private:
    QTableWidget *_atr_widg;
    QList<QPair<QString, QString>> _atributes;
};

#endif // QTAGMODEL_H

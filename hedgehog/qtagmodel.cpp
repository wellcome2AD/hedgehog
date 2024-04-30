#include "qtagmodel.h"

QTagModel::QTagModel(int rows, int columns, QTableWidget *atr_widg, QObject *parent)
    : QStandardItemModel(rows, columns, parent),
      _atr_widg(atr_widg)
{
}

void QTagModel::on_clicked(QStandardItemModel *item, int column)
{
    foreach(auto &&atr, _atributes)
    {
        int row_num  = _atr_widg->rowCount();
        _atr_widg->insertRow(row_num);
        _atr_widg->setItem(row_num, 0, new QTableWidgetItem(atr.first));
        _atr_widg->setItem(row_num, 1, new QTableWidgetItem(atr.second));
    }
}

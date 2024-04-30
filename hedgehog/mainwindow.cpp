#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QXmlStreamReader>
#include <QStack>
#include <QDebug>
#include <QVariant>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    bool can_open_file = false;
    QString fileName;
    QFile file;
    while(!can_open_file) {
        fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "C:/", tr("XML files (*.xml)"));
        file.setFileName(fileName);
        if(!file.open(QIODevice::ReadOnly))
            qDebug() << "Error while opening file. Try another";
        can_open_file = true;
    }

    QStandardItemModel *model = new QStandardItemModel(0, 0, ui->treeView);
    QStandardItem *parentItem = model->invisibleRootItem();
    ui->treeView->setModel(model);

    QXmlStreamReader xmlReader(&file);
    QStack<QStandardItem *> tags;
    tags.push(parentItem);
    while(!xmlReader.atEnd()){
        xmlReader.readNext();
        switch (xmlReader.tokenType()){
        case QXmlStreamReader::StartElement: { // открывающий тэг
            auto new_tag = new QStandardItem(xmlReader.name().toString());
            auto parent_tag = tags.top();
            parent_tag->appendRow(new_tag);
            auto table_model = new QStandardItemModel();
            for(const QXmlStreamAttribute &attr: xmlReader.attributes()) {
                auto attr_name_item = new QStandardItem(attr.name().toString());
                auto attr_value_item = new QStandardItem(attr.value().toString());
                int row_count = table_model->rowCount();
                table_model->insertRow(row_count, QList<QStandardItem *>({attr_name_item, attr_value_item}));
            }
            table_model->setHorizontalHeaderLabels(QStringList({"Attribute", "Value"}));
            QVariant table_model_variant;
            table_model_variant.setValue(table_model);
            new_tag->setData(table_model_variant);
            new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
            tags.push(new_tag);
            break;
        }
        case QXmlStreamReader::Characters: { // текст внутри тэга
            QString str = xmlReader.text().toString().trimmed();
            if (!str.isEmpty()) {
                auto tag = tags.top();
                tag->setText(tag->text() + ": " + str);
            }
            break;
        }
        case QXmlStreamReader::EndElement: {
            tags.pop();
            break;
        }
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            break;
        default: {
            qDebug() << "error: unexpected token type " << xmlReader.tokenType();
            break;
        }
        }
    }
    ui->treeView->resizeColumnToContents(0);
    file.close(); // Закрываем файл
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->tableView->setModel(index.data(Qt::UserRole + 1).value<QStandardItemModel*>());
}

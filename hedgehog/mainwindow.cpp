#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QXmlStreamReader>
#include <QStack>
#include <QDebug>

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
    qDebug() << "Open " << fileName;
    QXmlStreamReader xmlReader(&file);
    QList<QTreeWidgetItem *> items;
    QStack<QTreeWidgetItem *> elements;
    elements.push(nullptr);
    while(!xmlReader.atEnd()){
        xmlReader.readNext();
        switch (xmlReader.tokenType()){
        case QXmlStreamReader::StartElement: {
            auto value = new QTreeWidgetItem(elements.top(), QStringList(xmlReader.name().toString()));
            foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                int row_num  = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row_num);
                ui->tableWidget->setItem(row_num, 0, new QTableWidgetItem(attr.name().toString()));
                ui->tableWidget->setItem(row_num, 1, new QTableWidgetItem(attr.value().toString()));
            }
            value->setFlags(value->flags() | Qt::ItemIsEditable);
            items.append(value);
            elements.push(value);
            break;
        }
        case QXmlStreamReader::Characters: {
            QString str = xmlReader.text().toString().trimmed();
            if (!str.isEmpty()) {
                auto value = new QTreeWidgetItem(elements.top(), QStringList(str));
                value->setFlags(value->flags() | Qt::ItemIsEditable);
                items.append(value);
            }
            break;
        }
        case QXmlStreamReader::EndElement: {
            elements.pop();
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
    ui->treeWidget->addTopLevelItems(items);
    ui->treeWidget->resizeColumnToContents(0);
    file.close(); // Закрываем файл
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{

}


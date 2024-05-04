#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QXmlStreamReader>
#include <QStack>
#include <QDebug>
#include <QVariant>
#include <QStandardItemModel>
#include <QMessageBox>

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
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "C:/", tr("XML files (*.xml)"));;
    if(file_name.size() == 0)
    {
        QMessageBox::warning(this, "Внимание", "Файл не был выбран");
        return;
    }
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл " + file_name);
        return;
    }

    QStandardItemModel *model = new QStandardItemModel(0, 0, ui->treeView);
    QStandardItem *parent_item = model->invisibleRootItem();
    ui->treeView->setModel(model);

    QXmlStreamReader xml_reader(&file);
    QStack<QStandardItem *> tags;
    tags.push(parent_item);
    while(!xml_reader.atEnd()){
        xml_reader.readNext();
        switch (xml_reader.tokenType()){
        case QXmlStreamReader::StartElement:
        { // открывающий тэг
            auto new_tag = new QStandardItem(xml_reader.name().toString());
            auto parent_tag = tags.top();
            parent_tag->appendRow(new_tag);
            auto table_model = new QStandardItemModel();
            for(auto &attr: xml_reader.attributes())
            {
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
        case QXmlStreamReader::Characters:
        { // текст внутри тэга
            QString str = xml_reader.text().toString().trimmed();
            if (!str.isEmpty())
            {
                auto tag = tags.top();
                tag->setText(tag->text() + ": " + str);
            }
            break;
        }
        case QXmlStreamReader::EndElement:
        {
            tags.pop();
            break;
        }
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            break;
        default:
        {
            qDebug() << "error: unexpected token type " << xml_reader.tokenType();
            break;
        }
        }
    }
    ui->treeView->resizeColumnToContents(0);
    file.close(); // Закрываем файл
}

static void writeAttributes(QXmlStreamWriter& xml_writer, const QStandardItemModel* model)
{
    for(int r = 0; r < model->rowCount(); ++r) {
        auto &&name = model->item(r, 0)->text();
        auto &&value = model->item(r, 1)->text();
        xml_writer.writeAttribute(name, value);
    }
}

static void writeTree(QXmlStreamWriter& xml_writer, const QStandardItem* root)
{
    auto &&tag_data = root->text().split(": ");
    xml_writer.writeStartElement(tag_data.front());
    auto &&model = root->data(Qt::UserRole + 1).value<QStandardItemModel*>();
    writeAttributes(xml_writer, model);
    if (tag_data.size() > 1)
    {
        tag_data.pop_front();
        xml_writer.writeCharacters(tag_data.join(": "));
    }
    for(int r = 0; root->hasChildren() && r < root->rowCount(); ++r)
    {
        auto &&item = root->child(r);
        writeTree(xml_writer, item);
    }
    xml_writer.writeEndElement();
}

void MainWindow::on_actionSave_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save As"), "C:/", tr("XML files (*.xml)"));
    if(file_name.size() == 0)
    {
        QMessageBox::warning(this, "Внимание", "Файл не был выбран");
        return;
    }
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в файл " + file_name);
        return;
    }

    QXmlStreamWriter xml_writer(&file);
    xml_writer.setAutoFormatting(true);  // Устанавливаем автоформатирование текста

    auto &&model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    auto &&first_item = model->item(0, 0);
    xml_writer.writeStartDocument();
    writeTree(xml_writer, first_item);
    xml_writer.writeEndDocument();
    file.close();
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->tableView->setModel(index.data(Qt::UserRole + 1).value<QStandardItemModel*>());
    for(int i =  0; i < ui->horizontalLayout->count(); ++i)
    {
        ui->horizontalLayout->itemAt(i)->widget()->setDisabled(false);
    }
    if(index.parent() == ui->treeView->rootIndex())
    {
        ui->pushButton_plus->setDisabled(true);
    }
}

void MainWindow::on_pushButton_plus_clicked()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (indexes.size() == 1) {
        QModelIndex selected_index = indexes.at(0);
        ui->treeView->model()->insertRow(selected_index.row() + 1, selected_index.parent());

        QModelIndex insertedElement = selected_index.siblingAtRow(selected_index.row() + 1);

        ui->treeView->selectionModel()->select(insertedElement, QItemSelectionModel::ClearAndSelect);
        ui->treeView->edit(insertedElement);

    }
}

void MainWindow::on_pushButton_minus_clicked()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (indexes.size() == 1) {

        QModelIndex selectedIndex = indexes.at(0);
        ui->treeView->model()->removeRow(selectedIndex.row(), selectedIndex.parent());
    }
    if (ui->treeView->children().size() == 0)
    {
        for(int i =  0; i < ui->horizontalLayout->count(); ++i)
        {
            ui->horizontalLayout->itemAt(i)->widget()->setDisabled(false);
        }
    }
}


void MainWindow::on_pushButton_new_child_clicked()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (indexes.size() == 1) {
        QModelIndex index = indexes.at(0);
        if (auto && model = qobject_cast<const QStandardItemModel*>(ui->treeView->model()))
        {
            if (auto && selected_tag = model->itemFromIndex(index))
            {
                auto &&tag_text = selected_tag->text();
                auto &&tag_text_vec = tag_text.split(": ");
                if(tag_text_vec.size() > 1)
                {
                    selected_tag->setText(tag_text_vec[0]);
                }

                auto child_tag = new QStandardItem();
                selected_tag->insertRow(0, child_tag);
                ui->treeView->expand(selected_tag->index());
                ui->treeView->selectionModel()->select(child_tag->index(), QItemSelectionModel::ClearAndSelect);
                ui->treeView->edit(child_tag->index());
            }
        }
    }
}


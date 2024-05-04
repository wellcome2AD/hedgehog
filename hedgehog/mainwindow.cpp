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

    auto model = new QStandardItemModel(0, 0, ui->treeView);
    ui->treeView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static QStandardItem *createTag(QStandardItem * parent_tag, QStandardItemModel *attribute_table_view, const QString &text)
{
    if(attribute_table_view == nullptr)
    {
        attribute_table_view = new QStandardItemModel();
        attribute_table_view->setHorizontalHeaderLabels(QStringList({"Attribute", "Value"}));
        attribute_table_view->insertRow(0, new QStandardItem(""));
    }

    auto new_tag = new QStandardItem(text);
    parent_tag->appendRow(new_tag);
    QVariant table_model_variant;
    table_model_variant.setValue(attribute_table_view);
    new_tag->setData(table_model_variant);
    new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
    return new_tag;
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

    QStandardItemModel* model = nullptr;
    if (!(model = qobject_cast<QStandardItemModel*>(ui->treeView->model())))
        qDebug() << "Error: can't cast treeView->model() to QStandardItemModel*";
    QStandardItem *parent_item = model->invisibleRootItem();

    QXmlStreamReader xml_reader(&file);
    QStack<QStandardItem *> tags;
    tags.push(parent_item);
    while(!xml_reader.atEnd()){
        xml_reader.readNext();
        switch (xml_reader.tokenType()){
        case QXmlStreamReader::StartElement:
        { // открывающий тэг
            auto table_model = new QStandardItemModel();
            for(auto &attr: xml_reader.attributes())
            {
                auto attr_name_item = new QStandardItem(attr.name().toString());
                auto attr_value_item = new QStandardItem(attr.value().toString());
                int row_count = table_model->rowCount();
                table_model->insertRow(row_count, QList<QStandardItem *>({attr_name_item, attr_value_item}));
            }
            table_model->setHorizontalHeaderLabels(QStringList({"Attribute", "Value"}));
            auto &&new_tag = createTag(tags.top(), table_model, xml_reader.name().toString());
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

static void setDisableForLayoutElements(QLayout* layout, bool is_disabled)
{
    for(int i =  0; i < layout->count(); ++i)
    {
        layout->itemAt(i)->widget()->setDisabled(is_disabled);
    }
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->tableView->setModel(index.data(Qt::UserRole + 1).value<QStandardItemModel*>());
    setDisableForLayoutElements(ui->horizontalLayout, false);
    if(index.parent() == ui->treeView->rootIndex())
    {
        ui->pushButton_plus->setDisabled(true);
    }
}

void MainWindow::on_pushButton_plus_clicked()
{
    if(ui->treeView->model()->rowCount() == 0)
    {
        QStandardItemModel* model = nullptr;
        if (!(model = qobject_cast<QStandardItemModel*>(ui->treeView->model())))
        {
            qDebug() << "Error: can't cast treeView->model() to QStandardItemModel*";
            return;
        }
        QStandardItem *parent_item = model->invisibleRootItem();
        auto new_tag = createTag(parent_item, nullptr, "");
        ui->treeView->selectionModel()->select(new_tag->index(), QItemSelectionModel::ClearAndSelect);
        ui->treeView->edit(new_tag->index());
        setDisableForLayoutElements(ui->horizontalLayout, false);
        return;
    }

    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (indexes.size() == 1) {
        QModelIndex selected_index = indexes.at(0);
        if(selected_index == ui->treeView->rootIndex())
            return;
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
    if (ui->treeView->model()->rowCount() == 0)
    {
        ui->tableView->setModel(nullptr);
        setDisableForLayoutElements(ui->horizontalLayout, true);
        ui->pushButton_plus->setDisabled(false);
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
                auto child_tag = createTag(selected_tag, nullptr, "");
                ui->treeView->expand(selected_tag->index());
                ui->treeView->selectionModel()->select(child_tag->index(), QItemSelectionModel::ClearAndSelect);
                ui->treeView->edit(child_tag->index());
            }
        }
    }
}


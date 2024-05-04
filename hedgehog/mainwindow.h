#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_treeView_clicked(const QModelIndex &index);

    void on_pushButton_plus_clicked();

    void on_pushButton_minus_clicked();

    void on_pushButton_new_child_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtSql>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPdfWriter>

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
    void on_editAdd_clicked();
    void on_editUpdate_clicked();
    void on_editDelete_clicked();
    void on_viewAll_clicked();
    void on_viewSpecific_clicked();
    void on_helpReadMe_clicked();


    void on_enterButton_clicked();

    void on_searchDeleteEnter_returnPressed();

    void on_idEnter_returnPressed();

    void on_webSearch_returnPressed();

    void on_searchButton_clicked();

    void on_passEnter_returnPressed();

    void on_deleteButton_clicked();

    void on_updateEnter_returnPressed();

    void on_updateWebsiteSearchButton_clicked();

    void on_submitUpdate_clicked();

    void on_updatePassEnter_returnPressed();

    void on_searchUpdateButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    bool checkWebsiteQuery(QString);
};
#endif // MAINWINDOW_H


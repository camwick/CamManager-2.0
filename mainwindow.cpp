#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER))
    {
        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName("mydb.db");
    }

    if(!db.open())
        qWarning() << "ERROR: " << db.lastError();

    connect (ui -> actionAdd, SIGNAL(triggered()), SLOT(on_editAdd_clicked()));
    connect (ui -> actionUpdate, SIGNAL(triggered()), SLOT(on_editUpdate_clicked()));
    connect (ui -> actionDelete, SIGNAL(triggered()), SLOT(on_editDelete_clicked()));
    connect (ui -> actionAll, SIGNAL(triggered()), SLOT(on_viewAll_clicked()));
    connect (ui -> actionSpecific_Entry, SIGNAL(triggered()), SLOT(on_viewSpecific_clicked()));
    connect (ui -> actionRead_Me, SIGNAL(triggered()), SLOT(on_helpReadMe_clicked()));

    ui -> stackedWidget -> setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ------- menu bar actions -----------
void MainWindow::on_editAdd_clicked()
{
    ui -> stackedWidget -> setCurrentIndex(0);
}

void MainWindow::on_editUpdate_clicked()
{
    ui -> stackedWidget -> setCurrentIndex(1);
    ui -> updateEnter -> clear();
    ui -> updateUserEnter -> clear();
    ui -> updatePassEnter -> clear();
}

void MainWindow::on_editDelete_clicked()
{
    ui -> stackedWidget -> setCurrentIndex(2);
    ui -> searchDeleteEnter -> clear();
    ui -> idSearchResult -> clear();
    ui -> idEnter -> clear();
}

void MainWindow::on_viewAll_clicked()
{
    ui -> stackedWidget -> setCurrentIndex(3);

    QSqlQuery query;

    QString data = "";

    query.exec("SELECT * FROM websites");
    while(query.next())
    {
        data += "Website:    " + query.value(1).toString() + "\n";
        data += "Username: " + query.value(2).toString() + "\n";
        data += "Password:  " + query.value(3).toString() + "\n\n";

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();
    }

    if(data == "")
        ui -> entriesBrowser -> setText("No Entries in database");
    else
        ui -> entriesBrowser -> setText(data);
}

void MainWindow::on_viewSpecific_clicked()
{
    ui -> stackedWidget -> setCurrentIndex(4);
    ui -> searchResults -> clear();
    ui -> searchUpdateButton -> hide();
}

void MainWindow::on_helpReadMe_clicked()
{
    ui -> stackedWidget -> setCurrentIndex(5);
}

// ------- window functions -----------
// Enter data
void MainWindow::on_enterButton_clicked()
{
    QString website = ui -> webEnter -> text().toLower();
    QString username = ui -> userEnter -> text();
    QString password = ui -> passEnter -> text();

    // if website entry already exists, prompts user to update entry instead
    if(checkWebsiteQuery(website) == false)
    {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("'" + website + "' already in database. Please update entry instead");
        msg.exec();

        ui -> webEnter -> clear();
        ui -> userEnter -> clear();
        ui -> passEnter -> clear();
    }
    else
    {
        QSqlQuery query;
        query.prepare("INSERT INTO websites (website, username, password) VALUES (:website, :username, :password)");
        query.bindValue(":website", website);
        query.bindValue(":username", username);
        query.bindValue(":password", password);
        query.exec();

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();

        ui -> webEnter -> clear();
        ui -> userEnter -> clear();
        ui -> passEnter -> clear();
    }
}

void MainWindow::on_passEnter_returnPressed()
{
    on_enterButton_clicked();
}

// Delete
void MainWindow::on_searchDeleteEnter_returnPressed()
{
    ui -> idSearchResult -> clear();
    QString website = ui -> searchDeleteEnter -> text();

    QSqlQuery query;
    query.prepare("SELECT ID FROM websites WHERE website = ?");
    query.addBindValue(website);

    if(!query.exec())
    {
        qWarning() << "MainWindow::on_searchDeleteEnter_returnPressed - ERROR: " << query.lastError().text();
    }

    QString data = "";
    if(query.first())
    {
        data += "Website: " + website + "\n";
        data += "ID: " + query.value(0).toString();

        ui -> idSearchResult -> setText(data);
    }
    else
    {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("'" + website + "' doesn't exist in database");
        msg.exec();
    }
}

void MainWindow::on_idEnter_returnPressed()
{
    on_deleteButton_clicked();
}

void MainWindow::on_deleteButton_clicked()
{
    QString id = ui -> idEnter -> text();

    QSqlQuery query;
    query.prepare("SELECT * FROM websites where ID = ?");
    query.addBindValue(id);

    QMessageBox msg;
    if(!query.exec())
        qWarning() << "ERROR: " << query.lastError().text();

    if(query.first())
    {
        QSqlQuery q;
        q.prepare("DELETE FROM websites WHERE ID = ?");
        q.addBindValue(query.value(0).toString());

        if(!q.exec())
            qWarning() << "ERROR: " << q.lastError().text();

        msg.setIcon(QMessageBox::Information);
        msg.setText("Entry ID# " + id + "\nWebsite: " + query.value(1).toString() + "\nDeleted Successfully");
        msg.exec();
    }
    else
    {
       msg.setIcon(QMessageBox::Warning);
       msg.setText("Invalid ID number entered: " + id);
       msg.exec();
    }


    ui -> searchDeleteEnter -> clear();
    ui -> idSearchResult -> clear();
    ui -> idEnter -> clear();
}

// specific view
void MainWindow::on_webSearch_returnPressed()
{
    on_searchButton_clicked();
}

void MainWindow::on_searchButton_clicked()
{
    ui -> searchResults -> clear();
    QString website = ui -> webSearch -> text().toLower();

    QSqlQuery query;
    query.prepare("SELECT * FROM websites WHERE website = ?");
    query.addBindValue(website);

    if(!query.exec())
            qWarning() << "MainWindow::on_searchButton_clicked - ERROR: " << query.lastError().text();

    QString data = "";
    if(query.first())
    {
        data += "Website:    " + query.value(1).toString() + "\n";
        data += "Username: " + query.value(2).toString() + "\n";
        data += "Password:  " + query.value(3).toString() + "\n\n";

        ui -> searchResults -> setText(data);
        ui -> searchUpdateButton -> show();
        ui -> webSearch -> clear();
    }
    else
    {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("'" + website + "' not found in database");
        msg.exec();

        ui -> webSearch -> clear();
    }
}

bool MainWindow::checkWebsiteQuery(QString data){
    bool test = true;

    QSqlQuery query;
    query.prepare("SELECT * FROM websites WHERE website = ?");
    query.addBindValue(data);

    if(!query.exec())
        qWarning() << "checkWebsiteQuery() - ERROR: " << query.lastError().text();

    if(query.first())
        test = false;

    return test;
}

void MainWindow::on_updateEnter_returnPressed()
{
    on_updateWebsiteSearchButton_clicked();
}

void MainWindow::on_updateWebsiteSearchButton_clicked()
{
    QString website = ui -> updateEnter -> text().toLower();
    QString updateUser, updatePass;

    QSqlQuery query;
    query.prepare("SELECT * FROM websites WHERE website = ?");
    query.addBindValue(website);
    if(!query.exec())
        qWarning() << "checkWebsiteQuery() - ERROR: " << query.lastError().text();

    if(query.first())
    {
        updateUser = query.value(2).toString();
        updatePass = query.value(3).toString();

        ui -> updateUserEnter -> setPlaceholderText("Username: " + updateUser);
        ui -> updatePassEnter -> setPlaceholderText("Password: " + updatePass);
    }
    else
    {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("'" + website + "' not in database");
        msg.exec();
    }
}

void MainWindow::on_submitUpdate_clicked()
{
    QString website = ui -> updateEnter -> text().toLower();
    QString username = ui -> updateUserEnter -> text();
    QString password = ui -> updatePassEnter -> text();

    QSqlQuery query;
    query.prepare("UPDATE websites SET username = ?, password = ? "
                  "WHERE website = ? ");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(website);

    if(!query.exec())
       qWarning() << "on_submitUpdate_clicked() - ERROR: " << query.lastError().text();

    ui -> updateEnter -> clear();
    ui -> updateUserEnter -> clear();
    ui -> updatePassEnter -> clear();
    ui -> updateUserEnter -> setPlaceholderText("");
    ui -> updatePassEnter -> setPlaceholderText("");
}

void MainWindow::on_updatePassEnter_returnPressed()
{
    on_submitUpdate_clicked();
}

void MainWindow::on_searchUpdateButton_clicked()
{
    ui -> stackedWidget -> setCurrentIndex(1);
}

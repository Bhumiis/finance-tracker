#include "databasemanager.h"
#include<QSqlDatabase>
#include<QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QVariant>
#include <QDebug>


bool DatabaseManager::initialize(const QString &path) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if (!db.open()) {
        qWarning() << "Cannot open DB:" << db.lastError().text();
        return false;
    }
    QSqlQuery q;
    // columns: id, type (Income/Expense), category, amount, date, note
    if (!q.exec("CREATE TABLE IF NOT EXISTS transactions ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "type TEXT, "
                "category TEXT, "
                "amount REAL, "
                "date TEXT, "
                "note TEXT)")) {
        qWarning() << "Create table failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addTransaction(const QString &type,
                                     const QString &category,
                                     double amount,
                                     const QDate &date,
                                     const QString &note) {
    QSqlQuery q;
    q.prepare("INSERT INTO transactions (type, category, amount, date, note) "
              "VALUES (:t, :c, :a, :d, :n)");
    q.bindValue(":t", type);
    q.bindValue(":c", category);
    q.bindValue(":a", amount);
    q.bindValue(":d", date.toString(Qt::ISODate));
    q.bindValue(":n", note);
    if (!q.exec()) {
        qWarning() << "Insert failed:" << q.lastError().text();
        return false;
    }
    return true;
}

QSqlTableModel* DatabaseManager::createTableModel(QObject *parent) {
    QSqlTableModel *model = new QSqlTableModel(parent);
    model->setTable("transactions");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Category");
    model->setHeaderData(3, Qt::Horizontal, "Amount");
    model->setHeaderData(4, Qt::Horizontal, "Date");
    model->setHeaderData(5, Qt::Horizontal, "Note");
    return model;
}

QMap<QString,double> DatabaseManager::categorySums(const QString &type) {
    QMap<QString,double> map;
    QSqlQuery q;
    q.prepare("SELECT category, SUM(amount) FROM transactions WHERE type = :t GROUP BY category");
    q.bindValue(":t", type);
    if (!q.exec()) {
        qWarning() << "Query failed:" << q.lastError().text();
        return map;
    }
    while (q.next()) {
        QString cat = q.value(0).toString();
        double sum = q.value(1).toDouble();
        map[cat] = sum;
    }
    return map;
}

double DatabaseManager::totalByType(const QString &type) {
    QSqlQuery q;
    q.prepare("SELECT SUM(amount) FROM transactions WHERE type = :t");
    q.bindValue(":t", type);
    if (!q.exec()) return 0.0;
    if (q.next()) return q.value(0).toDouble();
    return 0.0;
}

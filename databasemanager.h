#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include<QMap>
#include<QString>
#include<QDate>
#include <QObject>
#include <QSqlTableModel>

class QSqlTableModel;
class DatabaseManager
{
public:
    static bool initialize(const QString &path ="finance.db");
    static bool addTransaction(const QString &type,
                               const QString &category,
                               double amount,
                               const QDate &date,
                               const QString &note);

    static QSqlTableModel* createTableModel(QObject *parent = nullptr);
    static QMap<QString,double> categorySums(const QString &type = "Expense");
    static double totalByType(const QString &type);
};

#endif // DATABASEMANAGER_H

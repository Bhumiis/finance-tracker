#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QMessageBox>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr) {
    ui->setupUi(this);

    // Initialize DB
    if (!DatabaseManager::initialize()) {
        QMessageBox::critical(this, "DB Error", "Cannot open or create database (QSQLITE).");
        return;
    }

    // Setup categories (you can make this dynamic later)
    ui->comboCategory->addItems({"Food", "Rent", "Transport", "Salary", "Bills", "Misc"});

    // Setup model and view
    model = DatabaseManager::createTableModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Connect Add button
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::on_btnAdd_clicked);

    // initial refresh
    refreshData();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_btnAdd_clicked() {
    QString type = ui->comboType->currentText();
    QString category = ui->comboCategory->currentText();
    double amount = ui->amountSpin->value();
    QDate date = ui->dateEdit->date();
    QString note = ui->noteEdit->text();

    if (amount <= 0.0) {
        QMessageBox::warning(this, "Input", "Amount must be > 0");
        return;
    }

    if (!DatabaseManager::addTransaction(type, category, amount, date, note)) {
        QMessageBox::warning(this, "DB Error", "Failed to insert transaction");
        return;
    }

    // clear inputs slightly
    ui->amountSpin->setValue(0.0);
    ui->noteEdit->clear();

    refreshData();
}

void MainWindow::refreshData() {
    if (model) model->select();
    updateBalanceLabel();
    updateChart();
}

void MainWindow::updateBalanceLabel() {
    double totalIncome = DatabaseManager::totalByType("Income");
    double totalExpense = DatabaseManager::totalByType("Expense");
    double balance = totalIncome - totalExpense;
    ui->lblBalance->setText(QString("Balance: %1").arg(QString::number(balance, 'f', 2)));
}

void MainWindow::updateChart() {
    // Build pie chart for expenses
    QMap<QString,double> sums = DatabaseManager::categorySums("Expense");
    QPieSeries *series = new QPieSeries();
    for (auto it = sums.constBegin(); it != sums.constEnd(); ++it) {
        if (it.value() > 0.0)
            series->append(it.key(), it.value());
    }
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Expenses by Category");
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);

    // replace existing chartView content (assumes chartView is a QWidget placeholder)
    // If you used a QChartView directly in UI (objectName chartView) you can:
    if (ui->chartView->chart()) {
        // delete old chart to avoid leak
        delete ui->chartView->chart();
    }
    ui->chartView->setChart(chart);
}

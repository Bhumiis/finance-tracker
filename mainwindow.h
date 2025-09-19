#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:

    void refreshData();
    void on_btnAdd_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    void updateBalanceLabel();
    void updateChart();
};

#endif // MAINWINDOW_H

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <includes.h>

#include <QDebug>
#include <QChartView>
#include <QStandardItem>
#include <QtNetwork/QtNetwork>

QT_CHARTS_USE_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    //Widget functions
    void fetchData();
    void drawWidget();
    void currencyGetData();
    void table1GetData();
    void showChart();
    void timerFunc();
    void updateProfit();


private:
    QJsonArray jsonArray;
    QJsonArray ja;
    QNetworkAccessManager networkManager;

    //Menu bar
    QMenuBar* menuBar;
    QMenu*fileMenu;

    //Chart
    QChartView* chartView;
    QChart* chart;
    QStackedBarSeries* series;


    //Table view Layout/////////////
    QTableView* table1;
    QStandardItemModel *table1Model;
    QTableView* table2;
    QStandardItemModel *table2Model;

    QPushButton* saveButton;
    QPushButton* loadButton;
    QPushButton* clearButton;

    //Profit Layout///////////
    //Currency
    QLabel*currencyLabel;
    QComboBox* currencyCBox;

    //Amount
    QLabel* amountLabel;
    QDoubleSpinBox* amountDSBox;

    //Buy price
    QLabel*buyPriceLabel;
    QSpinBox* buyPriceSBox;

    //Profit button
    QPushButton* profitButton;

    //Init button
    QPushButton* initButton;

    //Timer to fetch values every 5 min
    QTimer timer;
    bool timerIsRunning;

    //Calculating variables
    double amount;
    int buyPrice;
    double total;

    QJsonObject object;

private slots:
    void profitOnClick();
    void saveOnClick();
    void loadOnClick();
    void clearOnClick();
    void initOnClick();





};

#endif // WIDGET_H

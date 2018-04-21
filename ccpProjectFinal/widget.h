#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <includes.h>

#include <QDebug>
#include <QChartView>
#include <QStandardItem>

QT_CHARTS_USE_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    //Widget functions
    void drawWidget();
    void currencyGetData();
    void table1GetData();


private:

    //Menu bar
    QMenuBar* menuBar;
    QMenu*fileMenu;

    //Chart
    QChartView* chartView;
    QPieSeries *series;


    //Table view Layout/////////////
    QTableView* table1;
    QStandardItemModel *table1Model;
    QTableView* table2;
    QStandardItemModel *table2Model;

    QPushButton* saveButton;
    QPushButton* loadButton;

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

private slots:
    void profitOnClick();
    void saveOnClick();
    void loadOnClick();




};

#endif // WIDGET_H

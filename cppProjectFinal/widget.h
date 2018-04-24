#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <includes.h>

QT_CHARTS_USE_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    //Widget funksjoner
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

    //Qtgui klasser- Objekter

    //BarChart layout///////
    //Diagram(BarChart)
    QChartView* chartView;
    QChart* chart;
    QStackedBarSeries* series;

    //TableViews layout///////
    //Filhåndterings knapper(QPushButton)
    QTableView* table1;
    QStandardItemModel *table1Model;

    //Kjøpt table(Tableview)
    QTableView* table2;
    QStandardItemModel *table2Model;

    //Profit(Input) Layout///////////
    //Value input(Currency)
    QPushButton* saveButton;
    QPushButton* loadButton;
    QPushButton* clearButton;

    //Profit Layout///////////
    //Currency
    QLabel*currencyLabel;
    QComboBox* currencyCBox;

    //Mengde input(Amount)
    QLabel* amountLabel;
    QDoubleSpinBox* amountDSBox;

    //Kjøpspris input(BuyPrice)
    QLabel*buyPriceLabel;
    QSpinBox* buyPriceSBox;

    //Fortjeneste submit(Profit)
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
    QMessageBox* errorbox;

//slots funksjoner som vi deklarerer for å utføre forskjellige operasjoner ved et knappetrykk
//som å lagre og laste opp fil og oppdatere diagrammet
private slots:
    void profitOnClick();
    void saveOnClick();
    void loadOnClick();
    void clearOnClick();
    void initOnClick();
};

#endif // WIDGET_H

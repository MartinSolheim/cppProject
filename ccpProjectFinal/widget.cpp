#include "widget.h"
#include <includes.h>
#include <calculations.h>
#include <calculations.cpp>

#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace std;
using std::cout;
using std::endl;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    fetchData();
    drawWidget();
}

Widget::~Widget()
{

}

void Widget::fetchData(){


    cout << "før network greier" << endl;
    cout << "isEmpty før connect: " << jsonArray.isEmpty() << endl;
    QObject::connect(&networkManager, &QNetworkAccessManager::finished,
                     [&](QNetworkReply* reply){

        cout << "inne i network. før if" << endl;
        if(reply->error() != QNetworkReply::NoError){
            cout << "network connection feil" << endl;
            networkManager.clearAccessCache();
        } else {

            //Hele json teksten
            jsonArray = QJsonDocument::fromJson(reply->readAll()).array();
            //Liste med alle array elementene
            //vm0 = jsonArray.toVariantList();
            //QString vm = "";
            //looper gjennom for å søke på IDen.
            cout << "INNE isEmpty: " << jsonArray.isEmpty() << endl;
        }
        reply->deleteLater();
    });
    //query om nødvendig
    //QUrlQuery query;
    //query.addQueryItem("id", "bitcoin");
    QUrl url("https://api.coinmarketcap.com/v1/ticker/?limit=11");
    //kanskje vi får bruk for det
    //url.setQuery(query);
    QNetworkRequest networkRequest(url);
    networkManager.get(networkRequest);

}

void Widget::drawWidget(){
    ////////////////////////Layout///////////////////////////////

    //Menu Layout

    fileMenu = new QMenu("File");
    fileMenu->addAction("Save");
    fileMenu->addAction("Load");
    fileMenu->addAction("Exit");
    menuBar = new QMenuBar();
    menuBar->addMenu(fileMenu);


    //Chart
    chartView = new QChartView();
    //chartView->setRenderHint(QPainter::Antialiasing);

    chart = new QChart();
    series = new QStackedBarSeries();
    //QChart *chart1 = new QChart();
    //QStackedBarSeries *series1 = new QStackedBarSeries();

    //Tables Layout
    table1 = new QTableView();
    table1Model = new QStandardItemModel;

    table1Model->setColumnCount(2);
    table1Model->setHorizontalHeaderLabels(QStringList()<<"Currency"<<"Value");
    table1->setModel(table1Model);

    table2 = new QTableView();
    table2Model = new QStandardItemModel;

    table2Model->setColumnCount(4);
    table2Model->setHorizontalHeaderLabels(QStringList()<<"Currency"<<"Value"<<"Buy price"<<"Profit");
    table2->setModel(table2Model);

    saveButton = new QPushButton("Save data");
    QObject::connect(saveButton, SIGNAL(clicked(bool)),this,SLOT(saveOnClick()));
    loadButton = new QPushButton("Load data");
    QObject::connect(loadButton, SIGNAL(clicked(bool)),this,SLOT(loadOnClick()));
    clearButton = new QPushButton("Clear table");
    QObject::connect(clearButton, SIGNAL(clicked(bool)),this,SLOT(clearOnClick()));

    QHBoxLayout*buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(clearButton);

    QVBoxLayout*table2ButtonLayout = new QVBoxLayout();
    table2ButtonLayout->addLayout(buttonLayout);
    table2ButtonLayout->addWidget(table2);


    QHBoxLayout* tablesLayout = new QHBoxLayout;
    tablesLayout->addWidget(table1);
    tablesLayout->addLayout(table2ButtonLayout);

    //Profit calculations////////////////

    //Currency Layout
    currencyLabel = new QLabel("Currency:");
    currencyCBox = new QComboBox;

    QHBoxLayout* currencyLayout = new QHBoxLayout;
    currencyLayout->addWidget(currencyLabel);
    currencyLayout->addWidget(currencyCBox);

    //Amount Layout
    amountLabel = new QLabel("Amount:");
    amountDSBox = new QDoubleSpinBox;
        amountDSBox->setRange(0.0,1000000);

    QHBoxLayout* amountLayout = new QHBoxLayout;
    amountLayout->addWidget(amountLabel);
    amountLayout->addWidget(amountDSBox);

    //Buy price Layout
    buyPriceLabel = new QLabel("Buy price:");
    buyPriceSBox = new QSpinBox;
        buyPriceSBox->setRange(0, 1000000);

    QHBoxLayout* buyPriceLayout = new QHBoxLayout;
    buyPriceLayout->addWidget(buyPriceLabel);
    buyPriceLayout->addWidget(buyPriceSBox);

    //Profit Button
    profitButton = new QPushButton("Beregn");

    QObject::connect(profitButton, SIGNAL(clicked(bool)),this,SLOT(profitOnClick()));

    //Profit calculations Layout
    QVBoxLayout* profitLayout = new QVBoxLayout;
    profitLayout->addLayout(currencyLayout);
    profitLayout->addLayout(amountLayout);
    profitLayout->addLayout(buyPriceLayout);
    profitLayout->addWidget(profitButton);

    //Main Layout////////////////////////
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
    mainLayout->addLayout(tablesLayout);
    mainLayout->addLayout(profitLayout);

    setLayout(mainLayout);
    setMinimumSize(800, 1000);
    layout()->setMenuBar(menuBar);
    setWindowTitle("Crypto");
    setWindowIcon(QIcon("../ccpProjectFinal/source/Bitcoin-icon.png"));
    setAutoFillBackground(false);

}

//Loading chart
void Widget::showChart(){

    series->clear();
    QVector<QString>currencyName;
    QVector<double>percentChange;

    QBarSet *plus = new QBarSet("Negative % change");
    QBarSet *minus = new QBarSet("Positive % change");

    QStringList curList;

    foreach(const QJsonValue & value, jsonArray){
            QJsonObject object = value.toObject();
            currencyName.push_back(object["name"].toString());
            curList.append((object["name"].toString()));
            percentChange.push_back(object["percent_change_24h"].toString().toDouble());
    }

    for(int i = 0; i < 10; i++){
        if(percentChange.at(i) > 0){
            *plus << percentChange.at(i);
            *minus << 0;
        } else{
            *minus << percentChange.at(i);
            *plus << 0;
        }
    }


    series->append(plus);
    series->append(minus);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(curList);
    axis->setTitleText("Percent Change last 24 hours");

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->axisY(series)->setRange(-25, 25);
    chart->axisY(series)->setTitleText("%change");
    chart->setTitle("Market");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);

}

//JSON to combobox
void Widget::currencyGetData(){

    QVector<QString>currencyName;

    /*
    //Open file
    QString text;
    QFile file;
    file.setFileName("../ccpProjectFinal/source/currencyData.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    text = file.readAll();
    file.close();

    QJsonDocument jResponse = QJsonDocument::fromJson(text.toUtf8());
    QJsonArray jArray = jResponse.array();
    */
    foreach(const QJsonValue & value, jsonArray){
        QJsonObject object = value.toObject();
        currencyName.push_back(object["name"].toString());
    }

    //List only first objects
    for(int i=0; i < 10; i++){
            currencyCBox->addItem(currencyName.at(i));
    }
}

//JSON to table1(Price list)
void Widget::table1GetData(){

    QVector<QString>currencyName;
    QVector<double>currencyValue;


    /*
    //Open file
    QString text;
    QFile file;
    file.setFileName("../ccpProjectFinal/source/currencyData.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    text = file.readAll();
    file.close();

    QJsonDocument jResponse = QJsonDocument::fromJson(text.toUtf8());
    QJsonArray jArray = jResponse.array();
    */

    foreach(const QJsonValue & value, jsonArray){
            QJsonObject object = value.toObject();
            currencyName.push_back(object["name"].toString());
            currencyValue.push_back(object["price_usd"].toString().toDouble());
    }


    //List only first objects
    for(int i=0;i < 10; i++){
        QStandardItem* name = new QStandardItem(QString(currencyName.at(i)));
        QStandardItem* value = new QStandardItem(QString::number(currencyValue.at(i)));
        table1Model->setItem(i,0,name);
        table1Model->setItem(i,1,value);
    }
}


//Calculate on table2(Price bought)
void Widget::profitOnClick(){


    currencyGetData();
    table1GetData();
    showChart();

    double amount = amountDSBox->value();
    int buyPrice = buyPriceSBox->value();
    double total;
    /*
    QString text;
    QFile file;
    file.setFileName("../ccpProjectFinal/source/currencyData.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    text = file.readAll();
    file.close();

    QJsonDocument jResponse = QJsonDocument::fromJson(text.toUtf8());
    QJsonArray jArray = jResponse.array();
    */
    //int row =  table2->rowCount();
    //table2->setRowCount(row+1);

    foreach(const QJsonValue & value, jsonArray){
        QJsonObject object = value.toObject();
        if(currencyCBox->currentText() == object["name"].toString()){

                total = profitCalc(object["price_usd"].toString().toDouble(), amount ,buyPrice);
                //qDebug()<< total;

                QStandardItem* itemCurrency = new QStandardItem(QString(currencyCBox->currentText()));
                QStandardItem* itemAmount = new QStandardItem(QString::number(amount));
                QStandardItem* itemBuyPrice = new QStandardItem(QString::number(buyPrice));
                QStandardItem* itemProfit = new QStandardItem(QString::number(total));


                table2Model->setRowCount(table2Model->rowCount()+1);

                table2Model->setItem(table2Model->rowCount()-1,0,itemCurrency);
                table2Model->setItem(table2Model->rowCount()-1,1,itemAmount);
                table2Model->setItem(table2Model->rowCount()-1,2,itemBuyPrice);
                table2Model->setItem(table2Model->rowCount()-1,3,itemProfit);

        }

    }
}

//Save table2(Price bought) to csv
void Widget::saveOnClick(){
    //Open file
    QString filename = QFileDialog::getSaveFileName(
                this,tr("Save File"),"C:/","CSV file (*.csv) ");

    if (filename.isEmpty())return;
        else {
            QFile file(filename);

            if (file.open(QIODevice::ReadWrite)) {
                   QTextStream stream(&file);
                   QStringList strList;

                       for( int r = 0; r <table2Model->rowCount(); ++r )
                       {
                           strList.clear();
                           for( int c = 0; c < table2Model->columnCount(); ++c )
                           {   QStandardItem* item = table2Model->item(r,c);       //Load items
                               if (!item || item->text().isEmpty())                        //Test if there is something at item(r,c)
                               {
                                   table2Model->setItem(r,c,new QStandardItem("0"));//IF there is nothing write 0
                               }
                               strList << " "+table2Model->item( r, c )->text()+" ";

                           }
                           stream << strList.join( ";" )+"\n";
                       }
                       file.close();
                   }

               }

    }

//Load table2(Price bought)
void Widget::loadOnClick(){

    QString filename = QFileDialog::getOpenFileName(
                this,tr("Save File"),"C:/","CSV file (*.csv) ");

    QFile file(filename);

    if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
            qDebug() << "File not exists";
        } else {
            QTextStream in(&file);
            while (!in.atEnd())
            {
                QString line = in.readLine();

                QList<QStandardItem *> standardItemsList;
                for (QString item : line.split(";")) {
                    standardItemsList.append(new QStandardItem(item));
                }
                table2Model->insertRow(table2Model->rowCount(), standardItemsList);
            }
            file.close();
        }

}

//Clear table
void Widget::clearOnClick(){
    table2Model->clear();

    table2Model->setColumnCount(4);
    table2Model->setHorizontalHeaderLabels(QStringList()<<"Currency"<<"Value"<<"Buy price"<<"Profit");
}

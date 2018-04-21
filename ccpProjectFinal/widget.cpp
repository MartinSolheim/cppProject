#include "widget.h"
#include <includes.h>
#include <calculations.h>
#include <calculations.cpp>

#include <QHBoxLayout>
#include <QVBoxLayout>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    drawWidget();
    currencyGetData();
    table1GetData();

}

Widget::~Widget()
{

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
        chartView->setRenderHint(QPainter::Antialiasing);

    series = new QPieSeries();


        series->append("Jane", 2000);
        series->append("Joe", 1000);
        series->append("Andy", 2000);
        series->append("Barbara", 3000);
        series->append("Axel", 2000);
        series->setLabelsVisible();
        series->setLabelsPosition(QPieSlice::LabelInsideHorizontal);

        for(int i = 0;i<5;i++){
            QPieSlice *slice = series->slices().at(i);
            slice->setPen(QPen(Qt::white, 1));
            slice->setBrush(Qt::green);

        }
        chartView->chart()->addSeries(series);


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

    QHBoxLayout*buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(loadButton);

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
    setMinimumSize(800, 600);
    layout()->setMenuBar(menuBar);
    setWindowTitle("Crypto");
    setWindowIcon(QIcon("../ccpProjectFinal/source/currencyIcon.png"));
    setAutoFillBackground(false);

    //Stylesheet
    //setStyleSheet("background-color:lightblue;");
    //table1->setStyleSheet("background-color:white;");

}

//JSON to combobox
void Widget::currencyGetData(){

    //Open file
    QString text;
    QFile file;
    file.setFileName("../ccpProjectFinal/source/currencyData.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    text = file.readAll();
    file.close();

    QJsonDocument jResponse = QJsonDocument::fromJson(text.toUtf8());
    QJsonArray jArray = jResponse.array();

    foreach(const QJsonValue & value, jArray){
        QJsonObject object = value.toObject();
        currencyCBox->addItem(object["name"].toString());
    }
}

//JSON to table1(Price list)
void Widget::table1GetData(){

    QVector<QString>currencyName;
    QVector<double>currencyValue;



    //Open file
    QString text;
    QFile file;
    file.setFileName("../ccpProjectFinal/source/currencyData.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    text = file.readAll();
    file.close();

    QJsonDocument jResponse = QJsonDocument::fromJson(text.toUtf8());
    QJsonArray jArray = jResponse.array();

    foreach(const QJsonValue & value, jArray){
            QJsonObject object = value.toObject();
            currencyName.push_back(object["name"].toString());
            currencyValue.push_back(object["price_usd"].toString().toDouble());

    }



    for(int i=0;i < 10; i++){
        QStandardItem* name = new QStandardItem(QString(currencyName.at(i)));
        QStandardItem* value = new QStandardItem(QString::number(currencyValue.at(i)));
        table1Model->setItem(i,0,name);
        table1Model->setItem(i,1,value);
    }
}


//Calculate on table2(Price bought)
void Widget::profitOnClick(){

    double amount = amountDSBox->value();
    int buyPrice = buyPriceSBox->value();
    double total;

    QString text;
    QFile file;
    file.setFileName("../ccpProjectFinal/source/currencyData.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    text = file.readAll();
    file.close();

    QJsonDocument jResponse = QJsonDocument::fromJson(text.toUtf8());
    QJsonArray jArray = jResponse.array();

    //int row =  table2->rowCount();
    //table2->setRowCount(row+1);

    foreach(const QJsonValue & value, jArray){
        QJsonObject object = value.toObject();
        if(currencyCBox->currentText() == object["name"].toString()){

                total = profitCalc(object["price_usd"].toString().toDouble(), amount ,buyPrice);
                qDebug()<< total;

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


















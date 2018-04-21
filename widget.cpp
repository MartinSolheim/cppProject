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


    //Tables Layout
    table1 = new QTableWidget();
        table1->setColumnCount(2);

        QTableWidgetItem* column1 = new QTableWidgetItem();
        column1->setText("Currency");
        table1->setHorizontalHeaderItem(0,column1);

        QTableWidgetItem* column2 = new QTableWidgetItem();
        column2->setText("Value(USD)");
        table1->setHorizontalHeaderItem(1,column2);




    table2 = new QTableWidget();
        table2->setColumnCount(4);

        for(int i=0;i<4;i++){
            table2->setColumnWidth(i,63);
        }

        QTableWidgetItem* tab2Column1 = new QTableWidgetItem();
        tab2Column1->setText("Currency");
        table2->setHorizontalHeaderItem(0,tab2Column1);

        QTableWidgetItem* tab2Column2 = new QTableWidgetItem();
        tab2Column2->setText("Amount");
        table2->setHorizontalHeaderItem(1,tab2Column2);

        QTableWidgetItem* tab2Column3 = new QTableWidgetItem();
        tab2Column3->setText("Buy price");
        table2->setHorizontalHeaderItem(2,tab2Column3);

        QTableWidgetItem* tab2Column4 = new QTableWidgetItem();
        tab2Column4->setText("Profit");
        table2->setHorizontalHeaderItem(3,tab2Column4);


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
    mainLayout->addLayout(tablesLayout);
    mainLayout->addLayout(profitLayout);


    setLayout(mainLayout);
    layout()->setMenuBar(menuBar);
    setWindowTitle("Crypto");
    setWindowIcon(QIcon("../ccpProjectFinal/source/currencyIcon.png"));
    setAutoFillBackground(false);

    //Stylesheet
    //setStyleSheet("background-color:lightblue;");
    //table1->setStyleSheet("background-color:white;");

}


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

    table1->setRowCount(currencyName.size());

    for(int i=0;i<currencyName.size();i++){
        //qDebug()<<currencyName.at(i);
        //qDebug()<<currencyValue.at(i);
        QTableWidgetItem* itemName = new QTableWidgetItem();
        QTableWidgetItem* itemNo = new QTableWidgetItem();

        itemName->setText(currencyName.at(i));
        itemNo->setText(QString::number(currencyValue.at(i)));

        table1->setItem(i,0, itemName);
        table1->setItem(i,1,itemNo);

    }
}

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

    int row =  table2->rowCount();
    table2->setRowCount(row+1);

    foreach(const QJsonValue & value, jArray){
        QJsonObject object = value.toObject();
        if(currencyCBox->currentText() == object["name"].toString()){

            total = profitCalc(object["price_usd"].toString().toDouble(), amount ,buyPrice);
            qDebug()<< total;

            QTableWidgetItem* itemCurrency = new QTableWidgetItem();
            QTableWidgetItem* itemAmount = new QTableWidgetItem();
            QTableWidgetItem* itemBuyPrice = new QTableWidgetItem();
            QTableWidgetItem* itemProfit = new QTableWidgetItem();

            itemCurrency->setText(currencyCBox->currentText());
            itemAmount->setText(QString::number(amount));
            //qDebug()<<amount;
            itemBuyPrice->setText(QString::number(buyPrice));

            if(total <0){
                itemProfit->setBackgroundColor(Qt::red);
            }else{
                itemProfit->setBackgroundColor(Qt::green);
            }
            itemProfit->setText(QString::number(total));

            table2->setItem(row,0,itemCurrency);
            table2->setItem(row,1,itemAmount);
            table2->setItem(row,2,itemBuyPrice);
            table2->setItem(row,3,itemProfit);

        }


}
}

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
                   //stream << "[" << endl<< " {";

                   /*
                   for( int c = 0; c < table2->columnCount(); ++c )
                       {
                           strList <<
                                      "\" " +
                                        table2->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString() +
                                      "\" ";
                       }


                       stream << strList.join(";") << "-";

                       */
                       for( int r = 0; r <table2->rowCount(); ++r )
                       {
                           strList.clear();
                           for( int c = 0; c < table2->columnCount(); ++c )
                           {   QTableWidgetItem* item = table2->item(r,c);;       //Load items
                               if (!item || item->text().isEmpty())                        //Test if there is something at item(r,c)
                               {
                                   table2->setItem(r,c,new QTableWidgetItem("0"));//IF there is nothing write 0
                               }
                               strList << "\" "+table2->item( r, c )->text()+"\" ";

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

    QFile loadedFile(filename);

    if (loadedFile.open(QFile::ReadOnly))
        {
        QTextStream in(&loadedFile);
        QStringList loadCsv;
         QStringList rowCsv;


        while(!loadedFile.atEnd()){

            loadCsv<<in.readLine().split(";");
            rowCsv<<in.readLine().split("\n");

            qDebug()<< loadCsv.size();
            qDebug()<< rowCsv.size();

            table2->setColumnCount(loadCsv.size());
            for(int row=0;row<in.readLine().size();++row){
                table2->setRowCount(loadCsv.size()); //It reading only first line.

                for(int col=0;col<loadCsv.size();++col){

                    table2->setColumnCount(rowCsv.size());
                    QTableWidgetItem *Items= new QTableWidgetItem(loadCsv[col]);
                    table2->setItem(row,col,Items);

                }
            }
        }

        }


}




/*
void Widget::sync()
{/*
    QUrl url("https://api.coinmarketcap.com/v1/ticker/");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(syncRequestFinished(QNetworkReply* )));

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        QNetworkRequest request;
        QUrl url("https://api.coinmarketcap.com/v1/ticker/");
        request.setUrl(url);
        QNetworkReply *reply = manager->get(request);

        qDebug()<<reply;



}

*/















#include "widget.h"
#include <includes.h>
#include <calculations.h>
#include <calculations.cpp>

#include <QHBoxLayout>
#include <QVBoxLayout>


using namespace std;
using std::cout;
using std::endl;

/* Martin Solheim, s305033
 * Suphakin Riempraser, s315572
*/

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    fetchData();
    drawWidget();
    timerFunc();
}

Widget::~Widget()
{

}


void Widget::fetchData(){

    QObject::connect(&networkManager, &QNetworkAccessManager::finished,
                     [&](QNetworkReply* reply){

        if(reply->error() != QNetworkReply::NoError){
            networkManager.clearAccessCache();
        } else {
            ja = QJsonDocument::fromJson(reply->readAll()).array();
            if(!ja.isEmpty()) jsonArray = ja;
        }
        reply->deleteLater();


    });
    QUrl url("https://api.coinmarketcap.com/v1/ticker/?limit=15");
    QNetworkRequest networkRequest(url);
    networkManager.get(networkRequest);
}


void Widget::drawWidget(){
    ////////////////////////Layout///////////////////////////////


    //BarChart layout///////
    //Diagram(BarChart)
    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chart = new QChart();
    series = new QStackedBarSeries();

    //TableView layout////////
    //Valuta tabell(TableView)
    table1 = new QTableView();
    table1Model = new QStandardItemModel;

    table1->setModel(table1Model);
    table1Model->setHorizontalHeaderLabels(QStringList()<<"Currency"<<"Value"<<"%Change 24h");



    //TableViews layout///////
    //Kjøpt tabell(TableView)
    table2 = new QTableView();
    table2Model = new QStandardItemModel;
    table2Model->setColumnCount(4);
    table2Model->setHorizontalHeaderLabels(QStringList()<<"Currency"<<"Amount"<<"Buy price"<<"Profit");
    table2->setModel(table2Model);

    //Filhåndterings knapper(QPushButton)
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

    //Profit(Input) Layout///////////
    //Value input(Currency)
    currencyLabel = new QLabel("Currency:");
    currencyCBox = new QComboBox;

    QHBoxLayout* currencyLayout = new QHBoxLayout;
    currencyLayout->addWidget(currencyLabel);
    currencyLayout->addWidget(currencyCBox);

    //Amount input(Amount)
    amountLabel = new QLabel("Amount:");
    amountDSBox = new QDoubleSpinBox;
    amountDSBox->setRange(0.0,1000000);

    QHBoxLayout* amountLayout = new QHBoxLayout;
    amountLayout->addWidget(amountLabel);
    amountLayout->addWidget(amountDSBox);

    //Buy price input(BuyPrice)
    buyPriceLabel = new QLabel("Buy price:");
    buyPriceSBox = new QSpinBox;
    buyPriceSBox->setRange(0, 1000000);

    QHBoxLayout* buyPriceLayout = new QHBoxLayout;
    buyPriceLayout->addWidget(buyPriceLabel);
    buyPriceLayout->addWidget(buyPriceSBox);

    //Profit Button
    profitButton = new QPushButton("Add crypto");
    QObject::connect(profitButton, SIGNAL(clicked(bool)),this,SLOT(profitOnClick()));

    //Init button
    initButton = new QPushButton("Start tracking");
    QObject::connect(initButton, SIGNAL(clicked(bool)), this, SLOT(initOnClick()));

    //Profit calculations Layout
    QVBoxLayout* profitLayout = new QVBoxLayout;
    profitLayout->addLayout(currencyLayout);
    profitLayout->addLayout(amountLayout);
    profitLayout->addLayout(buyPriceLayout);
    profitLayout->addWidget(profitButton);
    profitLayout->addWidget(initButton);

    //Main Layout////////////////////////
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
    mainLayout->addLayout(tablesLayout);
    mainLayout->addLayout(profitLayout);

    setLayout(mainLayout);
    setMinimumSize(900, 1000);
    setWindowTitle("Crypto");
    setWindowIcon(QIcon("/source/Bitcoin-icon.png"));
    setAutoFillBackground(false);
}

//Loading chart
void Widget::showChart(){

    //Tømmer seriene i Diagrammet
    series->clear();

    //Deklarerer to vektorer av typen String og double
    //cout << jsonArray.count() << endl;

    //Deklarerer to vektorer av typen String og double
    QVector<QString>currencyName;
    QVector<double>percentChange;

    if(!currencyName.isEmpty())currencyName.clear();
    if(!percentChange.isEmpty())percentChange.clear();

    QBarSet *plus = new QBarSet("Positive % change");
    QBarSet *minus = new QBarSet("Negative % change");

    QStringList curList;
    curList.clear();
    int topValue = 0;

    //Henter ut alle elementer fra JSON-array, og lagre det som et objekt
    //Deklare hvilken object man ønsker å hente ved hjelp av navnet på objektet
    foreach(const QJsonValue & value, jsonArray){
            object = value.toObject();
            currencyName.push_back(object["name"].toString());
            curList.append((object["symbol"].toString()));
            percentChange.push_back(object["percent_change_24h"].toString().toDouble());
    }

    for(int i = 0; i < jsonArray.size(); i++){
        if(percentChange.at(i) > 0){
            *plus << percentChange.at(i);
            *minus << 0;
            if(percentChange.at(i) > topValue) topValue = percentChange.at(i);
        } else{
            *minus << percentChange.at(i);
            *plus << 0;
            if(percentChange.at(i) * -1 > topValue) topValue = percentChange.at(i) * -1;
        }
    }

    series->append(plus);
    series->append(minus);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(curList);
    axis->setTitleText("Currency");

    chart->removeSeries(series);
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->axisY(series)->setRange(-topValue, topValue);
    chart->axisY(series)->setTitleText("%change");
    chart->setTitle("Percent change last 24 hours");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);
}

//Widget funksjon som henter henter dataene fra en JSON-array og legger til i Combobox
void Widget::currencyGetData(){

    //Deklarerer en ny vektor av typen QString
    //cout << jsonArray.count() << endl;

    //Deklarerer en vector av typen QString
    //Hvis vektoren er ikke tomt så blir det tømmet
    QVector<QString>currencyName;
    if(!currencyName.isEmpty())currencyName.clear();
    currencyCBox->clear();

    //Henter ut alle elementer fra JSON-array, og lagre det som et objekt
    //Deklare hvilken object man ønsker å hente ved hjelp av navnet på objektet
    foreach(const QJsonValue & value, jsonArray){
        object = value.toObject();
        //Konvertere typen til objektene til det som passer av string eller double
        //og legger det inn i vektoren som vi deklarerte tidligere
        currencyName.push_back(object["name"].toString());
    }

    //Putter inn de ti første verdiene som vi får ut fra indeksen til vektoren over
    //og legger det inn i Combobox. Det vil da være ti elementer i Combobox
    for(int i=0; i < jsonArray.size(); i++){
            currencyCBox->addItem(currencyName.at(i));
    }
}

//JSON to table1(Price list)
//Widget funksjon som deler JSON-formatet opp, og putter det inn i tabel1
void Widget::table1GetData(){

    //Kaller på en funksjon som tømmer alle elementene i tabelen
    table1Model->clear();
    table1Model->setColumnCount(3);
    table1Model->setHorizontalHeaderLabels(QStringList()<<"Currency"<<"Value"<<"%Change 24h");

    //Deklarere 3 vektorer av type double og string
    QVector<QString>currencyName;
    QVector<double>currencyValue;
    QVector<double>percentChange;

    //Dersom vektorene ikke er tomt, så blir det tømt
    if(!currencyName.isEmpty())currencyName.clear();
    if(!currencyValue.isEmpty())currencyValue.clear();
    if(!percentChange.isEmpty())percentChange.clear();

    //Henter ut alle elementer fra JSON-array, og lagre det som et objekt
    //Deklare hvilken object man ønsker å hente ved hjelp av navnet på objektet
    foreach(const QJsonValue & value, jsonArray){
            object = value.toObject();
            //Konvertere typen til objektene til det som passer av string eller double
            //og legger det inn i vektoren
            currencyName.push_back(object["name"].toString());
            currencyValue.push_back(object["price_usd"].toString().toDouble());
            percentChange.push_back(object["percent_change_24h"].toString().toDouble());
    }


    //Begrenser hvor mange elementer vi skal hente og legger det ut i table1(Tableview)
    //I dette tilfølge har vi valgt å hande ut kun 10 rader med 3 elementer per rad
    for(int i=0; i < jsonArray.size(); i++){
        QStandardItem* name = new QStandardItem(QString(currencyName.at(i)));
        QStandardItem* value = new QStandardItem(QString::number(currencyValue.at(i)));
        QStandardItem* pc = new QStandardItem(QString::number(percentChange.at(i)));
        table1Model->setItem(i,0,name);
        table1Model->setItem(i,1,value);
        table1Model->setItem(i,2,pc);
    }
}

//Timer funksjon som blir kalt annenhvert minutt.
void Widget::timerFunc(){

    QObject::connect(&timer, &QTimer::timeout, [&](){
        fetchData();
        table1GetData();
        showChart();
        currencyGetData();
        updateProfit();
    });
}

//Oppdater funksjon som regner hvor mye man har tjent eller tapt etter et knappetrykk
//Kalkulasjonen blir vist på kjøpt-tabellen(høyre)
void Widget::updateProfit(){
    amount = 0;
    buyPrice = 0;
    total = 0;
    QJsonObject jo;
    QString hoho;



    if(table2Model->rowCount() > 0){

            for(int j = 0; j < table2Model->rowCount(); j++){
                for(int i = 0; i < jsonArray.size()-1; i++){
                    jo = jsonArray[i].toObject();
                    //hoho = jo["name"].toString();
                    //cout << hoho.toStdString() << endl;
                    if(table2Model->item(j, 0)->text() == jo["name"].toString()){
                        break;
                    }
            }
                cout << "updateProfit arraySize: " << jsonArray.size() << endl;
                amount = table2Model->item(j, 1)->text().toDouble();
                buyPrice = table2Model->item(j, 2)->text().toInt();


                total = profitCalc(jo["price_usd"].toString().toDouble(), amount, buyPrice);
                QStandardItem* itemProfit = new QStandardItem(QString::number(total));

                table2Model->setItem(j,3,itemProfit);
        }
    }
}

//Initialize widget funksjon. Starter en timer som henter ny data og oppdaterer tabellene og chart.
void Widget::initOnClick(){

    if(!timerIsRunning){
        table1GetData();
        showChart();
        currencyGetData();
        timer.start(120*1000);
        timerIsRunning = true;
        initButton->setText("Stop Tracking");
    }else{
        timer.stop();
        timerIsRunning = false;
        initButton->setText("Start Tracking");
    }
}

//Slot funksjon som regner hvor mye man har tjent eller tapt etter et knappetrykk
//Kalkulasjonen blir vist på kjøpt-tabellen(høyre)
void Widget::profitOnClick(){

    //Setter inn verdier på kalkulasjons variablene
    //Verdien er det som blir skrevet inn i double spinbox(amount) og spinbox(buyprice)
    amount = amountDSBox->value();
    buyPrice = buyPriceSBox->value();
    total = 0;

    //Henter ut alle elementer fra JSON-array, og lagre det som et objekt
    //Deklare hvilken object man ønsker å hente ved hjelp av navnet på objektet
    foreach(const QJsonValue & value, jsonArray){
        object = value.toObject();
        //Dersom teksten matcher med et JSON-objekt
        //Så blir alt koden under kjørt
        if(currencyCBox->currentText() == object["name"].toString()){

                //Kaller på profitCalc funksjonen og det blir kalkulert fortjenesten
                total = profitCalc(object["price_usd"].toString().toDouble(), amount, buyPrice);

                //De ulike verdiene blir plassert i nye QStandardItem
                QStandardItem* itemCurrency = new QStandardItem(QString(currencyCBox->currentText()));
                QStandardItem* itemAmount = new QStandardItem(QString::number(amount));
                QStandardItem* itemBuyPrice = new QStandardItem(QString::number(buyPrice));
                QStandardItem* itemProfit = new QStandardItem(QString::number(total));

                //Med en gang knappen er trykket så blir det opprettet en ny rad
                table2Model->setRowCount(table2Model->rowCount()+1);

                //Alle standarditems som ble opprettet blir lagt inn en modell
                //som igjen blir lagt inn i kjøptstabellen(TableView)
                table2Model->setItem(table2Model->rowCount()-1,0,itemCurrency);
                table2Model->setItem(table2Model->rowCount()-1,1,itemAmount);
                table2Model->setItem(table2Model->rowCount()-1,2,itemBuyPrice);
                table2Model->setItem(table2Model->rowCount()-1,3,itemProfit);
        }
    }
}



//Slot funksjon som lagrer alle dataene fra table1(høyre) i en CSV-fil
void Widget::saveOnClick(){
    //Open et vindu slik at man kan velge hvor mange vil lagre filen
    //Path til der mange velger blir lagre til Qstring
    QString filename = QFileDialog::getSaveFileName(
                this,tr("Save File"),"C:/","CSV file (*.csv) ");

    //Dersom man trykker på avbryt med tomt Qstring så blir det returner ingenting
    if (filename.isEmpty())return;
    //Dersom man har valgt destinasjonen
        else {
            //Path blir lagret til en fil med det navnet man ønsker
            QFile file(filename);

            //Om det går ann å åpne fila, så kan man endre(write) innholdet
            if (file.open(QIODevice::ReadWrite)) {

                    //Variable av type TextStream og StringList
                   QTextStream stream(&file);
                   QStringList strList;

                       //Går gjennom alle radene som finnes i tabel2
                       for( int r = 0; r <table2Model->rowCount(); ++r )
                       {
                           //Tømmer innholdet i StringList
                           strList.clear();
                           //Går gjennom alle kolonnene i table 2
                           for( int c = 0; c < table2Model->columnCount(); ++c )
                           {
                               //Lager et standarditem av alle radene og kolonne som finnes i table2
                               QStandardItem* item = table2Model->item(r,c);
                               if (!item || item->text().isEmpty())
                               {
                                   //så blir det lagt et itemobjekt med teksten 0 på tabellen
                                   table2Model->setItem(r,c,new QStandardItem("0"));
                               }
                               //Dersom det er et verdi i et eksisterende felt, så blir det skrevet(overført)
                               //inn i et QStringList
                               strList << ""+table2Model->item( r, c )->text()+"";

                           }
                            //Skriver ut alle dataene i QStringlisten og over til den filen som vi har valgt
                           stream << strList.join( ";" )+"\n";
                       }
                       //Lukker filen
                       file.close();
                   }
               }
    }

//Slot funksjon som laste opp alle dataene fra en CSV-fil til table1(høyre)
void Widget::loadOnClick(){

    //Open et vindu slik at man kan velge hvor mange vil laste filen
    //Path til der mange velger blir lagre til Qstring
    QString filename = QFileDialog::getOpenFileName(
                this,tr("Save File"),"C:/","CSV file (*) ");

    //Path blir lagret til en fil med det navnet man ønsker
    QFile file(filename);

    //Dersom det ikke går ann å open filen, slik at vi kan lese innholdet
    //Så kommer det opp et advarsel
    if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
            //Det blir tilkalt et vindu med feilmeldinger
            qDebug() << "File not exists";
        }
        //Hvis man kan åpne fila
        else {
            //Variable av type TextStream
            QTextStream in(&file);
            //Så lenge filen innerholder data
            while (!in.atEnd()){
                //Deklarerer en QString varibel som leser linje per line fra den filen man åpner
                QString line = in.readLine();

                //Deklarer en QList som innerholder QStandardItems
                QList<QStandardItem *> standardItemsList;
                //Lager en loop som spliter alle elementene i fila med :
                //og putter det inn i QList
                for (QString item : line.split(";")) {
                    standardItemsList.append(new QStandardItem(item));
                }
                //Putter inn alle Standarditems fra QList inn i tabellen
                table2Model->insertRow(table2Model->rowCount(), standardItemsList);
            }
            //Lukker filen
            file.close();
        }
}

//Slot funksjon som fjerner alle elementene i table2(høyre)
void Widget::clearOnClick(){
    //Kaller på en funksjon som fjerner alle elementene i tabelen
    table2Model->clear();

    //Etter å ha fjernet alt, så oppretter vi 4 nye kolonner
        //deklarere kolonnenavnet på nytt, slik at det blir en ny tabell
    table2Model->setColumnCount(4);
    table2Model->setHorizontalHeaderLabels(QStringList()<<"Currency"<<"Value"<<"Buy price"<<"Profit");
}

#include <QtWidgets>
#include <QtNetwork/QtNetwork>
#include <QJsonArray>
#include <iostream>
#include <QLabel>
#include <QTableWidget>


using namespace std;
using std::cout;
using std::endl;

int profitFunc(double value, double amount, int buyPrice);
void appendTableItem(QStandardItemModel twm2, string id, int amount, int buyPrice, int profit);
//void deleteTableItem(QTableWidget tw);

//https://api.coinmarketcap.com/v1/ticker/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget widget;
    widget.resize(800, 500);

    QString title = "Crypto Portfolio";
    widget.setWindowTitle(title);
    QFormLayout layout(&widget);
    QComboBox currencyDropDown;
    QDoubleSpinBox amount;
    amount.setRange(0, numeric_limits<double>::max());
    QSpinBox buyPrice;
    buyPrice.setRange(1, 1000000);
    QSpinBox profit;
    profit.setRange(numeric_limits<int>::min(), numeric_limits<int>::max());
    profit.setReadOnly(true);

    //now-value table
    QTableView tw;
    QStandardItemModel *twm = new QStandardItemModel(10, 3);
    tw.setModel(twm);

    //input table
    QTableView tw2;
    QStandardItemModel *twm2 = new QStandardItemModel(0, 4);
    tw2.setModel(twm2);

    //now-value table size
    //tw.setColumnCount(3);

    //input table size
    //tw2.setRowCount(0);
    //tw2.setColumnCount(4);

    QStringList headerList;
    headerList << "crypto" << "price" << "% change 24h";
    tw.setFixedWidth(500);
    tw2.setFixedWidth(500);
    twm->setHorizontalHeaderLabels(headerList);
    QStringList headerListInput;
    headerListInput << "crypto" << "amount" << "buyPrice" << "profit";
    twm2->setHorizontalHeaderLabels(headerListInput);


    //QVBoxLayout *Vlayout = new QVBoxLayout();

    //tw.set
    layout.addRow(&tw);
    layout.addRow(&tw2);
    //textBrowserContent.setReadOnly(true);
    layout.addRow("currency", &currencyDropDown);
    layout.addRow("amount:", &amount);
    layout.addRow("buy price", &buyPrice);
    layout.addRow("profit", &profit);
    QPushButton button("get shit");
    layout.addRow(&button);

    //denne inneholder alle json objectene
    //kalles med index, feks vm0[0] er bitcoin
    QVariantList vm0;

    QNetworkAccessManager networkManager;
    QObject::connect(&networkManager, &QNetworkAccessManager::finished,
                     [&](QNetworkReply* reply){

        if(reply->error() != QNetworkReply::NoError){
            cout << "network connection feil" << endl;
            networkManager.clearAccessCache();
        } else {

            //Hele json teksten
            QJsonArray jsonArray = QJsonDocument::fromJson(reply->readAll()).array();
            //Liste med alle array elementene
            vm0 = jsonArray.toVariantList();
            QString vm = "";
            //looper gjennom for å søke på IDen.
            for(int i = 0; i <= 10; i++){
                QJsonObject json1 = vm0[i].toJsonObject();

                currencyDropDown.addItem(json1["id"].toString());
                twm->setItem(i,0, new QStandardItem(json1["id"].toString()));
                //tw.setItem(i,0, new QTableWidgetItem(json1["id"].toString()));
                twm->setItem(i,1, new QStandardItem(json1["price_usd"].toString()));
                twm->setItem(i,2, new QStandardItem(json1["percent_change_24h"].toString()));
                //tw.itemAt(i,2)->setBackgroundColor("#ff8080");
            }
        }
        reply->deleteLater();
    });
    //query om nødvendig
    QUrlQuery query;
    //query.addQueryItem("id", "bitcoin");
    QUrl url("https://api.coinmarketcap.com/v1/ticker/?limit=11");
    //kanskje vi får bruk for det
    //url.setQuery(query);
    QNetworkRequest networkRequest(url);
    networkManager.get(networkRequest);




    //klikk knappe knapp funksjon
    QObject::connect(&button, &QPushButton::clicked, [&](){

        QJsonObject tmpObj = vm0[currencyDropDown.currentIndex()].toJsonObject();
        QString val0 = tmpObj["price_usd"].toString();
        QString valId = tmpObj["id"].toString();
        double val = val0.toDouble();

        double am = amount.value();
        int bp = buyPrice.text().toInt();
        int pro = profitFunc(val, am, bp);

        //for testing
        cout << "cur dropdown ting: " << currencyDropDown.currentIndex() << endl  << "val: " << val << endl << "amount: " << am << endl << "buyPrice: " << bp << endl << "pro output: " << pro << endl;

        //setter farge på %change
        if(pro > 0) profit.setStyleSheet("background-color: #66ff66");
        else profit.setStyleSheet("background-color: #ff8080");
        profit.setValue(pro);
        //appendTableItem(twm2, valId, am, bp, pro);


    twm2->setRowCount(twm2->rowCount()+1);
    twm2->setItem(twm2->rowCount()-1, 0, new QStandardItem(valId));
    twm2->setItem(twm2->rowCount()-1, 1, new QStandardItem(QString::number(am)));
    twm2->setItem(twm2->rowCount()-1, 2, new QStandardItem(QString::number(bp)));
    twm2->setItem(twm2->rowCount()-1, 3, new QStandardItem(QString::number(pro)));



});

    widget.show();
    return a.exec();
}

void appendTableItem(QStandardItemModel* twm2, QString id, double amount, int buyPrice, int profit){
    /*
    twm2->setRowCount(twm2->rowCount()+1);
    twm2->setItem(twm2->rowCount()-1, 0, new QStandardItem(id));
    twm2->setItem(twm2->rowCount()-1, 1, new QStandardItem(QString::number(amount)));
    twm2->setItem(twm2->rowCount()-1, 2, new QStandardItem(QString::number(buyPrice)));
    twm2->setItem(twm2->rowCount()-1, 3, new QStandardItem(QString::number(profit)));
    */
}
/*
void deleteTableItem(QTableWidget tw){

}
*/
int profitFunc(double value, double amount, int buyPrice){
    return (amount*value) - buyPrice;
}

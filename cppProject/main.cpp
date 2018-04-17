#include <QtWidgets>
#include <QtNetwork/QtNetwork>
#include <QJsonArray>
#include <iostream>
#include <QLabel>
#include <QTableWidget>


using namespace std;
using std::cout;
using std::endl;

int profitFunc(int value, double amount, int buyPrice);

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
    QTableWidget tw;

    //input table
    QTableWidget tw2;

    //now-value table size
    tw.setRowCount(10);
    tw.setColumnCount(3);

    //input table size
    tw2.setRowCount(5);
    tw2.setColumnCount(3);

    QStringList headerList;
    headerList << "crypto" << "price" << "% change 24h";
    tw.setHorizontalHeaderLabels(headerList);
    QStringList headerListInput;
    headerListInput << "crypto" << "amount" << "buyPrice";
    tw2.setHorizontalHeaderLabels(headerListInput);


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
                tw.setItem(i,0, new QTableWidgetItem(json1["id"].toString()));
                tw.setItem(i,1, new QTableWidgetItem(json1["price_usd"].toString()));
                tw.setItem(i,2, new QTableWidgetItem(json1["percent_change_24h"].toString()));
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
        int val = val0.toDouble();

        double am = amount.value();
        int bp = buyPrice.text().toInt();
        int pro = profitFunc(val, am, bp);

        cout << "cur dropdown ting: " << currencyDropDown.currentIndex() << endl  << "val: " << val << endl << "amount: " << am << endl << "buyPrice: " << bp << endl << "pro output: " << pro << endl;
        if(pro > 0) profit.setStyleSheet("background-color: #66ff66");
        else profit.setStyleSheet("background-color: #ff8080");
        profit.setValue(pro);
    });

    widget.show();
    return a.exec();
}

int profitFunc(int value, double amount, int buyPrice){
    return (amount*value) - buyPrice;
}

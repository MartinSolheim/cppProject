#include <calculations.h>

double profitCalc(double currency, double amount,int buyPrice){

    return (buyPrice*amount)-(currency*amount);
}

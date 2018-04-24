#include <calculations.h>

//Beregningsfunksjon, beregner hvor mye man har tjent siden man kjøpte det
//Får å finne fram må man ta den totale summen(Buy price) minus med
//valutaverdien(value) ganger mengden(amount)
double profitCalc(double value, double amount,int buyPrice){

    //Returnerer sum verdien(fortjenelsen)
    return (value*amount) - buyPrice;
}

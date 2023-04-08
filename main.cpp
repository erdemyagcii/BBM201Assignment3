#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;

struct Customer{
    double arrivalTime;
    double orderTime;
    double brewTime;
    double priceOfOrder;// double priceOfOrder = -1; idi sildim
    double arriveMoment;
    double coffeeDoneMoment;
    bool isJoined = false;
    Customer* prev = nullptr;//I used it for linkedlist implementation of Queue.
};

struct Cashier{
    double busyTime = 0;
    double notBusyMoment = -1;// -1 means the Cashier is available.
    Customer* customerBeingServed = nullptr;
};

struct Barista{
    double busyTime = 0;
    double notBusyMoment = -1;
    Customer* pbaristaQueue = nullptr;
    int allocatedArraySize = 16;//it is the max size of barista queue(it is important for model 2 not important for model1). Please don't change it. If you do it bigger than 16, C++'s own array sort function will give error or if you do it small enough it can give Segmentation fault
    Customer* baristaQueueArray[16];// the size of the array should be same with allocatedMemorySize.
    int baristaQueueLenght = 0;
    int baristaQueueMax = 0;
};

typedef struct Customer Customer;
typedef struct Cashier Cashier;
typedef struct Barista Barista;

bool compare( Customer* a, Customer* b) {// I used this function to sort barista queues by PriceOfOrder.
    if(a == nullptr && b != nullptr)
        return 0;
    else if(a != nullptr && b == nullptr)
        return 1;
    else if(a == nullptr && b == nullptr)
        return 1;
    else {
        if (a->priceOfOrder > b->priceOfOrder)
            return 1;
        else
            return 0;
    }
}

void Model1(string inputFilePath, string outputFilePath);
void Model2(string inputFilePath, string outputFilePath);

int main(int argc, char** argv) {
    Model1(argv[1], argv[2]);
    Model2(argv[1], argv[2]);
    return 0;
}

void Model1(string inputFilePath, string outputFilePath){
    //This is the reading input file part///////////////////////////////////////////////////////////////////////////////
    int cashierNumber;
    int baristaNumber;
    int customerNumber;

    ifstream readInput1(inputFilePath);
    ifstream readInput2(inputFilePath);
    string line = "";
    int i = 0;
    while(getline(readInput1, line)) {
        istringstream ss(line);
        string del;
        if (i < 2) {
            if (i == 0) {
                cashierNumber = atoi(line.c_str());
                baristaNumber = cashierNumber / 3;
            } else
                customerNumber = atoi(line.c_str());
            i++;
        }
    }
    readInput1.close();
    i = 0; line = "";
    Customer customers[customerNumber];
    while(getline(readInput2, line)) {
        string arr[4];
        istringstream ss(line);
        string del;
        if (i >= 2) {
            int j = 0;
            while(getline(ss, del, ' ')){
                arr[j] = del;
                j++;
            }
            customers[i - 2].arrivalTime = atof(arr[0].c_str());
            customers[i - 2].orderTime = atof(arr[1].c_str());
            customers[i - 2].brewTime = atof(arr[2].c_str());
            customers[i - 2].priceOfOrder = atof(arr[3].c_str());
        }
        i++;
    }
    readInput2.close();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //These variables are the most important variables of the program///////////////////////////////////////////////////
    double clock = 0;
    Cashier cashiers[cashierNumber];
    Barista baristas[baristaNumber];
    Customer* pcashierQueue = nullptr;
    int cashierQueueLenght = 0;
    int cashierQueueMax = 0;
    Customer* pbaristaQueue = nullptr;
    int baristaQueueLenght = 0;
    int baristaQueueMax = 0;
    Customer* baristaQueueArray[customerNumber];//it is used for sorting by money than its information transferred to barista
    for(int i = 0; i < customerNumber; i++)
        baristaQueueArray[i] = nullptr;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool customerKey = false;
    bool cashierKey = false;
    bool baristaKey = false;
    int doneCustomerNumber = -1;
    int doneCashierNumber = -1;
    int doneBaristaNumber = -1;

    while(true) {
        double minNewClockValue = clock;
        //This part used to decide how much the time goes///////////////////////////////////////////////////////////////
        for(int i = 0; i < customerNumber; i++){
            if(customers[i].isJoined == false){
                minNewClockValue = customers[i].arrivalTime;
                customers[i].isJoined = true;
                doneCustomerNumber = i;
                customerKey = true; cashierKey = false; baristaKey = false;
                break;
            }
        }
        if(customerKey == false){
            for (int i = 0; i < cashierNumber; i++) {
                if (cashiers[i].notBusyMoment != -1) {
                    minNewClockValue = cashiers[i].notBusyMoment;
                    doneCashierNumber = i;
                    cashierKey = true; customerKey = false; baristaKey = false;
                    break;
                }
            }
        }
        for (int i = 0; i < cashierNumber; i++) {
            if (cashiers[i].notBusyMoment < minNewClockValue && cashiers[i].notBusyMoment != -1) {
                minNewClockValue = cashiers[i].notBusyMoment;
                doneCashierNumber = i;
                if(doneCustomerNumber != -1)
                    customers[doneCustomerNumber].isJoined = false;
                cashierKey = true; customerKey = false; baristaKey = false;
            }
        }
        if(customerKey == false && cashierKey == false){
            for (int i = 0; i < baristaNumber; i++) {
                if (baristas[i].notBusyMoment != -1) {
                    minNewClockValue = baristas[i].notBusyMoment;
                    doneBaristaNumber = i;
                    baristaKey = true; customerKey = false; cashierKey = false;
                    break;
                }
            }
        }
        for (int i = 0; i < baristaNumber; i++) {
            if (baristas[i].notBusyMoment < minNewClockValue && baristas[i].notBusyMoment != -1) {
                minNewClockValue = baristas[i].notBusyMoment;
                doneBaristaNumber = i;
                if(doneCustomerNumber != -1)
                    customers[doneCustomerNumber].isJoined = false;
                baristaKey = true; customerKey = false; cashierKey = false;
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if(customerKey == true) {
            if (pcashierQueue == nullptr) {
                for (int i = 0; i < cashierNumber; i++) {
                    if (cashiers[i].notBusyMoment == -1) {
                        clock = minNewClockValue;
                        customers[doneCustomerNumber].arriveMoment = clock;
                        cashiers[i].busyTime += customers[doneCustomerNumber].orderTime;
                        cashiers[i].notBusyMoment = clock + customers[doneCustomerNumber].orderTime;
                        cashiers[i].customerBeingServed = &customers[doneCustomerNumber];
                        break;
                    }
                    if (i == cashierNumber - 1) {
                        pcashierQueue = &customers[doneCustomerNumber];
                        cashierQueueLenght++;
                        if (cashierQueueMax < cashierQueueLenght)
                            cashierQueueMax++;
                        clock = minNewClockValue;
                        customers[doneCustomerNumber].arriveMoment = clock;
                    }
                }
            }
            else {
                Customer *temp = pcashierQueue;
                while(temp->prev != nullptr)
                    temp = temp->prev;
                temp->prev = &customers[doneCustomerNumber];
                cashierQueueLenght++;
                if (cashierQueueMax < cashierQueueLenght)
                    cashierQueueMax++;
                clock = minNewClockValue;
                customers[doneCustomerNumber].arriveMoment = clock;
            }
        }

        else if(cashierKey == true){
            clock = minNewClockValue;
            cashiers[doneCashierNumber].notBusyMoment = -1;
            if(baristaQueueLenght == 0){
                for (int i = 0; i < baristaNumber; i++) {
                    if (baristas[i].notBusyMoment == -1) {
                        baristas[i].busyTime += cashiers[doneCashierNumber].customerBeingServed->brewTime;
                        baristas[i].notBusyMoment = clock + cashiers[doneCashierNumber].customerBeingServed->brewTime;
                        cashiers[doneCashierNumber].customerBeingServed->coffeeDoneMoment = baristas[i].notBusyMoment;
                        break;
                    }
                    if (i == baristaNumber - 1) {
                        if (pbaristaQueue == nullptr) {
                            baristaQueueArray[0] = cashiers[doneCashierNumber].customerBeingServed;
                            baristaQueueLenght++;
                        }
                        if (baristaQueueMax < baristaQueueLenght)
                            baristaQueueMax++;
                    }
                }
            }
            else {
                baristaQueueArray[baristaQueueLenght + 1] = cashiers[doneCashierNumber].customerBeingServed;
                baristaQueueLenght++;
                if (baristaQueueMax < baristaQueueLenght)
                    baristaQueueMax++;
            }
            if(pcashierQueue != nullptr){
                cashiers[doneCashierNumber].busyTime += pcashierQueue->orderTime;
                cashiers[doneCashierNumber].notBusyMoment = clock + pcashierQueue->orderTime;
                cashiers[doneCashierNumber].customerBeingServed = pcashierQueue;
                pcashierQueue = pcashierQueue->prev;
                cashierQueueLenght--;
            }
        }

        else if(baristaKey == true){
            clock = minNewClockValue;
            baristas[doneBaristaNumber].notBusyMoment = -1;
            if(baristaQueueLenght != 0){
                sort(baristaQueueArray, baristaQueueArray + customerNumber, compare);
                pbaristaQueue = baristaQueueArray[0];
                Customer* temp = baristaQueueArray[0];
                for(int i = 1; i < customerNumber; i++){
                    if(baristaQueueArray[i] == nullptr)
                        break;
                    temp->prev = baristaQueueArray[i];
                    temp = temp->prev;
                }
            }
            if(baristaQueueLenght != 0){
                baristas[doneBaristaNumber].busyTime += pbaristaQueue->brewTime;
                baristas[doneBaristaNumber].notBusyMoment = clock + pbaristaQueue->brewTime;
                pbaristaQueue->coffeeDoneMoment = baristas[doneBaristaNumber].notBusyMoment;
                pbaristaQueue = pbaristaQueue->prev;
                baristaQueueArray[0] = nullptr;
                baristaQueueLenght--;
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        if(customerKey == false && cashierKey == false && baristaKey == false)
            break;
        doneCustomerNumber = -1; doneCashierNumber = -1; doneBaristaNumber = -1;
        customerKey = false; cashierKey = false; baristaKey = false;
    }

    //This is the writing to output file part of model1///////////////////////////////////////////////////////////////////////////
    ofstream writeToOutput(outputFilePath);
    writeToOutput<<clock<<endl;
    writeToOutput<<cashierQueueMax <<endl;
    writeToOutput<<baristaQueueMax <<endl;
    for(int i = 0; i < cashierNumber; i++)
        writeToOutput<<cashiers[i].busyTime / clock <<endl;
    for(int i = 0; i < baristaNumber; i++)
        writeToOutput<<baristas[i].busyTime / clock <<endl;
    for(int i = 0; i < customerNumber; i++)
        writeToOutput<< customers[i].coffeeDoneMoment - customers[i].arriveMoment<< endl;
    writeToOutput<<endl;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


void Model2(string inputFilePath, string outputFilePath){

    //This is the reading input file part///////////////////////////////////////////////////////////////////////////////
    int cashierNumber;
    int baristaNumber;
    int customerNumber;
    ifstream readInput1(inputFilePath);
    ifstream readInput2(inputFilePath);
    string line = "";
    int i = 0;
    while(getline(readInput1, line)) {
        istringstream ss(line);
        string del;
        if (i < 2) {
            if (i == 0) {
                cashierNumber = atoi(line.c_str());
                baristaNumber = cashierNumber / 3;
            } else
                customerNumber = atoi(line.c_str());
            i++;
        }
    }
    readInput1.close();
    i = 0; line = "";
    Customer customers[customerNumber];
    while(getline(readInput2, line)) {
        string arr[4];
        istringstream ss(line);
        string del;
        if (i >= 2) {
            int j = 0;
            while(getline(ss, del, ' ')){
                arr[j] = del;
                j++;
            }
            customers[i - 2].arrivalTime = atof(arr[0].c_str());
            customers[i - 2].orderTime = atof(arr[1].c_str());
            customers[i - 2].brewTime = atof(arr[2].c_str());
            customers[i - 2].priceOfOrder = atof(arr[3].c_str());
        }
        i++;
    }
    readInput2.close();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //These variables are the most important variables of the program///////////////////////////////////////////////////
    double clock = 0;
    Cashier cashiers[cashierNumber];
    Barista baristas[baristaNumber];
    Customer* pcashierQueue = nullptr;
    int cashierQueueLenght = 0;
    int cashierQueueMax = 0;
    for(int j = 0; j < baristaNumber; j++) {
        for (int i = 0; i < baristas[0].allocatedArraySize; i++)
            baristas[j].baristaQueueArray[i] = nullptr;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool customerKey = false;//These key variables is used to decide. Which part of the coffee shop has done its own job. For example if barista finishes his job then baristakey = true others are false
    bool cashierKey = false;
    bool baristaKey = false;
    int doneCustomerNumber = -1;//I used -1 as a default value and it is useful for program's main loop.
    int doneCashierNumber = -1;
    int doneBaristaNumber = -1;

    while(true) {
        double minNewClockValue = clock;

        //This part used to decide how much the time goes///////////////////////////////////////////////////////////////
        for(int i = 0; i < customerNumber; i++){
            if(customers[i].isJoined == false){
                minNewClockValue = customers[i].arrivalTime;
                customers[i].isJoined = true;
                doneCustomerNumber = i;
                customerKey = true; cashierKey = false; baristaKey = false;
                break;
            }
        }
        if(customerKey == false){
            for (int i = 0; i < cashierNumber; i++) {
                if (cashiers[i].notBusyMoment != -1) {
                    minNewClockValue = cashiers[i].notBusyMoment;
                    doneCashierNumber = i;
                    cashierKey = true; customerKey = false; baristaKey = false;
                    break;
                }
            }
        }
        for (int i = 0; i < cashierNumber; i++) {
            if (cashiers[i].notBusyMoment < minNewClockValue && cashiers[i].notBusyMoment != -1) {
                minNewClockValue = cashiers[i].notBusyMoment;
                doneCashierNumber = i;
                if(doneCustomerNumber != -1)
                    customers[doneCustomerNumber].isJoined = false;
                cashierKey = true; customerKey = false; baristaKey = false;
            }
        }
        if(customerKey == false && cashierKey == false){
            for (int i = 0; i < baristaNumber; i++) {
                if (baristas[i].notBusyMoment != -1) {
                    minNewClockValue = baristas[i].notBusyMoment;
                    doneBaristaNumber = i;
                    baristaKey = true; customerKey = false; cashierKey = false;
                    break;
                }
            }
        }
        for (int i = 0; i < baristaNumber; i++) {
            if (baristas[i].notBusyMoment < minNewClockValue && baristas[i].notBusyMoment != -1) {
                minNewClockValue = baristas[i].notBusyMoment;
                doneBaristaNumber = i;
                if(doneCustomerNumber != -1)
                    customers[doneCustomerNumber].isJoined = false;
                baristaKey = true; customerKey = false; cashierKey = false;
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if(customerKey == true) {
            if (pcashierQueue == nullptr) {
                for (int i = 0; i < cashierNumber; i++) {
                    if (cashiers[i].notBusyMoment == -1) {
                        clock = minNewClockValue;
                        customers[doneCustomerNumber].arriveMoment = clock;
                        cashiers[i].busyTime += customers[doneCustomerNumber].orderTime;
                        cashiers[i].notBusyMoment = clock + customers[doneCustomerNumber].orderTime;
                        cashiers[i].customerBeingServed = &customers[doneCustomerNumber];
                        break;
                    }
                    if (i == cashierNumber - 1) {
                        pcashierQueue = &customers[doneCustomerNumber];
                        cashierQueueLenght++;
                        if (cashierQueueMax < cashierQueueLenght)
                            cashierQueueMax++;
                        clock = minNewClockValue;
                        customers[doneCustomerNumber].arriveMoment = clock;
                    }
                }
            }
            else {
                Customer *temp = pcashierQueue;
                while(temp->prev != nullptr)
                    temp = temp->prev;
                temp->prev = &customers[doneCustomerNumber];
                cashierQueueLenght++;
                if (cashierQueueMax < cashierQueueLenght)
                    cashierQueueMax++;
                clock = minNewClockValue;
                customers[doneCustomerNumber].arriveMoment = clock;
            }
        }

        else if(cashierKey == true){
            clock = minNewClockValue;
            cashiers[doneCashierNumber].notBusyMoment = -1;

            int whichBaristasQueue;
            if(doneCashierNumber  % 3 == 0) {
                if((doneCashierNumber / 3) == 0)
                    whichBaristasQueue = 0;
                else
                    whichBaristasQueue = (doneCashierNumber / 3);
            }
            else
                whichBaristasQueue = (doneCashierNumber / 3);

            if(baristas[whichBaristasQueue].baristaQueueLenght == 0){
                for (int i = 0; i < baristaNumber; i++) {
                    if (baristas[i].notBusyMoment == -1) {
                        baristas[i].busyTime += cashiers[doneCashierNumber].customerBeingServed->brewTime;
                        baristas[i].notBusyMoment = clock + cashiers[doneCashierNumber].customerBeingServed->brewTime;
                        cashiers[doneCashierNumber].customerBeingServed->coffeeDoneMoment = baristas[i].notBusyMoment;
                        break;
                    }
                    if (i == baristaNumber - 1) {
                        if (baristas[whichBaristasQueue].pbaristaQueue == nullptr) {
                            baristas[whichBaristasQueue].baristaQueueArray[0] = cashiers[doneCashierNumber].customerBeingServed;
                            baristas[whichBaristasQueue].baristaQueueLenght++;
                        }
                        if (baristas[whichBaristasQueue].baristaQueueMax < baristas[whichBaristasQueue].baristaQueueLenght)
                            baristas[whichBaristasQueue].baristaQueueMax++;
                    }
                }
            }
            else {
                baristas[whichBaristasQueue].baristaQueueArray[baristas[whichBaristasQueue].baristaQueueLenght +1 ] = cashiers[doneCashierNumber].customerBeingServed;
                baristas[whichBaristasQueue].baristaQueueLenght++;
                if (baristas[whichBaristasQueue].baristaQueueMax < baristas[whichBaristasQueue].baristaQueueLenght)
                    baristas[whichBaristasQueue].baristaQueueMax++;
            }
            if(pcashierQueue != nullptr){
                cashiers[doneCashierNumber].busyTime += pcashierQueue->orderTime;
                cashiers[doneCashierNumber].notBusyMoment = clock + pcashierQueue->orderTime;
                cashiers[doneCashierNumber].customerBeingServed = pcashierQueue;
                pcashierQueue = pcashierQueue->prev;
                cashierQueueLenght--;
            }
        }

        else if(baristaKey == true){
            clock = minNewClockValue;
            baristas[doneBaristaNumber].notBusyMoment = -1;
            if(baristas[doneBaristaNumber].baristaQueueLenght != 0){
                sort(baristas[doneBaristaNumber].baristaQueueArray, baristas[doneBaristaNumber].baristaQueueArray + baristas[0].allocatedArraySize, compare);
                baristas[doneBaristaNumber].pbaristaQueue = baristas[doneBaristaNumber].baristaQueueArray[0];
                Customer* temp = baristas[doneBaristaNumber].baristaQueueArray[0];
                for(int i = 1; i < baristas[0].allocatedArraySize; i++){
                    if(baristas[doneBaristaNumber].baristaQueueArray[i] == nullptr)
                        break;
                    temp->prev = baristas[doneBaristaNumber].baristaQueueArray[i];
                    temp = temp->prev;
                }
            }
            if(baristas[doneBaristaNumber].baristaQueueLenght != 0){
                baristas[doneBaristaNumber].busyTime += baristas[doneBaristaNumber].pbaristaQueue->brewTime;
                baristas[doneBaristaNumber].notBusyMoment = clock +  baristas[doneBaristaNumber].pbaristaQueue->brewTime;
                baristas[doneBaristaNumber].pbaristaQueue->coffeeDoneMoment = baristas[doneBaristaNumber].notBusyMoment;
                baristas[doneBaristaNumber].pbaristaQueue = baristas[doneBaristaNumber].pbaristaQueue->prev;
                baristas[doneBaristaNumber].baristaQueueArray[0] = nullptr;
                baristas[doneBaristaNumber].baristaQueueLenght--;
            }
        }

        if(customerKey == false && cashierKey == false && baristaKey == false)
            break;
        doneCustomerNumber = -1; doneCashierNumber = -1; doneBaristaNumber = -1;
        customerKey = false; cashierKey = false; baristaKey = false;
    }


    //This is the writing to output file part///////////////////////////////////////////////////////////////////////////
    ofstream writeToOutput;
    writeToOutput.open(outputFilePath, std::ios_base::app);
    writeToOutput<<clock<<endl;
    writeToOutput<<cashierQueueMax <<endl;
    for(int i = 0; i < baristaNumber; i++)
        writeToOutput<<baristas[i].baristaQueueMax <<endl;
    for(int i = 0; i < cashierNumber; i++)
        writeToOutput<<cashiers[i].busyTime / clock <<endl;
    for(int i = 0; i < baristaNumber; i++)
        writeToOutput<<baristas[i].busyTime / clock <<endl;
    for(int i = 0; i < customerNumber; i++)
        writeToOutput<< customers[i].coffeeDoneMoment - customers[i].arriveMoment<< endl;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}





























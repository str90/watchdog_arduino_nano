#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialPort.h"
#include <fstream>

using namespace std;
using std::cout;
using std::endl;

int connEstNum = 1986;
char sendMessage[] = "PC";
char aliveMessage[] = "AL";
int negFlag = 0;
string comPort;

int main() {
    cout << "Watchdog host program version 0.1" << endl;
    printf("\n");
    printf("Enter watchdog com port number (example: COM12) : ");
    printf("\n");

    ifstream configFileRead("config.cnf");
    if(!configFileRead) {
        configFileRead.close();
        ofstream configFileWrite("config.cnf");
        int flag = 0;
        while(flag == 0) {
            cin >> comPort;
            if(comPort.length() > 3 && comPort.length() < 6) {
                flag = 1;
                configFileWrite << comPort;
            }
            else printf("Input not valid, try again\nEnter watchdog com port number (example: COM12) : \n");
        }
        configFileWrite.close();
    }
    else {
        configFileRead >> comPort;
        configFileRead.close();
    }

    char c_string_buffer[comPort.length() + 8] = "\\\\.\\";
    strcat(c_string_buffer, comPort.c_str());

    printf("Attempt to find watchdog stick... ");
    SerialPort uartInstance(c_string_buffer);
    while(negFlag == 0) {
        if(uartInstance.isConnected()) {
            printf(" watchdog connection negotiated\n");
            uartInstance.writeSerialPort(sendMessage, MAX_DATA_LENGTH);
            negFlag = 1;
        }
    } 

  /*  printf("Attempt to detect watchdog... ");
    int findFlag = 0;
    for(int i = 0; i < 254; i++) {
        char incomingData[MAX_DATA_LENGTH];
        char comPortNumber = i;
        char com_port_buffer[i + 10] = "\\\\.\\COM";
        strcat(c_string_buffer, comPortNumber);
        SerialPort uartInstance(com_port_buffer);
        if(uartInstance) if(uartInstance.isConnected()) if(uartInstance.readSerialPort(incomingData, MAX_DATA_LENGTH) == 1986) break;
        delete uartInstance;
    } */

    printf("\nWatchdog enabled successfully");
    Sleep(2500);
    while(true) {
        uartInstance.writeSerialPort(aliveMessage, MAX_DATA_LENGTH);
        printf("\nHost is alive");
        Sleep(2500);
    } 
    return 0;
}

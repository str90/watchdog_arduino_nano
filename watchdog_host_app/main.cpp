#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialPort.h"
#include <fstream>

using namespace std;
using std::cout;
using std::endl;

char sendMessage[] = "PC";
char aliveMessage[] = "AL";
string comPort;
int negFlag = 0;

void removeChars(char *sanitazible) {
    int i, j;
    for(i=0; sanitazible[i]!='\0'; ++i)
    {
        while (!((sanitazible[i]< 1) || (sanitazible[i] > 5 || sanitazible[i]=='\0')))
        {
            for(j=i;sanitazible[j]!='\0';++j)
            {
                sanitazible[j]=sanitazible[j+1];
            }
            sanitazible[j]='\0';
        }
    }
}

int main() {
    printf("Watchdog host program version 0.1\n\n");

    ifstream configFileRead("config.cnf");
    if(!configFileRead) {
        configFileRead.close();
        printf("Attempt to detect watchdog... ");
        int findFlag = 0;
        while(findFlag == 0) {
            for(int i = 1; i < 254; i++) {
            int portWide = 0;
            int portIterator = i;
            while(portIterator > 0) {
                portWide++;
                portIterator /= 10;
            }
            char com_port_buffer[portWide + 10] = "\\\\.\\COM";
            char port_num_buffer[portWide];
            char incomingData[5] = " ";
            char messageEtalon[] = "1460";
            sprintf(port_num_buffer, "%d", i);
            strcat(com_port_buffer, port_num_buffer);
            SerialPort findInstance(com_port_buffer);
            int hasRead = findInstance.readSerialPort(incomingData, 5);
            removeChars(incomingData);
            if(hasRead > 0) if(strcmp(incomingData, messageEtalon) == 0) {
                printf("Successfully found watchdog connected to COM%d\n", i);
                findFlag = 1;
                ofstream configFileWrite("config.cnf");
                configFileWrite << com_port_buffer;
                configFileWrite.close();
                comPort = string(com_port_buffer);
                break;
            }
            if(hasRead > 0) puts(incomingData);
        }
    }
    }
    else {
        configFileRead >> comPort;
        configFileRead.close();
    }

    char c_string_buffer[comPort.length() + 8] = "\\\\.\\";
    strcat(c_string_buffer, comPort.c_str());

    printf("Connecting to watchdog stick... ");
    SerialPort uartInstance(c_string_buffer);
    while(negFlag == 0) {
        if(uartInstance.isConnected()) {
            printf(" watchdog connection negotiated\n");
            uartInstance.writeSerialPort(sendMessage, MAX_DATA_LENGTH);
            negFlag = 1;
        }
    }


    printf("\nWatchdog enabled successfully\n");
    Sleep(2500);
    while(true) {
        uartInstance.writeSerialPort(aliveMessage, MAX_DATA_LENGTH);
        printf("\nHost is alive");
        Sleep(2500);
    }

    return 0;
}

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "SerialPort.h"
#include <fstream>
#include <strsafe.h>

using namespace std;
using std::cout;
using std::endl;

char sendMessage[] = "PC";
char aliveMessage[] = "AL";
string comPort;
int negFlag = 0;

void removeChars(char *sanitazible) {
	int i, j;
	for (i = 0; sanitazible[i] != '\0'; ++i)
	{
		while (!((sanitazible[i]< 1) || (sanitazible[i] > 5 || sanitazible[i] == '\0')))
		{
			for (j = i; sanitazible[j] != '\0'; ++j)
			{
				sanitazible[j] = sanitazible[j + 1];
			}
			sanitazible[j] = '\0';
		}
	}
}

void detectDevice(string &comPortString) {
	printf("Attempt to detect watchdog... ");
	int findFlag = 0;
	for (int i = 1; i < 254; i++) {
		int portWide = 0;
		int portIterator = i;
		while (portIterator > 0) {
			portWide++;
			portIterator /= 10;
		}
		char incomingData[MAX_DATA_LENGTH] = " ";
		char messageEtalon[] = "1460";
		string com_result = ("\\\\.\\COM" + to_string(i));
		char *com_cstring = new char[com_result.length() + 1];
		strcpy(com_cstring, com_result.c_str());
		SerialPort findInstance(com_cstring);
		int hasRead = findInstance.readSerialPort(incomingData, 5);
		removeChars(incomingData);
		if (hasRead > 0) if (strcmp(incomingData, messageEtalon) == 0) {
			printf("Successfully found watchdog connected to COM%d\n", i);
			findFlag = 1;
			ifstream configFileRead("config.cnf");
			if (configFileRead) {
				configFileRead.close();
				remove("config.cnf");
			}
			ofstream configFileWrite("config.cnf");
			configFileWrite << com_result;
			configFileWrite.close();
			comPortString = com_result;
		}
	}
}

int main() {
	printf("Watchdog host program version 0.2\n\n");

	ifstream configFileRead("config.cnf");
	if (!configFileRead) {
		configFileRead.close();
		detectDevice(comPort);
	}
	else {
		configFileRead >> comPort;
		configFileRead.close();
	}

	printf("Connecting to watchdog stick... ");
	while (negFlag == 0) {
		char *com_cstring = new char[comPort.length()];
		strcpy(com_cstring, comPort.c_str());
		SerialPort uartInstance(com_cstring);
		if (uartInstance.isConnected()) {
			printf(" watchdog connection negotiated\n");
			uartInstance.writeSerialPort(sendMessage, MAX_DATA_LENGTH);
			negFlag = 1;
		}
		else detectDevice(comPort);
	}


	printf("\nWatchdog enabled successfully\n");
	Sleep(2500);
	char *com_cstring = new char[comPort.length()];
	strcpy(com_cstring, comPort.c_str());
	SerialPort uartInstance(com_cstring);
	while (true) {
		uartInstance.writeSerialPort(aliveMessage, MAX_DATA_LENGTH);
		printf("\nHost is alive");
		Sleep(2500);
	}
	return 0;
}

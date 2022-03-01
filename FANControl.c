#include <stdio.h>
#include <reg52.h>

#define PWMMAX 99

sbit FAN1 = P0^0;
sbit FAN2 = P0^1;
sbit FAN3 = P0^2;
sbit FAN4 = P0^3;
sbit FAN5 = P0^4;
sbit FAN6 = P0^5;
sbit FAN7 = P0^6;
sbit FAN8 = P0^7;
sbit FAN9 = P2^4;
sbit FAN10 = P2^5;
sbit FAN11 = P2^6;
sbit FAN12 = P2^7;

sbit P35 = P3^5;

sbit P32 = P3^2;
sbit dataPin = P3^7;

bit Reset,StartUpStatus = 1,ESStatus; //Startup
unsigned char startUpCount,startUpCountx2,startUpCountx3,startUpCountx4;
unsigned char debugLevel,ESC = 0; //Config
unsigned char FAN[12]; //FAN
unsigned char input[16],inputCache[2],inputBit = 0,inputCacheCount = 0,currorBit = 0; //input
unsigned char getBit,getBitCount,getData[16],getDataCount; //GetData
unsigned char sendDataCache[16],sendCount,sendBitCount,sendDataControl; //SendData
unsigned char globalCount; //Counts

void InitInterrupt(){
    PS = 1;
    EA = 1;
    EX0 = 1;
    IT0 = 1;
}

void InitTimer(){
    TMOD = 0x22;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TH0 = 0xFD;
    TL0 = 0xFD;
    TR1 = 1;
    TR0 = 1;
}

void InitSerial(){
    PCON = 0x80;
    SCON = 0x50;
}

void Init(){
    P0 = 0;
    P1 = 0;
    P2 = 0;
    P3 = 0x3F;
    dataPin = 1;
    InitInterrupt();
    InitTimer();
    InitSerial();
}

void StartUp(){
    ES = 1;
}

void SendSerialData(unsigned char hex){
    ESStatus = ES;
    ES = 0;
    SBUF = hex;
    while(!TI);
    TI = 0;
    if(ESStatus) ES = 1;
}

void print(unsigned char *string){
    while(*string != '\0'){
        SendSerialData(*string);
        string++;
    }
}

char putchar(char inChar){
    SendSerialData(inChar);
    return 0;
}

bit IsNum(unsigned char inChar){
    if(inChar >= 48 && inChar <= 57) return 1;
    else return 0;
}

unsigned char CharToDec(unsigned char inChar){
    if(inChar >= 48 && inChar <= 57){
        return inChar - 48;
    }else print("Input range error. (0-9)\r\n");
    return 0;
}

void PWM(){
    unsigned char num,FANList = 1,FAN2List = 1,FANNum;
    while(1){
        for(;FANNum < 12;FANNum++){
            if(FANNum < 8){
                if(PWMMAX - FAN[FANNum] == num && FAN[FANNum] > 0) P1 = P1 | FANList;
                if(num == PWMMAX && FAN[FANNum] < PWMMAX) P1 = P1 & ~FANList;
                FANList *= 2;
            }
            if(FANNum >= 8){
                if(PWMMAX - FAN[FANNum] == num && FAN[FANNum] > 0) P2 = P2 | FAN2List;
                if(num == PWMMAX && FAN[FANNum] < PWMMAX) P2 = P2 & ~FAN2List;
                FAN2List *= 2;
            }
        }
        FANList = 1;
        FAN2List = 1;
        FANNum = 0;
        num++;
        if(num > PWMMAX) num = 0;
        if(Reset == 1) break;
    }
}

void main(){
    Init();
    print("\r\n");
    print("Loading...\r\n");
    if(P35 == 0) StartUpStatus = 0;
    else ET0 = 1;
    while(StartUpStatus);
    StartUp();
    print("Finish.\r\n");
    print("root@C51:~$ ");
    PWM();
    print("Shutting Down...\r\n");
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void ExecData(){
    if(getData[0] == 'f' && getData[1] == 'a' && getData[2] == 'n' && getData[3] == 0x20) print("FAN!\r\n");
}

void StoreData(){
    getData[getDataCount] = getBit;
    getDataCount++;
    if(getDataCount >= 16){
        ExecData();
        getDataCount = 0;
    }
}

void ReciveData() interrupt 0{
    if(dataPin == 1) getBit++;
    getBitCount++;
    if(getBitCount >= 8){
        StoreData();
        getBit = 0;
        getBitCount = 0;
    }
    getBit *= 2;
}

void Timer0() interrupt 1{
    if(sendDataControl == 1){
        EX0 = 0;
        if(sendDataCache[sendCount] & 0x80) dataPin = 1;
        else dataPin = 0;
        P32 = 0;
        P32 = 1;
        sendBitCount++;
        sendDataCache[sendCount] *= 2;
        if(sendBitCount >= 8){
            sendCount++;
            sendBitCount = 0;
        }
        if(sendCount >= 16){
            sendCount = 0;
            sendDataControl = 0;
            dataPin = 1;
            ET0 = 0;
        }
        EX0 = 1;
    }
    if(StartUpStatus){
        switch(startUpCountx4){
            case 1:
                if(FAN1 == 0){
                    FAN1 = 1;
                    print("FAN 1 Started.\r\n");
                }
                break;
            case 2:
                if(FAN2 == 0){
                    FAN2 = 1;
                    print("FAN 2 Started.\r\n");
                }
                break;
            case 3:
                if(FAN3 == 0){
                    FAN3 = 1;
                    print("FAN 3 Started.\r\n");
                }
                break;
            case 4:
                if(FAN4 == 0){
                    FAN4 = 1;
                    print("FAN 4 Started.\r\n");
                }
                break;
            case 5:
                if(FAN5 == 0){
                    FAN5 = 1;
                    print("FAN 5 Started.\r\n");
                }
                break;
            case 6:
                if(FAN6 == 0){
                    FAN6 = 1;
                    print("FAN 6 Started.\r\n");
                }
                break;
            case 7:
                if(FAN7 == 0){
                    FAN7 = 1;
                    print("FAN 7 Started.\r\n");
                }
                break;
            case 8:
                if(FAN8 == 0){
                    FAN8 = 1;
                    print("FAN 8 Started.\r\n");
                }
                break;
            case 9:
                if(FAN9 == 0){
                    FAN9 = 1;
                    print("FAN 9 Started.\r\n");
                }
                break;
            case 10:
                if(FAN10 == 0){
                    FAN10 = 1;
                    print("FAN 10 Started.\r\n");
                }
                break;
            case 11:
                if(FAN11 == 0){
                    FAN11 = 1;
                    print("FAN 11 Started.\r\n");
                }
                break;
            case 12:
                if(FAN12 == 0){
                    FAN12 = 1;
                    print("FAN 12 Started.\r\n");
                }
                StartUpStatus = 0;
                ET0 = 0;
                break;
        }
        if(startUpCount == 255){
            startUpCountx2++;
        }
        if(startUpCountx2 == 255){
            startUpCountx2 = 0;
            startUpCountx3++;
        }
        if(startUpCountx3 == 2){
            startUpCountx3 = 0;
            startUpCountx4++;
        }
        startUpCount++;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void SendData(){
    int count = 5,sendCountLocal = 0;
    for(;count < 16;count++,sendCountLocal++) sendDataCache[sendCountLocal] = input[count];
    ET0 = 1;
    sendDataControl = 1;
}

void Input(unsigned char inChar){
    input[currorBit] = inChar;
    SendSerialData(inChar);
    inputBit++;
    currorBit++;
}

void Insert(unsigned char inChar){
    unsigned char count = inputBit;
    for(;count > currorBit;count--){
        input[count] = input[count - 1];
        input[count - 1] = inChar;
    }
    for(count = currorBit;count <= inputBit;count++){
        SendSerialData(input[count]);
    }
    for(count = currorBit;count < inputBit;count++){
        SendSerialData(0x08);
    }
    inputBit++;
    currorBit++;
}

void Backspace(){
    unsigned char count = currorBit;
    if(currorBit == inputBit){
        SendSerialData(0x08);
        SendSerialData(0x20);
        SendSerialData(0x08);
        input[currorBit - 1] = 0;
    }else if(count < inputBit){
        SendSerialData(0x08);
        SendSerialData(0x20);
        SendSerialData(0x08);
        for(;count < inputBit;count++){
            input[count - 1] = input[count];
            SendSerialData(input[count]);
        }
        for(;count <= 16;count++) input[count - 1] = 0;
        SendSerialData(0x20);
        for(count = currorBit - 1;count < inputBit;count++){
            SendSerialData(0x08);
        }
    }
    inputBit--;
    currorBit--;
}

void Enter(){
    unsigned char count = 0;
    print("\r\n");
    if(input[0] != 0x00){
        if(input[0] == 'r' && input[1] == 'e' && input[2] == 'b' && input[3] == 'o' && input[4] == 'o' && input[5] == 't'){ //reboot
            Reset = 1;
        }else if(input[0] == 'e' && input[1] == 'c' && input[2] == 'h' && input[3] == 'o'){ //echo
            for(count = 5;count < inputBit;count++) SendSerialData(input[count]);
        }else if(input[0] == 's' && input[1] == 'e' && input[2] == 'n' && input[3] == 'd'){ //send
            SendData();
            for(count = 5;count < inputBit;count++) SendSerialData(input[count]);
        }else if(input[0] == 'f' && input[1] == 'a' && input[2] == 'n'){ //fan
            if(input[3] == 0x20){
                if(IsNum(input[4]) && input[5] == 0x20 && IsNum(input[6])){ //<10
                    if(input[4] > '0'){
                        if(IsNum(input[8])){
                            FAN[CharToDec(input[4]) - 1] = (CharToDec(input[6]) * 10 + CharToDec(input[7])) * 10 + CharToDec(input[8]);
                            print("Set FAN ");
                            SendSerialData(input[4]);
                            print(" speed: ");
                            SendSerialData(input[6]);
                            SendSerialData(input[7]);
                            SendSerialData(input[8]);
                        }else if(IsNum(input[7])){
                            FAN[CharToDec(input[4]) - 1] = CharToDec(input[6]) * 10 + CharToDec(input[7]);
                            print("Set FAN ");
                            SendSerialData(input[4]);
                            print(" speed: ");
                            SendSerialData(input[6]);
                            SendSerialData(input[7]);
                        }else{
                            FAN[CharToDec(input[4]) - 1] = CharToDec(input[6]);
                            print("Set FAN ");
                            SendSerialData(input[4]);
                            print(" speed: ");
                            SendSerialData(input[6]);
                        }
                    }else print("FAN ID Error.");
                }else if(IsNum(input[4]) && IsNum(input[5]) && input[6] == 0x20 && IsNum(input[7])){ //>10
                    if(CharToDec(input[4]) * 10 + CharToDec(input[5]) > 0 && CharToDec(input[4]) * 10 + CharToDec(input[5]) <= 12){
                        if(IsNum(input[9])){
                            FAN[CharToDec(input[4]) * 10 + CharToDec(input[5]) - 1] = (CharToDec(input[7]) * 10 + CharToDec(input[8])) * 10 + CharToDec(input[9]);
                            print("Set FAN ");
                            SendSerialData(input[4]);
                            SendSerialData(input[5]);
                            print(" speed: ");
                            SendSerialData(input[7]);
                            SendSerialData(input[8]);
                            SendSerialData(input[9]);
                        }else if(IsNum(input[8])){
                            FAN[CharToDec(input[4]) * 10 + CharToDec(input[5]) - 1] = CharToDec(input[7]) * 10 + CharToDec(input[8]);
                            print("Set FAN ");
                            SendSerialData(input[4]);
                            SendSerialData(input[5]);
                            print(" speed: ");
                            SendSerialData(input[7]);
                            SendSerialData(input[8]);
                        }else{
                            FAN[CharToDec(input[4]) * 10 + CharToDec(input[5]) - 1] = CharToDec(input[7]);
                            print("Set FAN ");
                            SendSerialData(input[4]);
                            SendSerialData(input[5]);
                            print(" speed: ");
                            SendSerialData(input[7]);
                        }
                    }else print("FAN ID Error.");
                }else print("Error.");
            }
        }else{ // Unknow Command
            print("Unknow command:");
            while(count < 16 && input[count] != 0x20 && input[count] != 0x00){
                SendSerialData(input[count]),count++;
            }
            print(".");
        }
        print("\r\n");
        print("root@C51:~$ ");
    }else print("root@C51:~$ ");
    for(count = 0;count < 15;count++) input[count] = 0;
    inputBit = 0;
    currorBit = 0;
}

void SerialInterrupt() interrupt 4{
    unsigned char receiveData;
    receiveData = SBUF;
    RI = 0;
    if(ESC == 0){
        if(receiveData == 0x0D){ // Enter
            Enter();
        }else if(receiveData == 0x1B){ //0x1B
            ESC = 1;
        }else if(receiveData == 0x08 || receiveData == 0x7F){ //backspace
            if(currorBit > 0) Backspace();
        }else{
            if(inputBit <= 15 && currorBit == inputBit) Input(receiveData); //Input
            else if(inputBit <= 15 && currorBit < inputBit) Insert(receiveData); //Insert
        }
    }else if(ESC == 1 && receiveData != 0x5B){ //0x1B
        SendSerialData(receiveData);
        ESC = 0;
        inputBit++;
        currorBit++;
    }else if(ESC == 1 || ESC == 2){ //0x1B
        inputCache[inputCacheCount] = receiveData;
        if(ESC == 2){
            if(inputCache[1] == 0x44 && currorBit > 0){
                SendSerialData(0x1B);
                SendSerialData(inputCache[0]);
                SendSerialData(inputCache[1]);
                currorBit--;
            }else if(inputCache[1] == 0x43 && currorBit < inputBit){
                SendSerialData(0x1B);
                SendSerialData(inputCache[0]);
                SendSerialData(inputCache[1]);
                if(currorBit < inputBit) currorBit++;
            }else if(inputCache[1] != 0x41 && inputCache[1] != 0x42 && inputCache[1] != 0x43 && inputCache[1] != 0x44){
                SendSerialData(inputCache[1]);
                inputBit++;
                currorBit++;
            }
        }
        inputCacheCount++;
        ESC++;
        if(inputCache[1] == 0x44 || inputCache[1] == 0x43 || inputCache[1] == 0x42) inputCache[1] = 0;
        if(ESC > 2) ESC = 0;
        if(inputCacheCount > 1) inputCacheCount = 0;
    }
}

//Hash_Define

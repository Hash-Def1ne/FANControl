#include <stdio.h>
#include <reg52.h>

sbit P32 = P3^2;
sbit dataPin = P3^7;

unsigned char getBit,getBitCount,getData[16],getDataCount;
unsigned char sendDataCache[16],sendCount,sendBitCount,sendDataControl; //SendData

void InitInterrupt(){
    EA = 1;
    EX0 = 1;
    IT0 = 1;
}

void InitTimer(){
    TMOD = 0x02;
    TH0 = 0xFD;
    TL0 = 0xFD;
    TR0 = 1;
}

void Init(){
    P0 = 0;
    P1 = 0;
    P2 = 0;
    P3 = 0x0C;
    dataPin = 1;
    InitInterrupt();
    initTimer();
}

void SendData(){
    ET0 = 1;
    sendDataControl = 1;
}

void main(){
    Init();
    while(1);
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

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
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void GetFanSpeed(){
    P1 = 255;
    sendDataCache[0] = 'f';
    sendDataCache[1] = 'a';
    sendDataCache[2] = 'n';
    sendDataCache[3] = 0x20;
    SendData();
}

void ExecData(){
    if(getData[0] == 'g' && getData[1] == 'e' && getData[2] == 't' && getData[3] == 'f' && getData[4] == 'a' && getData[5] == 'n' && getData[6] == 0) GetFanSpeed();
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

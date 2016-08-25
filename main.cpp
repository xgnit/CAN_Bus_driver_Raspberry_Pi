 #include"sendDriver.h"
#include<QDebug>
#include<QString>

int main(int argc, char *argv[])
{
    struct _Msg{
        uint32_t canId;
        uchar bytes[8];
    }msgToSend;

    msgToSend.canId = 0;
    msgToSend.bytes[0] = 1;
    msgToSend.bytes[1] = 2;
    msgToSend.bytes[2] = 3;
    msgToSend.bytes[3] = 4;
    msgToSend.bytes[4] = 5;
    msgToSend.bytes[5] = 6;
    msgToSend.bytes[6] = 7;
    msgToSend.bytes[7] = 8;

    std::string deviceName("can0");
    sendDriver _driver(deviceName);
    QString res = _driver.sendByte(msgToSend.canId,msgToSend.bytes);
    //the returned value designates the state of the driver
    qDebug()<<res;

}


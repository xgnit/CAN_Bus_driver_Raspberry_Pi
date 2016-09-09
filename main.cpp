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
    for(int i=0;i<8;i++)
    msgToSend.bytes[i] = i;
    
    std::string deviceName("can0");
    sendDriver _driver(deviceName);
    QString res = _driver.sendByte(msgToSend.canId,msgToSend.bytes);
    //the returned value designates the state of the driver
    qDebug()<<res;

}


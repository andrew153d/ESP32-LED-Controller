#ifndef UDPHANDLER_H
#define UDPHANDLER_H

#include <Arduino.h>
#include <esp_wifi.h>
#include <AsyncUdp.h>
#include <string.h>
#include "config.h"
#include "string.h"
#include <sstream>
#include <iostream>
#include <cstdint>

class UDPHandler
{
private:
    AsyncUDP udp;
    int *time;
    int lastMsgTime;
    int senderPort;

public:
    bool isInitialized;

    UDPHandler(int i);
    void init(int *time);
    void sendUDPevery(const char *data, int msec);
    void sendUDP(const char *data);
    void sendUDP(int *data);
};

extern UDPHandler UDP;

#endif
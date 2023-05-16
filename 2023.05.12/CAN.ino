#include <mcp_can.h>
#include <SPI.h>

long unsigned int txId = 0x00;
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

MCP_CAN CAN0(10);                               // Set CS to pin 10

void setup()
{
    Serial.begin(115200);
    if(CAN0.begin(CAN_500KBPS,MCP_8MHz) == CAN_OK) Serial.print("can init ok!!\r\n");
    else Serial.print("Can init fail!!\r\n");
    pinMode(2, INPUT_PULLUP);                  // Set pin 2 as input with pull-up
}

void loop()
{
    // Send data
    CAN0.sendMsgBuf(txId, 0, 8, stmp);
    delay(1000);                                // send data per 1000ms

    // Receive data
    if(!digitalRead(2))                          // If pin 2 is low, read receive buffer
    {
        CAN0.readMsgBuf(&len, rxBuf);           // Read data: len = data length, buf = data byte(s)
        rxId = CAN0.getCanId();                 // Get message ID

        Serial.print("ID: ");
        Serial.print(rxId, HEX);
        Serial.print("  Data: ");

        for(int i = 0; i<len; i++)              // Print each byte of the data
        {
            Serial.print(rxBuf[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}

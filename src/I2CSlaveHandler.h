#ifndef __POKEROPO_I2C_SLAVE_HANDLER_H__
#define __POKEROPO_I2C_SLAVE_HANDLER_H__

#include <Arduino.h>

class I2CSlaveInterface {
  public:
    virtual void processRxData(const void* rxBuffer, const int rxBufferSize);
    virtual void prepareTxData(const void* txBuffer, const int txBufferSize);
};

class I2CSlaveHandler: public I2CSlaveInterface {
  public:
    I2CSlaveHandler(void* rxBuffer, int rxBufferSize, void* txBuffer, int txBufferSize, I2CSlaveInterface* ioProcessor=NULL);
    void begin(byte slaveAddress, void (*receiveEvent)(int), void (*requestEvent)(void));
    void next();
    void receiveEvent(int numBytesReceived);
    void requestEvent();
    void reset();
  protected:
    void setTxBuffer(void* txBuffer, int txBufferSize);
    void setRxBuffer(void* rxBuffer, int rxBufferSize);
    void set(I2CSlaveInterface* ioProcessor);
    void processRxData(const void* rxBuffer, const int rxBufferSize);
    void prepareTxData(const void* txBuffer, const int txBufferSize);
    virtual void debugReset_();
  private:
    I2CSlaveInterface* _ioProcessor = NULL;
    void* _txBuffer = NULL;
    int _txBufferSize = 0;
    void* _rxBuffer = NULL;
    int _rxBufferSize = 0;
    bool _txDataReady = false;
    bool _rxDataReady = false;
    bool _requestSent = false;
};

#endif

#ifndef __POKEROPO_I2C_MASTER_HANDLER_H__
#define __POKEROPO_I2C_MASTER_HANDLER_H__

#include <Arduino.h>

class I2CMasterInterface {
  public:
    virtual void prepareTxData(const void* txBuffer, const int txBufferSize);
    virtual void processRxData(const void* rxBuffer, const int rxBufferSize);
};

class I2CMasterHandler: public I2CMasterInterface {
  public:
    I2CMasterHandler(void* rxBuffer, int rxBufferSize, void* txBuffer, int txBufferSize,
        I2CMasterInterface* ioProcessor=NULL);
    void begin(byte masterAddress);
    void call(byte slaveAddress);
    int8_t transmit(byte slaveAddress, void* buffer, int bufferSize);
    int8_t request(byte slaveAddress, void* buffer, int bufferSize);
  protected:
    void prepareTxData(const void* txBuffer, const int txBufferSize);
    void processRxData(const void* rxBuffer, const int rxBufferSize);
    int8_t transmit(byte slaveAddress);
    int8_t request(byte slaveAddress);
  private:
    I2CMasterInterface* _ioProcessor = NULL;
    void* _txBuffer = NULL;
    int _txBufferSize = 0;
    void* _rxBuffer = NULL;
    int _rxBufferSize = 0;
    bool _txDataReady = false;
    bool _rxDataReady = false;
    bool _rqDataReady = false;
    unsigned long _updateInterval = 100;
    unsigned long _prevUpdateTime = 0;
};

#endif

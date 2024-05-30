#include "I2CSlaveHandler.h"
#include <Wire.h>

I2CSlaveHandler::I2CSlaveHandler(void* rxBuffer, int rxBufferSize,
    void* txBuffer, int txBufferSize,
    I2CSlaveInterface* ioProcessor) {
  setRxBuffer(rxBuffer, rxBufferSize);
  setTxBuffer(txBuffer, txBufferSize);
  set(ioProcessor);
}

void I2CSlaveHandler::setTxBuffer(void* txBuffer, int txBufferSize) {
  _txBuffer = txBuffer;
  _txBufferSize = txBufferSize;
}

void I2CSlaveHandler::setRxBuffer(void* rxBuffer, int rxBufferSize) {
  _rxBuffer = rxBuffer;
  _rxBufferSize = rxBufferSize;
}

void I2CSlaveHandler::set(I2CSlaveInterface* ioProcessor) {
  _ioProcessor = ioProcessor;
}

void I2CSlaveHandler::begin(byte slaveAddress, void (*receiveEvent)(int), void (*requestEvent)(void)) {
  Wire.begin(slaveAddress);

  if (receiveEvent != NULL) {
    // register function to be called when a message arrives
    Wire.onReceive(receiveEvent);
  }

  if (requestEvent != NULL) {
    // register function to be called when a request arrives
    Wire.onRequest(requestEvent);
  }
}

void I2CSlaveHandler::reset() {
  debugReset_();
  _txDataReady = false;
  _rxDataReady = false;
  _requestSent = false;
}

void I2CSlaveHandler::next() {
  // checks if a message has been received
  if (_rxDataReady == true) {
    processRxData(_rxBuffer, _rxBufferSize);
    _rxDataReady = false;
  }

  // prepares the data to response
  if (_requestSent == true) {
    _requestSent = false;
    prepareTxData(_txBuffer, _txBufferSize);
  }
}

void I2CSlaveHandler::processRxData(const void* rxBuffer, const int rxBufferSize) {
  if (_ioProcessor != NULL) {
    _ioProcessor->processRxData(rxBuffer, rxBufferSize);
  }
}

void I2CSlaveHandler::prepareTxData(const void* txBuffer, const int txBufferSize) {
  if (_ioProcessor != NULL) {
    _ioProcessor->prepareTxData(txBuffer, txBufferSize);
  }
}

void I2CSlaveHandler::receiveEvent(int numBytesReceived) {
  if (_rxDataReady == false) {
    // copy the data to the buffer
    Wire.readBytes((byte*)_rxBuffer, numBytesReceived);
    _rxDataReady = true;
  } else {
    // dump the data
    while(Wire.available() > 0) {
        byte c = Wire.read();
    }
  }
}

void I2CSlaveHandler::requestEvent() {
  if (_txBuffer != NULL && _txBufferSize > 0) {
    Wire.write((byte*)_txBuffer, _txBufferSize);
  }
  _requestSent = true;
}

void I2CSlaveHandler::debugReset_() {
  Serial.println("Reset the I2CSlaveHandler instance");
}

#include "I2CMasterHandler.h"
#include <Wire.h>

I2CMasterHandler::I2CMasterHandler(void* txBuffer, int txBufferSize, void* rxBuffer, int rxBufferSize,
    I2CMasterInterface* ioProcessor) {
  _txBuffer = txBuffer;
  _txBufferSize = txBufferSize;
  _rxBuffer = rxBuffer;
  _rxBufferSize = rxBufferSize;
  _ioProcessor = ioProcessor;
}

void I2CMasterHandler::begin(byte masterAddress) {
  Wire.begin(masterAddress);
}

void I2CMasterHandler::call(byte slaveAddress) {
  // checks if a message has been received
  if (_rxDataReady == true) {
      processRxData(_rxBuffer, _rxBufferSize);
      _rxDataReady = false;
  }

  // updates the data in txDataBuffer
  if (millis() - _prevUpdateTime >= _updateInterval) {
    _prevUpdateTime = millis();
    if (_txDataReady == false) { // ensure previous message has been sent
      prepareTxData(_txBuffer, _txBufferSize);
      _txDataReady = true;
    }
  }

  // sends the data if it is ready to be sent
  if (_txDataReady == true) {
      transmit(slaveAddress);
      _txDataReady = false;
      _rqDataReady = true;
  }

  // just one request following every transmission
  if (_rqDataReady == true) {
    request(slaveAddress);
    _rxDataReady = true;
    _rqDataReady = false;
  }
}

void I2CMasterHandler::processRxData(const void* rxBuffer, const int rxBufferSize) {
  if (_ioProcessor != NULL) {
    _ioProcessor->processRxData(rxBuffer, rxBufferSize);
  }
}

void I2CMasterHandler::prepareTxData(const void* txBuffer, const int txBufferSize) {
  if (_ioProcessor != NULL) {
    _ioProcessor->prepareTxData(txBuffer, txBufferSize);
  }
}

int8_t I2CMasterHandler::transmit(byte slaveAddress) {
  return transmit(slaveAddress, _rxBuffer, _rxBufferSize);
}

int8_t I2CMasterHandler::transmit(byte slaveAddress, void* buffer, int bufferSize) {
  Wire.beginTransmission(slaveAddress);
  Wire.write((byte*)buffer, bufferSize);
  return Wire.endTransmission(); // actually sends the data
}

int8_t I2CMasterHandler::request(byte slaveAddress) {
  return request(slaveAddress, _rxBuffer, _rxBufferSize);
}

int8_t I2CMasterHandler::request(byte slaveAddress, void* buffer, int bufferSize) {
  size_t numBytes = bufferSize;
  // the request is immediately followed by the read for the response
  size_t numBytesRequested = Wire.requestFrom(slaveAddress, numBytes);
  size_t numBytesReceived = Wire.readBytes((byte*)buffer, numBytes);
  return numBytesReceived - numBytes;
}

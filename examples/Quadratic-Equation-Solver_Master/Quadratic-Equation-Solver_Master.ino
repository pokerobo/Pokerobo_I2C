#include "Pokerobo_I2C.h"

struct EquationDefinition {
  uint32_t no;              //  4 byte(s)
  float a;                  //  4
  float b;                  //  4
  float c;                  //  4
  byte padding[16];         // 16
                            //------
                            // 32
};

struct EquationSolution {
  uint32_t no;              //  4 byte(s)
  byte numberOfSolutions;   //  1
  float x1;                 //  4
  float x2;                 //  4
  byte padding[19];         // 19
                            //------
                            // 32
};

class I2CMasterProcessorDemo: public I2CMasterInterface {
  public:
    void prepareTxData(const void* txBuffer, const int txBufferSize);
    void processRxData(const void* rxBuffer, const int rxBufferSize);
  private:
    uint32_t _counter = 0;
};

int randomSignal() {
  return (random(6) > 2) ? 1 : -1;
}

void I2CMasterProcessorDemo::prepareTxData(const void* txBuffer, const int txBufferSize) {
  EquationDefinition* txData = (EquationDefinition*)txBuffer;
  txData->no = ++_counter;
  txData->a = randomSignal() * random(1, 101);
  txData->b = randomSignal() * random(1, 101);
  txData->c = randomSignal() * random(1, 101);
}

void I2CMasterProcessorDemo::processRxData(const void* rxBuffer, const int rxBufferSize) {
  EquationSolution* rxData = (EquationSolution*) rxBuffer;
  Serial.print("Result ("), Serial.print(rxData->no), Serial.print("): ");
  if (rxData->numberOfSolutions <= 0) {
    Serial.println("the equation has no solution");
  } else if (rxData->numberOfSolutions == 1) {
    Serial.print("the equation has exactly one solution x="),
        Serial.print(rxData->x1);
    Serial.println();
  } else {
    Serial.print("the equation has two solutions x1="),
        Serial.print(rxData->x1),
        Serial.print(", x2="), Serial.print(rxData->x2);
    Serial.println();
  }
}

//-------------------------------------------------------------------------------------------------

EquationDefinition eqDefinition;
EquationSolution eqSolution;

I2CMasterProcessorDemo i2cMasterProcessor;

I2CMasterHandler i2cMaster(
    (void*)&eqDefinition, sizeof(eqDefinition),
    (void*)&eqSolution, sizeof(eqSolution),
    &i2cMasterProcessor);

const byte masterAddress = 8;
const byte slaveAddress = 9;

void setup() {
  Serial.begin(115200);
  i2cMaster.begin(masterAddress);
}

void loop() {
  i2cMaster.call(slaveAddress);
}

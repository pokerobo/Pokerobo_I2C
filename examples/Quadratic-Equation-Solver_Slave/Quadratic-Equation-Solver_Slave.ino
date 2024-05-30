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

class I2CSlaveProcessorDemo: public I2CSlaveInterface {
  public:
    void processRxData(const void* rxBuffer, const int rxBufferSize) {
      EquationDefinition* rxData = (EquationDefinition*)rxBuffer;
      Serial.print("Solving quadratic equation ("),
          Serial.print(rxData->no), Serial.print("): ");
      Serial.print(rxData->a), Serial.print("x^2");
      Serial.print(" + ");
      Serial.print(rxData->b), Serial.print("x");
      Serial.print(" + ");
      Serial.print(rxData->c), Serial.print(" = 0");
      Serial.println();

      solution.no = rxData->no;
      float delta = rxData->b*rxData->b - rxData->a*rxData->c*4;
      if (delta > 0) {
        float sqrtDelta = sqrt(delta);
        solution.numberOfSolutions = 2;
        solution.x1 = (-rxData->b + sqrtDelta)/rxData->a/2.0;
        solution.x2 = (-rxData->b - sqrtDelta)/rxData->a/2.0;
      } else if (delta == 0.0) {
        solution.numberOfSolutions = 1;
        solution.x1 = -rxData->b/rxData->a/2.0;
        solution.x2 = 0;
      } else {
        solution.numberOfSolutions = 0;
        solution.x1 = 0;
        solution.x2 = 0;
      }
    };
    void prepareTxData(const void* txBuffer, const int txBufferSize) {
      EquationSolution* txData = (EquationSolution*)txBuffer;
      memcpy(txData, &solution, sizeof(solution));
    };
  private:
    EquationSolution solution;
};

//-------------------------------------------------------------------------------------------------

EquationDefinition eqDefinition;
EquationSolution eqSolution;

I2CSlaveProcessorDemo i2cSlaveProcessor;
I2CSlaveHandler i2cWorker(
    (void*)&eqDefinition, sizeof(eqDefinition),
    (void*)&eqSolution, sizeof(eqSolution),
    &i2cSlaveProcessor);

const byte slaveAddress = 9;

void setup() {
  Serial.begin(115200);

  i2cWorker.begin(slaveAddress,
    [] (int numBytesReceived) { i2cWorker.receiveEvent(numBytesReceived); },
    [] () { i2cWorker.requestEvent(); }
  );
}

void loop() {
  i2cWorker.next();
}

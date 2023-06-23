#include <Arduino.h>

namespace
{
  const uint8_t pinA = 2;
  const uint8_t pinB = 3;
  char  buf[20];

  typedef enum : uint8_t {
    STATE_BIT_UNSET   = 0,
    STATE_BIT_A       = 1,
    STATE_BIT_B       = 2,
    STATE_BIT_UPDATED = 128
  } State;
  volatile State state = STATE_BIT_UNSET;
  
  typedef enum {
    A0B0 = 0x0,
    A1B0 = 0x1,
    A0B1 = 0x2,
    A1B1 = 0x3
  } Phase;
  Phase next_phase_cw[4]  = { A1B0, A1B1, A0B0, A0B1 };
  Phase next_phase_ccw[4] = { A0B1, A0B0, A1B1, A1B0 };

  void handlePinAB()
  {
    digitalRead(pinA) ? state |= STATE_BIT_A : state &= ~STATE_BIT_A;
    digitalRead(pinB) ? state |= STATE_BIT_B : state &= ~STATE_BIT_B;
    state |= STATE_BIT_UPDATED;
  }

  void showState()
  {
    char a = state & STATE_BIT_A ? 'A' : 'a';
    char b = state & STATE_BIT_B ? 'B' : 'b';
    sprintf(buf, "%c%c\n", a, b);
    Serial.print(buf);
  }
}

void setup()
{
  Serial.begin(115200);
  
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), handlePinAB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), handlePinAB, CHANGE);
  
  handlePinAB();
}

void loop()
{
  if (!(state & STATE_BIT_UPDATED))
    { ; }
  else
    {
      showState();
      state &= ~STATE_BIT_UPDATED;
    }
}

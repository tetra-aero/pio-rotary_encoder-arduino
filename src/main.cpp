#include <Arduino.h>
#include <Keyboard.h>

namespace
{
  const uint8_t pinA = 2;
  const uint8_t pinB = 3;
  const uint8_t debounce_timer_ms = 0;
  int8_t counter = 0;
  char  buf[20];

  const uint8_t STATE_BIT_CLEARED = 0;
  const uint8_t STATE_BIT_PHASE_A = 1;
  const uint8_t STATE_BIT_PHASE_B = 2;
  volatile bool state_updated;
  typedef enum : uint8_t {
    A0B0 = STATE_BIT_CLEARED,
    A1B0 = STATE_BIT_PHASE_A,
    A0B1 = STATE_BIT_PHASE_B,
    A1B1 = STATE_BIT_PHASE_A | STATE_BIT_PHASE_B
  } State;
  State previous = A0B0;
  State volatile current  = A0B0;
  State cw_to[4]  = { A1B0, A1B1, A0B0, A0B1 };
  State ccw_to[4] = { A0B1, A0B0, A1B1, A1B0 };

  void handlePinAB()
  {
    digitalRead(pinA) ? current |= STATE_BIT_PHASE_A : current &= ~STATE_BIT_PHASE_A;
    digitalRead(pinB) ? current |= STATE_BIT_PHASE_B : current &= ~STATE_BIT_PHASE_B;
    state_updated = true;
  }

  void showState()
  {
    char a = current & STATE_BIT_PHASE_A ? 'A' : 'a';
    char b = current & STATE_BIT_PHASE_B ? 'B' : 'b';
    sprintf(buf, "%c%c %d\n", a, b, counter);
    Serial.print(buf);
  }
  void showClick(char direction)
  {
    static uint8_t column;
    sprintf(buf, "%c", direction);
    Serial.print(buf);
    column++;
    if (column > 80)
      {
        column = 0;
        Serial.println();
      }
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
  static unsigned long last_step_ms;
  if (millis() - last_step_ms < debounce_timer_ms)
    { return; }
  else if (!state_updated)
    { return; }
  else if (current == cw_to[previous])
    {
      if (previous==A0B0) showClick('+');
      previous = current;
      last_step_ms = millis();
    }
    else if (current == ccw_to[previous])
    {
      if (previous==A1B0) showClick('-');
      previous = current;
      last_step_ms = millis();
    }
    state_updated = false;
}

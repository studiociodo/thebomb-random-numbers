#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include "SevenSegmentTM1637.h"
#define massimo 29
#define randomSeed(s) srandom(s)

volatile uint32_t seed;
volatile int8_t nrot;
SevenSegmentTM1637    display(A5, A4);
int ordine[massimo + 1];
int posizione = 0;

boolean giapresente(int estratto) {
  for (int i = 0; i < massimo + 1; i++) {
    if (ordine[i] == estratto) {
      return true;
    }
  }
  return false;
}

void CreateTrulyRandomSeed()
{
  seed = 0;
  nrot = 32;
  cli();
  MCUSR = 0;
  _WD_CONTROL_REG |= (1 << _WD_CHANGE_BIT) | (1 << WDE);
  _WD_CONTROL_REG = (1 << WDIE);
  sei();
  while (nrot > 0);
  cli();
  MCUSR = 0;
  _WD_CONTROL_REG |= (1 << _WD_CHANGE_BIT) | (0 << WDE);
  _WD_CONTROL_REG = (0 << WDIE);
  sei();
}

ISR(WDT_vect)
{
  nrot--;
  seed = seed << 8;
  seed = seed ^ TCNT1L;
}

void setup()
{
  CreateTrulyRandomSeed();
  randomSeed(seed);
  Serial.begin(9600);
  Serial.print("seed = ");
  Serial.println(seed);
  display.begin();
  display.setBacklight(30);

  while (posizione < massimo) {
    int numero = random(1, massimo + 1);
    if (!giapresente(numero)) {
      ordine[posizione] = numero;
      posizione = posizione + 1;
      Serial.println(numero);
    }
  }
  Serial.println("OK");

}

void loop()
{
  posizione = 0;
  while (posizione < massimo) {
    for (int i = 0; i <= 120; i++) {
      int numero = random(1, massimo + 1);
      display.print(numero);
      tone(A3, numero * 100);
      delay(25);
      noTone(A3);
      display.clear();
    }

    display.print(ordine[posizione]);
    posizione = posizione + 1;
    delay(200);
    tone(A3, 784, 180);
    delay(200);
    noTone(A3);
    tone(A3, 784, 180);
    delay(200);
    noTone(A3);
    tone(A3, 784, 180);
    delay(200);
    noTone(A3);
    tone(A3, 622, 720);
    delay(800);
    noTone(A3);
    delay(4000);
  }
}

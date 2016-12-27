/*
  Remote firework igniter
  This core is separated on two parts. Transmitter has 4 buttons and when they are pressed matching signal is passed through radio link to receiver. Receiver then activated matching relay to ignite rocket with heating element. All relays are activated if button 4 is pressed. 
  
  created   Nov 2016
  by CheapskateProjects
  ---------------------------
  Dependencies: https://github.com/maniacbug/RF24
  ---------------------------
  The MIT License (MIT)

  Copyright (c) 2016 CheapskateProjects

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// nRF24L01 radio: SPI bus and pins 9, 10 
RF24 radio(9,10);
// Radio addresses
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
  // Wait for inits
  delay(5000);

  // Init output pins
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  // Init radio
  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
}

void loop(void)
{
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

    // if there is data ready
    if ( radio.available() )
    {
      // Read target rocket id
      int targetRocket;
      radio.read( &targetRocket, sizeof(int) );

      // Wait for sender state transfer
      delay(20);
      radio.stopListening();

      // Send confirmation
      radio.write( &targetRocket, sizeof(int) );

      if(targetRocket == 1)
      {
        digitalWrite(6, HIGH);
        delay(2000);
        digitalWrite(6, LOW);
      }
      if(targetRocket == 2)
      {
        digitalWrite(7, HIGH);
        delay(2000);
        digitalWrite(7, LOW);
      }
      if(targetRocket == 3)
      {
        digitalWrite(8, HIGH);
        delay(2000);
        digitalWrite(8, LOW);
      }
      if(targetRocket == 4)// Alpha strike
      {
        digitalWrite(6, HIGH);
        digitalWrite(7, HIGH);
        digitalWrite(8, HIGH);
        delay(3000);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
        digitalWrite(8, LOW);
      }

      // Resume listening for next round
      radio.startListening();
    }
}


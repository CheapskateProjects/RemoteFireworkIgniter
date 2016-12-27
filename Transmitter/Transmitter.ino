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

// How long to wait response after sending message
int RESPONSE_TIMEOUT_MILLIS = 500;

// Button states. Button is activated only when it has first been deactivated
bool firstAvailable = true;
bool secondAvailable = true;
bool thirdAvailable = true;
bool allAvailable = true;

void setup(void)
{
  // Wait for inits
  delay(5000);

  // Init input
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  // Init radio
  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
}

bool info = true;
void loop(void)
{
  int toSend = 0;
  int firstState = digitalRead(8);
  int secondState = digitalRead(7);
  int thirdState = digitalRead(6);
  int allState = digitalRead(5);
  
  if(firstState == HIGH)
  {
    if(firstAvailable)
    {
      toSend = 1;
      firstAvailable = false;
    }
  }
  else
  {
    firstAvailable = true;
  }

  if(secondState == HIGH)
  {
    if(secondAvailable)
    {
      toSend = 2;
      secondAvailable = false;
    }
  }
  else
  {
    secondAvailable = true;
  }

  if(thirdState == HIGH)
  {
    if(thirdAvailable)
    {
      toSend = 3;
      thirdAvailable = false;
    }
  }
  else
  {
    thirdAvailable = true;
  }

  if(allState == HIGH)
  {
    if(allAvailable)
    {
      toSend = 4;
      allAvailable = false;
    }
  }
  else
  {
    allAvailable = true;
  }

  if(toSend > 0)
  {
    writeTarget(toSend);
  }
}

void writeTarget(int target)
{
  // Open pipes
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  // Write target rocket number
  radio.stopListening();
  radio.write( &target, sizeof(int) );

  // Wait for confirmation to become available
  radio.startListening();
  unsigned long start_time = millis();
  bool timeout = false;
  while ( !radio.available())
  {
    if (millis() - start_time > RESPONSE_TIMEOUT_MILLIS )
    {
      timeout = true;
      break;
    }
  }

  // Read confirmation
  int confirmation;
  radio.read( &confirmation, sizeof(int) );
}

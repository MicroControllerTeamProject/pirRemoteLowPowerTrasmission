/* 1byte 433Mhz RECEIVER example.
/* Tutorial link: http://electronoobs.com/eng_arduino_tut99.php
 * Code: http://electronoobs.com/eng_arduino_tut99_code2.php
 * Scheamtic: http://electronoobs.com/eng_arduino_tut99_sch1.php
 * Youtube Channel: http://www.youtube/c/electronoobs
// Arduino          433Mhz RX
// GND              GND
// 5V               VCC
// D3               Data
*/

#include <SoftwareSerial.h>
#include <VirtualWire.h>              //Download it here: http://electronoobs.com/eng_arduino_virtualWire.php

SoftwareSerial softwareSerial(10, 3, false);
bool isDisableAlarm = false;

//unsigned long _sendMessageTime;

void setup()
{
	Serial.begin(9600);
	softwareSerial.begin(38400);
	setVirtualWireForRF433Receiver();
	pinMode(13, OUTPUT);
	pinMode(10, OUTPUT);
	digitalWrite(13, LOW);
	pinMode(3, INPUT_PULLUP);
	//_sendMessageTime = millis();
}

void setVirtualWireForRF433Receiver()
{
	vw_set_ptt_pin(999);
	vw_set_ptt_inverted(false);
	vw_set_rx_pin(2);
	vw_setup(100); // Bits per sec
	vw_rx_start();
	attachInterrupt(1, disableAlarm, CHANGE);
}

void disableAlarm()
{
	isDisableAlarm = true;
}

unsigned long disableSoundTime = 0;

void loop()
{
	if (isDisableAlarm == true && disableSoundTime == 0)
	{
		disableSoundTime = millis();
	}

	if ((millis() - disableSoundTime) > 60000)
	{
		isDisableAlarm = false;
		disableSoundTime = 0;
	}

	uint8_t message[3];
	uint8_t messageLength = 3; // the size of the message
	vw_wait_rx_max(10000);
	if (vw_get_message(message, &messageLength)) // non-blocking
	{
		for (int i = 0; i < messageLength; i++)
		{
			Serial.print((char)message[i]);
			if ((char)message[i] == 'B')
			{
				if (isDisableAlarm == false && digitalRead(3) == 1)
				{
					tone(10, 400, 200);
					delay(200);
					tone(10, 1000, 200);
					delay(200);
					noTone(10);
					//delay(10);
				}
				digitalWrite(13, HIGH);
				delay(100);
				digitalWrite(13, LOW);
				delay(100);
			}
		}
		Serial.println("");
	}

}


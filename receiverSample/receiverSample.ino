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

//unsigned long _sendMessageTime;

void setup()
{
	Serial.begin(9600);
	softwareSerial.begin(38400);
	setVirtualWireForRF433Receiver();
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	//_sendMessageTime = millis();
}

void setVirtualWireForRF433Receiver()
{
	vw_set_ptt_pin(999);
	vw_set_ptt_inverted(false);
	vw_set_rx_pin(2);
	vw_setup(100); // Bits per sec
	vw_rx_start();
}



void loop()
{
	uint8_t message[1];
	uint8_t messageLength = 1; // the size of the message
	vw_wait_rx_max(10000);
	if (vw_get_message(message, &messageLength)) // non-blocking
	{
		for (int i = 0; i < messageLength; i++)
		{
			softwareSerial.println((char)message[i]);
			if ((char)message[i] == 'B')
			{
				/*for (int i = 0; i < 2; i++)
				{*/
				digitalWrite(13, HIGH);
				delay(1000);
				digitalWrite(13, LOW);
				delay(1000);
				//}
				Serial.println("beccato");
				//_sendMessageTime = 0;
				//delay(1000);
			}
			//received_number = message[i];
		}
	}
	//digitalWrite(13, HIGH);
	//delay(500);
	//digitalWrite(13, LOW);
	//delay(500);

}


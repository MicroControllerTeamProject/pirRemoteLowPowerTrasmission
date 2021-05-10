#include <SoftwareSerial.h>
#include <VirtualWire.h> //Download it here: http://electronoobs.com/eng_arduino_virtualWire.php
#include <avr/sleep.h>
//SoftwareSerial mySerial(666, 3);

bool isSystemActivated = false;

byte adcsra;

byte transistorPin = 0;

void setup() {
	setVirtualWireForRF433Receiver();
	//mySerial.begin(9600);
	vw_set_ptt_pin(999);
	vw_set_ptt_inverted(false);
	vw_set_tx_pin(4);
	vw_setup(100); // Bits per sec

	pinMode(0, OUTPUT);
	//pinMode(1, OUTPUT);
	PCMSK |= bit(PCINT2);  // want pin D3 / pin 2
	GIFR |= bit(PCIF);    // clear any outstanding interrupts
	GIMSK |= bit(PCIE);    // enable pin change interrupts

	randomSeed(analogRead(A0));

	attachInterrupt(0, activateSystemInterrupt, CHANGE);
}

void activateSystemInterrupt()
{
	isSystemActivated = true;
}
long number = 0;

bool canGo = true;

uint8_t messageDelivered = 0;

void loop() {

	//char message[1];// = "A";   // array to hold the result.
	//strcpy(message, "B"); // copy string one into the result.
	//vw_send((uint8_t*)message, 1);
	//vw_wait_tx();
	//digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
	//delay(5000);                       // wait for a second
	//digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW}



	uint8_t message[1];
	uint8_t messageLength = 1; // the size of the message

	if (isSystemActivated)
	{
		messageDelivered = 0;
		for (int i = 0; i < 5; i++)
		{
			canGo = true;

			for (int i = 0; i < 100; i++)
			{
				delay(100);
				if (vw_get_message(message, &messageLength)) // non-blocking
				{
					for (int i = 0; i < messageLength; i++)
					{
						if ((char)message[i] == 'B')
						{
							for (int i = 0; i < 20; i++)
							{
								digitalWrite(0, HIGH);
								delay(500);
								digitalWrite(0, LOW);
								delay(500);
							}
							//_sendMessageTime = 0;
							delay(50000);
							canGo = false;
						}
						//received_number = message[i];
					}
				}
			}
			if (canGo)
			{
				for (int i = 0; i < 3; i++)
				{
					char message[3];// = "A";   // array to hold the result.
					strcpy(message, "B1X"); // copy string one into the result.
					vw_send((uint8_t*)message, 3);
					vw_wait_tx();
					digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
					delay(5000);                       // wait for a second
					digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW}
				}
				messageDelivered++;
				number = random(1, 10);
				delay(number * 10000);
			}

			//}
		}
		//mySerial.println("System sleep mode");
		//delay(100000);
		if (canGo || messageDelivered > 2)
		{
			isSystemActivated = false;
			digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
			delay(50000);                       // wait for a second
			digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW}
			cli();
			sleep_enable();
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			adcsra = ADCSRA;               //save the ADC Control and Status Register A
			ADCSRA = 0;                    //disable ADC
			sei();
			sleep_cpu();                   //go to sleep
			sleep_disable();               //wake up here
			ADCSRA = adcsra;               //restore ADCSRA
		}
	}
}

void setVirtualWireForRF433Receiver()
{
	vw_set_ptt_pin(999);
	vw_set_ptt_inverted(false);
	vw_set_rx_pin(3);
	vw_setup(100); // Bits per sec
	vw_rx_start();
}

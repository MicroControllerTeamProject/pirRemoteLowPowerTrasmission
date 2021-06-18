#include <SoftwareSerial.h>
#include <VirtualWire.h> //Download it here: http://electronoobs.com/eng_arduino_virtualWire.php
#include <avr/sleep.h>

bool isSystemActivated = false;

byte adcsra;
byte pinLed = 0;
byte pinInterrupt = 2;

void setup() {
	setVirtualWireForRF433Receiver();
	vw_set_ptt_pin(999);
	vw_set_ptt_inverted(false);
	vw_set_tx_pin(4);
	vw_setup(100); // Bits per sec

	pinMode(pinLed, OUTPUT);

	pinMode(pinInterrupt, INPUT_PULLUP);

	//pinMode(1, OUTPUT);
	PCMSK |= bit(PCINT2);  // want pin D3 / pin 2
	GIFR |= bit(PCIF);    // clear any outstanding interrupts
	GIMSK |= bit(PCIE);    // enable pin change interrupts

	randomSeed(analogRead(A0));
}


ISR(PCINT2_vect)
{
	isSystemActivated = true;
}

long number = 0;

bool canTransmitMessage = true;

uint8_t messageDelivered = 0;

void loop() {
	//if (isSystemActivated)
	//{
	//	digitalWrite(0, HIGH);  // let led blink
	//	delay(1000);
	//	digitalWrite(0, LOW);
	//	delay(1000);
	//	isSystemActivated = false;
	//}

	//return;

	//char message[1];// = "A";   // array to hold the result.
	//strcpy(message, "B"); // copy string one into the result.
	//vw_send((uint8_t*)message, 1);
	//vw_wait_tx();
	//digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
	//delay(5000);                       // wait for a second
	//digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW}

	uint8_t message[1];
	uint8_t messageLength = 1; // the size of the message

	if (isSystemActivated || digitalRead(pinInterrupt) == HIGH)
	{
		messageDelivered = 0;
		for (int i = 0; i < 5; i++)
		{
			canTransmitMessage = true;

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
							canTransmitMessage = false;
						}
						//received_number = message[i];
					}
				}
			}
			if (canTransmitMessage)
			{
				for (int i = 0; i < 3; i++)
				{
					char message[3];
					strcpy(message, "B1X"); 
					vw_send((uint8_t*)message, 3);
					vw_wait_tx();
					digitalWrite(0, HIGH);  
					delay(5000);                      
					digitalWrite(0, LOW); 
				}
				messageDelivered++;
				number = random(1, 10);
				delay(number * 10000);
			}
		}
		if ((canTransmitMessage || messageDelivered > 2) && digitalRead(pinInterrupt) != HIGH)
		{
			goToSleep();
		}
	}
	else
	{
		goToSleep();
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
void goToSleep()
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
	delay(1000);
}

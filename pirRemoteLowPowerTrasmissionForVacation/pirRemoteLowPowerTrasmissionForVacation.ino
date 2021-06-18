//#include <SoftwareSerial.h>
#include <VirtualWire.h> //Download it here: http://electronoobs.com/eng_arduino_virtualWire.php
#include <avr/sleep.h>
#include <avr/power.h>
//SoftwareSerial mySerial(666, 3);

bool isSystemActivated = false;

byte adcsra;

byte transistorPin = 1;

void setup() {

	/*pinMode(2, INPUT_PULLUP);*/
	pinMode(transistorPin, OUTPUT);
	digitalWrite(transistorPin, LOW);

	setVirtualWireForRF433Receiver();
	//mySerial.begin(9600);
	vw_set_ptt_pin(999);
	vw_set_ptt_inverted(false);
	vw_set_tx_pin(3);
	vw_setup(100); // Bits per sec

	PCMSK |= _BV(PCINT2);  // want pin D3 / pin 2
	//MCUCR |= _BV(ISC00);
	//MCUCR |= _BV(ISC01);
	//GIFR |= bit(PCIF);    // clear any outstanding interrupts
	GIMSK |= _BV(PCIE);    // enable pin change interrupts

	randomSeed(analogRead(A0));

	isSystemActivated = true;

	//attachInterrupt(0, activateSystemInterrupt, CHANGE);
}


ISR(PCINT0_vect)
{
	isSystemActivated = true;
}

long number = 0;

bool canGo = true;

uint8_t messageDelivered = 0;

void loop() {
	digitalWrite(transistorPin,HIGH);
	//delay(10000);
	//cli();
	//sleep_enable();
	//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//adcsra = ADCSRA;               //save the ADC Control and Status Register A
	//ADCSRA = 0;                    //disable ADC
	//sei();
	//digitalWrite(transistorPin, LOW);
	//sleep_cpu();                   //go to sleep
	//sleep_disable();               //wake up here
	//ADCSRA = adcsra;               //restore ADCSRA

	//return;


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

	if (isSystemActivated)
	{
		digitalWrite(transistorPin, HIGH);

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
							//for (int i = 0; i < 20; i++)
							//{
							//	digitalWrite(0, HIGH);
							//	delay(500);
							//	digitalWrite(0, LOW);
							//	delay(500);
							//}
							////_sendMessageTime = 0;
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
					//digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
					//delay(5000);                       // wait for a second
					//digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW}
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
			digitalWrite(transistorPin, LOW);
			//digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
			//delay(50000);                       // wait for a second
			//digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW}
			cli();
			// Enter sleep
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			ADCSRA = 0; // Turn off ADC
			power_all_disable(); // Power off ADC, Timer 0 and 1, serial interface
			sleep_enable();
			sei();
			sleep_cpu();

			// …zzz

			// Wake up
			sleep_disable();
			power_all_enable(); // Power everything back on
		}
	}
}

void setVirtualWireForRF433Receiver()
{
	vw_set_ptt_pin(999);
	vw_set_ptt_inverted(false);
	vw_set_rx_pin(4);
	vw_setup(100); // Bits per sec
	vw_rx_start();
}

#include <p18cxxx.h>
#include "user\spihd.h"

#define SW_CS_PIN PORTCbits.RC2
// Chip Select
#define TRIS_SW_CS_PIN TRISCbits.TRISC2
#define SW_DIO_PIN PORTBbits.RB7 // Data
#define TRIS_SW_DIO_PIN TRISBbits.TRISB7
#define SW_SCK_PIN PORTBbits.RB6 // Clock
#define TRIS_SW_SCK_PIN TRISBbits.TRISB6



char ReadSpihd(char address)
{
	char Bit;
	char input;
	TRIS_SW_CS_PIN = 0; // Make the CS pin an output
	TRIS_SW_SCK_PIN = 0; // Make the SCK pin an output
	TRIS_SW_DIO_PIN = 0; // Make the DOUT pin an output
	SW_CS_PIN = 0; // Enable the mouse
	input = 0;
	for(Bit=7; Bit>=0;Bit--)
	{
		SW_SCK_PIN = 0; // Clear the SCK pin
		Nop(); // Pause a little bit
		SW_DIO_PIN = (address>>Bit)&1; // Set the Data line with a bit of the address
		SW_SCK_PIN = 1; // Set the SCK pin
		Nop(); // Pause a little bit
	}

	TRIS_SW_DIO_PIN = 1; // Make the DOUT pin an input

	for(Bit=7; Bit>=0;Bit--)
	{
		SW_SCK_PIN = 0; // Clear the SCK pin
		Nop();
		input = (input<<1) | SW_DIO_PIN; // Read data line
		SW_SCK_PIN = 1; // Set the SCK pin
		Nop(); // Pause a little bit
	}
	SW_CS_PIN = 1; // Disable the mouse

	return(input); // Return the received data
}

void WriteSpihd(char address, char content)
{
	char Bit;
	char input;
	TRIS_SW_CS_PIN = 0; // Make the CS pin an output
	TRIS_SW_SCK_PIN = 0; // Make the SCK pin an output
	TRIS_SW_DIO_PIN = 0; // Make the DOUT pin an output
	SW_CS_PIN = 0; // Enable the mouse
	input = 0;
	for(Bit=7; Bit>=0;Bit--)
	{
		SW_SCK_PIN = 0; // Clear the SCK pin
		Nop(); // Pause a little bit
		SW_DIO_PIN = (address>>Bit)&1; // Set the Data line with a bit of the address
		SW_SCK_PIN = 1; // Set the SCK pin
		Nop(); // Pause a little bit
	}

	for(Bit=7; Bit>=0;Bit--)
	{
		SW_SCK_PIN = 0; // Clear the SCK pin
		Nop(); // Pause a little bit
		SW_DIO_PIN = (content>>Bit)&1; // Set the Data line with a bit of the content
		SW_SCK_PIN = 1; // Set the SCK pin
		Nop(); // Pause a little bit
	}
	SW_CS_PIN = 1; // Disable the mouse

	return; 
}
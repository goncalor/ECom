#include "xparameters.h"
#include "xgpio.h"

//This is a macro that gets my GPIO ID from the parameters file xparameters.h
#define MY_GPIO_ID XPAR_RUNNING_LEDS_DEVICE_ID
//This is a delay between updates in the leds, or we will not be able to visualize it
#define LED_DELAY 1000000
//following constant is used to determine which channel of the GPIO is used
#define LED_CHANNEL 1

int main(void)
{
	//this variable will contain the value presented in the leds
	unsigned int OutData;
	//this is the driver for our GPIO
	XGpio GpioDrv;
	//delay control variable
	int DelayCtl;
	//Initialize the GPIO driver so that it's ready to use,
	XGpio_Initialize(&GpioDrv, MY_GPIO_ID);
	//Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&GpioDrv, LED_CHANNEL, 0x0);
	//we will perform a shift of the light in the leds
	OutData = 0x80;
	while(1)
	{
		if(OutData & 0x01)
			OutData = 0x80;
		else
			OutData >>= 1;
		//write to the correct channel of the GPIO
		XGpio_DiscreteWrite(&GpioDrv, LED_CHANNEL, OutData);
		//apply the delay before next update
		for(DelayCtl = 0; DelayCtl < LED_DELAY; DelayCtl++)
			;
	}
	return 0;
}
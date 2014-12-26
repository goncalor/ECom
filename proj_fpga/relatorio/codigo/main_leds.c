//Delay between updates in the leds to visualize the switching
#define LED_DELAY 1000000

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
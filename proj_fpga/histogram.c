#include "xparameters.h"
#include "xgpio.h"
#include "fsl.h"
#define MY_GPIO_ID XPAR_RUNNING_LEDS_DEVICE_ID // macro that gets my GPIO ID from the parameters file xparameters.h
#define LED_DELAY 100000 // This is a delay between updates in the leds (in microseconds)
#define LED_CHANNEL 1 // used to determine which channel of the GPIO is used
#define GPIO_BITWIDTH 8 // This is the width we set for the GPIO
#define camerain_slot_id 0 // FSL related macro
#define cameraout_slot_id 0 // FSL related macro
#define num_lin (64) // number of lines in the frame
#define num_col (128) // number of columns in the frame
#define pixel(a,b) (image[a * num_lin + b]) // pixel address macro
#define N (num_lin*num_col) // number of pixels in a frame
#define GRAYLEVELS 128
unsigned char image[N]; //image buffer
unsigned int histogram[GRAYLEVELS];
unsigned char outimage[N]; //image buffer


int main(void)
{
	unsigned int OutData = 0x01;	// will contain the value presented in the leds
	XGpio GpioDrv;	// this is the driver for our GPIO
	int DelayCtl;	// delay control variable
	int i,j;	// pixel index
	//unsigned int middle;
	//unsigned char max;
	XGpio_Initialize(&GpioDrv, MY_GPIO_ID);	// Initialize the GPIO driver
	XGpio_SetDataDirection(&GpioDrv, LED_CHANNEL, 0x0);	// Set the direction for all signals to be outputs
	while (1)	// repeat forever:
	{
		if (OutData & 0x80) OutData = 0x01;
		else OutData <<= 1;		// shift the current on-led to the left
		XGpio_DiscreteWrite(&GpioDrv, LED_CHANNEL, OutData); 		// write to the correct channel of the GPIO
		for(DelayCtl = 0; DelayCtl < LED_DELAY; DelayCtl++);		// apply the delay before next update
		for(i=0;i<GRAYLEVELS;i++){
			histogram[i] = 0;
		}
		//max = 0;
		for (i = 0; i < N; i++) microblaze_bread_datafsl(image[i], cameraout_slot_id);		// read a full frame from the camera
		for (i = 0; i < N; i++){
			histogram[image[i]>>1] += 1;
		//	if (histogram[image[i]>>1] > max) max = histogram[image[i]>>1];
		}
		for (i = 0; i < GRAYLEVELS; i++){
			histogram[i] = histogram[i] / 128;
		}
		for(j=0;j<num_lin;j++){
			for(i=0;i<num_col;i++){
				//outimage[j * num_col + i] = 255;
				if((64-histogram[i])<j){
				//if(histogram[i]>=j){
					outimage[j * num_col + i] = 0;
				}else{
					outimage[j * num_col + i] = 255;
				}
			}
		}
		/*for (i=0;i<num_col;i++){
			for (j=0; j<histogram[i]; j++)
			{
				outimage[j * num_lin + i] = 0;
			}
			for(j=histogram[i]; j<num_lin; j++){
				outimage[j * num_lin + i] = 255;
			}
			//
			middle = (histogram[i] / max) * num_lin;
			for(j=0;j<middle;j++){
				outimage[j * num_lin + i] = 255;
			}
			for(j=middle;j<num_lin;j++){
				outimage[j * num_lin + i] = 0;
			}

		}*/
		for(i = 0; i < N; i++) microblaze_bwrite_datafsl(outimage[i],camerain_slot_id);		// write a full frame to the VGA output
	}
	return 0;	// end application
}

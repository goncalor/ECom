#include "xparameters.h"
#include "xgpio.h"
#include "fsl.h"
#include <limits.h>
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
unsigned char image[num_lin][num_col]; //image buffer
short derivative[num_lin][num_col];
char Kernel[3][3] = {{-2,-2,0},{-2,6,0},{0,0,0}};


int main(void)
{
	unsigned int OutData = 0x01;	// will contain the value presented in the leds
	XGpio GpioDrv;	// this is the driver for our GPIO
	int DelayCtl;	// delay control variable
	short i,j,k,l;	// pixel index
	short max,min;
	XGpio_Initialize(&GpioDrv, MY_GPIO_ID);	// Initialize the GPIO driver
	XGpio_SetDataDirection(&GpioDrv, LED_CHANNEL, 0x0);	// Set the direction for all signals to be outputs
	while (1)	// repeat forever:
	{
		if (OutData & 0x80) OutData = 0x01;
		else OutData <<= 1;		// shift the current on-led to the left
		XGpio_DiscreteWrite(&GpioDrv, LED_CHANNEL, OutData); 		// write to the correct channel of the GPIO
		for(DelayCtl = 0; DelayCtl < LED_DELAY; DelayCtl++);		// apply the delay before next update

		for(i = 0; i < num_lin; i++) for(j=0; j<num_col;j++) microblaze_bread_datafsl(image[i][j], cameraout_slot_id);		// read a full frame from the camera

		for(i=0;i < num_lin; i++){
			for(j=0; j<num_col;j++){
				derivative[i][j] = 0;
				for(k=0;k<3;k++){
					for(l=0;l<3;l++){
						if(i+k<num_lin && j+l<num_col) derivative[i][j] += image[i+k][j+l] * Kernel[k][l] + image[i+2-k][j+2-l] * Kernel[2-k][2-l];
						/// /!\ else derivative[i][j] += image[i][j] * Kernel[k][l] + image[i][j] * Kernel[2-k][2-l];
					}
				}
			}
		}
		min = SHRT_MAX;
		max = SHRT_MIN;
		for(i=0;i < num_lin; i++){
			for(j=0; j<num_col;j++){
				if(derivative[i][j] < min) min = derivative[i][j];
				if(derivative[i][j] > max) max = derivative[i][j];
			}
		}
		for(i=0;i < num_lin; i++){
			for(j=0; j<num_col;j++){
				derivative[i][j] -= min;
				derivative[i][j] = derivative[i][j] * 256 / (max-min);
			}
		}

		for(i = 0; i < num_lin; i++) for(j=0; j<num_col;j++) microblaze_bwrite_datafsl(derivative[i][j],camerain_slot_id);		// write a full frame to the VGA output
	}
	return 0;	// end application
}

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
	int i,j;	// pixel index
	unsigned char max;
	
	while (1)	// repeat forever:
	{
		for(i=0;i<GRAYLEVELS;i++){
			histogram[i] = 0;
		}
		max = 0;
		for (i = 0; i < N; i++) microblaze_bread_datafsl(image[i], cameraout_slot_id);		// read a full frame from the camera
		for (i = 0; i < N; i++){
			histogram[image[i]>>1] += 1;
		}
		for (i = 0; i < GRAYLEVELS; i++){
			if (histogram[i] > max) max = histogram[i];
		}
		for (i = 0; i < GRAYLEVELS; i++){
			histogram[i] = histogram[i] * 64 / max;
		}

		for(i=0; i<num_col; i++){
			for(j=0; j<num_lin; j++){
				if(64-j<=histogram[i]){
					outimage[j*num_col + i] = 0;
				}else{
					outimage[j*num_col + i] = 255;
				}
			}
		}
		for(i = 0; i < N; i++) microblaze_bwrite_datafsl(outimage[i],camerain_slot_id);		// write a full frame to the VGA output
	}
	return 0;	// end application
}

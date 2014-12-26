#define num_lin (64) // number of lines in the frame
#define num_col (128) // number of columns in the frame
#define pixel(a,b) (image[a * num_lin + b]) // pixel address macro
#define N (num_lin*num_col) // number of pixels in a frame
unsigned char image[num_lin][num_col]; //image buffer
short derivative[num_lin][num_col];
char Kernel[2][2] = {{-2,-2},{-2,6}};

int main(void)
{
	short i,j,k,l;	// pixel index
	short max,min;

	while (1)	// repeat forever:
	{
		// read a full frame from the camera
		for(i = 0; i < num_lin; i++) for(j=0; j<num_col;j++) microblaze_bread_datafsl(image[i][j], cameraout_slot_id);		
		
		min = SHRT_MAX;
		max = SHRT_MIN;
		
		for(i=0;i < num_lin; i++){
			for(j=0; j<num_col;j++){
				derivative[i][j] = 0;
				for(k=0;k<2;k++){
					for(l=0;l<2;l++){
						if(i+k<num_lin && j+l<num_col) derivative[i][j] += image[i+k][j+l] * Kernel[k][l] + image[i+1+k][j+1+l] * Kernel[1-k][1-l];
					}
				}
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

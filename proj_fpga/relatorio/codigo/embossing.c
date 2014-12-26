#define pixel(a,b) (image[a * num_lin + b]) // pixel address macro
#define N (num_lin*num_col) // number of pixels in a frame
char Kernel[2][2] = {{-2,-2},{-2,6}};

// read a full frame from the camera
for(i=0; i<N; i++) microblaze_bread_datafsl(image[i][j], cameraout_slot_id);		

min = SHRT_MAX;
max = SHRT_MIN;

for(i=0; i<num_lin; i++){
	for(j=0; j<num_col; j++){
		deriv[i][j] = 0; // inicialize at 0
		for(k=0; k<2; k++){
			for(l=0; l<2; l++){
				if(i+k<num_lin && j+l<num_col){ // calculate the derivative
				 	deriv[i][j] += image[i+k][j+l] * Kernel[k][l] 
				 		+ image[i+1+k][j+1+l] * Kernel[1-k][1-l];
				}
			}
		}

		// calculate the limits to later normalize
		if(deriv[i][j] < min) min = deriv[i][j];
		if(deriv[i][j] > max) max = deriv[i][j];
	}
}

for(i=0; i<num_lin; i++){ // normalize the output image
	for(j=0; j<num_col; j++){
		deriv[i][j] -= min;
		deriv[i][j] = deriv[i][j] * 256 / (max-min);
	}
}

// write a full frame from the camera
for(i=0; i<N; i++) microblaze_bwrite_datafsl(deriv[i][j],camerain_slot_id);
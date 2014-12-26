for(i=0; i<GRAYLEVELS; i++){ 
	histogram[i] = 0; // reset the vector to start a new count
}
max = 0; // reset the maximum. It's used to normalize the results

// read a full frame from the camera
for(i=0; i<N; i++) microblaze_bread_datafsl(image[i], cameraout_slot_id);		

for(i=0; i<N; i++){ // count each ocurrence of each tone of gray
	histogram[image[i]>>1] += 1; // shift right divides by 2
}
 
for(i=0; i<GRAYLEVELS; i++){ // find the maximum value
	if (histogram[i] > max) max = histogram[i]; 
}

for(i=0; i<GRAYLEVELS; i++){ // normalize all the results
	histogram[i] = histogram[i] * 64 / max;
}

for(i=0; i<num_col; i++){ // draw the histogram in memory
	for(j=0; j<num_lin; j++){
		if(64-j <= histogram[i]){
			outimage[j*num_col + i] = 0;
		}else{
			outimage[j*num_col + i] = 255;
		}
	}
}

// write a full frame from the camera
for(i=0; i<N; i++) microblaze_bwrite_datafsl(outimage[i],camerain_slot_id);
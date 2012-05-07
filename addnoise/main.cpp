#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "addnoise_function.h"
#include "io_png.h"

using namespace std;


int main(int argc, char **argv)
{
    //! Check if there is the right call for the algorithm
	if (argc < 3)
	{
      cout << "addnoise input.png noise_sigma output.png"<<endl;
		return EXIT_FAILURE;
	}

	//! read input image
	cout << "Read input image...";
	size_t height, width, chnls;
	float *img = NULL;
	img = read_png_f32(argv[1], &width, &height, &chnls);
	if (!img)
	{
		cout << "error :: " << argv[1] << " not found  or not a correct png image" << endl;
		return EXIT_FAILURE;
	}
	cout << "done." << endl;


	//! Variables initialization
	float    fSigma    = atof(argv[2]);
   unsigned wh        = (unsigned) width * height;
   unsigned whc       = (unsigned) wh * chnls;


	//! Add noise
	cout << "Add noise [sigma = " << fSigma << "] ...";
	float *img_noisy    = (float *) malloc(whc * sizeof(float));

	for (unsigned c = 0; c < chnls; c++)
		fiAddNoise(&img[c * wh], &img_noisy[c * wh], fSigma, c, wh);
    cout << "done." << endl;

	//! save noisy, denoised and differences images
	cout << "Save images...";
	if (write_png_f32(argv[3], img_noisy, width, height, chnls) != 0)
		cout << "... failed to save png image " << argv[3] << endl;

	//! Free Memory
	free(img_noisy   );

	return EXIT_SUCCESS;
}




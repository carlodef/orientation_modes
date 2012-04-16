/*
 * Copyright (C) 2012, Carlo De Franchis <carlo.de-franchis@polytechnique.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and
 * documentation are those of the authors and should not be
 * interpreted as representing official policies, either expressed
 * or implied, of the copyright holder.
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "libpng_io.h"
#include "Histo.h"
#include "modes_detection.h"

#define EPSILON 1

int main(int c, char *v[])
{
    // Parameters loading
    if (c < 7)
    {
        cout << "missing arguments" << endl;
	cout << "usage: " << v[0] << " image x y r n_bins flag_norm" << endl;
        return 1;
    }

    char *image_file = v[1];
    int x = atoi(v[2]);
    int y = atoi(v[3]);
    int r = atoi(v[4]);
    int n_bins = atoi(v[5]);
    int flag_norm = atoi(v[6]);

    // Image loading
    size_t nx, ny;
    float *im = read_png_f32_gray(image_file, &nx, &ny);


    // First step : A Contrario detection

    // Create and save histogram
    Histo h_ac = histo_orientation(im,nx,ny,x,y,r,n_bins,flag_norm,0);
    h_ac.print("histo_ac.txt");

    // Save the number of pixels used for the histogram construction
    ofstream flux("nb_pixels_ac.txt");
    flux << h_ac.get_M() << endl;
    flux.close();

    // Detect modes
    vector<float> modes = max_modes_detection(h_ac,EPSILON);

    // Save modes, orientations and NFA (approximated)
    flux.open("modes_ac.txt");
    for (int i(0); i<modes.size()/3.0; i++)
    {
        flux << "[" << modes[3*i] << "," << modes[3*i+1] << "]" << " ; "
            << compute_orientation(h_ac,modes[3*i],modes[3*i+1]) << " ; "
            << modes[3*i+2] << endl;
    }
    flux.close();



    // Second step : Lowe's detection

    // Create and save histogram
    // For Lowe's peak detection, histogram has to be weighted with gradient norms
    Histo h_lowe = histo_orientation(im,nx,ny,x,y,r,n_bins,1,1);
    h_lowe.print("histo_lowe.txt");

    // Detect local maxima
    vector<int> maxima;
    float max_histo = h_lowe.max();

    for (int i(0); i<n_bins; i++)
        if ((h_lowe[i] > 0.8*max_histo) && (h_lowe[i] > h_lowe[i-1]) && (h_lowe[i] > h_lowe[i+1])) 
	    maxima.push_back(i);

    // Save orientations associated to local maxima
    flux.open("modes_lowe.txt");
    for (size_t i(0); i<maxima.size(); i++)
    {
        flux <<  "[" << maxima[i] << "," << maxima[i] << "]" << " ; "
        << h_lowe.angle(maxima[i],1) << " ; "
        << h_lowe[maxima[i]]/max_histo << endl;
    }
    flux.close();


    // Clear memory
    delete im;
}

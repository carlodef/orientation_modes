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

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "Histo.h"
#include "modes_detection.h"

using namespace std;


// Function that computes the histogram of local orientation around a given (x,y)
// point of image im, at scale r. The number of bins of the histogram is L.
// If the parameter flag_norm is set to 1 the histogram is weighted by the norm
// of gradient, else it is not. If the parameter flag_gauss is set to 1 the histogram
// is weighted by a Gaussian-weighted circular window with a standard deviation that
// is 1.5 times that of the scale, r, of the keypoint.
Histo histo_orientation(float *im, int nx, int ny, int x, int y, int r, int L, int flag_norm, int flag_gauss)
{
    Histo histo(L);
    int count(0);

    if (flag_gauss)
    {
        float sigma = 1.5*r;
        // Loop over all the pixels in a big square window around the keypoint
        for (int i = max(1,(int) (x-3*sigma)); i <= min((int) (x+3*sigma),nx-2); i++)
        {
            for (int j = max(1,(int) (y-3*sigma)); j <= min((int) (y+3*sigma),ny-2); j++)
            {
                // The contributing pixels are in a circle centered in (x,y)
                if ((i-x)*(i-x)+(j-y)*(j-y) <= 9*sigma*sigma)
                {
                    // Computation of the gradient : the pixel of coordinates (k,l)
                    // is stored in im[l*nx+k]
                    float gx = im[j*nx+i+1]-im[j*nx+i-1];
                    float gy = -im[(j+1)*nx+i]+im[(j-1)*nx+i];
                    float norm = sqrtf(gx*gx+gy*gy);

                    if (flag_norm)
                    {
                        count++; // We count the number of points which contribute to the histogram

                        // Computation of the angle and the associated bin
                        float theta = atan2f(gy,gx);
                        int bin = floor((L/(2*M_PI))*(theta+M_PI+M_PI/L));
                        // If theta=M_PI, we are in the bin number L which is the bin 0
                        if (bin == L) bin = 0;

                        histo.incr(bin,norm*exp(-((i-x)*(i-x)+(j-y)*(j-y))/(2*sigma*sigma)));
                    }

                    else
                    {
                        if (norm > 3*sqrt(2))
                        {
                            count++;

                            // Computation of the angle and the associated bin
                            float theta = atan2f(gy,gx);
                            int bin = floor((L/(2*M_PI))*(theta+M_PI+M_PI/L));
                            // If theta=M_PI, we are in the bin number L which is the bin 0
                            if (bin == L) bin = 0;

                            histo.incr(bin,exp(-((i-x)*(i-x)+(j-y)*(j-y))/(2*sigma*sigma)));
                        }
                    }
                }
            }
        }
    }

    else
    {
        // Loop over all the pixels in a square window around the keypoint
        for (int i = max(1,(x-r)); i <= min((x+r),nx-2); i++)
        {
            for (int j = max(1,(y-r)); j <= min((y+r),ny-2); j++)
            {
                // The contributing pixels are in a circle centered in (x,y)
                if ((i-x)*(i-x)+(j-y)*(j-y) <= r*r)
                {
                    // Computation of the gradient : the pixel of coordinates (k,l)
                    // is stored in im[l*nx+k]
                    float gx = im[j*nx+i+1]-im[j*nx+i-1];
                    float gy = -im[(j+1)*nx+i]+im[(j-1)*nx+i];
                    float norm = sqrtf(gx*gx+gy*gy);

                    if (flag_norm)
                    {
                        count++; // We count the number of points which contribute to the histogram

                        // Computation of the angle and the associated bin
                        float theta = atan2f(gy,gx);
                        int bin = floor((L/(2*M_PI))*(theta+M_PI+M_PI/L));
                        // If theta=M_PI, we are in the bin number L which is the bin 0
                        if (bin == L) bin = 0;

                        histo.incr(bin,norm);
                    }

                    else
                    {
                        if (norm > 3*sqrt(2))
                        {
                            count++;

                            // Computation of the angle and the associated bin
                            float theta = atan2f(gy,gx);
                            int bin = floor((L/(2*M_PI))*(theta+M_PI+M_PI/L));
                            // If theta=M_PI, we are in the bin number L which is the bin 0
                            if (bin == L) bin = 0;

                            histo.incr(bin);
                        }
                    }

                }
            }
        }
    }


    // Normalization : the sum of the histogram has to be equal to the number of pixels contributing
    // If M=0, the histogram is empty and there is nothing to do
    if (histo.get_M() > 0)
        histo *= count/histo.get_M();

    return histo;
}


// This is the principal function. It takes as an input the histogram histo, and
// the parameter epsilon required by the a contrario model. It returns the list of
// detected modes, concatenated. The list contains the entropy of each mode : if there
// are two modes [a1,b1] and [a2,b2] with log_nfa values nfa1 and nfa2 the output is
// the list [a1,b1,nfa1,a2,b2,nfa2]
vector<float> max_modes_detection(Histo &histo, float epsilon)
{
    // Returned list
    vector<float> list;

    // If the histogram is empty (M=0), it's done (there are no modes)
    if (histo.get_M() > 0)
    {

        int L(histo.get_L());

        // L-by-L matrix which will remember if interval [a,b] is a meaningful interval or a meaningful gap
        int **intervals;
        intervals  = new int*[L]; // On ne peut pas faire "new int[L][L]"
        for (int i=0; i<L; i++)
            intervals[i] = new int[L];

        // L-by-L matrix which will remember the entropy of interval [a,b]
        float **entropy;
        entropy = new float*[L]; // On ne peut pas faire "new float[L][L]"
        for (int i=0; i<L; i++)
            entropy[i] = new float[L];

        // Computation of the matrix named "intervals"
        browse_intervals(histo,epsilon,intervals,entropy);

        // We discard the intervals containing gaps
        spread_gaps(L, intervals);

        // We keep only the maximal modes
        discard_modes(L,intervals,entropy);

        // Now we put in "list" the maximal modes corresponding
        // to coefficients > 1 in the matrix "intervals"
        for (int a=0; a<L; a++)
        {
            for (int b=0; b<L; b++)
            {
                if (intervals[a][b] > 1)
                {
                    list.push_back(a);
                    list.push_back(b);

                    // Compute -log_{10}(NFA) (an approximation)
                    float M = histo.get_M();
                    float log_nfa = -log10(histo.get_N())+M*entropy[a][b]/log(10);

                    //int k = histo.sum(a,b);
                    //float p = (1+b-a)/((float) L) + (b<a);
                    //float nfa = binomial_tail(floor(M+0.5),k,p)*histo.get_N();
                    list.push_back(log_nfa);
                }
            }
        }

        // Clear memory
        for (int i=0; i<L; i++)
        {
            delete intervals[i];
            delete entropy[i];
        }

        delete intervals;
        delete entropy;

    }
    return list;
}


// Function running over all the circular intervals contained in [1,L],
// computing if they are meaningful intervals or gaps, or if they are not.
// The results are stored in the matrices intervals and entropy. In the matrix
// intervals, we use the following convention :
// -meaningful interval : 2
// -meaningful gap : -1
// -neither meaningful interval or gap : 0
void browse_intervals(Histo &histo, float epsilon, int **intervals, float **entropy)
{
    int L = histo.get_L();
    int M = histo.get_M();
    int N = histo.get_N();
    float thresh = log(N/epsilon)/M;

    for (int a=0; a<L; a++)
    {
        for (int b=0; b<L; b++)
        {
            // Compute k,r,p,e
            int k = histo.sum(a,b);
            float r = (float) k/M;
            float p = (1+b-a)/((float) L) + (b<a); // p is equal to (1+b-a)/L if b>=a and to (1+b-a+L)/L if b<a
            float e = compute_entropy(r,p);

            // Save the entropy value e
            entropy[a][b] = e;

            // Memorize if the interval [a,b] is a meaningful interval or gap
            if (e>thresh)
            {
                if (r>p)
                    intervals[a][b] = 2;
                else
                    intervals[a][b] = -1;
            }
            else
                intervals[a][b] = 0;
        }
    }
}


// This function "propagates" the -1 values such that if an interval contains a gap, its
// marker in the "intervals" matrix goes to -1
void spread_gaps(int L, int **intervals)
{
    // loop over all the lines of the matrix intervals
    for (int a(0); a<L; a++)
    {
        // the loop over the columns is separated in two parts
        for (int b(0); b<a; b++)
        {
            if (intervals[a][b] < 0)
                for (int i(a); i>b; i--)
                    for (int j(b); j<i; j++)
                        intervals[i][j] = 0;
        }

        // second part
        for (int b(a); b<L; b++)
        {
            if (intervals[a][b] < 0)
                for (int i(a); i>(b-L); i--)
                    for (int j(b); j<(i+L); j++)
                        intervals[(i+L) % L][j % L] = 0; 
			// (i+L) instead of i because the modulo operator %i
			// doesn't like negative entries
        }
    }
}


// For each mode (marker = 2), this function looks at the intervals inside, and
// if there is one with bigger entropy, it discards the
// mode (by putting the marker to 1)
void discard_modes(int L, int **intervals, float **entropy)
{
    // loop over all the lines of the matrix intervals
    for (int a(0); a<L; a++)
    {
        // the loop over the columns is separated in two parts
        for (int b(a); b<L; b++)
        {
            if (intervals[a][b] > 1) // [a,b] is a possible maximal mode
            {
                float e(entropy[a][b]);
                int i(a);

                while (i<=b)
                {
                    int j(b);
                    while (j>=i)
                    {
                        if ((j-i < b-a) && (intervals[i][j] > 0))
                            // [i,j] is a sub-mode strictly included in [a,b]
                        {
                            if (entropy[i][j] < e)
                                intervals[i][j] = 1;
                            else
                                intervals[a][b] = 1;
                                // [a,b] is not a maximal mode
                        }
                        j--;
                    }
                    i++;
                }
            }
        }

        // second part
        for (int b(0); b<a; b++)
        {
            if (intervals[a][b] > 1) // [a,b] is a possible maximal mode
            {
                float e(entropy[a][b]);
                int i(a);

                while (i<=(b+L))
                {
                    int j(b);
                    while (j>=(i-L))
                    {
                        if (((j-i+L)%L < (b-a+L)%L) && (intervals[i % L][(j+L) % L] > 0))
                            // [i,j] is a sub-mode strictly included in [a,b]
                        {
                            if (entropy[i % L][(j+L) % L] < e)
                                intervals[i % L][(j+L) % L] = 1;
                            else
                                intervals[a][b] = 1;
                        }
                        j--;
                    }
                    i++;
                }
            }
        }
    }
}


// Function that compute the relative entropy between r and p. It is also
// called the Kullback-Leider distance
float compute_entropy(float r, float p)
{
    if (r==1)
        return -log(p);
    
    float h = r*log(r/p)+(1-r)*log((1-r)/(1-p));
    return h;
}


// Function that compute the binomial tail B(M,k,p)
// TO DO
//long double binomial_tail(int M, int k, float p)
//{
//    long double s(0);
//    for (int i(k); i<=M; i++)
//    {
//        s += 0;
//    }
//    return s;
//}


// Function that compute the angle corresponding to a given mode [a,b]. This angle is
// equal to the weighted average of the values in the mode [a,b] of the histogram h
float compute_orientation(Histo &h, int a, int b)
{
    double theta = 0.0;
    if (a <= b)
        for (int i(a); i<=b; i++)
            theta += h.angle(i)*h[i];
    else
    {
        // We compute the weighted average mod 2*PI
        for (int i(a); i<h.get_L(); i++)
            theta += h.angle(i)*h[i];
        for (int i=0; i<=b; i++)
            theta += (h.angle(i)+2*M_PI)*h[i];
    }

    theta /= h.sum(a,b);
    return fmod(theta, 2*M_PI);
}

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

#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

#include "Histo.h"

/**
* Constructors
*/
Histo::Histo() : m_L(0), m_N(1), m_M(0), m_data(0)
{
}

Histo::Histo(int L) : m_L(L), m_N(m_L*(m_L-1)+1), m_M(0), m_data(new float[L])
{
    for (int i=0; i<L; i++) {
        m_data[i] = 0;
    }
}

Histo::Histo(const Histo& h) : m_L(h.m_L), m_N(h.m_N), m_M(h.m_M), m_data(0)
{
    if (m_L > 0) {
        m_data = new float[m_L];
        for (int i=0; i < m_L; i++) m_data[i] = h.m_data[i];
    }
}


/**
* Destructor
*/
Histo::~Histo()
{
    delete m_data;
}


/**
* Accessors
*/
int Histo::get_L() const
{
    return m_L;
}
int Histo::get_N() const
{
    return m_N;
}
float Histo::get_M() const
{
    return m_M;
}
float *Histo::get_data() const
{
    return m_data;
}


/**
* Infos
*/
int Histo::sum(int a, int b) const
{
    float s(0);
    if (a <= b) {
        for (int i=a; i<=b; i++) {
            s += m_data[i];
        }
    } else {
        for (int i=a; i<m_L; i++) {
            s += m_data[i];
        }
        for (int i=0; i<=b; i++) {
            s += m_data[i];
        }
    }
    return s;
}

float Histo::max() const
{
    float m(0);
    for (int i(0); i < m_L; i++) {
        if (m_data[i] > m) {
            m=m_data[i];
        }
    }
    return m;
}


// The parameter 'flag_parabola' can take 2 values : 0 and 1.
// O : the angle corresponds exactly to bin i
// 1 : the angle is refined by fitting a parabola on the three values of
// histogram around the bin
float Histo::angle(int bin, int flag_parabola) const
{
    float x, l, m, r;
    if (flag_parabola) {

        if (bin > 0) {
            l = m_data[bin-1];
        } else {
            l = m_data[m_L-1];
        }

        m = m_data[bin];

        if (bin < (m_L-1)) {
            r = m_data[bin+1];
        } else {
            r = m_data[0];
        }

        x = bin-0.5*(l-r)/(-l+2*m-r);
    } else {
        x = bin;
    }
    return -M_PI + x*(2*M_PI/m_L);
}

void Histo::print(string filename) const
{
    ofstream flux(filename.c_str());
    for (int i(0); i < m_L; i++) {
        flux << m_data[i] << " ";
    }
}


/**
* Modifications of the histo
*/
void Histo::incr(int bin, float x)
{
    // Check the arguments
    if (bin < 0 || bin >= m_L) {
        cout << "Histo::incr : parameter 1 has to be an int between 0 and "
             << (m_L-1) << endl;
    }

    // Do the job
    m_data[bin] += x;
    m_M += x;
}

void Histo::operator*= (float a)
{
    if (m_data) for (int j=0; j<m_L ; j++) m_data[j] *= a;
    m_M *= a;
}

/**
* Static methods
*/

int Histo::good_modulus(int n, int p)
{
    if (p < 0) return good_modulus(n, -p);

    int r;
    if (n >= 0)
        r = n % p;
    else {
        r = p - (-n) % p;
        if (r == p)
            r = 0;
    }
    return r;
}

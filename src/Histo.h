#ifndef HISTO_H_INCLUDED
#define HISTO_H_INCLUDED

#include <string>
using namespace std;

class Histo
{
    public :

    /**
    * Constructors
    */
    Histo();
    Histo(int L);
    Histo(const Histo& h); // Passage par référence constante

    /**
    * Destructor
    */
    ~Histo();


    /**
    * Accessors
    */
    int get_L() const;
    int get_N() const;
    float get_M() const;
    float *get_data() const;
    float operator[](int i) const {return this->m_data[good_modulus(i,m_L)];};

    /**
    * Infos
    */
    int sum(int a, int b) const;
    float max() const;
    float angle(int bin, int flag_parabola = 0) const;
    void print(string filename) const;

    /**
    * Modifications of the histo
    */
    void incr(int bin, float x = 1);
    void operator*= (float a);

    /**
    * Static methods
    */
    static int good_modulus(int n, int p);

    private :

    int const m_L; // number of bins
    int const m_N; // L(L-1)+1
    float m_M; // number of samples in the histogram
    float *m_data; // pointer to the array of data
};

#endif // HISTO_H_INCLUDED

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

Histo histo_orientation(float *im, int nx, int ny, int x, int y, int r, int L, int flag_norm, int flag_gauss);

std::vector<float> max_modes_detection(Histo &h, float epsilon);

void browse_intervals(Histo &histo, float epsilon, int **intervals, float **entropy);
void spread_gaps(int L, int **intervals);
void discard_modes(int L, int **intervals, float **entropy);

float compute_entropy(float r, float p);
float compute_orientation(Histo &h, int a, int b);

#endif // FUNCTIONS_H_INCLUDED

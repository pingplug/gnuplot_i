#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "gnuplot_i.h"

#define N_LINES 5
#define N_POINTS 20

int main(int argc, char * argv[])
{
	gnuplot_ctrl * g = gnuplot_init();

	gnuplot_cmd(g, "set terminal png");
	gnuplot_cmd(g, "set output \"sine.png\"");
	gnuplot_cmd(g, "set key box opaque");
	gnuplot_plot_equation(g, "sin(x)", "Sine wave");

	gnuplot_resetplot(g);
	gnuplot_setstyle(g, "lines");
	gnuplot_cmd(g, "set output \"rand_%d.png\"", N_LINES);
	gnuplot_cmd(g, "set key noopaque");
	double x[N_POINTS];
	double** y = (double**)malloc(sizeof(double*) * N_LINES);
	for (int i = 0; i < N_LINES; i++) {
		y[i] = (double*)malloc(sizeof(double) * N_POINTS);
	}
	for (int i = 0; i < N_POINTS; i++) {
		x[i] =  i + (double)(rand()) / INT_MAX;
		for (int j = 0; j < N_LINES; j++) {
			y[j][i] = (double)(rand()) / INT_MAX * (j + 1);
		}
	}

	char** t = (char**)malloc(sizeof(char*) * N_LINES);
	for (int i = 0; i < N_LINES; i++) {
		t[i] = (char*)malloc(sizeof(char) * 512);
		sprintf(t[i], "rand * %d", i + 1);
	}
	gnuplot_plot_x_multi_y(g, x, y, N_POINTS, N_LINES, t);
	gnuplot_close(g);

	return 0 ;
}


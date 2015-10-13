/*-------------------------------------------------------------------------*/
/**
  @file     gnuplot_i.c
  @author   N. Devillard
  @date Sep 1998
  @version  $Revision: 2.10 $
  @brief    C interface to gnuplot.

  gnuplot is a freely available, command-driven graphical display tool for
  Unix. It compiles and works quite well on a number of Unix flavours as
  well as other operating systems. The following module enables sending
  display requests to gnuplot through simple C calls.

*/
/*--------------------------------------------------------------------------*/

/*
    $Id: gnuplot_i.c,v 2.10 2003/01/27 08:58:04 ndevilla Exp $
    $Author: ndevilla $
    $Date: 2003/01/27 08:58:04 $
    $Revision: 2.10 $
 */

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/

#include "gnuplot_i.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <io.h>
#endif // #ifdef _WIN32

/*---------------------------------------------------------------------------
                                Defines
 ---------------------------------------------------------------------------*/

// alloc the buffer (64K)
#define BUF_SIZE 1 << 16
char* BUF;

/*---------------------------------------------------------------------------
                          Prototype Functions
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/

gnuplot_ctrl* gnuplot_init(void)
{
    gnuplot_ctrl* handle;

#ifndef _WIN32
    if (getenv("DISPLAY") == NULL) {
        fprintf(stderr, "cannot find DISPLAY variable: is it set?\n");
    }
#endif // #ifndef _WIN32

    /*
     * Structure initialization:
     */
    handle = (gnuplot_ctrl*)malloc(sizeof(gnuplot_ctrl));
    handle->nplots = 0;
    gnuplot_setstyle(handle, "points");

    handle->gnucmd = popen("gnuplot", "w");
    if (handle->gnucmd == NULL) {
        fprintf(stderr, "error starting gnuplot, is gnuplot or gnuplot.exe in your path?\n");
        free(handle);
        return NULL;
    }

    // set the buffer, in an easy way
    BUF = (char*)malloc(BUF_SIZE);
    setbuf(handle->gnucmd, BUF);

    return handle;
}

void gnuplot_close(gnuplot_ctrl* handle)
{
    if (pclose(handle->gnucmd) == -1) {
        fprintf(stderr, "problem closing communication to gnuplot\n");
        return;
    }

    free(BUF);
    free(handle);
}

void gnuplot_cmd(gnuplot_ctrl* handle, char const* cmd, ...)
{
    va_list ap;

    va_start(ap, cmd);
    vfprintf(handle->gnucmd, cmd, ap);
    va_end(ap);

    fputs("\n", handle->gnucmd);
    fflush(handle->gnucmd);
}

void gnuplot_printf(gnuplot_ctrl* handle, char const* cmd, ...)
{
    va_list ap;

    va_start(ap, cmd);
    vfprintf(handle->gnucmd, cmd, ap);
    va_end(ap);

    fputs("\n", handle->gnucmd);
}

void gnuplot_setstyle(gnuplot_ctrl* handle, char* plot_style)
{
    if (strstr(plot_style, "lines") || strstr(plot_style, "points") || strstr(plot_style, "linespoints") || strstr(plot_style, "impulses") || strstr(plot_style, "dots") || strstr(plot_style, "steps") || strstr(plot_style, "errorbars") || strstr(plot_style, "boxes") || strstr(plot_style, "boxerrorbars")) {
        strcpy(handle->pstyle, plot_style);
    } else {
        fprintf(stderr, "warning: unknown requested style: using points\n");
        strcpy(handle->pstyle, "points");
    }
}

void gnuplot_set_xlabel(gnuplot_ctrl* handle, char* label)
{
    gnuplot_cmd(handle, "set xlabel \"%s\"", label);
}

void gnuplot_set_ylabel(gnuplot_ctrl* handle, char* label)
{
    gnuplot_cmd(handle, "set ylabel \"%s\"", label);
}

void gnuplot_resetplot(gnuplot_ctrl* handle)
{
    handle->nplots = 0;
}

void gnuplot_plot_x(
    gnuplot_ctrl* handle,
    double* d,
    int n,
    char* title)
{
    if (handle == NULL || d == NULL || (n < 1))
        return;
    char const* cmd = (handle->nplots > 0) ? "replot" : "plot";
    title = (title == NULL) ? "(none)" : title;

    gnuplot_cmd(handle, "%s '-' title \"%s\" with %s",
        cmd, title, handle->pstyle);

    for (int i = 0; i < n; i++) {
        gnuplot_printf(handle, "%18e", d[i]);
    }
    gnuplot_cmd(handle, "e");

    handle->nplots++;
}

void gnuplot_plot_multi_x(
    gnuplot_ctrl* handle,
    double** d,
    int n,
    int l,
    char** title)
{
    if (handle == NULL || d == NULL || (n < 1) || (l < 1))
        return;
    for (int i = 0; i < l; i++) {
        if (d[i] == NULL)
            return;
    }
    char const* cmd = (handle->nplots > 0) ? "replot" : "plot";

    if (title == NULL) {
        for (int i = 0; i < l; i++) {
            title[i] = "(none)";
        }
    } else {
        for (int i = 0; i < l; i++) {
            title[i] = (title[i] == NULL) ? "(none)" : title[i];
        }
    }

    gnuplot_printf(handle, "%s '-' title \"%s\" with %s \\",
        cmd, title[0], handle->pstyle);

    for (int i = 1; i < l; i++) {
        gnuplot_printf(handle, ", '-' title \"%s\" with %s \\",
            title[i], handle->pstyle);
    }

    gnuplot_cmd(handle, "");

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < n; j++) {
            gnuplot_printf(handle, "%18e", d[i][j]);
        }

        gnuplot_cmd(handle, "e");
    }

    handle->nplots += l;
}

void gnuplot_plot_xy(
    gnuplot_ctrl* handle,
    double* x,
    double* y,
    int n,
    char* title)
{
    if (handle == NULL || x == NULL || y == NULL || (n < 1))
        return;
    char const* cmd = (handle->nplots > 0) ? "replot" : "plot";
    title = (title == NULL) ? "(none)" : title;

    gnuplot_cmd(handle, "%s '-' title \"%s\" with %s",
        cmd, title, handle->pstyle);

    for (int i = 0; i < n; i++) {
        gnuplot_printf(handle, "%18e %18e", x[i], y[i]);
    }
    gnuplot_cmd(handle, "e");

    handle->nplots++;
}

void gnuplot_plot_x_multi_y(
    gnuplot_ctrl* handle,
    double* x,
    double** y,
    int n,
    int l,
    char** title)
{
    if (handle == NULL || x == NULL || y == NULL || (n < 1) || (l < 1))
        return;
    for (int i = 0; i < l; i++) {
        if (y[i] == NULL)
            return;
    }
    char const* cmd = (handle->nplots > 0) ? "replot" : "plot";

    if (title == NULL) {
        for (int i = 0; i < l; i++) {
            title[i] = "(none)";
        }
    } else {
        for (int i = 0; i < l; i++) {
            title[i] = (title[i] == NULL) ? "(none)" : title[i];
        }
    }

    gnuplot_cmd(handle, "%s '-' title \"%s\" with %s \\",
        cmd, title[0], handle->pstyle);

    for (int i = 1; i < l; i++) {
        gnuplot_printf(handle, ", '-' title \"%s\" with %s \\",
            title[i], handle->pstyle);
    }

    gnuplot_cmd(handle, "");

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < n; j++) {
            gnuplot_printf(handle, "%18e %18e", x[j], y[i][j]);
        }

        gnuplot_cmd(handle, "e");
    }

    handle->nplots += l;
}

void gnuplot_plot_multi_xy(
    gnuplot_ctrl* handle,
    double** x,
    double** y,
    int* n,
    int l,
    char** title)
{
    if (handle == NULL || x == NULL || y == NULL || n == NULL || (l < 1))
        return;
    for (int i = 0; i < l; i++) {
        if (x[i] == NULL || y[i] == NULL || (n[i] < 1))
            return;
    }
    char const* cmd = (handle->nplots > 0) ? "replot" : "plot";

    if (title == NULL) {
        for (int i = 0; i < l; i++) {
            title[i] = "(none)";
        }
    } else {
        for (int i = 0; i < l; i++) {
            title[i] = (title[i] == NULL) ? "(none)" : title[i];
        }
    }

    gnuplot_cmd(handle, "%s '-' title \"%s\" with %s \\",
        cmd, title[0], handle->pstyle);

    for (int i = 1; i < l; i++) {
        gnuplot_printf(handle, ", '-' title \"%s\" with %s \\",
            title[i], handle->pstyle);
    }

    gnuplot_cmd(handle, "");

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < n[i]; j++) {
            gnuplot_printf(handle, "%18e %18e", x[i][j], y[i][j]);
        }

        gnuplot_cmd(handle, "e");
    }

    handle->nplots += l;
}

void gnuplot_plot_slope(
    gnuplot_ctrl* handle,
    double a,
    double b,
    char* title)
{
    char const* cmd = (handle->nplots > 0) ? "replot" : "plot";
    title = (title == NULL) ? "(none)" : title;

    gnuplot_cmd(handle, "%s %18e * x + %18e title \"%s\" with %s",
        cmd, a, b, title, handle->pstyle);

    handle->nplots++;
}

void gnuplot_plot_equation(
    gnuplot_ctrl* handle,
    char* equation,
    char* title)
{
    char const* cmd = (handle->nplots > 0) ? "replot" : "plot";
    title = (title == NULL) ? "(none)" : title;

    gnuplot_cmd(handle, "%s %s title \"%s\" with %s",
        cmd, equation, title, handle->pstyle);
    handle->nplots++;
}

/* vim: set ts=4 et sw=4 tw=80 */

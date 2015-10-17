/*--------------------------------------------------------------------------*/
/**
  @file     gnuplot_i.h
  @author   N. Devillard
  @date     Sep 1998
  @version  $Revision: 1.11 $
  @brief    C interface to gnuplot.

  gnuplot is a freely available, command-driven graphical display tool for
  Unix. It compiles and works quite well on a number of Unix flavours as
  well as other operating systems. The following module enables sending
  display requests to gnuplot through simple C calls.

*/
/*--------------------------------------------------------------------------*/

/*
    $Id: gnuplot_i.h,v 1.11 2003/01/27 08:58:04 ndevilla Exp $
    $Author: ndevilla $
    $Date: 2003/01/27 08:58:04 $
    $Revision: 1.11 $
 */

#ifndef _GNUPLOT_PIPES_H_
#define _GNUPLOT_PIPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/
#include <stdio.h>

/*---------------------------------------------------------------------------
                                New Types
 ---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/**
  @typedef  gnuplot_ctrl
  @brief    gnuplot session handle (opaque type).

  This structure holds all necessary information to talk to a gnuplot
  session. It is built and returned by gnuplot_init() and later used
  by all functions in this module to communicate with the session, then
  meant to be closed by gnuplot_close().

  This structure is meant to remain opaque, you normally do not need
  to know what is contained in there.
 */
/*--------------------------------------------------------------------------*/

typedef struct _GNUPLOT_CTRL_ {
    /** Pipe to gnuplot process */
    FILE* gnucmd;
    /** Output buffer */
    char* BUF;

    /** Number of currently active plots */
    int nplots;
    /** Current plotting style */
    char pstyle[128];
    /** If we are in multiplot */
    int multiplot;
} gnuplot_ctrl;

/*---------------------------------------------------------------------------
                        Function ANSI C prototypes
 ---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/**
  @brief    Opens up a gnuplot session, ready to receive commands.
  @return   Newly allocated gnuplot control structure.

  This opens up a new gnuplot session, ready for input. The struct
  controlling a gnuplot session should remain opaque and only be
  accessed through the provided functions.

  The session must be closed using gnuplot_close().
 */
/*--------------------------------------------------------------------------*/
gnuplot_ctrl* gnuplot_init(void);

/*--------------------------------------------------------------------------*/
/**
  @brief    Closes a gnuplot session previously opened by gnuplot_init()
  @param    handle Gnuplot session control handle.
  @return   void

  Kills the child PID and deletes all opened temporary files.
  It is mandatory to call this function to close the handle, otherwise
  temporary files are not cleaned and child process might survive.

 */
/*--------------------------------------------------------------------------*/
void gnuplot_close(gnuplot_ctrl* handle);

/*--------------------------------------------------------------------------*/
/**
  @brief    Sends a command to an active gnuplot session.
  @param    handle Gnuplot session control handle
  @param    cmd    Command to send, same as a printf statement.

  This sends a string to an active gnuplot session, to be executed.
  There is strictly no way to know if the command has been
  successfully executed or not.
  The command syntax is the same as printf.

  Examples:

  @code
    gnuplot_cmd(g, "plot %d * x", 23.0);
    gnuplot_cmd(g, "plot %g * cos(%g * x)", 32.0, -3.0);
  @endcode

  Since the communication to the gnuplot process is run through
  a standard Unix pipe, it is only unidirectional. This means that
  it is not possible for this interface to query an error status
  back from gnuplot.
 */
/*--------------------------------------------------------------------------*/
void gnuplot_cmd(gnuplot_ctrl* handle, char const* cmd, ...);

/*--------------------------------------------------------------------------*/
/**
  @brief    Sends a string to an active gnuplot session, without flush.
  @param    handle Gnuplot session control handle
  @param    cmd    Command to send, same as a printf statement.

  This sends a string to an active gnuplot session, without flush.
  The command syntax is the same as printf.

  It is useful if you want to put a huge amount of data to gnuplot.

  Examples:

  @code
    gnuplot_cmd(g, "plot '-' with lines");

    for (int i = 0; i < n; i++) {
        gnuplot_printf(g, "%11e", data[i]);
    }
    gnuplot_cmd(g, "e");
  @endcode

  Since the communication to the gnuplot process is run through
  a standard Unix pipe, it is only unidirectional. This means that
  it is not possible for this interface to query an error status
  back from gnuplot.
 */
/*--------------------------------------------------------------------------*/
void gnuplot_printf(gnuplot_ctrl* handle, char const* cmd, ...);

/*--------------------------------------------------------------------------*/
/**
  @brief    Switch a gnuplot session from/to multiplot mode
  @param    handle Gnuplot session control handle.
  @param    other options append to "set multiplot"
  @return   void

  When in a multiplot mode, gnuplot_i will only send plot instead of replot.
  You can append your options to "set multiplot", this is optional.

 */
/*--------------------------------------------------------------------------*/
void gnuplot_multiplot(gnuplot_ctrl* handle, char* opt);

/*--------------------------------------------------------------------------*/
/**
  @brief    Change the plotting style of a gnuplot session.
  @param    handle Gnuplot session control handle
  @param    plot_style Plotting-style to use (character string)
  @return   void

  The provided plotting style is a character string. It must be one of
  the following:

  - lines
  - points
  - linespoints
  - impulses
  - dots
  - steps
  - errorbars
  - boxes
  - boxeserrorbars
 */
/*--------------------------------------------------------------------------*/
void gnuplot_setstyle(gnuplot_ctrl* handle, char* plot_style);

/*--------------------------------------------------------------------------*/
/**
  @brief    Sets the x label of a gnuplot session.
  @param    handle Gnuplot session control handle.
  @param    label Character string to use for X label.
  @return   void

  Sets the x label for a gnuplot session.
 */
/*--------------------------------------------------------------------------*/
void gnuplot_set_xlabel(gnuplot_ctrl* handle, char* label);

/*--------------------------------------------------------------------------*/
/**
  @brief    Sets the y label of a gnuplot session.
  @param    handle Gnuplot session control handle.
  @param    label Character string to use for Y label.
  @return   void

  Sets the y label for a gnuplot session.
 */
/*--------------------------------------------------------------------------*/
void gnuplot_set_ylabel(gnuplot_ctrl* handle, char* label);

/*--------------------------------------------------------------------------*/
/**
  @brief    Resets a gnuplot session (next plot will erase previous ones).
  @param    handle Gnuplot session control handle.
  @return   void

  Resets a gnuplot session, i.e. the next plot will erase all previous
  ones.
 */
/*--------------------------------------------------------------------------*/
void gnuplot_resetplot(gnuplot_ctrl* handle);

/*--------------------------------------------------------------------------*/
/**
  @brief    Plots a 2d graph from a list of double.
  @param    handle  Gnuplot session control handle.
  @param    d       Array of double.
  @param    n       Number of values in the passed array.
  @param    title   Title of the plot.
  @return   void

  Plots out a 2d graph from a list of double. The x-coordinate is the
  index of the double in the list, the y coordinate is the double in
  the list.

  Example:

  @code
    gnuplot_ctrl* h;
    double d[50];
    int i;

    h = gnuplot_init();
    for (i = 0; i < 50; i++) {
        d[i] = i * i;
    }
    gnuplot_plot_x(h, d, 50, "parabola");
    sleep(2);
    gnuplot_close(h);
  @endcode
 */
/*--------------------------------------------------------------------------*/
void gnuplot_plot_x(
    gnuplot_ctrl* handle,
    double* d,
    int n,
    char* title);

/*--------------------------------------------------------------------------*/
/**
  @brief    Plots a 2d graph from several lists of double.
  @param    handle  Gnuplot session control handle.
  @param    d       Pointer to the arrays of double.
  @param    n       Number of values in the passed array.
  @param    l       Number of lists.
  @param    title   Pointer to titles of the plot.
  @return   void

  Plots out a 2d graph from several lists of double with the same
  length. The x-coordinate is the index of the double in the list,
  the y coordinate is the double in the list.

  It is useful if you want to draw more than one line to a file

  Example:

  @code
    gnuplot_ctrl* h;
    double** d = (double**)malloc(sizeof(double*) * 2);
    d[0] = (double*)malloc(sizeof(double) * 50);
    d[1] = (double*)malloc(sizeof(double) * 50);
    char t0[] = "2^i";
    char t1[] = "i^2";
    char* t[2] = { t0, t1 };
    int i;

    h = gnuplot_init();
    for (i = 0; i < 50; i++) {
        d[0][i] = pow(2, i);
        d[1][i] = pow(i, 2);
    }
    gnuplot_plot_multi_x(h, d, 50, 2, t);
    sleep(2);
    free(d[0]);
    free(d[1]);
    free(d);
    gnuplot_close(h);
  @endcode
 */
/*--------------------------------------------------------------------------*/
void gnuplot_plot_multi_x(
    gnuplot_ctrl* handle,
    double** d,
    int n,
    int l,
    char** title);

/*--------------------------------------------------------------------------*/
/**
  @brief    Plot a 2d graph from a list of points.
  @param    handle      Gnuplot session control handle.
  @param    x           Pointer to a list of x coordinates.
  @param    y           Pointer to a list of y coordinates.
  @param    n           Number of double in x (assumed the same as in y).
  @param    title       Title of the plot.
  @return   void

  Plots out a 2d graph from a list of points. Provide points through a list
  of x and a list of y coordinates. Both provided arrays are assumed to
  contain the same number of values.

  Example:

  @code
    gnuplot_ctrl* h;
    double x[50];
    double y[50];
    int i;

    h = gnuplot_init();
    for (i = 0; i < 50; i++) {
        x[i] = (double)(i) / 10.0;
        y[i] = x[i] * x[i];
    }
    gnuplot_plot_xy(h, x, y, 50, "parabola");
    sleep(2);
    gnuplot_close(h);
  @endcode
 */
/*--------------------------------------------------------------------------*/
void gnuplot_plot_xy(
    gnuplot_ctrl* handle,
    double* x,
    double* y,
    int n,
    char* title);

/*--------------------------------------------------------------------------*/
/**
  @brief    Plot a 2d graph from several lists of double with the same x.
  @param    handle      Gnuplot session control handle.
  @param    x           Pointer to a list of x coordinates.
  @param    y           Pointer to lists of y coordinates.
  @param    n           Number of double in x (assumed the same as in y).
  @param    l           Number of lists.
  @param    title       Pointer to titles of the plot.
  @return   void

  Plots out a 2d graph from several lists of points with the same x.
  Provide points through a list of x and several lists of y coordinates.
  All provided arrays are assumed to contain the same number of values.

  It is useful if you want to draw more than one line to a file

  Example:

  @code
    gnuplot_ctrl* h;
    double x[50];
    double** y = (double**)malloc(sizeof(double*) * 2);
    y[0] = (double*)malloc(sizeof(double) * 50);
    y[1] = (double*)malloc(sizeof(double) * 50);
    char t0[] = "2^i";
    char t1[] = "i^2";
    char* t[2] = { t0, t1 };
    int i;

    h = gnuplot_init();
    for (i = 0; i < 50; i++) {
        x[i] = (double)(i) / 10.0;
        y[0][i] = pow(2, i);
        y[1][i] = pow(i, 2);
    }
    gnuplot_plot_x_multi_y(h, x, y, 50, 2, t);
    sleep(2);

    free(y[0]);
    free(y[1]);
    free(y);
    gnuplot_close(h);
  @endcode
 */
/*--------------------------------------------------------------------------*/
void gnuplot_plot_x_multi_y(
    gnuplot_ctrl* handle,
    double* x,
    double** y,
    int n,
    int l,
    char** title);

/*--------------------------------------------------------------------------*/
/**
  @brief    Plot a 2d graph from several lists of double.
  @param    handle      Gnuplot session control handle.
  @param    x           Pointer to lists of x coordinates.
  @param    y           Pointer to lists of y coordinates.
  @param    n           Pointer to numbers of double in x.
  @param    l           Number of lists.
  @param    title       Pointer to titles of the plot.
  @return   void

  Plots out a 2d graph from several lists of points. Provide points
  through several lists of x and several lists of y coordinates. All
  provided arrays are assumed to contain the same number of values.

  It is useful if you want to draw more than one line to a file

  Example:

  @code
    gnuplot_ctrl* h;
    double** x = (double**)malloc(sizeof(double*) * 2);
    x[0] = (double*)malloc(sizeof(double) * 50);
    x[1] = (double*)malloc(sizeof(double) * 50);
    double** y = (double**)malloc(sizeof(double*) * 2);
    y[0] = (double*)malloc(sizeof(double) * 50);
    y[1] = (double*)malloc(sizeof(double) * 50);
    int a[2] = { 50, 50 };
    char t0[] = "2^i";
    char t1[] = "i^2";
    char* t[2] = { t0, t1 };
    int i;

    h = gnuplot_init();
    for (i = 0; i < 50; i++) {
        x[0][i] = (double)(i) / 10.0;
        x[1][i] = (double)(i) / 10.0 + 1.0;
        y[0][i] = pow(2, i);
        y[1][i] = pow(i, 2);
    }
    gnuplot_plot_multi_xy(h, x, y, a, 2, t);
    sleep(2);

    free(x[0]);
    free(x[1]);
    free(x);
    free(y[0]);
    free(y[1]);
    free(y);
    gnuplot_close(h);
  @endcode
 */
/*--------------------------------------------------------------------------*/
void gnuplot_plot_multi_xy(
    gnuplot_ctrl* handle,
    double** x,
    double** y,
    int* n,
    int l,
    char** title);

/*--------------------------------------------------------------------------*/
/**
  @brief    Plot a slope on a gnuplot session.
  @param    handle      Gnuplot session control handle.
  @param    a           Slope.
  @param    b           Intercept.
  @param    title       Title of the plot.
  @return   void

  Plot a slope on a gnuplot session. The provided slope has an
  equation of the form y=ax+b

  Example:

  @code
    gnuplot_ctrl* h;
    double a, b;

    h = gnuplot_init();
    gnuplot_plot_slope(h, 1.0, 0.0, "unity slope");
    sleep(2);
    gnuplot_close(h);
  @endcode
 */
/*--------------------------------------------------------------------------*/
void gnuplot_plot_slope(
    gnuplot_ctrl* handle,
    double a,
    double b,
    char* title);

/*--------------------------------------------------------------------------*/
/**
  @brief    Plot a curve of given equation y=f(x).
  @param    handle      Gnuplot session control handle.
  @param    equation    Equation to plot.
  @param    title       Title of the plot.
  @return   void

  Plots out a curve of given equation. The general form of the
  equation is y=f(x), you only provide the f(x) side of the equation.

  Example:

  @code
    gnuplot_ctrl* h;
    char eq[80];

    h = gnuplot_init();
    strcpy(eq, "sin(x) * cos(2 * x)");
    gnuplot_plot_equation(h, eq, "sine wave", normal);
    gnuplot_close(h);
  @endcode
 */
/*--------------------------------------------------------------------------*/
void gnuplot_plot_equation(
    gnuplot_ctrl* handle,
    char* equation,
    char* title);

#ifdef __cplusplus
}
#endif

#endif

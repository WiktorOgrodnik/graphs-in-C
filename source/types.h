#ifndef TYPES_H
#define TYPES_H

#define WINDOWS

#ifdef WINDOWS
#define _USE_MATH_DEFINES
#else
#define _GNU_SOURCE
#endif

#define CHART_WIDTH 800
#define CHART_HEIGHT 600

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

typedef struct eqData_
{
    //Application elements
    GtkWidget* equation[4];
    GtkWidget* interval;
    GtkWidget* res;
    GtkWidget* chartLegendLeft[11];
    GtkWidget* chartLegendBottom[11];

    GdkPixbuf* chartData;
    GtkWidget* chart;

    GtkWidget* chartLegendLeftBox;
    GtkWidget* chartLegendBottomBox;

    GtkWidget* rasterizationCheckBox;
    GtkWidget* experimentalMicroSamplingCheckBox;

    //Settings
    bool rasterization;
    bool microSampling;
    
} eqData;

#endif
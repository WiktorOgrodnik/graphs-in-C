#ifndef TYPES_H
#define TYPES_H

#define WINDOWS

#ifdef WINDOWS
#define _USE_MATH_DEFINES
#else
#define _GNU_SOURCE
#endif

#define CHART_WIDTH 600
#define CHART_HEIGHT CHART_WIDTH

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gstdio.h>

typedef struct eqData_
{
    //Application elements
    GtkWidget* chartLegendLeft[11];
    GtkWidget* chartLegendBottom[11];

    GdkPixbuf* chartData;
    GtkWidget* chart;

    GtkEntryBuffer* intervalBuffer;
    GtkEntryBuffer* scaleBuffer;
    GtkEntryBuffer* equationBuffer[4];

    //Settings
    bool rasterization;
    bool microSampling;
    
} eqData;

#endif
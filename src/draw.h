#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gstdio.h>

#include "eval.h"

#define CHART_WIDTH 600
#define CHART_HEIGHT CHART_WIDTH

typedef struct eqData {
    //Application elements
    GtkWidget* chartLegendLeft [11];
    GtkWidget* chartLegendBottom [11];

    GdkPixbuf* chartData;
    GtkWidget* chart;

    GtkEntryBuffer* intervalBuffer;
    GtkEntryBuffer* scaleBuffer;
    GtkEntryBuffer* equationBuffer [4];
    bool derivative [4];

    //Settings
    bool rasterization;
    bool microSampling;
    bool darkmode;
    
} eqData;

//Read equations and draw grpah
void draw_chart(eqData* data, char* error_message);

//Draw grpah grid
void draw_put_lines_to_chart(GdkPixbuf* pixbuf, bool darkmode);

//Match the legend with an interval
void draw_make_legend(GtkWidget* chartLeft[], gdouble delta);

#endif
#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

typedef struct eqData_
{
    GtkWidget* equation;
    GtkWidget* interval;
    GtkWidget* res;
    GtkWidget* chartLegendLeft[11];
    GtkWidget* chartLegendBottom[11];

    GdkPixbuf* chartData;
    GtkWidget* chart;

    GtkWidget* chartLegendLeftBox;
    GtkWidget* chartLegendBottomBox;

} eqData;

#endif
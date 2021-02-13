#ifndef DRAW_H
#define DRAW_H

#include "eval.h"
#include "types.h"
#include "convert.h"

//Read equations and draw grpah
void draw_chart (GtkWidget* widget, eqData* data, char* error_message);

//Draw grpah grid
void put_lines_to_chart(GdkPixbuf* pixbuf);

//Match the legend with an interval
void draw_make_legend(GtkWidget* chartLeft[], gdouble delta);

#endif
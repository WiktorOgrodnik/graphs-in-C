#ifndef DRAW_H
#define DRAW_H

#include "eval.h"

//Read equations and draw grpah
void draw_chart (eqData* data, char* error_message);

//Draw grpah grid
void draw_put_lines_to_chart (GdkPixbuf* pixbuf, bool darkmode);

//Match the legend with an interval
void draw_make_legend (GtkWidget* chartLeft [], gdouble delta);

#endif
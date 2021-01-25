#ifndef DRAW_H
#define DRAW_H

#include "eval.h"
#include "convert.h"
#include "types.h"

void draw_chart (GtkWidget *widget, eqData* data);
void put_lines_to_chart(GdkPixbuf* pixbuf, gdouble l, gdouble p);
void makeLegend(GtkWidget* chartLeft[], gdouble delta);

#endif
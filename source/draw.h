#ifndef DRAW_H
#define DRAW_H

#include "eval.h"
#include "types.h"
#include "convert.h"

void draw_chart (GtkWidget *widget, eqData* data);
void put_lines_to_chart(GdkPixbuf* pixbuf, gdouble l, gdouble p);
void draw_make_legend(GtkWidget* chartLeft[], gdouble delta);

#endif
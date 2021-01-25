#ifndef DRAW_H
#define DRAW_H

#include "eval.h"
#include "ui.h"

void draw_chart (GtkWidget *widget, eqData* data);
void put_lines_to_chart(GdkPixbuf* pixbuf, gdouble l, gdouble p);

#endif
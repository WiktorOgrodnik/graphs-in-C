#ifndef UI_H
#define UI_H

#include <types.h>
#include <convert.h>

GtkWidget* ui_menu();
void ui_init_inputs(eqData* data);
void ui_make_legend(GtkWidget* chartLeft[], gdouble delta);

#endif
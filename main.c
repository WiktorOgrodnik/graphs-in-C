#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "ui.h"

static void destroy (GtkWidget* widget, gpointer data);
static void calculate (GtkWidget* widget, gpointer data);

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window, *vbox, *menubar, *button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_window_set_default_size(GTK_WINDOW(window), (gint)(600), (gint)200);
    gtk_window_set_title(GTK_WINDOW(window), "Wykresy");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    button = gtk_button_new_with_label("Wprowadź");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    menubar = ui_menu(); 

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    GtkWidget* box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget* boxt = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_container_set_border_width(GTK_CONTAINER(box1), 10);

    eqData inputs;

    ui_init_inputs(&inputs);

    inputs.chartData = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 800, 600);
    put_lines_to_chart(inputs.chartData, -10.0, 10.0);
    inputs.chart = gtk_image_new_from_pixbuf(inputs.chartData);
    gtk_widget_set_margin_top(inputs.chart, 4);

    GtkWidget* box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(box2), 10);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(calculate), (gpointer)&inputs);

    GtkWidget* boxEntryProps = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_bottom(boxEntryProps, 20);

    GtkWidget* label1 = gtk_label_new("Przedział: ");
    GtkWidget* label2 = gtk_label_new("Skala: ");
    gtk_widget_set_margin_start(label2, 10);

    gtk_box_pack_start(GTK_BOX(boxEntryProps), label1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(boxEntryProps), inputs.interval, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(boxEntryProps), label2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(boxEntryProps), inputs.res, TRUE, TRUE, 0);

    GtkWidget* boxEntryFunc = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_bottom(boxEntryFunc, 20);

    GtkWidget* label3 = gtk_label_new("f(x) = ");

    gtk_box_pack_start(GTK_BOX(boxEntryFunc), label3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(boxEntryFunc), inputs.equation, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(box1), inputs.chartLegendLeftBox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(boxt), inputs.chart, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(boxt), inputs.chartLegendBottomBox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box1), boxt, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box2), boxEntryProps, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box2), boxEntryFunc, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box2), button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), box1, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), box2, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

static void destroy (GtkWidget* widget, gpointer data) 
{
    gtk_main_quit();
}

static void calculate (GtkWidget *widget, gpointer data) 
{
    draw_chart(widget, (eqData*)data);
}
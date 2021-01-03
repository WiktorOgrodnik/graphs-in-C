#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "eval.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define LENGTH 1000

struct eqData
{
    GtkWidget* equation;
    GtkWidget* intervalL;
    GtkWidget* intervalP;
};

GdkPixbuf* chartData;
GtkWidget* chart;

void scan_statement(char* statement);

static void destroy (GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}

static void put_pixel (GdkPixbuf* pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha)
{
    int width, height, rowstride, n_channels;
    guchar *pixels, *p;

    n_channels = gdk_pixbuf_get_n_channels (pixbuf);

    g_assert (gdk_pixbuf_get_colorspace(pixbuf) == GDK_COLORSPACE_RGB);
    g_assert (gdk_pixbuf_get_bits_per_sample(pixbuf) == 8);
    g_assert (gdk_pixbuf_get_has_alpha (pixbuf));
    g_assert (n_channels == 4);

    width = gdk_pixbuf_get_width (pixbuf);
    height = gdk_pixbuf_get_height (pixbuf);

    if (x < 0 || x >= width) {g_print("No nie %d x\n", x); return;}
    if (y < 0 || y >= height) {g_print("No nie %d y\n", y); return;}

    rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    pixels = gdk_pixbuf_get_pixels(pixbuf);

    p = pixels + y * rowstride + x * n_channels;
    p[0] = red;
    p[1] = green;
    p[2] = blue;
    p[3] = alpha;
}

static void calculate (GtkWidget *widget, gpointer data_) 
{
    struct eqData* data = (struct eqData*) data_;
    
    const gchar* wejscie, *intervalL, *intervalP;
    gchar* eptr;
    gdouble l, p, delta;
    gint r;

    wejscie = gtk_entry_get_text(GTK_ENTRY(data->equation));
    intervalL = gtk_entry_get_text(GTK_ENTRY(data->intervalL));
    intervalP = gtk_entry_get_text(GTK_ENTRY(data->intervalP));

    l = strtod(intervalL, &eptr); p = strtod(intervalP, &eptr);
    r = (gint)800;
    delta = (p - l) / (gdouble)r;

    //gdouble wyniki[r + 1];

    for (gint i = 0; i < r; i++)
    {
        bool dziedzina = true;
        double wynik = calc(wejscie, strlen(wejscie), l, &dziedzina);

        //wyniki[i] = wynik;
        if (dziedzina) put_pixel(chartData, i, 300 - wynik, 0, 0, 255, 255);
        //if (dziedzina) g_print("f(%lf) = %lf\n", l, wynik);
        //else g_print("f(%lf) = \"Podza dziedzina!\"\n", l);
        l+=delta;
    }

    gtk_image_set_from_pixbuf(GTK_IMAGE(chart), chartData);

    //g_print("%lf\n", calc(wejscie, strlen(wejscie), 1.0));
}

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget* window, *vbox, *menubar, *button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_window_set_default_size(GTK_WINDOW(window), (gint)(600), (gint)200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    button = gtk_button_new_with_label("Wprowadź");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    menubar = gtk_menu_bar_new();

    GtkWidget *file_item, *quit_item;
    GtkWidget *file_menu;

    file_item = gtk_menu_item_new_with_label("Plik");
    file_menu = gtk_menu_new();
    quit_item = gtk_menu_item_new_with_label("Zakończ");

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    file_item = gtk_menu_item_new_with_label("Pomoc");
    file_menu = gtk_menu_new();
    quit_item = gtk_menu_item_new_with_label("Wykresy - informacje");

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    g_signal_connect(G_OBJECT(quit_item), "activate", G_CALLBACK(destroy), NULL);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    GtkWidget* box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(box1), 10);

    chartData = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 800, 600);
    chart = gtk_image_new_from_pixbuf(chartData);

    struct eqData inputs;

    inputs.equation = gtk_entry_new(); inputs.intervalL = gtk_entry_new(); inputs.intervalP = gtk_entry_new(); //inputs.res = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(inputs.equation), (gint)LENGTH);
    gtk_entry_set_max_length(GTK_ENTRY(inputs.intervalL), (gint)50);
    gtk_entry_set_max_length(GTK_ENTRY(inputs.intervalP), (gint)50);
    //gtk_entry_set_max_length(GTK_ENTRY(inputs.res), (gint)50);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(calculate), (gpointer)&inputs);

    gtk_box_pack_start(GTK_BOX(box1), chart, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box1), inputs.equation, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box1), inputs.intervalL, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box1), inputs.intervalP, TRUE, TRUE, 0);
    //gtk_box_pack_start(GTK_BOX(box1), inputs.res, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), box1, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    /*char str[LENGTH];

    scan_statement(str);

    printf("%lf", calc(str, strlen(str), 1.0));

    return 0;*/
}

void scan_statement(char* statement)
{
    char c = getchar();

    while(isspace(c)) c = getchar();

    while(true)
    {
        *statement++ = c;
        if(c == '\n') break;
        c = getchar();
    }

    *(statement - 1) = '\0';
}
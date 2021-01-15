#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "eval.h"

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define LENGTH 1000

struct eqData
{
    GtkWidget* equation;
    GtkWidget* interval;
    GtkWidget* res;
    GtkWidget* chartLegendLeft[9];
    GtkWidget* chartLegendBottom[9];
};

GdkPixbuf* chartData;
GtkWidget* chart;

static void put_lines_to_chart(GdkPixbuf* pixbuf, gdouble l, gdouble p);
static void destroy (GtkWidget *widget, gpointer data);
static void about_run (GtkWidget *widget, gpointer data);
static void put_pixel (GdkPixbuf* pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha);
static void calculate (GtkWidget *widget, gpointer data_);

void double_to_gchar(gdouble a, int afterpoint, gchar str[]);
void makeLegend(GtkWidget* chart[], gdouble l, gdouble p, bool axis);

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window, *vbox, *menubar, *button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_window_set_default_size(GTK_WINDOW(window), (gint)(600), (gint)200);
    gtk_window_set_title(GTK_WINDOW(window), "Wykresy");
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

    g_signal_connect(G_OBJECT(quit_item), "activate", G_CALLBACK(destroy), NULL);

    file_item = gtk_menu_item_new_with_label("Pomoc");
    file_menu = gtk_menu_new();
    quit_item = gtk_menu_item_new_with_label("Wykresy - informacje");

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    g_signal_connect(G_OBJECT(quit_item), "activate", G_CALLBACK(about_run), NULL);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    GtkWidget* box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget* boxt = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_container_set_border_width(GTK_CONTAINER(box1), 10);

    struct eqData inputs;

    GtkWidget* chartLegendLeft = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* chartLegendBottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    for (gint i = 0; i < 9; i++)
    {
        inputs.chartLegendLeft[i] = gtk_label_new("");
        inputs.chartLegendBottom[i] = gtk_label_new("");

        gtk_box_pack_start(GTK_BOX(chartLegendLeft), inputs.chartLegendLeft[i], TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(chartLegendBottom), inputs.chartLegendBottom[i], TRUE, TRUE, 0);
    }

    makeLegend(inputs.chartLegendLeft, -300, 300, 0);
    makeLegend(inputs.chartLegendBottom, -10, 10, 1);


    GtkWidget* box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(box2), 10);

    chartData = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 800, 600);
    put_lines_to_chart(chartData, -10.0, 10.0);
    chart = gtk_image_new_from_pixbuf(chartData);

    inputs.equation = gtk_entry_new(); inputs.interval = gtk_entry_new(); inputs.res = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(inputs.equation), (gint)LENGTH);
    gtk_entry_set_max_length(GTK_ENTRY(inputs.interval), (gint)50);
    gtk_entry_set_max_length(GTK_ENTRY(inputs.res), (gint)50);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(calculate), (gpointer)&inputs);

    gtk_box_pack_start(GTK_BOX(box1), chartLegendLeft, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(boxt), chart, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(boxt), chartLegendBottom, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box1), boxt, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box2), inputs.equation, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box2), inputs.interval, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box2), inputs.res, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box2), button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), box1, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), box2, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

static void put_lines_to_chart(GdkPixbuf* pixbuf, gdouble l, gdouble p)
{
    for (gint i = 0; i < 800; i++)
    {
        put_pixel(pixbuf, i, 299, 0, 0, 0, 255);
        put_pixel(pixbuf, i, 300, 0, 0, 0, 255);
    }

    gdouble s = l, t = 0.0, delta = (p - l) / 800.0;

    for (gint i = 0; i < 800; i++)
    {   
        s += delta;

        if (t * s <= 0 && i)
        {
            for (gint j = 0; j < 600; j++)
            {
                put_pixel(pixbuf, i - 1, j, 0, 0, 0, 255);
                put_pixel(pixbuf, i, j, 0, 0, 0, 255);
            }

            break;
        }

        t = s;
    }
    
}

static void destroy (GtkWidget *widget, gpointer data) 
{
    gtk_main_quit();
}

static void about_run (GtkWidget *widget, gpointer data)
{
    GtkWidget* about = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), "Wykresy");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), "0.1");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "c Wiktor Ogrodnik 2021");
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about), GTK_LICENSE_MIT_X11);

    gtk_dialog_run(GTK_DIALOG(about));
    gtk_widget_destroy(about);
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

    if (x < 0 || x >= width) return;
    if (y < 0 || y >= height) return;

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
    
    const gchar* wejscie, *intervalL, *intervalP, *scale_;
    gchar* eptr;
    gdouble l, p, delta, scale, lorg;
    gint r;

    wejscie = gtk_entry_get_text(GTK_ENTRY(data->equation));
    intervalL = gtk_entry_get_text(GTK_ENTRY(data->intervalL));
    intervalP = gtk_entry_get_text(GTK_ENTRY(data->intervalP));
    scale_ = gtk_entry_get_text(GTK_ENTRY(data->res));

    if (strcmp(intervalL, "") == 0) l = -10.0;
    else l = strtod(intervalL, &eptr); 

    if (strcmp(intervalP, "") == 0) p = 10.0;
    p = strtod(intervalP, &eptr); 

    r = (gint)800;
    delta = (p - l) / (gdouble)r;

    lorg = l;

    gdouble wyniki[r];
    gdouble max = -800.0;

    chartData = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 800, 600);
    put_lines_to_chart(chartData, l, p);

    for (gint i = 0; i < r; i++)
    {
        double wynik = calc(wejscie, strlen(wejscie), l);

        wyniki[i] = wynik;

        if (!isnan(wynik) && wynik > max) max = wynik;
        l+=delta;
    }

    max += max * 0.4;
    if (max > 300) max = 300;

    if (strcmp(scale_, "") == 0) scale = 300 / max;
    else scale = strtod(scale_, &eptr);

    makeLegend(data->chartLegendLeft, 0, (300/scale) / 4, 0);
    makeLegend(data->chartLegendBottom, lorg, p, 1);

    for (gint i = 0; i < r; i++) 
    {
        wyniki[i] = (gint)(300 - (wyniki[i]) * scale);

        if (!isnan(wyniki[i])) put_pixel(chartData, i, wyniki[i], 0, 0, 255, 255);

        if (i && !isnan(wyniki[i]))
        {
            gint start = wyniki[i - 1] < wyniki[i] ? (gint)wyniki[i - 1] : (gint)wyniki[i];
            gint end = wyniki[i - 1] < wyniki[i] ? (gint)wyniki[i] : (gint)wyniki[i - 1];

            if (end - start >= 2 && end - start < 500)
            {
                for (gint j = start; j < end; j++)
                {
                    put_pixel(chartData, i, j, 0, 0, 255, 255);
                }
            }
        }
    }
    
    gtk_image_set_from_pixbuf(GTK_IMAGE(chart), chartData);
}

void reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
} 

int int_to_string(int a, gchar str[], int d)
{
    if (a == 0) 
    {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    int i = 0; 
    while (a) { 
        str[i++] = (a % 10) + '0'; 
        a = a / 10; 
    }

    while (i < d) 
        str[i++] = '0';  
  
    reverse(str, i); 
    str[i] = '\0'; 

    return i;
}

void double_to_gchar(gdouble a, int afterpoint, gchar str[])
{ 
    int ipart = (int)a; 
    double dpart = a - (double)ipart; 
  
    int i = int_to_string(ipart, str, 0); 
  
    if (afterpoint != 0) { 

        str[i] = ','; 

        dpart = dpart * pow(10, afterpoint); 
  
        int_to_string((int)dpart, str + i + 1, afterpoint); 
    } 
}

void makeLegend(GtkWidget* chartLeft[], gdouble l, gdouble p, bool axis)
{
    if (axis)
    {
        gdouble delta = (p - l) / 9.0;

        for (gint i = 1; i <= 9; i++)
        {
            gchar str[20];
            gdouble lgchar = l;
            if (lgchar < 0) lgchar *= -1; 
            double_to_gchar(lgchar, 2, str);
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), str);

            l+=delta;
        }

        return;
    }

    gdouble delta = p / 4;
    for (gint i = 1; i <= 9; i++)
    {
        if (i < 5)
        {
            gchar str[20]; double_to_gchar((gdouble)((5 - i) * delta), 2, str);
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), str);
        }
        else if (i == 5)
        {
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), "0");
        }
        else
        {   
            gchar str[20]; double_to_gchar((gdouble)((i - 5) * delta), 2, str);
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), str);
        }
    }
}
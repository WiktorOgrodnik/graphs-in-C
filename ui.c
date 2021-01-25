#include "ui.h"

static void destroy (GtkWidget* widget, gpointer data);
static void about_run (GtkWidget* widget, gpointer data);

GtkWidget* ui_menu()
{
    GtkWidget* menubar = gtk_menu_bar_new();

    GtkWidget *file_item, *quit_item, *file_menu;

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

    return menubar;
}

void ui_init_inputs(eqData* data)
{
    data->chartLegendLeftBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    data->chartLegendBottomBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    for (gint i = 0; i < 11; i++)
    {
        data->chartLegendLeft[i] = gtk_label_new("");
        data->chartLegendBottom[i] = gtk_label_new("");

        gtk_box_pack_start(GTK_BOX(data->chartLegendLeftBox), data->chartLegendLeft[i], FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(data->chartLegendBottomBox), data->chartLegendBottom[i], FALSE, FALSE, 0);
    }

    for(gint i = 0; i < 10; i++)
    {
        gtk_widget_set_margin_end(data->chartLegendBottom[i], 65);
        gtk_widget_set_margin_bottom(data->chartLegendLeft[i], 45);
    }

    ui_make_legend(data->chartLegendLeft, (gdouble)300 / (gdouble)5);
    ui_make_legend(data->chartLegendBottom, (gdouble)10 / (gdouble)5);

    data->equation = gtk_entry_new(); data->interval = gtk_entry_new(); data->res = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(data->equation), (gint)1000);
    gtk_entry_set_max_length(GTK_ENTRY(data->interval), (gint)20);
    gtk_entry_set_max_length(GTK_ENTRY(data->res), (gint)20);

    gtk_entry_set_text(GTK_ENTRY(data->interval), "10.0");
    gtk_entry_set_text(GTK_ENTRY(data->res), "30.0");

    gtk_widget_set_margin_end(data->res, (gint)300);

}

void ui_make_legend(GtkWidget* chartLeft[], gdouble delta)
{
    for (gint i = 1; i <= 11; i++)
    {
        if (i < 6)
        {
            gchar str[20]; double_to_char((gdouble)((6 - i) * delta), 2, str);
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), str);
        }
        else if (i == 6)
        {
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), "0");
        }
        else
        {   
            gchar str[20]; double_to_char((gdouble)((i - 6) * delta), 2, str);
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), str);
        }
    }
}

static void destroy (GtkWidget* widget, gpointer data) 
{
    gtk_main_quit();
}

static void about_run (GtkWidget* widget, gpointer data)
{
    GtkWidget* about = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), "Wykresy");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), "0.1");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "c Wiktor Ogrodnik 2021");
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about), GTK_LICENSE_MIT_X11);

    gtk_dialog_run(GTK_DIALOG(about));
    gtk_widget_destroy(about);
}
#include "main.h"

static void activate (GtkApplication* app, gpointer data);
static void calculate (GtkWindow* window);
static void quit (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void about (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void checkbox_rasterization_toggle (GtkWidget* widget, gpointer data);
static void checkbox_experimental_toggle (GtkWidget* widget, gpointer data);

static GActionEntry app_entries[] =
{
  { "about", about, NULL, NULL, NULL, {0, 0, 0}},
  { "quit", quit, NULL, NULL, NULL, {0, 0, 0}}
};

int main (int argc, char* argv[])
{

#ifdef GTK_SRCDIR
    g_chdir(GTK_SRCDIR);
#endif

    app = gtk_application_new ("wiktor.ogrodnik.uwr.wykresy", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run (G_APPLICATION(app), argc, argv);

    g_object_unref (app);
    return status;
}

static void activate (GtkApplication* app, gpointer data)
{
    wdata.chartData = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, CHART_WIDTH, CHART_HEIGHT);
    draw_put_lines_to_chart (wdata.chartData);

    GtkBuilder* builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "schema/interface.xml", NULL);

    g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);

    GObject* window = gtk_builder_get_object (builder, "window");
    gtk_window_set_application (GTK_WINDOW(window), app);

    GObject* gears = gtk_builder_get_object (builder, "gears");
    GMenuModel* menu = G_MENU_MODEL (gtk_builder_get_object (builder, "menu"));
    gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (gears), menu);

    GObject* chart = gtk_builder_get_object (builder, "chart");
    gtk_image_set_from_pixbuf (GTK_IMAGE(chart), wdata.chartData);
    gtk_image_set_pixel_size (GTK_IMAGE(chart), CHART_WIDTH);
    wdata.chart = GTK_WIDGET(chart);

    GObject* bottomLegend = gtk_builder_get_object (builder, "gridLB");
    GObject* leftLegend = gtk_builder_get_object (builder, "gridLL");

    for(gint i = 0; i < 11; i++)
    {
        wdata.chartLegendBottom[i] = gtk_label_new ("");
        gtk_widget_set_hexpand (wdata.chartLegendBottom[i], TRUE);
        gtk_grid_attach(GTK_GRID(bottomLegend), wdata.chartLegendBottom[i], i, 0, 1, 1);

        wdata.chartLegendLeft[i] = gtk_label_new ("");
        gtk_widget_set_vexpand (wdata.chartLegendLeft[i], TRUE);
        gtk_grid_attach (GTK_GRID(leftLegend), wdata.chartLegendLeft[i], 0, i, 1, 1);
    }

    draw_make_legend (wdata.chartLegendBottom, 2.0);
    draw_make_legend (wdata.chartLegendLeft, 2.0);

    wdata.intervalBuffer = gtk_entry_buffer_new ("10.0", 4);
    wdata.scaleBuffer = gtk_entry_buffer_new ("30.0", 4);

    GObject* intervalEntry = gtk_builder_get_object (builder, "entry1");
    gtk_entry_set_buffer (GTK_ENTRY(intervalEntry), wdata.intervalBuffer);

    GObject* scaleEntry = gtk_builder_get_object (builder, "entry2");
    gtk_entry_set_buffer (GTK_ENTRY(scaleEntry), wdata.scaleBuffer);

    for(gint i = 0; i < 4; i++)
    {
        wdata.equationBuffer[i] = gtk_entry_buffer_new ("", 0);
        char equationName[16]; equationName[0] = '\0';
        sprintf (equationName, "entryEquation%d", i);

        GObject* entry = gtk_builder_get_object (builder, equationName);
        gtk_entry_set_buffer (GTK_ENTRY(entry), wdata.equationBuffer[i]);
    }

    GObject* button = gtk_builder_get_object (builder, "buttonRun");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (calculate), window);

    wdata.rasterization = true;

    GObject* checkBox = gtk_builder_get_object (builder, "checkbox1");
    g_signal_connect (checkBox, "toggled", G_CALLBACK(checkbox_rasterization_toggle), &wdata);

    checkBox = gtk_builder_get_object (builder, "checkbox2");
    g_signal_connect (checkBox, "toggled", G_CALLBACK(checkbox_experimental_toggle), &wdata);

    gtk_widget_show (GTK_WIDGET(window));

    g_object_unref (builder);
}

static void calculate (GtkWindow* window)
{
    char message [100]; message[0] = '\0';
    draw_chart (&wdata, message);

    if (strlen(message))
    {
        GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
        GtkWidget* dialog = gtk_message_dialog_new (window, flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);

        g_signal_connect (dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

        gtk_widget_show (dialog);
    }
}

static void quit (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    g_application_quit (G_APPLICATION (app));
}

static void about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    const char license[] = "MIT License\n\nCopyright (c) 2021 Wiktor Ogrodnik\n\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files (the \"Software\"), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.\n";

    GtkWidget* about = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(about), "Wykresy");
    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(about), "0.1");
    gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(about), "c Wiktor Ogrodnik 2021");
    gtk_about_dialog_set_license (GTK_ABOUT_DIALOG(about), license);

    gtk_widget_show (about);
}

static void checkbox_rasterization_toggle (GtkWidget* widget, gpointer data)
{
    /**
     * @brief this function operate rasterization checkbox
     * 
     * @return void
     */

    if(gtk_check_button_get_active (GTK_CHECK_BUTTON(widget)))
        wdata.rasterization = true;
    else
        wdata.rasterization = false;
}

static void checkbox_experimental_toggle (GtkWidget* widget, gpointer data)
{
    /**
     * @brief this function operate micro sampling checkbox
     * 
     * @return void
     */
    
    if(gtk_check_button_get_active (GTK_CHECK_BUTTON(widget)))
        wdata.microSampling = true;
    else
        wdata.microSampling = false;
}


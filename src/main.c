#include "main.h"

/*** Local functions declarations ***/
static void activate (GtkApplication* app, gpointer data);
static void calculate (GtkWindow* window);
static void open_file (GSimpleAction* action, GVariant* parameter, gpointer user_data);
static void save_file (GSimpleAction* action, GVariant* parameter, gpointer user_data);
static void quit (GSimpleAction* action, GVariant* parameter, gpointer user_data);
static void about (GSimpleAction* action, GVariant* parameter, gpointer user_data);
static void load_from_file (GtkNativeDialog* native, gint response);
static void save_to_file (GtkNativeDialog* native, gint response);
static void checkbox_derivative_toggle (GtkWidget* widget, gpointer data);
static void checkbox_rasterization_toggle (GtkWidget* widget, gpointer data);
static void checkbox_experimental_toggle (GtkWidget* widget, gpointer data);
static void changeTheme (GSimpleAction* action, GVariant* parameter, gpointer user_data);

static GActionEntry app_entries[] = {
  { "open", open_file, NULL, NULL, NULL, {0, 0, 0}},
  { "save", save_file, NULL, NULL, NULL, {0, 0, 0}},
  { "about", about, NULL, NULL, NULL, {0, 0, 0}},
  { "quit", quit, NULL, NULL, NULL, {0, 0, 0}},
  { "theme", NULL, NULL, "false", changeTheme, {0, 0, 0}}
};


int main (int argc, char* argv[]) {
#ifdef GTK_SRCDIR
    g_chdir (GTK_SRCDIR);
#endif

    // Create application
    app = gtk_application_new ("wiktor.ogrodnik.uwr.wykresy", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
    
    // Run application
    int status = g_application_run (G_APPLICATION(app), argc, argv);

    g_object_unref (app);
    return status;
}

static void activate (GtkApplication* app, gpointer data) {
    /**
     * @brief Initialize user interface and read app structure from .xml files
     * 
     * @return void
     */

    // Initalize pixbuf (graph bitmap) 
    wdata.chartData = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, CHART_WIDTH, CHART_HEIGHT);
    draw_put_lines_to_chart (wdata.chartData, wdata.darkmode);

    // Load interface form xml files
    GtkBuilder* builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "schema/interface.xml", NULL);

    // Configure menu entries
    g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);

    // Set main window
    window = gtk_builder_get_object (builder, "window");
    gtk_window_set_application (GTK_WINDOW(window), app);

    // Configure main menu button
    GObject* gears = gtk_builder_get_object (builder, "gears");
    GMenuModel* menu = G_MENU_MODEL (gtk_builder_get_object (builder, "menu"));
    gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (gears), menu);

    // Initialize bitmap container
    GObject* chart = gtk_builder_get_object (builder, "chart");
    gtk_image_set_from_pixbuf (GTK_IMAGE(chart), wdata.chartData);
    gtk_image_set_pixel_size (GTK_IMAGE(chart), CHART_WIDTH);
    wdata.chart = GTK_WIDGET(chart);

    // Initialzie legend labels
    GObject* bottomLegend = gtk_builder_get_object (builder, "gridLB");
    GObject* leftLegend = gtk_builder_get_object (builder, "gridLL");

    for (gint i = 0; i < 11; i++) {
        wdata.chartLegendBottom[i] = gtk_label_new ("");
        gtk_widget_set_hexpand (wdata.chartLegendBottom[i], TRUE);
        gtk_grid_attach(GTK_GRID(bottomLegend), wdata.chartLegendBottom[i], i, 0, 1, 1);

        wdata.chartLegendLeft[i] = gtk_label_new ("");
        gtk_widget_set_vexpand (wdata.chartLegendLeft[i], TRUE);
        gtk_grid_attach (GTK_GRID(leftLegend), wdata.chartLegendLeft[i], 0, i, 1, 1);
    }

    draw_make_legend (wdata.chartLegendBottom, 2.0);
    draw_make_legend (wdata.chartLegendLeft, 2.0);

    // Initialize props entry boxes
    wdata.intervalBuffer = gtk_entry_buffer_new ("10.0", 4);
    wdata.scaleBuffer = gtk_entry_buffer_new ("30.0", 4);

    GObject* intervalEntry = gtk_builder_get_object (builder, "entry1");
    gtk_entry_set_buffer (GTK_ENTRY (intervalEntry), wdata.intervalBuffer);

    GObject* scaleEntry = gtk_builder_get_object (builder, "entry2");
    gtk_entry_set_buffer (GTK_ENTRY (scaleEntry), wdata.scaleBuffer);

    gint num [] = {0, 1, 2, 3};

    // Initialize equations entry boxes
    for (gint i = 0; i < 4; i++) {
        wdata.equationBuffer[i] = gtk_entry_buffer_new ("", 0);
        char equationName[26]; equationName[0] = '\0';
        sprintf (equationName, "entryEquation%d", i);

        GObject* entry = gtk_builder_get_object (builder, equationName);
        gtk_entry_set_buffer (GTK_ENTRY (entry), wdata.equationBuffer[i]);

        wdata.derivative[i] = false;

        sprintf (equationName, "entryEquationDerivative%d", i);
        GObject* checkBoxDer = gtk_builder_get_object (builder, equationName);
        g_signal_connect (checkBoxDer, "toggled", G_CALLBACK (checkbox_derivative_toggle), GINT_TO_POINTER (num [i]));
    }

    // Initialize button
    GObject* button = gtk_builder_get_object (builder, "buttonRun");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (calculate), window);

    wdata.rasterization = true;
    wdata.darkmode = false;

    // Initialize checkboxes
    GObject* checkBox = gtk_builder_get_object (builder, "checkbox1");
    g_signal_connect (checkBox, "toggled", G_CALLBACK (checkbox_rasterization_toggle), &wdata);

    checkBox = gtk_builder_get_object (builder, "checkbox2");
    g_signal_connect (checkBox, "toggled", G_CALLBACK (checkbox_experimental_toggle), &wdata);

    gtk_widget_show (GTK_WIDGET (window));

    g_object_unref (builder);
}

static void calculate (GtkWindow* window) {
    /**
     * @brief Draws graph, signal function. Redirects to draw function and print error messages.
     * 
     * @return void
     */
    char message [100]; message[0] = '\0';
    draw_chart (&wdata, message);

    if (strlen (message)) {
        GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
        GtkWidget* dialog = gtk_message_dialog_new (window, flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);

        g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);

        gtk_widget_show (dialog);
    }
}

static void quit (GSimpleAction* action, GVariant* parameter, gpointer user_data) {
    /**
     * @brief Exit program
     * 
     * @return void
     */

    g_application_quit (G_APPLICATION (app));
}

static void about (GSimpleAction* action, GVariant* parameter, gpointer user_data) {
    /**
     * @brief print about dialog
     * 
     * @return void
     */

    const char license[] = "MIT License\n\nCopyright (c) 2021 Wiktor Ogrodnik\n\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files (the \"Software\"), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.\n";

    GtkWidget* about = gtk_about_dialog_new ();
    gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(about), "Wykresy");
    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(about), "0.21");
    gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(about), "c Wiktor Ogrodnik 2021");
    gtk_about_dialog_set_license (GTK_ABOUT_DIALOG(about), license);

    gtk_widget_show (about);
}

static void open_file (GSimpleAction* action, GVariant* parameter, gpointer user_data) {
    /**
     * @brief open dialog
     * 
     * @return void
     */

    GtkFileChooserNative* fileChooser = gtk_file_chooser_native_new("Open file", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Open", "Cancel");

    g_signal_connect(fileChooser, "response", G_CALLBACK(load_from_file), NULL);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
}

static void save_file(GSimpleAction* action, GVariant* parameter, gpointer user_data) {

    GtkFileChooserNative* fileChooser = gtk_file_chooser_native_new("Save file", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "Save", "Cancel");
    GtkFileChooser* chooser = GTK_FILE_CHOOSER(fileChooser);

    gtk_file_chooser_set_current_name(chooser, "new_data.txt");

    g_signal_connect(fileChooser, "response", G_CALLBACK(save_to_file), NULL);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
}


static void load_from_file(GtkNativeDialog* native, gint response) {
    if (response == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(native);
        GFile* file = gtk_file_chooser_get_file(chooser);

        gchar* napis;

        if (g_file_load_contents(file, NULL, &napis, NULL, NULL, NULL)) {
            for (gint i = 0; i < 4; i++) {
                gchar* pos = strchr(napis, '\n');
                if (pos) {
                    *pos = '\0';
                    gtk_entry_buffer_set_text(wdata.equationBuffer[i], napis, strlen(napis));
                    strcpy(napis, pos + 1);
                } else {
                    gtk_entry_buffer_set_text(wdata.equationBuffer[i], napis, strlen(napis));
                    break;
                }
            }

            free(napis);

        } else {
            GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
            GtkWidget* dialog = gtk_message_dialog_new(NULL, flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", "Problem with loading file.");

            g_signal_connect (dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);

            gtk_widget_show (dialog);
        }
        
        g_object_unref(file);
    }
    g_object_unref(native);
}

static void save_to_file(GtkNativeDialog* native, gint response) {
if (response == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(native);
        GFile* file = gtk_file_chooser_get_file(chooser);        
        GError* error = NULL;

        GOutputStream* stream = G_OUTPUT_STREAM(g_file_create(file, G_FILE_CREATE_NONE, NULL, &error));

        if (error && error->code == G_IO_ERROR_EXISTS) {
            stream = g_io_stream_get_output_stream(G_IO_STREAM(g_file_open_readwrite(file, NULL, NULL)));
        } else if (error && error->code != 0) {
            GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
            GtkWidget* dialog = gtk_message_dialog_new(NULL, flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", "Can not save file!");

            g_signal_connect (dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);

            gtk_widget_show (dialog);
        }

        for(gint i = 0; i < 4; i++) {
            g_output_stream_printf(stream, NULL, NULL, NULL, "%s\n", gtk_entry_buffer_get_text(wdata.equationBuffer[i]));
        }
        g_object_unref(stream);
        g_object_unref(file);
    }
    g_object_unref(native);
}

static void checkbox_rasterization_toggle (GtkWidget* widget, gpointer data) {
    /**
     * @brief this function operate rasterization checkbox
     * 
     * @return void
     */

    if (gtk_check_button_get_active (GTK_CHECK_BUTTON (widget)))
        wdata.rasterization = true;
    else
        wdata.rasterization = false;
}

static void checkbox_experimental_toggle (GtkWidget* widget, gpointer data) {
    /**
     * @brief this function operate micro sampling checkbox
     * 
     * @return void
     */
    
    if(gtk_check_button_get_active (GTK_CHECK_BUTTON (widget)))
        wdata.microSampling = true;
    else
        wdata.microSampling = false;
}

static void checkbox_derivative_toggle (GtkWidget* widget, gpointer data) {
    
    /**
     * @brief
     * 
     * @return void
     */

    int s = GPOINTER_TO_INT (data);
    
    if(gtk_check_button_get_active (GTK_CHECK_BUTTON (widget)))
        wdata.derivative [s] = true;
    else
        wdata.derivative [s] = false;
}

static void changeTheme (GSimpleAction* action, GVariant* parameter, gpointer user_data) {
    /**
     * @brief change theme between darkmode and light mode
     * 
     * @return void
     */

    GVariant* action_state;
    action_state = g_action_get_state (G_ACTION(action));
    const gboolean flag = g_variant_get_boolean (action_state);

    if (flag) {
        g_object_set (gtk_settings_get_default (), "gtk-theme-name", "Adwaita", "gtk-application-prefer-dark-theme", FALSE, NULL);       
        g_simple_action_set_state (action, g_variant_new_boolean (FALSE));
        wdata.darkmode = false;
        calculate (GTK_WINDOW(window));
    }
    else {
        g_object_set (gtk_settings_get_default (), "gtk-theme-name", "Adwaita", "gtk-application-prefer-dark-theme", TRUE, NULL); 
        g_simple_action_set_state (action, g_variant_new_boolean (TRUE));
        wdata.darkmode = true;
        calculate (GTK_WINDOW(window));
    }
}
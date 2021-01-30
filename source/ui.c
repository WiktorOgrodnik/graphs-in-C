#include "ui.h"

static void destroy (GtkWidget* widget, gpointer data);
static void about_run (GtkWidget* widget, gpointer data);
static void calculate (GtkWidget* widget, gpointer data);

static void checkbox_rasterization_toggle(GtkWidget* widget, gpointer data);
static void checkbox_experimental_toggle(GtkWidget* widget, gpointer data);

static GtkWidget* ui_init_menu();
static void ui_init_inputs(eqData* data);
static void ui_init_window();

static GtkWidget* window;
static eqData inputs;

void ui_init(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    //Init application Window
    ui_init_window();
    //Init textboxes, chart and charts legend
    ui_init_inputs(&inputs); 
    //Init menubar (File, Edit, etc.)
    GtkWidget* menubar = ui_init_menu(); 

    /* All program Box */
        
        GtkWidget* mainestBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

        GtkWidget* windowScroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(windowScroll), mainBox);

        gtk_box_pack_start(GTK_BOX(mainestBox), menubar, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mainestBox), windowScroll, TRUE, TRUE, 0);

        gtk_container_add(GTK_CONTAINER(window), mainestBox);

        /* Box with Legend and the chart */
            GtkWidget* boxLegendAndChart = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_container_set_border_width(GTK_CONTAINER(boxLegendAndChart), 10);

            gtk_box_pack_start(GTK_BOX(boxLegendAndChart), inputs.chartLegendLeftBox, TRUE, TRUE, 0);

            /* Box with chat and horizontal legend*/
                GtkWidget* boxChartAndLegendBottom = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

                gtk_box_pack_start(GTK_BOX(boxChartAndLegendBottom), inputs.chart, TRUE, TRUE, 0);
                gtk_box_pack_start(GTK_BOX(boxChartAndLegendBottom), inputs.chartLegendBottomBox, TRUE, TRUE, 0);

            gtk_box_pack_start(GTK_BOX(boxLegendAndChart), boxChartAndLegendBottom, TRUE, TRUE, 0);

        gtk_box_pack_start(GTK_BOX(mainBox), boxLegendAndChart, TRUE, TRUE, 0);

        /* Box with textboxes and button */
            GtkWidget* boxInputs = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_set_border_width(GTK_CONTAINER(boxInputs), 10);
            /* Box with properties textboxes and settings checkboxes */

                GtkWidget* boxCheckAndProps = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); //Change to Horizontal
                gtk_widget_set_margin_bottom(boxCheckAndProps, 20);

                /* Box woth scale textbox and interval textbox */
                    GtkWidget* boxEntryProps = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

                    GtkWidget* label1 = gtk_label_new("Przedział: ");
                    GtkWidget* label2 = gtk_label_new("Skala: ");
                    gtk_widget_set_margin_start(label2, 10);

                    gtk_box_pack_start(GTK_BOX(boxEntryProps), label1, FALSE, FALSE, 0);
                    gtk_box_pack_start(GTK_BOX(boxEntryProps), inputs.interval, FALSE, FALSE, 0);
                    gtk_box_pack_start(GTK_BOX(boxEntryProps), label2, FALSE, FALSE, 0);
                    gtk_box_pack_start(GTK_BOX(boxEntryProps), inputs.res, FALSE, FALSE, 0);

                gtk_box_pack_start(GTK_BOX(boxCheckAndProps), boxEntryProps, FALSE, FALSE, 0);

                /* Box with checkboxes */
                    GtkWidget* checkBoxBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                    gtk_widget_set_margin_top(checkBoxBox, 20);

                    GtkWidget* label3 = gtk_label_new("Rasteryzacja: ");
                    GtkWidget* label4 = gtk_label_new("Mikropróbkowanie (eksperymentalne): ");

                    gtk_widget_set_margin_end(label3, 5);
                    gtk_widget_set_margin_end(label4, 5);
                    gtk_widget_set_margin_start(label4, 10);

                    gtk_box_pack_start(GTK_BOX(checkBoxBox), label3, FALSE, FALSE, 0);
                    gtk_box_pack_start(GTK_BOX(checkBoxBox), inputs.rasterizationCheckBox, FALSE, FALSE, 0);
                    gtk_box_pack_start(GTK_BOX(checkBoxBox), label4, FALSE, FALSE, 0);
                    gtk_box_pack_start(GTK_BOX(checkBoxBox), inputs.experimentalMicroSamplingCheckBox, FALSE, FALSE, 0);
                
                gtk_box_pack_start(GTK_BOX(boxCheckAndProps), checkBoxBox, FALSE, FALSE, 0);

            gtk_box_pack_start(GTK_BOX(boxInputs), boxCheckAndProps, TRUE, TRUE, 0);

            /* Boxes with functions textbox */
            for (gint i = 0; i < 4; i++)
            {
                GtkWidget* boxEntryFunc = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                gtk_widget_set_margin_bottom(boxEntryFunc, 10);

                char labelContent[10];
                sprintf(labelContent, "f%d(x) = ", i + 1);
                GtkWidget* label5 = gtk_label_new(labelContent);

                /*GtkWidget* colorInfoBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

                cairo_t* cr = cairo_create(NULL);
                //cr->

                GdkRectangle colorInfo;
                colorInfo.x = colorInfo.y = 0;
                colorInfo.width = colorInfo.height = 10;

                gdk_cairo_rectangle(cr, &colorInfo);
                cairo_clip(cr);

                cairo_set_source_rgb(cr, 0, 0, 255);
                cairo_fill(cr);
                //colorInfo.height
                gtk_widget_draw(colorInfoBox, cr);*/

                gtk_box_pack_start(GTK_BOX(boxEntryFunc), label5, FALSE, FALSE, 0);
                gtk_box_pack_start(GTK_BOX(boxEntryFunc), inputs.equation[i], TRUE, TRUE, 0);
                //gtk_box_pack_start(GTK_BOX(boxEntryFunc), colorInfoBox, FALSE, FALSE, 0);
                
                gtk_box_pack_start(GTK_BOX(boxInputs), boxEntryFunc, TRUE, TRUE, 0);
            }

            //Insert button
            GtkWidget* button = gtk_button_new_with_label("Wprowadź");

            gtk_widget_set_margin_start(button, 500);
            gtk_widget_set_margin_end(button, 70);
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(calculate), (gpointer)&inputs);
        
            gtk_box_pack_start(GTK_BOX(boxInputs), button, TRUE, TRUE, 0);

        gtk_box_pack_start(GTK_BOX(mainBox), boxInputs, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();
}

void error_dialog(const char* message)
{
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);
    
    gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);
}

// Local functions
static GtkWidget* ui_init_menu()
{
    GtkWidget* menubar = gtk_menu_bar_new();

    GtkWidget* file_item = gtk_menu_item_new_with_label("Plik");
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* quit_item = gtk_menu_item_new_with_label("Zakończ");

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

static void ui_init_inputs(eqData* data)
{
    data->rasterization = true;
    data->microSampling = false;

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

    draw_make_legend(data->chartLegendLeft, (gdouble)300 / (gdouble)5);
    draw_make_legend(data->chartLegendBottom, (gdouble)10 / (gdouble)5);

    for (gint i = 0; i < 4; i++)
    {
        data->equation[i] = gtk_entry_new(); 
        gtk_entry_set_max_length(GTK_ENTRY(data->equation[i]), (gint)1000);
    }
    
    data->interval = gtk_entry_new(); data->res = gtk_entry_new();
    
    gtk_entry_set_max_length(GTK_ENTRY(data->interval), (gint)20);
    gtk_entry_set_width_chars(GTK_ENTRY(data->interval), (gint)20);
    gtk_entry_set_max_length(GTK_ENTRY(data->res), (gint)20);
    gtk_entry_set_width_chars(GTK_ENTRY(data->res), (gint)20);

    gtk_widget_set_size_request(data->res, 50, -1);

    gtk_entry_set_text(GTK_ENTRY(data->interval), "10.0");
    gtk_entry_set_text(GTK_ENTRY(data->res), "30.0");

    gtk_widget_set_margin_end(data->res, (gint)300);

    data->chartData = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 800, 600);
    put_lines_to_chart(data->chartData, -10.0, 10.0);
    data->chart = gtk_image_new_from_pixbuf(data->chartData);
    gtk_widget_set_margin_top(data->chart, 4);

    data->rasterizationCheckBox = gtk_check_button_new();
    data->experimentalMicroSamplingCheckBox = gtk_check_button_new();

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->rasterizationCheckBox), TRUE);

    g_signal_connect(data->rasterizationCheckBox, "toggled", G_CALLBACK(checkbox_rasterization_toggle), data);
    g_signal_connect(data->experimentalMicroSamplingCheckBox, "toggled", G_CALLBACK(checkbox_experimental_toggle), data);
}

static void ui_init_window()
{
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_window_set_default_size(GTK_WINDOW(window), 900, 800);
    gtk_window_set_title(GTK_WINDOW(window), "Wykresy");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
}

static void destroy (GtkWidget* widget, gpointer data) 
{
    gtk_main_quit();
}

static void about_run (GtkWidget* widget, gpointer data)
{
    const char license[] = "MIT License\n\nCopyright (c) 2021 Wiktor Ogrodnik\n\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files (the \"Software\"), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.\n";

    GtkWidget* about = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), "Wykresy");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), "0.1");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "c Wiktor Ogrodnik 2021");
    gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), license);

    gtk_dialog_run(GTK_DIALOG(about));
    gtk_widget_destroy(about);
}

static void calculate (GtkWidget *widget, gpointer data) 
{
    draw_chart(widget, (eqData*)data);
}

static void checkbox_rasterization_toggle(GtkWidget* widget, gpointer data)
{
    eqData* localData = (eqData*)data;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
       localData->rasterization = true;
    else
        localData->rasterization = false;
}

static void checkbox_experimental_toggle(GtkWidget* widget, gpointer data)
{
    eqData* localData = (eqData*)data;
    
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
       localData->microSampling = true;
    else
        localData->microSampling = false;
}
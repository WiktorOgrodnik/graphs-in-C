#include "main.h"

static void activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget* window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Wykresy");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 800);
    gtk_widget_show(window);
}

int main(int argc, char* argv[])
{
    int status;

    app = gtk_application_new("wiktor.ogrodnik.uwr.wykresy", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    //ui_init();

    g_object_unref(app);
    return status;
}


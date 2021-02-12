#include "draw.h"

/*** Local functions declarations ***/
static void put_pixel (GdkPixbuf* pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha);
static void draw_rasterizaton(eqData* data, gdouble wyniki[], gint column, gdouble l, gdouble delta, gint color);

void draw_chart (GtkWidget *widget, eqData* data, char* error_message) 
{
    const gchar* equation, *interval, *scale_;
    gdouble l, p, delta, scale;
    gint r;

    //get text from GtkEntry
    interval = gtk_entry_get_text(GTK_ENTRY(data->interval));
    scale_ = gtk_entry_get_text(GTK_ENTRY(data->res));

    //convert string to double
    if (strcmp(interval, "") == 0) l = 10.0;
    else l = char_to_double(interval);

    p = l;
    l *= -1;
    
    gdouble lorg = l;

    r = (gint)800;
    delta = (p - l) / (gdouble)r;

    gdouble results[4][r];
    gdouble max = -800.0;

    //initialize array with results (one for each equation)
    for (gint i = 0; i < 4; i++)
    {
        for (gint j = 0; j < r; j++)
            results[i][j] = nan("out");
    }

    data->chartData = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 800, 600);
    put_lines_to_chart(data->chartData, l, p);

    //The loop is run 4 times, once for each equation 
    for (gint i = 3; i >= 0; i--)
    {
        for (gint j = 0; j < r; j++) //r = 800 - chart width 
        {
            equation = gtk_entry_get_text(GTK_ENTRY(data->equation[i]));
            if (strlen(equation) == 0) 
                break;

            int error = 0;
            char message[100];

            double result = calc(equation, strlen(equation), l, &error, message); //calculate the equation by substituting for x point l
            if (error) //if error, there was syntax error in equation
            {
                switch(error)
                {
                    case 1:
                        sprintf(error_message, "Error: Can not find: %s", message);
                        return;
                    break;
                    case 2:
                        sprintf(error_message, "Error: expected number or '(', or '|', or '{', or '['");
                        return;
                    break;
                    case 3:
                        sprintf(error_message, "Error: Incorrect parenthesis, expected: %s", message);
                        return;
                    break;
                    default:
                    break;
                }
                
            } 

            results[i][j] = result;

            if (!isnan(result) && result > max) max = result;
            l+=delta;
        }
        l = lorg;
    }
    
    max += max * 0.4;
    if (max > 300) max = 300;

    if (strcmp(scale_, "") == 0) scale = 300 / max;
    else scale = char_to_double(scale_);

    //Match the legend with an interval and scale
    draw_make_legend(data->chartLegendLeft, (300.0/scale) / 5.0);
    draw_make_legend(data->chartLegendBottom, p / 5.0);

    //for(gint i = 0; i < 10; i++)
    //{
        //Problem w tym miejscu ->
        //GtkRequisition r;

        /*gint w;
        gtk_widget_get_preferred_width(data->chartLegendBottom[i], NULL, &w);

        gint width = 80 - w;
        gtk_widget_set_margin_end(GTK_WIDGET(data->chartLegendBottom[i]), width);*/
    //}

    //draw and rasterization
    for (gint k = 3; k >= 0; k--)
    {
        for (gint i = 0; i < r; i++) 
        {
            results[k][i] = (gint)(300 - (results[k][i]) * scale);

            if (!isnan(results[k][i]) && results[k][i] >=0 && results[k][i] <= 600) 
            {
                if (k == 0) put_pixel(data->chartData, i, results[k][i], 0, 0, 255, 255);
                else if (k == 1) put_pixel(data->chartData, i, results[k][i], 255, 0, 0, 255);
                else if (k == 2) put_pixel(data->chartData, i, results[k][i], 0, 255, 0, 255);
                else put_pixel(data->chartData, i, results[k][i], 125, 27, 186, 255);
            }

            if (i && !isnan(results[k][i]) && data->rasterization)
                draw_rasterizaton(data, results[k], i, lorg, delta, k);
        }
    }
    
    gtk_image_set_from_pixbuf(GTK_IMAGE(data->chart), data->chartData);
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

static void draw_rasterizaton(eqData* data, gdouble wyniki[], gint column, gdouble l, gdouble delta, gint color)
{
    gint start = wyniki[column - 1] < wyniki[column] ? (gint)wyniki[column - 1] : (gint)wyniki[column];
    gint end = wyniki[column - 1] < wyniki[column] ? (gint)wyniki[column] : (gint)wyniki[column - 1];

    bool stopRast = false;
    gdouble sampling[2341];

    //Micro sampling algorithm
    if (end - start >= 2 && end - start < 400 && data->microSampling)
    {
        const gchar* equation = gtk_entry_get_text(GTK_ENTRY(data->equation[color]));

        for (gint i = 0; i <= 2340; i++)
        {
            int error = 0;
            char message [10];
            sampling[i] = calc(equation, strlen(equation), l + (delta*(gdouble)(column - 1)) + ((gdouble)(i+30)/2340 * delta), &error, message);
        }

        for (gint i = 1; i <= 2340 && !stopRast; i++)
        {
            if (fabs(sampling[i - 1] - sampling[i]) > 0.5) 
                stopRast = true;
        }
    }

    //Rasterization
    if (end - start >= 2 && end - start < 400 && !stopRast)
    {
        for (gint i = start; i < end; i++)
        {
            if (color == 0) put_pixel(data->chartData, column, i, 0, 0, 255, 255);
            else if (color == 1) put_pixel(data->chartData, column, i, 255, 0, 0, 255);
            else if (color == 2) put_pixel(data->chartData, column, i, 0, 255, 0, 255);
            else put_pixel(data->chartData, column, i, 125, 27, 186, 255);
        }
    }
}

//Draw lines on chart
void put_lines_to_chart(GdkPixbuf* pixbuf, gdouble l, gdouble p)
{
    for (gint i = 0; i < 800; i++)
    {
        for (gint j = 0; j < 600; j++)
        {
            if (j % 60 == 0 && j) put_pixel(pixbuf, i, j, 0, 0, 0, 127);
            else if (j % 12 == 0 && j) put_pixel(pixbuf, i, j, 0, 0, 0, 63);
        }
    }

    for (gint i = 0; i < 600; i++)
    {
        for (gint j = 0; j < 800; j++)
        {
            if (j % 80 == 0 && j) put_pixel(pixbuf, j, i, 0, 0, 0, 127);
            else if (j % 16 == 0 && j) put_pixel(pixbuf, j, i, 0, 0, 0, 63);
        }
    }

    for (gint i = 0; i < 800; i++)
    {
        put_pixel(pixbuf, i, 299, 0, 0, 0, 255);
        put_pixel(pixbuf, i, 300, 0, 0, 0, 255);
    }

    for (gint i = 0; i < 600; i++)
    {
        put_pixel(pixbuf, 399, i, 0, 0, 0, 255);
        put_pixel(pixbuf, 400, i, 0, 0, 0, 255);
    }

    for(int i = 385, j = 15; j >= -15; i++, j--)
    {
        put_pixel(pixbuf, i, abs(j), 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, i, abs(j) - 1, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, i, abs(j) - 2, 0u, 0u, 0u, 255u);

        put_pixel(pixbuf, 800 - abs(j), i - 100, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, 799 - abs(j), i - 100, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, 798 - abs(j), i - 100, 0u, 0u, 0u, 255u);
    }
}

void draw_make_legend(GtkWidget* chartLeft[], gdouble delta)
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
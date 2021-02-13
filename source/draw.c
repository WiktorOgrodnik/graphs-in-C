#include "draw.h"

/*** Local functions declarations ***/
static void put_pixel (GdkPixbuf* pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha);
static void draw_rasterizaton(eqData* data, gdouble wyniki[], gint column, gdouble l, gdouble delta, gint color);

void draw_chart (GtkWidget* widget, eqData* data, char* error_message) 
{
    const gchar *equation, *interval, *scale_;
    gdouble l, p, delta, scale;
    
    //get text from GtkEntry
    interval = gtk_entry_get_text(GTK_ENTRY(data->interval));
    scale_ = gtk_entry_get_text(GTK_ENTRY(data->res));

    //convert string to double
    if (strcmp(interval, "") == 0) l = 10.0;
    else l = char_to_double(interval);

    if (strcmp(scale_, "") == 0) scale = 30.0;
    else scale = char_to_double(scale_);

    p = l;
    l *= -1;
    
    gdouble lorg = l;
    delta = (p - l) / CHART_WIDTH;

    gdouble results[4][CHART_WIDTH];

    //initialize array with results (one for each equation)
    for (gint i = 0; i < 4; i++)
    {
        for (gint j = 0; j < CHART_WIDTH; j++)
            results[i][j] = nan("out");
    }

    data->chartData = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, CHART_WIDTH, CHART_HEIGHT);
    put_lines_to_chart(data->chartData);

    //The loop is run 4 times, once for each equation 
    for (gint i = 3; i >= 0; i--)
    {
        for (gint j = 0; j < CHART_WIDTH; j++)
        {
            equation = gtk_entry_get_text(GTK_ENTRY(data->equation[i]));
            if (!strlen(equation)) 
                break;

            int error = 0;
            char message[100];
            error_message[0] = message[0] = '\0';

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
                        sprintf(error_message, "Error: Unknown error!");
                        return;
                    break;
                }   
            } 

            results[i][j] = result;
            l+=delta;
        }
        l = lorg;
    }

    //Match the legend with an interval and scale
    draw_make_legend(data->chartLegendLeft, ((gdouble)(CHART_HEIGHT / 2)/scale) / 5.0);
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
        for (gint i = 0; i < CHART_WIDTH; i++) 
        {
            results[k][i] = (gint)((CHART_HEIGHT / 2) - (results[k][i]) * scale);

            if (!isnan(results[k][i]) && results[k][i] >=0 && results[k][i] <= CHART_HEIGHT) 
            {
                if (k == 0) put_pixel(data->chartData, i, results[k][i], 0u, 0u, 255u, 255u);
                else if (k == 1) put_pixel(data->chartData, i, results[k][i], 255u, 0u, 0u, 255u);
                else if (k == 2) put_pixel(data->chartData, i, results[k][i], 0u, 255u, 0u, 255u);
                else put_pixel(data->chartData, i, results[k][i], 125u, 27u, 186u, 255u);
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
    if (end - start >= 2 && end - start < (CHART_HEIGHT / 2) && data->microSampling)
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
    if (end - start >= 2 && end - start < (CHART_WIDTH / 2) && !stopRast)
    {
        for (gint i = start; i < end; i++)
        {
            if (color == 0) put_pixel(data->chartData, column, i, 0u, 0u, 255u, 255u);
            else if (color == 1) put_pixel(data->chartData, column, i, 255u, 0u, 0u, 255u);
            else if (color == 2) put_pixel(data->chartData, column, i, 0u, 255u, 0u, 255u);
            else put_pixel(data->chartData, column, i, 125u, 27u, 186u, 255u);
        }
    }
}

//Draw lines on chart
void put_lines_to_chart(GdkPixbuf* pixbuf)
{
    for (gint i = 0; i < CHART_WIDTH; i++)
    {
        for (gint j = 0; j < CHART_HEIGHT; j++)
        {
            if (j % (CHART_HEIGHT / 10) == 0 && j) put_pixel(pixbuf, i, j, 0u, 0u, 0u, 127u);
            else if (j % (CHART_HEIGHT / 50) == 0 && j) put_pixel(pixbuf, i, j, 0u, 0u, 0u, 63u);
        }
    }

    for (gint i = 0; i < CHART_HEIGHT; i++)
    {
        for (gint j = 0; j < CHART_WIDTH; j++)
        {
            if (j % (CHART_WIDTH / 10) == 0 && j) put_pixel(pixbuf, j, i, 0u, 0u, 0u, 127u);
            else if (j % (CHART_WIDTH / 50) == 0 && j) put_pixel(pixbuf, j, i, 0u, 0u, 0u, 63u);
        }
    }

    for (gint i = 0; i < CHART_WIDTH; i++)
    {
        put_pixel(pixbuf, i, (CHART_HEIGHT / 2) - 1, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, i, (CHART_HEIGHT / 2), 0u, 0u, 0u, 255u);
    }

    for (gint i = 0; i < CHART_HEIGHT; i++)
    {
        put_pixel(pixbuf, (CHART_WIDTH / 2) - 1, i, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, (CHART_WIDTH / 2), i, 0u, 0u, 0u, 255u);
    }

    for(gint i = CHART_WIDTH / 2 - 15, j = 15; j >= -15; i++, j--)
    {
        put_pixel(pixbuf, i, abs(j), 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, i, abs(j) - 1, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, i, abs(j) - 2, 0u, 0u, 0u, 255u);

        put_pixel(pixbuf, CHART_WIDTH - abs(j), i - 100, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, CHART_WIDTH - 1 - abs(j), i - 100, 0u, 0u, 0u, 255u);
        put_pixel(pixbuf, CHART_WIDTH - 2 - abs(j), i - 100, 0u, 0u, 0u, 255u);
    }
}

void draw_make_legend(GtkWidget* chartLeft[], gdouble delta)
{
    for (gint i = 1; i <= 11; i++)
    {
        gchar str[20];
        if (i < 6)
        {
            sprintf(str, "%.1f", (double)((6 - i) * delta));
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), str);
        }
        else if (i == 6)
        {
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), "0");
        }
        else
        {   
            sprintf(str, "%.1f", (double)((i - 6) * delta));
            gtk_label_set_text(GTK_LABEL(chartLeft[i - 1]), str);
        }
    }
}
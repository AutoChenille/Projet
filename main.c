#include <gtk/gtk.h>
#include <stddef.h>

//Preprocessing
#include "Preprocessing/pretreatment.h"

//Processing
#include "Processing/main.c"
#include "NeuronalNetwork/mainNN.h"
#include "NeuronalNetwork_Hexa/mainNNH.h"
#include "sudoku_solver/solver16.h"
#include "sudoku_solver/solver9.h"

#define SwitchOn 1
#define SwitchOff 0

char WEIGHT_PATH[] = "./NeuronalNetwork/800x3_with_blank_handwrite/";

char **tosolve;

int is_activated = 0;

// Callback function for the "Open" menu item
void on_open_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    g_print("Open menu item clicked\n");
}


// Callback function for the "Quit" menu item and the main window's close button
void on_quit_activate()
{
    gtk_main_quit();
}


// Callback function for the "About" menu item
void on_about_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(NULL, flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "About SudoC");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


void file_opener(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        // Do something with the filename here
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}


int get_switch_state(GtkSwitch *sw, gpointer user_data)
{
    if (gtk_switch_get_active(sw))
    {
        g_print("Switch state: %d\n", SwitchOn);
        is_activated = SwitchOn;
        return SwitchOn;
    }

    g_print("Switch state: %d\n", SwitchOff);
    is_activated = SwitchOff;
    return SwitchOff;
}


void on_save(GtkButton *button, gpointer user_data)
{
    // Get the output image widget
    GtkImage *output_image = GTK_IMAGE(user_data);

    // Get the pixbuf from the output image
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(output_image);

    // Get the current timestamp as a string
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", local_time);

    // Construct the filename
    char filename[256];
    snprintf(filename, sizeof(filename), "saved_%s.png", timestamp);

    // Save the pixbuf to the file
    GError *error = NULL;
    if (!gdk_pixbuf_savev(pixbuf, filename, "png", NULL, &error, NULL)) {
        g_printerr("Error saving file: %s\n", error->message);
        g_error_free(error);
    } else {
        g_print("File saved successfully: %s\n", filename);
    }
}



char* on_choose_image(GtkButton *button, gpointer user_data)
{
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *image = GTK_WIDGET(gtk_builder_get_object(builder, "input_image"));

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Choose an image file",
                                                    GTK_WINDOW(gtk_builder_get_object(builder, "main_window")),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel",
                                                    GTK_RESPONSE_CANCEL,
                                                    "_Open",
                                                    GTK_RESPONSE_ACCEPT,
                                                    NULL);

    char *filename = NULL;
    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = g_strdup(gtk_file_chooser_get_filename(chooser));

        // Load the image using GdkPixbuf
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

        // Check if the image size is already 300x300 or less
        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);
        if (width > 400 || height > 400) {
            // Resize the image to fit into a 300x300 square
            int new_width, new_height;
            if (width > height) {
                new_width = 400;
                new_height = height * 400 / width;
            } else {
                new_width = width * 400 / height;
                new_height = 400;
            }
            GdkPixbuf *new_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
            g_object_unref(pixbuf);
            pixbuf = new_pixbuf;
        }

        // Set the image in the GtkImage widget
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
        g_object_unref(pixbuf);
    }

    gtk_widget_destroy(dialog);
    return filename;
}


void open_image(GtkButton *button, gpointer user_data)
{
    // Call the on_choose_image() function to get the path to the image

    char *paf = on_choose_image(button,user_data);//VERIFIER QUE CA FONCTIONNE
    int NB_CELLS = is_activated == SwitchOn ? 16 : 9;

    SDL_Surface **loaded = ProcessImage(paf, NB_CELLS);

    if(is_activated == SwitchOn)
    {
        tosolve = PredictSurface_16x16(loaded, NB_CELLS ,"./NeuronalNetwork_Hexa/800x3_with_blank_handwrite");
        draw_hexadoku(tosolve, "./unsolved.png");
    }
    else
    {
        tosolve = PredictSurface_9x9(loaded, NB_CELLS,"./NeuronalNetwork/800x3_with_blank_handwrite");
        for(size_t i = 0; i < NB_CELLS; i++)
        {
            for(size_t j = 0; j < NB_CELLS; j++)
            {
                g_print("%c ", tosolve[i][j]);
            }
            g_print("\n");
        }
        if(tosolve == NULL)
            g_print("Error while loading grid");
        g_print("unsolved 9x9\n");
        draw_sudoku(tosolve, "./unsolved.png");
        g_print("unsolved 9x9 done\n");
    }

    //free loaded surfaces
    SDL_FreeSurface(*loaded);
    free(loaded);

    // Load the new image into the output_image GtkImage widget
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *output_image = GTK_WIDGET(gtk_builder_get_object(builder, "output_image"));
    gtk_image_set_from_file(GTK_IMAGE(output_image), "./unsolved.png");
}


/*
void on_choose_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Choose button clicked!\n");
}
*/


void on_apply(GtkButton *button, gpointer user_data)
{
    GtkBuilder *builder = GTK_BUILDER(user_data);
    int NB_CELLS = is_activated == SwitchOn ? 16 : 9;

    // Get the input image widget
    GtkImage *input_image = GTK_IMAGE(gtk_builder_get_object(builder, "input_image"));

    // Get the pixbuf from the input image
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(input_image);

   if(is_activated == SwitchOn)
    {
        SolveHexadoku(tosolve);
        draw_hexadoku(tosolve, "./solved.png");
    }
    else
    {
        SolveSudoku(tosolve);
        draw_sudoku(tosolve, "./solved.png");
    }

    // Get the output image widget
    GtkImage *output_image = GTK_IMAGE(gtk_builder_get_object(builder, "output_image"));

    // Load the solved image into the output image widget
    GdkPixbuf *solved_pixbuf = gdk_pixbuf_new_from_file("./solved.png", NULL);
    gtk_image_set_from_pixbuf(output_image, solved_pixbuf);

    // Clean up
    g_object_unref(solved_pixbuf);
}



int main(int argc, char *argv[])
{
    GtkBuilder *builder;
    GtkWidget *window;

    // Initialize GTK+
    gtk_init(&argc, &argv);
 
    // Create a new builder object
    builder = gtk_builder_new();

    // Load the UI definition from the Glade file
    gtk_builder_add_from_file(builder, "UI/sudo.glade", NULL);

    // Connect signal handlers
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(gtk_builder_get_object(builder, "open"), "activate", G_CALLBACK(on_open_activate), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "close"), "select", G_CALLBACK(on_quit_activate), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "about"), "activate", G_CALLBACK(on_about_activate), NULL);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "choose_button")), "clicked", G_CALLBACK(open_image), builder);
    g_signal_connect(gtk_builder_get_object(builder, "switch_int_hex"), "state-changed", G_CALLBACK(get_switch_state), NULL);

    // Connect the save button to the on_save function
    GtkWidget *save_button = GTK_WIDGET(gtk_builder_get_object(builder, "save_button"));
    GtkWidget *output_image = GTK_WIDGET(gtk_builder_get_object(builder, "output_image"));
    g_signal_connect(G_OBJECT(save_button), "clicked", G_CALLBACK(on_save), output_image);

    // Connect the solve button to the on_apply function
    GtkWidget *solve_button = GTK_WIDGET(gtk_builder_get_object(builder, "solve_button"));
    g_signal_connect(solve_button, "clicked", G_CALLBACK(on_apply), builder);


    // Get the main window widget
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    // Create a new GdkRGBA color object and set its values to red
    GdkRGBA color;
    gdk_rgba_parse(&color, "white");

    // Set the background color of the GtkBox
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &color);

    // Show the main window
    gtk_widget_show_all(window);

    // Start the main event loop
    gtk_main();

    // Clean up
    g_object_unref(builder);

    return 0;

}

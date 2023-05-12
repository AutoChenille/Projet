#include <gtk/gtk.h>
#include "Processing/main.c"
#include "NeuronalNetwork/main.c"
#include "NeuronalNetwork_Hexa/main.c"
#include "sudoku_solver/solver.c"


#define SwitchOn 1
#define SwitchOff 0

char WEIGHT_PATH[] = "./NeuronalNetwork/800x3_with_blank_handwrite/";

char tosolve16[16][16];
char tosolve8[8][8];


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

void on_choose_image(GtkButton *button, gpointer user_data)
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

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkImage *image_widget = GTK_IMAGE(image);

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        gtk_image_set_from_file(image_widget, filename);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void open_image(GtkButton *button, gpointer user_data)
{
    on_choose_image(GtkButton *button, gpointer user_data);

    SDL_Surface **loaded = ProcessImage(paf, NB_CELLS);
    PredictSurface(loaded, NB_CELLS, char *params, WEIGHT_PATH);

    tosolve16 = NULL;
    tosolve8 = NULL;
    if(is_activated == SwitchOn) 
        tosolve16 = ConvertToSolveur; //TO DO THIBAULT entry : int[] -> char[][]
    else
        tosolve8 = ConvertToSolveur;
    PrintGrid //char[][] -> save image in ./unsolved.png

    //TODO ajouter './unsolved.png' à l'interface
    
}

void on_choose_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Choose button clicked!\n");
}


// Function to be called when the "solve_button" is clicked
void on_apply(GtkButton *button, gpointer user_data)
{
    GtkSwitch *switch_btn = GTK_SWITCH(user_data);
    int NB_CELLS = is_activated == SwitchOn ? 16 : 9;


    // Get the input image widget
    GtkImage *input_image = GTK_IMAGE(user_data);

    // Get the pixbuf from the input image
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(input_image);

    //TODO : inclure fonction renvoyant le path de l'imported image
    Solve // entry : char[][] -> save image in ./solved.png

    //TODO afficher './solved.png'

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
    gtk_builder_add_from_file(builder, "sudo.glade", NULL); //TODO: change glade's path

    // Connect signal handlers
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(gtk_builder_get_object(builder, "open"), "activate", G_CALLBACK(on_open_activate), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "close"), "select", G_CALLBACK(on_quit_activate), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "about"), "activate", G_CALLBACK(on_about_activate), NULL);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "choose_button")), "clicked", G_CALLBACK(open_image), builder);
    g_signal_connect(gtk_builder_get_object(builder, "switch_int_hex"), "state-changed", G_CALLBACK(get_switch_state), NULL);
    //g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "save_button")), "clicked", G_CALLBACK(on_save), builder);
   
    // Connect the save button to the on_save function
    GtkWidget *save_button = GTK_WIDGET(gtk_builder_get_object(builder, "save_button"));
    GtkWidget *output_image = GTK_WIDGET(gtk_builder_get_object(builder, "output_image"));
    g_signal_connect(G_OBJECT(save_button), "clicked", G_CALLBACK(on_save), output_image);


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

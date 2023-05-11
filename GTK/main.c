#include <gtk/gtk.h>

#define SwitchOn 1
#define SwitchOff 0

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


int get_switch_state(GtkSwitch *sw, gpointer user_data)
{
    if (gtk_switch_get_active(sw))
    {
        g_print("Switch state: %d\n", SwitchOn);
        return SwitchOn;
    }

    g_print("Switch state: %d\n", SwitchOff);
    return SwitchOff;
}

void on_choose_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Choose button clicked!\n");
}

void on_solve(GtkButton *button, gpointer user_data)
{
    
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
    gtk_builder_add_from_file(builder, "sudo.glade", NULL);

    // Connect signal handlers
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(gtk_builder_get_object(builder, "open"), "activate", G_CALLBACK(on_open_activate), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "close"), "select", G_CALLBACK(on_quit_activate), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "about"), "activate", G_CALLBACK(on_about_activate), NULL);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "choose_button")), "clicked", G_CALLBACK(on_choose_image), builder);
    g_signal_connect(gtk_builder_get_object(builder, "switch_int_hex"), "state-changed", G_CALLBACK(get_switch_state), NULL);

    // Get the main window widget
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    // Show the main window
    gtk_widget_show_all(window);

    // Start the main event loop
    gtk_main();

    // Clean up
    g_object_unref(builder);

    return 0;
}

#include <gtk/gtk.h>

// Action by the button Play from the main menu.
void on_start(GtkButton *button, gpointer user_data)
{
    // Goes on the page 1.
    GtkBuilder* builder = (GtkBuilder*) user_data;
    GtkStack* main_stack = (GtkStack*) gtk_builder_get_object(builder, "main_stack");
    GtkWidget* page1 = (GtkWidget*) gtk_builder_get_object(builder, "page1");
    gtk_stack_set_visible_child(main_stack, page1);
}

// Action by the button Import from the main menu.
void on_import(GtkButton *button, gpointer user_data)
{
    // Goes on the page 2.
    GtkBuilder* builder = (GtkBuilder*) user_data;
    GtkStack* main_stack = (GtkStack*) gtk_builder_get_object(builder, "main_stack");
    GtkWidget* page2 = (GtkWidget*) gtk_builder_get_object(builder, "page2");
    gtk_stack_set_visible_child(main_stack, page2);
}

// Action by the button Quit from the main menu.
void on_quit(GtkButton *button, gpointer user_data)
{
    gtk_window_close(GTK_WINDOW(user_data));
}

// Main function.
int main (int argc, char *argv[])
{
    // Initializes GTK.
    gtk_init(NULL, NULL);

    // Loads the UI description and build the UI. Exits if an error occurs.
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "sudo.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }
    
    // Widgets.
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));

    GtkButton* start_button = GTK_BUTTON(gtk_builder_get_object(builder, "start_button"));
    GtkButton* quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));
    GtkButton* import_button = GTK_BUTTON(gtk_builder_get_object(builder, "import_button"));

    // Connects signal.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start), builder);
    g_signal_connect(import_button, "clicked", G_CALLBACK(on_import), NULL);
    g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit), window);

    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
    
}

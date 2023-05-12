#include <gtk/gtk.h>
#include <stdio.h>
#include <time.h>

gulong timer_id;
gint64 start_time;

int isValidSudoku(int sudoku[9][9]) {
    int usedRow[9][9] = { };
    int usedColumn[9][9] = { };
    int usedBox[3][3][9] = { };

    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            int digit = sudoku[row][col];
            if (digit != 0) {
                if (usedRow[row][digit - 1] || usedColumn[col][digit - 1] || usedBox[row / 3][col / 3][digit - 1]) {
                    return 0; // Duplicate digit found
                }

                usedRow[row][digit - 1] = 1;
                usedColumn[col][digit - 1] = 1;
                usedBox[row / 3][col / 3][digit - 1] = 1;
            }
        }
    }

    return 1; // Grid is valid
}

int isSafe(int sudoku[9][9], int row, int col, int num) {
    // Check row
    for (int i = 0; i < 9; i++) {
        if (sudoku[row][i] == num) {
            return 0;
        }
    }

    // Check column
    for (int i = 0; i < 9; i++) {
        if (sudoku[i][col] == num) {
            return 0;
        }
    }

    // Check box
    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (sudoku[startRow + i][startCol + j] == num) {
                return 0;
            }
        }
    }

    return 1;
}

int solveSudoku(int sudoku[9][9]) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (sudoku[row][col] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (isSafe(sudoku, row, col, num)) {
                        sudoku[row][col] = num;

                        if (solveSudoku(sudoku)) {
                            return 1;
                        }

                        sudoku[row][col] = 0;
                    }
                }
                return 0;
            }
        }
    }
    return 1;
}

void generate_sudoku(int sudoku[9][9]) {
    // Seed the random number generator
    srand(time(NULL));

    // Initialize the sudoku grid with zeros
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sudoku[i][j] = 0;
        }
    }

    // Generate the complete sudoku grid
    while(!solveSudoku(sudoku));

    // Remove some digits to create a playable sudoku
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // Randomly remove digits
            if (rand() % 2 == 0 || rand() % 2 == 0) {
                sudoku[i][j] = 0;
            }
        }
    }
}

// Function to update the label text with the elapsed time
gboolean updateElapsedTime(GtkLabel* label) {
    gint64 current_time = g_get_monotonic_time();
    gint64 elapsed_time = current_time - start_time;

    // Convert the elapsed time to seconds
    gint64 elapsed_seconds = elapsed_time / G_USEC_PER_SEC;

    // Calculate minutes and seconds
    gint minutes = elapsed_seconds / 60;
    gint seconds = elapsed_seconds % 60;

    gchar *elapsed_string = g_strdup_printf("%02d : %02d", minutes, seconds);
    gtk_label_set_text(label, elapsed_string);
    g_free(elapsed_string);

    // Continue updating the elapsed time
    return G_SOURCE_CONTINUE;
}

void clear_board(GtkBuilder* builder)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            gchar *buffer = g_strdup_printf("%i%i", i, j);
            GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(builder, buffer));
            if (gtk_widget_get_sensitive(GTK_WIDGET(entry)))
                gtk_entry_set_text(entry, "");
        }
    }
}

void on_play(GtkButton* button, gpointer user_data)
{
    // Gets the builder.
    GtkBuilder* builder = (GtkBuilder*) user_data;
    GtkLabel* label = (GtkLabel*) gtk_builder_get_object(builder, "time");
    GtkButton* play_button = (GtkButton*) gtk_builder_get_object(builder, "play_button");

    if (timer_id == 0)
    {
        start_time = g_get_monotonic_time();
        updateElapsedTime(label);

        // Start the timer to update the elapsed time every second
        timer_id = g_timeout_add_seconds(1, (GSourceFunc)updateElapsedTime, label);

        gtk_button_set_label(play_button, "Stop");
    }
    else
    {
        g_source_remove(timer_id);
        timer_id = 0;

        clear_board(builder);
        gtk_label_set_text(label, "00 : 00");
        gtk_button_set_label(play_button, "Play");
    }
}

void on_clear(GtkButton* button, gpointer user_data)
{
    GtkBuilder* builder = (GtkBuilder*) user_data;
    clear_board(builder);
}

void printAndDisappear(const gchar *text, const gchar *color, GtkLabel* label)
{
    gtk_label_set_text(GTK_LABEL(label), text);

    gtk_widget_show(GTK_WIDGET(label));
    g_timeout_add_seconds(5, (GSourceFunc)gtk_widget_hide, label);
}

void on_validate(GtkButton* button, gpointer user_data)
{
    GtkBuilder* builder = (GtkBuilder*) user_data;

    int sudoku[9][9] = {};

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // Gets entry.
            gchar *buffer = g_strdup_printf("%i%i", i, j);
            GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, buffer));
            const gchar *text = gtk_entry_get_text(entry);
            char charValue = *text;
            int intValue = charValue - '0';
            sudoku[i][j] = intValue;
        }
    }

    GtkLabel* result_label = (GtkLabel*) gtk_builder_get_object(builder, "result_label");

    if (isValidSudoku(sudoku))
    {
        printAndDisappear("Success!", "#32A852", result_label);
    }
    else
    {
        printAndDisappear("You failed!", "#A84432", result_label);
    }
}

void on_new(GtkButton* button, gpointer user_data)
{
    GtkBuilder* builder = (GtkBuilder*) user_data;

    if (timer_id != 0)
    {
        g_source_remove(timer_id);
        timer_id = 0;

        clear_board(builder);
        GtkButton* play_button = (GtkButton*) gtk_builder_get_object(builder, "play_button");
        GtkLabel* label = (GtkLabel*) gtk_builder_get_object(builder, "time");
        gtk_label_set_text(label, "00 : 00");
        gtk_button_set_label(play_button, "Play");
    }

    int sudoku[9][9] = {};
    generate_sudoku(sudoku);
    upload_grid(sudoku, builder);
}

void upload_grid(int sudoku[9][9], GtkBuilder* builder)
{
    // Set not editable cases.
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // Gets entry.
            gchar *buffer = g_strdup_printf("%i%i", i, j);
            GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(builder, buffer));

            // Avoid empty cells.
            if (sudoku[i][j] != 0)
            {
                gchar *value = g_strdup_printf("%i", sudoku[i][j]);
                gtk_widget_set_sensitive(GTK_WIDGET(entry), FALSE);
                gtk_entry_set_text(entry, value);

                // Apply CSS style to make the text bold and set label color
                const char *css = "* { font-weight: bold; color: red; }";
                GtkCssProvider *provider = gtk_css_provider_new();
                gtk_css_provider_load_from_data(provider, css, -1, NULL);
                GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(entry));
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            }
            else
            {
                gtk_widget_set_sensitive(GTK_WIDGET(entry), TRUE);
                gtk_entry_set_text(entry, "");

                const char *css = "* { font-weight: bold; color: inherit; }";
                GtkCssProvider *provider = gtk_css_provider_new();
                gtk_css_provider_load_from_data(provider, css, -1, NULL);
                GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(entry));
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            }

            gtk_entry_set_alignment(entry, 0.5);
            gtk_entry_set_max_length(entry, 1);

        }
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
    GtkButton* play_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_button"));
    GtkButton* clear_button = GTK_BUTTON(gtk_builder_get_object(builder, "clear_button"));
    GtkButton* new_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_button"));
    GtkButton* validate_button = GTK_BUTTON(gtk_builder_get_object(builder, "validate_button"));

    // Connects signal.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(play_button, "clicked", G_CALLBACK(on_play), builder);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear), builder);
    g_signal_connect(new_button, "clicked", G_CALLBACK(on_new), builder);
    g_signal_connect(validate_button, "clicked", G_CALLBACK(on_validate), builder);

    // -- Generates a new grid. -- //
    on_new(new_button, builder);

    // --- OR use an existing grid. --- //
    // int sudoku[9][9];
    // upload_grid(sudoku, builder);

    // Runs the main loop.
    gtk_main();

    // Clean up resources
    g_source_remove(timer_id);

    // Exits.
    return 0;
    
}

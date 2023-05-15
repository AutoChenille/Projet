#include <SDL2/SDL.h>
#include <stddef.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <time.h>

//Preprocessing
#include "Preprocessing/pretreatment.h"

//Processing
#include "Processing/main.c"
#include "NeuronalNetwork/mainNN.h"
#include "sudoku_solver/solver9.h"

#define SwitchOn 1
#define SwitchOff 0

char WEIGHT_PATH[] = "./NeuronalNetwork/800x3_with_blank_handwrite/";
char **tosolve;
int is_activated = 0;
char *paf = NULL;
char solving = 0;
char sudoku[9][9] = {};


gulong timer_id;
gint64 start_time;

int isValidSudoku(char sudoku[9][9]) {
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

int isSafe(char sudoku[9][9], int row, int col, int num) {
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


// Helper function to check if the Sudoku is solved
int is_solved(char grid[9][9])
{
    int row, col;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            if (grid[row][col] == 0) {
                return 0;
            }
        }
    }
    return 1;
}


void solver_sudoku(char grid[9][9]) {
    int row, col;

    if (!findEmptyCell(grid, &row, &col)) {
        // If there are no empty cells left, the sudoku is solved
        return;
    }

    // Try values 1 to 9 in the empty cell
    for (int num = 1; num <= 9; num++) {
        if (isValid(grid, row, col, num)) {
            grid[row][col] = num; // Assign the number if it's valid

            // Recursively try to solve the remaining sudoku
            solveSudoku(grid);

            if (isSudokuSolved(grid)) {
                // If the sudoku is solved, return immediately
                return;
            }

            grid[row][col] = 0; // Backtrack by resetting the cell to 0
        }
    }
}



// Function to find the next empty cell in the grid
// Returns true if an empty cell is found, false otherwise

/* Find the next empty cell in the grid */
void findEmptyCell(char grid[9][9], int *row, int *col) {
    for (*row = 0; *row < 9; (*row)++) {
        for (*col = 0; *col < 9; (*col)++) {
            if (grid[*row][*col] == 0) {
                return;
            }
        }
    }
}

/* Check if the current value is valid in the given cell */
int isValid(char grid[9][9], int row, int col, int num) {
    /* Check row */
    for (int i = 0; i < 9; i++) {
        if (grid[row][i] == num) {
            return 0;
        }
    }
    
    /* Check column */
    for (int i = 0; i < 9; i++) {
        if (grid[i][col] == num) {
            return 0;
        }
    }
    
    /* Check sub-grid */
    int sub_row = (row / 3) * 3;
    int sub_col = (col / 3) * 3;
    for (int i = sub_row; i < sub_row + 3; i++) {
        for (int j = sub_col; j < sub_col + 3; j++) {
            if (grid[i][j] == num) {
                return 0;
            }
        }
    }
    
    return 1;
}

/* Check if the sudoku is solved */
int isSudokuSolved(char grid[9][9]) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (grid[row][col] == 0) {
                return 0;
            }
        }
    }
    return 1;
}

/* Solve the sudoku recursively */
int solve_sudoku(char grid[9][9]) {
    int row, col;
    
    /* Find the next empty cell */
    findEmptyCell(grid, &row, &col);
    
    /* If all cells are filled, the sudoku is solved */
    if (row == 9 && col == 0) {
        return 1;
    }
    
    /* Try all possible numbers in the empty cell */
    for (int num = 1; num <= 9; num++) {
        if (isValid(grid, row, col, num)) {
            /* Try the number in the cell */
            grid[row][col] = num;
            
            /* Recursively solve the sudoku */
            if (solveSudoku(grid)) {
                return 1;
            }
            
            /* If the recursion failed, reset the cell */
            grid[row][col] = 0;
        }
    }
    
    /* If no valid number was found, backtrack */
    return 0;
}

int solveSudoku(char sudoku[9][9]) {
    
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (sudoku[row][col] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (isSafe(sudoku, row, col, num)) 
                    {
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

void solveS(GtkButton* button,  gpointer user_data)
{

    GtkBuilder* builder = (GtkBuilder*) user_data;

    SolveSudoku(sudoku);
    for (int i = 0; i < 9; i++)
     {

        for (int j = 0; j < 9; j++)
        {
            // Gets entry.
            gchar *buffer = g_strdup_printf("%i%i", i, j);
            GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(builder, buffer));

            if (sudoku[i][j] != 0)
            {
                gchar *value = g_strdup_printf("%i", sudoku[i][j]);
                gtk_widget_set_sensitive(GTK_WIDGET(entry), TRUE);
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

}


void generate_sudoku(char sudoku[9][9]) 
{
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



void start_sudoku(char sudoku[9][9]) 
{

    FILE *fp;
    int i, j;

    fp = fopen("sudoku.txt", "r");
    if (fp == NULL) {
        g_warning("Unable to open file 'sudoku.txt'");
        return;
    }

    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            char c = fgetc(fp);
            while (c == ' ' || c == '\n') {
                c = fgetc(fp);  // skip whitespace and newlines
            }
            if (c == EOF) {
                g_warning("Reached end of file prematurely");
                fclose(fp);
                return;
            }
            if (c == '.') {
                sudoku[i][j] = 0;  // treat '.' as empty cell
            } else if (c >= '1' && c <= '9') {
                sudoku[i][j] = c - '0';  // convert character to integer
            } else {
                g_warning("Invalid character in file 'sudoku.txt'");
                fclose(fp);
                return;
            }
        }
    }

    fclose(fp);
    
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
        //g_print("coucou\n");
        g_source_remove(timer_id);
        timer_id = 0;

        clear_board(builder);
        GtkButton* play_button = (GtkButton*) gtk_builder_get_object(builder, "play_button");
        GtkLabel* label = (GtkLabel*) gtk_builder_get_object(builder, "time");
        gtk_label_set_text(label, "00 : 00");
        gtk_button_set_label(play_button, "Play");
        generate_sudoku(sudoku);

    }

       else 
    {
        start_sudoku(sudoku);
    }
    
    // Set not editable cases.
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // Gets entry.
            gchar *buffer = g_strdup_printf("%i%i", i, j);
            GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(builder, buffer));

            if (sudoku[i][j] != 0)
            {
                gchar *value = g_strdup_printf("%i", sudoku[i][j]);
                gtk_widget_set_sensitive(GTK_WIDGET(entry), TRUE);
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
}


// Callback function for the "Open" menu item
void on_open_activate(GtkMenuItem *MenuItem, gpointer user_data)
{
    (void) MenuItem;
    (void) user_data;

    g_print("Open menu item clicked\n");
}


// Callback function for the "Quit" menu item and the main window's close button
void on_quit_activate()
{
    gtk_main_quit();
}


// Callback function for the "About" menu item
void on_about_activate(GtkMenuItem *MenuItem, gpointer user_data)
{
    (void) MenuItem;
    (void) user_data;

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(NULL, flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "HELP US");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


void file_opener(GtkWidget *widget, gpointer data)
{
    (void) widget;

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
    (void) user_data;

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
    (void) button;

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
    if (!gdk_pixbuf_savev(pixbuf, filename, "png", NULL, NULL, NULL)) {
        g_printerr("Error saving file.");
    } else {
        g_print("File saved successfully: %s\n", filename);
    }
}



void on_choose_image(GtkButton *button, gpointer user_data)
{
    (void) button;

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
        if (width > 350 || height > 350) {
            // Resize the image to fit into a 300x300 square
            int new_width, new_height;
            if (width > height) {
                new_width = 350;
                new_height = height * 350 / width;
            } else {
                new_width = width * 350 / height;
                new_height = 350;
            }
            GdkPixbuf *new_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
            g_object_unref(pixbuf);
            pixbuf = new_pixbuf;
        }

        // Set the image in the GtkImage widget
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
        g_object_unref(pixbuf);
    }

    paf = filename;
    gtk_widget_destroy(dialog);
    
}


void resize_image(GtkWidget *image_widget, size_t height, size_t width)
{
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image_widget));
    GdkPixbuf *resized = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), resized);
    g_object_unref(resized);
}


void open_image(GtkButton *button, gpointer user_data)
{
    // Call the on_choose_image() function to get the path to the image
    on_choose_image(button,user_data);//VERIFIER QUE CA FONCTIONNE
}


/*
void on_choose_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Choose button clicked!\n");
}
*/


void on_apply(GtkButton *button, gpointer user_data)//on_solve
{
    (void) button;

    GtkBuilder *builder = GTK_BUILDER(user_data);
    // int NB_CELLS = is_activated == SwitchOn ? 16 : 9;

    // Get the input image widget
    // GtkImage *input_image = GTK_IMAGE(gtk_builder_get_object(builder, "input_image"));

    // Get the pixbuf from the input image
    // GdkPixbuf *pixbuf = gtk_image_get_pixbuf(input_image);

    if(is_activated == SwitchOn)
    {
        char tosolve16[16][16];
        for (size_t i = 0; i < 16; i++)
            for (size_t j = 0; j < 16; j++)
                tosolve16[i][j] = tosolve[i][j];

        SolveHexadoku(tosolve16);
        draw_hexadoku(tosolve16, "./solved.png");
    }
    else
    {
        char tosolve9[9][9];
        for (size_t i = 0; i < 9; i++)
            for (size_t j = 0; j < 9; j++)
                tosolve9[i][j] = tosolve[i][j];

        SolveSudoku(tosolve9);
        draw_sudoku(tosolve9, "./solved.png");
    }

    // Get the output image widget
    GtkImage *output_image = GTK_IMAGE(gtk_builder_get_object(builder, "output_image"));

    // Load the solved image into the output image widget
    GdkPixbuf *solved_pixbuf = gdk_pixbuf_new_from_file("./solved.png", NULL);
    gtk_image_set_from_pixbuf(output_image, solved_pixbuf);

    resize_image(output_image, 350, 350);
    // Clean up
    g_object_unref(solved_pixbuf);
}


void on_process(GtkButton *button, gpointer user_data)
{
   (void) button;
    int NB_CELLS = is_activated == SwitchOn ? 16 : 9;


    //SDL_Surface **loaded = calloc(81, sizeof(SDL_Surface*));
    /*
    for (int i = 0; i < 81; i++)
    {
        char filepath[20];
        snprintf(filepath, sizeof(filepath), "img_working/%i.png", i);
        loaded[i] = IMG_Load(filepath);
    }
    */


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1; // Quitter l'application avec un code d'erreur
    }


    SDL_Surface **loaded = ProcessImage(paf, NB_CELLS);

    printf("les thibaults sont pas droles\n");

    /*for (int i = 0; i < 81; i++)
    {
        char filepath[100];
        snprintf(filepath, sizeof(filepath), "img2/%i.png", i);
        IMG_SavePNG(loaded[i], filepath);
    }*/

    printf("all saved\n");
    
    if(is_activated == SwitchOff)
    {
        //TrainNetwork("./NeuronalNetwork/dataIMG1/", "./overfitWeigth");
        tosolve = PredictSurface_9x9(loaded, NB_CELLS,"./overfitWeigth");

	    char tosolve9[9][9];
	    for (size_t i = 0; i < 9; i++)
	        for (size_t j = 0; j < 9; j++)
	            tosolve9[i][j] = tosolve[i][j];

        if(tosolve == NULL)
            printf("Error while loading grid");

        draw_sudoku(tosolve9, "./unsolved.png");
    }

   
    // Load the new image into the output_image GtkImage widget
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *output_image = GTK_WIDGET(gtk_builder_get_object(builder, "output_image"));
    gtk_image_set_from_file(GTK_IMAGE(output_image), "./unsolved.png");

    resize_image(output_image, 350, 350);

}


void on_go_to_sudoku_clicked(GtkButton *button, gpointer user_data)
{
    // Get a pointer to the "sudo_window" widget
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *sudo_window = GTK_WIDGET(gtk_builder_get_object(builder, "sudo_window"));
    // Get a pointer to the "main_window" widget
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    // If the widget is found, show it
    if (sudo_window != NULL) {
        gtk_widget_show(sudo_window);
        gtk_widget_hide(main_window);
    } else {
        g_warning("Unable to find widget 'sudo_window'");
    }
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
    GtkWidget *solve_button = GTK_WIDGET(gtk_builder_get_object(builder, "solve_button"));
    GtkWidget *process_button = GTK_WIDGET(gtk_builder_get_object(builder, "process_button"));
    GtkButton* play_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_button"));
    GtkButton* clear_button = GTK_BUTTON(gtk_builder_get_object(builder, "clear_button"));
    GtkButton* new_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_button"));
    GtkButton* validate_button = GTK_BUTTON(gtk_builder_get_object(builder, "validate_button"));
    GtkButton* to_solve = GTK_BUTTON(gtk_builder_get_object(builder, "to_solve"));





    // Connects signal.
    g_signal_connect(G_OBJECT(save_button), "clicked", G_CALLBACK(on_save), output_image);
    g_signal_connect(solve_button, "clicked", G_CALLBACK(on_apply), builder);
    g_signal_connect(process_button, "clicked", G_CALLBACK(on_process), builder);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(play_button, "clicked", G_CALLBACK(on_play), builder);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear), builder);
    g_signal_connect(new_button, "clicked", G_CALLBACK(on_new), builder);
    g_signal_connect(validate_button, "clicked", G_CALLBACK(on_validate), builder);
    g_signal_connect(to_solve, "clicked", G_CALLBACK(solveS), builder);

    GtkWidget *go_sudoku_button = gtk_button_new_with_label("go_to_sudoku");
            // Get the main window widget
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_container_add(GTK_CONTAINER(window), go_sudoku_button);

    // Connect signal handlers
    gtk_builder_connect_signals(builder, NULL);

    // Get a pointer to the "go_to_sudoku" button widget
    go_sudoku_button = GTK_WIDGET(gtk_builder_get_object(builder, "go_to_sudoku"));

    // Connect the button's "clicked" signal to the on_go_to_sudoku_clicked function
    g_signal_connect(go_sudoku_button, "clicked", G_CALLBACK(on_go_to_sudoku_clicked), builder);

    // Get a pointer to the main window widget
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    // Create a new GdkRGBA color object and set its values to red
    //GdkRGBA color;
    //gdk_rgba_parse(&color, "white");

    // Set the background color of the GtkBox
    // gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &color);

    // Show the main window
    gtk_widget_show(window);


    // Generates a new grid.
    on_new(new_button, builder);


    // Start the main event loop
    gtk_main();

    // Clean up
    g_object_unref(builder);

    
    // Clean up resources
    g_source_remove(timer_id);

    SDL_Quit();
    return 0;

}

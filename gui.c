#include <gtk/gtk.h>
#include <stdio.h>
#include"pd.h"

/**make all gtk widgets as local variables and pass a struct
 * merge the 2 delete functions together
 * add function to only display the current file and not the whole path
 * add functionality to display the percentage for both files
 * add functionality to work for docx and pdf files
*/
GtkWidget *file1_label, *file2_label;
GtkWidget *file1_button, *file2_button;
GtkWidget *file1_delete_button, *file2_delete_button;
GtkWidget *detect_button;
GtkWidget *plagiarism_value_displayer;

// Function to open file dialog
void on_file_button_clicked(GtkWidget *button, gpointer user_data) {
    int file_number = GPOINTER_TO_INT(user_data);
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select File",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (file_number == 1) {
            gtk_label_set_text(GTK_LABEL(file1_label), filename);
        } else {
            gtk_label_set_text(GTK_LABEL(file2_label), filename);
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Function to delete file 1
void on_file1_delete_button_clicked(GtkWidget *button) {
    gtk_label_set_text(GTK_LABEL(file1_label), "No file selected");
}

// Function to delete file 2
void on_file2_delete_button_clicked(GtkWidget *button) {
    gtk_label_set_text(GTK_LABEL(file2_label), "No file selected");
}

// Function to handle detection logic for files 1 and 2
void on_detect_button_clicked(GtkWidget *button, gpointer plagiarism_value_holder) {
    const char *file1_text = gtk_label_get_text(GTK_LABEL(file1_label));
    const char *file2_text = gtk_label_get_text(GTK_LABEL(file2_label));
    if (g_strcmp0(file1_text, "No file selected") == 0 || g_strcmp0(file2_text, "No file selected") == 0) {
        g_print("Please select both files.\n");
        gtk_label_set_text(GTK_LABEL(plagiarism_value_displayer), "Please select files in both slots");
        return;
    }
    float *plagiarism_value = (float *)plagiarism_value_holder;
    gchar *plagiarism_value_as_string;

    char **file_names = (char **) malloc (sizeof(char *) * 2);
    file_names[0] = (char *)file1_text;
    file_names[1] = (char *)file2_text;

    *plagiarism_value = pd_main(file_names);

    int float_width = snprintf(NULL, 0, "%.2f", *plagiarism_value);
    plagiarism_value_as_string = (gchar *) malloc (float_width + 1);
    snprintf(plagiarism_value_as_string, float_width + 1, "%.2f", *plagiarism_value);
    free(file_names);
    gtk_label_set_text(GTK_LABEL(plagiarism_value_displayer), plagiarism_value_as_string);
    return;
}

// Function to initialize the GUI elements and attach signals
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *grid;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Plagiarism Detector");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Create a vertical box to center the grid
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Center the box horizontally and vertically
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    // Create the grid and attach it to the box
    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    
    // Set spacing for the grid
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE); // Equal column width
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);

    file1_button = gtk_button_new_with_label("Select File 1");
    g_signal_connect(file1_button, "clicked", G_CALLBACK(on_file_button_clicked), GINT_TO_POINTER(1));
    gtk_grid_attach(GTK_GRID(grid), file1_button, 0, 2, 1, 1);

    file1_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), file1_label, 1, 2, 1, 1);

    // Delete button for File 1
    file1_delete_button = gtk_button_new_with_label("Delete File 1");
    g_signal_connect(file1_delete_button, "clicked", G_CALLBACK(on_file1_delete_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), file1_delete_button, 0, 3, 2, 1);

    file2_button = gtk_button_new_with_label("Select File 2");
    g_signal_connect(file2_button, "clicked", G_CALLBACK(on_file_button_clicked), GINT_TO_POINTER(2));
    gtk_grid_attach(GTK_GRID(grid), file2_button, 0, 4, 1, 1);

    file2_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), file2_label, 1, 4, 1, 1);

    // Delete button for File 2
    file2_delete_button = gtk_button_new_with_label("Delete File 2");
    g_signal_connect(file2_delete_button, "clicked", G_CALLBACK(on_file2_delete_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), file2_delete_button, 0, 5, 2, 1);

    // Detect button for files 1 and 2
    detect_button = gtk_button_new_with_label("Detect Plagiarism");
    g_signal_connect(detect_button, "clicked", G_CALLBACK(on_detect_button_clicked), user_data);
    gtk_grid_attach(GTK_GRID(grid), detect_button, 0, 6, 2, 1);

    /*Label showing plagiarism extent*/
    plagiarism_value_displayer = gtk_label_new("No files checked for plagiarism");
    gtk_grid_attach(GTK_GRID(grid), plagiarism_value_displayer, 0, 7, 2, 1);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    float plagiarismValue;

    app = gtk_application_new("com.gmail.rotihaor.plagiarismdetector", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &plagiarismValue);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

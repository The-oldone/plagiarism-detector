#include <gtk/gtk.h>

GtkWidget *file1_label, *file2_label;
GtkWidget *file1_button, *file2_button;
GtkWidget *file1_delete_button, *file2_delete_button;
GtkWidget *detect_button;

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
void on_detect_button_clicked(GtkWidget *button) {
    const gchar *file1_text = gtk_label_get_text(GTK_LABEL(file1_label));
    const gchar *file2_text = gtk_label_get_text(GTK_LABEL(file2_label));

    if (g_strcmp0(file1_text, "No file selected") == 0 || g_strcmp0(file2_text, "No file selected") == 0) {
        g_print("Please select both files to detect plagiarism.\n");
        return;
    }

    // Mock detection operation
    g_print("Detecting plagiarism between:\n");
    g_print("File 1: %s\n", file1_text);
    g_print("File 2: %s\n", file2_text);
}

// Function to set up the GUI
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *center_box;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Plagiarism Detector");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600); // Set default window size

    // Create a vertical box to center the grid
    center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), center_box);

    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(center_box), grid, FALSE, FALSE, 0);

    // Set spacing for the grid
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE); // Equal column width

    // Normal heading
    GtkWidget *heading = gtk_label_new("Plagiarism Detector");
    gtk_grid_attach(GTK_GRID(grid), heading, 0, 0, 2, 1);

    // Small Files section heading
    GtkWidget *small_files_heading = gtk_label_new("Put Smaller File In File 1 For Higher Efficiency:");
    gtk_grid_attach(GTK_GRID(grid), small_files_heading, 0, 1, 2, 1); // Moved up

    // File selection buttons
    file1_button = gtk_button_new_with_label("Select File 1");
    g_signal_connect(file1_button, "clicked", G_CALLBACK(on_file_button_clicked), GINT_TO_POINTER(1));
    gtk_grid_attach(GTK_GRID(grid), file1_button, 0, 2, 1, 1); // Updated row
    gtk_widget_set_size_request(file1_button, 100, -1); // Set a smaller fixed width

    file1_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), file1_label, 1, 2, 1, 1); // Updated row

    // Delete button for File 1
    file1_delete_button = gtk_button_new_with_label("Delete File 1");
    g_signal_connect(file1_delete_button, "clicked", G_CALLBACK(on_file1_delete_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), file1_delete_button, 0, 3, 2, 1); // Updated row

    file2_button = gtk_button_new_with_label("Select File 2");
    g_signal_connect(file2_button, "clicked", G_CALLBACK(on_file_button_clicked), GINT_TO_POINTER(2));
    gtk_grid_attach(GTK_GRID(grid), file2_button, 0, 4, 1, 1);
    gtk_widget_set_size_request(file2_button, 100, -1); // Set a smaller fixed width

    file2_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), file2_label, 1, 4, 1, 1);

    // Delete button for File 2
    file2_delete_button = gtk_button_new_with_label("Delete File 2");
    g_signal_connect(file2_delete_button, "clicked", G_CALLBACK(on_file2_delete_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), file2_delete_button, 0, 5, 2, 1);

    // Detect button for files 1 and 2
    detect_button = gtk_button_new_with_label("Detect Plagiarism");
    g_signal_connect(detect_button, "clicked", G_CALLBACK(on_detect_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), detect_button, 0, 6, 2, 1);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.plagiarismdetector", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

#include <gtk/gtk.h>
#include <stdio.h>
#include"pd.h"

/**make all gtk widgets as local variables and pass a struct
 * add functionality to display the percentage for both files
 * add functionality to work for docx and pdf files
*/
typedef struct {
    GtkWidget *file1_label, *file2_label;
    GtkWidget *file1_button, *file2_button;
    GtkWidget *file1_delete_button, *file2_delete_button;
    GtkWidget *detect_button;
    GtkWidget *plagiarism_value_displayer;
}widget_data;

// Function to open file dialog
void on_file_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select files to compare",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    widget_data *widgets = (widget_data *) user_data;

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char *filename = g_path_get_basename(filepath);
        if (button == widgets -> file1_button) {
            gtk_label_set_text(GTK_LABEL(widgets -> file1_label), filename);
        } else {
            gtk_label_set_text(GTK_LABEL(widgets -> file2_label), filename);
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Function to delete file 1
void on_file_delete_button_clicked(GtkWidget *button, gpointer file_data) {
    widget_data *widgets = (widget_data *) file_data;
    if(widgets -> file1_delete_button == button) {
        gtk_label_set_text(GTK_LABEL(widgets -> file1_label), "No file selected");
        return;
    }
    gtk_label_set_text(GTK_LABEL(widgets -> file2_label), "No file selected");
    return;
}

// Function to handle detection logic for files 1 and 2
void on_detect_button_clicked(GtkWidget *button, gpointer user_data) {
    widget_data *widgets = (widget_data *) user_data;
    const char *file1_text = gtk_label_get_text(GTK_LABEL(widgets -> file1_label));
    const char *file2_text = gtk_label_get_text(GTK_LABEL(widgets -> file2_label));
    if (g_strcmp0(file1_text, "No file selected") == 0 || g_strcmp0(file2_text, "No file selected") == 0) {
        g_print("Please select both files.\n");
        gtk_label_set_text(GTK_LABEL(widgets -> plagiarism_value_displayer), "Please select files in both slots");
        return;
    }
    float plagiarism_value_file_1, plagiarism_value_file_2;
    gchar *plagiarism_value_file_1_as_string, *plagiarism_value_file_2_as_string;

    char **file_names = (char **) malloc (sizeof(char *) * 2);
    file_names[0] = (char *)file1_text;
    file_names[1] = (char *)file2_text;

    plagiarism_value_file_1 = pd_main_text_file(file_names);

    file_names[0] = (char *)file2_text;
    file_names[1] = (char *)file1_text;

    plagiarism_value_file_2 = pd_main_text_file(file_names);

    free(file_names);
    gchar *final_message = (gchar *) malloc(strlen("Plagiarism extent of file 1: 12345%%\nPlagiarism extent of file 2: 12345%%") * sizeof(char));
    snprintf(final_message, 99, "Plagiarism extent of file 1: %.2f%%\nPlagiarism extent of file 2: %.2f%%", plagiarism_value_file_1, plagiarism_value_file_2);
    gtk_label_set_text(GTK_LABEL(widgets -> plagiarism_value_displayer), final_message);
    free(final_message);
    return;
}

// Function to initialize the GUI elements and attach signals
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *grid;
    widget_data *widgets = (widget_data *) user_data;

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

    widgets -> file1_button = gtk_button_new_with_label("Select File 1");
    g_signal_connect(widgets -> file1_button, "clicked", G_CALLBACK(on_file_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file1_button, 0, 2, 1, 1);

    widgets -> file1_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), widgets -> file1_label, 1, 2, 1, 1);

    widgets -> file1_delete_button = gtk_button_new_with_label("Delete File 1");
    g_signal_connect(widgets -> file1_delete_button, "clicked", G_CALLBACK(on_file_delete_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file1_delete_button, 0, 3, 2, 1);

    widgets -> file2_button = gtk_button_new_with_label("Select File 2");
    g_signal_connect(widgets -> file2_button, "clicked", G_CALLBACK(on_file_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file2_button, 0, 4, 1, 1);

    widgets -> file2_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), widgets -> file2_label, 1, 4, 1, 1);

    // Delete button for File 2
    widgets -> file2_delete_button = gtk_button_new_with_label("Delete File 2");
    g_signal_connect(widgets -> file2_delete_button, "clicked", G_CALLBACK(on_file_delete_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file2_delete_button, 0, 5, 2, 1);

    // Detect button for files 1 and 2
    widgets -> detect_button = gtk_button_new_with_label("Detect Plagiarism");
    g_signal_connect(widgets -> detect_button, "clicked", G_CALLBACK(on_detect_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> detect_button, 0, 6, 2, 1);

    /*Label showing plagiarism extent*/
    widgets -> plagiarism_value_displayer = gtk_label_new("No files checked for plagiarism");
    gtk_grid_attach(GTK_GRID(grid), widgets -> plagiarism_value_displayer, 0, 7, 2, 1);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    widget_data widgets = {0};

    app = gtk_application_new("com.gmail.rotihaor.plagiarismdetector", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &widgets);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

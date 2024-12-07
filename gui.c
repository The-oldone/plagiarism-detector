#include <gtk/gtk.h>
#include <stdio.h>
#include"pd.h"

typedef struct {
    GtkWidget *file1_label, *file2_label;
    GtkWidget *file1_button, *file2_button;
    GtkWidget *file1_delete_button, *file2_delete_button;
    GtkWidget *detect_button;
    GtkWidget *plagiarism_value_displayer;
    GtkWidget *wordsInARowThreshold, *wordsInARowThresholdLabel;
    GtkWidget *historyHolders[5];
}widget_data;

// Function to open file dialog
void on_file_button_clicked(GtkWidget *button, gpointer user_data) {
    /*dialog to open files*/
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select files to compare",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    widget_data *widgets = (widget_data *) user_data;

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        /*extracts only the file name*/
        char *filename = g_path_get_basename(filepath);
    /*checks which file choose button is activated*/
        if (button == widgets -> file1_button) {
            gtk_label_set_text(GTK_LABEL(widgets -> file1_label), filename);
        } else {
            gtk_label_set_text(GTK_LABEL(widgets -> file2_label), filename);
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

/* Function to delete file 1*/
void on_file_delete_button_clicked(GtkWidget *button, gpointer file_data) {
    widget_data *widgets = (widget_data *) file_data;
    /*checks which file delete button is activated*/
    if(widgets -> file1_delete_button == button) {
        gtk_label_set_text(GTK_LABEL(widgets -> file1_label), "No file selected");
        return;
    }
    gtk_label_set_text(GTK_LABEL(widgets -> file2_label), "No file selected");
    return;
}
void assign_to_labels(gchar *final_message, gpointer widget_set) {
    widget_data *widgets = (widget_data *) widget_set;
    int i;
    /* Process no files checked yet*/
    if(strcmp(gtk_label_get_text(GTK_LABEL(widgets -> historyHolders[0])), "No files checked for plagiarism") == 0){
        gtk_label_set_text(GTK_LABEL(widgets -> historyHolders[0]), final_message);
        return;
    }
    GtkWidget **notNullValidifier = widgets -> historyHolders;
    /*reach next null value*/
    for(i = 1; i < 5 && *gtk_label_get_text(GTK_LABEL(notNullValidifier[i])); i++){
    }
    /*If last is empty, we get overflow, fix it*/
    if(i == 5) {
        i = 4;
    }
    /*shift all labels down by 1 and add new */
    for(; i > 0; i--) {
        gtk_label_set_text(GTK_LABEL(notNullValidifier[i]), gtk_label_get_text((GTK_LABEL(notNullValidifier[i - 1]))));
    }
    gtk_label_set_text(GTK_LABEL(widgets -> historyHolders[0]), final_message);
}

// Function to handle detection logic for files 1 and 2
void on_detect_button_clicked(GtkWidget *button, gpointer user_data) {
    widget_data *widgets = (widget_data *) user_data;
    const char *file1_text = gtk_label_get_text(GTK_LABEL(widgets -> file1_label));
    const char *file2_text = gtk_label_get_text(GTK_LABEL(widgets -> file2_label));
    /*handles case when either file isn't choosed*/
    if (g_strcmp0(file1_text, "No file selected") == 0 || g_strcmp0(file2_text, "No file selected") == 0) {
        g_print("Please select both files.\n");
        gtk_label_set_text(GTK_LABEL(widgets -> plagiarism_value_displayer), "Please select files in both slots");
        return;
    }
    float plagiarism_value_file_1, plagiarism_value_file_2;
    gchar *plagiarism_value_file_1_as_string, *plagiarism_value_file_2_as_string;
    /*gets the spin button value*/
    gint wordsInARowValue = gtk_spin_button_get_value_as_int((GtkSpinButton *) widgets -> wordsInARowThreshold);
    char **file_names = (char **) malloc (sizeof(char *) * 2);
    file_names[0] = (char *)file1_text;
    file_names[1] = (char *)file2_text;

    /*plagiarism value of first file*/
    plagiarism_value_file_1 = pd_main_text_file(file_names, wordsInARowValue);

    file_names[0] = (char *)file2_text;
    file_names[1] = (char *)file1_text;

    /*plagiarism value of second file*/
    plagiarism_value_file_2 = pd_main_text_file(file_names, wordsInARowValue);

    free(file_names);
    /*allocates enough memory for maximum file length*/
    gchar *final_message = (gchar *) malloc((strlen("Plagiarism extent of : 12345%%\nPlagiarism extent of : 12345%%") + 512)* sizeof(char));
    /*assignes formatted string*/
    snprintf(final_message, 600, "Plagiarism extent of %s: %.2f%%\nPlagiarism extent of %s: %.2f%%", gtk_label_get_text(GTK_LABEL(widgets -> file1_label)), plagiarism_value_file_1, gtk_label_get_text(GTK_LABEL(widgets -> file2_label)), plagiarism_value_file_2);
    gtk_label_set_text(GTK_LABEL(widgets -> plagiarism_value_displayer), final_message);
    assign_to_labels(final_message, (gpointer)widgets);
    free(final_message);
    return;
}

// Function to initialize the GUI elements and attach signals
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *grid;
    widget_data *widgets = (widget_data *) user_data;

    /*Creates a window*/
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Plagiarism Detector");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    /* Create a vertical box to center the grid*/
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    /* Center the box horizontally and vertically*/
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    /* Create the grid and attach it to the box*/
    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    
    /* Set spacing for the grid*/
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE); // Equal column width
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);

    /*Creates a file chooser button*/
    widgets -> file1_button = gtk_button_new_with_label("Select File 1");
    g_signal_connect(widgets -> file1_button, "clicked", G_CALLBACK(on_file_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file1_button, 0, 2, 1, 1);

    /*Creates a file name label*/
    widgets -> file1_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), widgets -> file1_label, 1, 2, 1, 1);

    /*Creates a file deleter button*/
    widgets -> file1_delete_button = gtk_button_new_with_label("Delete File 1");
    g_signal_connect(widgets -> file1_delete_button, "clicked", G_CALLBACK(on_file_delete_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file1_delete_button, 0, 3, 2, 1);

    /*Creates a file chooser button*/
    widgets -> file2_button = gtk_button_new_with_label("Select File 2");
    g_signal_connect(widgets -> file2_button, "clicked", G_CALLBACK(on_file_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file2_button, 0, 4, 1, 1);

    /*Creates a file name label*/
    widgets -> file2_label = gtk_label_new("No file selected");
    gtk_grid_attach(GTK_GRID(grid), widgets -> file2_label, 1, 4, 1, 1);

    /*Creates a file deleter button*/
    widgets -> file2_delete_button = gtk_button_new_with_label("Delete File 2");
    g_signal_connect(widgets -> file2_delete_button, "clicked", G_CALLBACK(on_file_delete_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> file2_delete_button, 0, 5, 2, 1);

    /*Creates a file plagiarism detector button*/
    widgets -> detect_button = gtk_button_new_with_label("Detect Plagiarism");
    g_signal_connect(widgets -> detect_button, "clicked", G_CALLBACK(on_detect_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), widgets -> detect_button, 0, 6, 2, 1);

    /*Creates a notice saying what the spin button does
    (sets a threshold for minimum words to be in a row to be considered
    plagiarised)*/
    widgets -> wordsInARowThresholdLabel = gtk_label_new("Specify minimum words in a row to be\nconsidered for checking");
    gtk_grid_attach(GTK_GRID(grid), widgets -> wordsInARowThresholdLabel, 0, 7, 2, 1);

    /*Creates an adjustment and a spinbutton for the minimum words 
    in a row threshold*/
    GtkAdjustment *adjustment;
    adjustment = gtk_adjustment_new(1.0, 0.0, 1000.0, 1.0, 5.0, 0.0);
    widgets -> wordsInARowThreshold = gtk_spin_button_new(adjustment, 1.0, 0);
    gtk_grid_attach(GTK_GRID(grid), widgets -> wordsInARowThreshold, 0, 8, 2, 1);

    /*Label showing plagiarism extent*/
    widgets -> plagiarism_value_displayer = gtk_label_new("No files checked for plagiarism");
    gtk_grid_attach(GTK_GRID(grid), widgets -> plagiarism_value_displayer, 0, 9, 2, 1);

    widgets -> historyHolders[0] = gtk_label_new("No files checked for plagiarism");
    gtk_grid_attach(GTK_GRID(grid), widgets -> historyHolders[0], 2, 2, 1, 1);

    widgets -> historyHolders[1] = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), widgets -> historyHolders[1], 2, 3, 1, 1);

    widgets -> historyHolders[2] = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), widgets -> historyHolders[2], 2, 4, 1, 1);

    widgets -> historyHolders[3] = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), widgets -> historyHolders[3], 2, 5, 1, 1);

    widgets -> historyHolders[4] = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), widgets -> historyHolders[4], 2, 6, 1, 1);

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

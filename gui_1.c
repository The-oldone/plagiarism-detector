#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 10000 // Max words to process
#define HASH_SIZE 1000  // Hash table size

// Node structure for the doubly linked list
typedef struct node {
    char word[20];
    struct node *next;
    struct node *prev;
} node;

// Stack implemented using doubly linked list
typedef struct {
    node *head;
    node *end;
} dllStack;

// Hash table entry structure
typedef struct hash_entry {
    char* word;
    int index;
    struct hash_entry* next;
} hash_entry;

// Function to initialize the stack
void init_stack(dllStack *s) {
    s->head = NULL;
    s->end = NULL;
}

// Function to push a word onto the stack
void push(dllStack *s, char* word) {
    node* new_node = (node*) malloc(sizeof(node));
    strcpy(new_node->word, word);
    new_node->next = NULL;
    new_node->prev = s->end;

    if (s->end != NULL) {
        s->end->next = new_node;
    } else {
        s->head = new_node;
    }
    s->end = new_node;
}

// Hash function to generate hash for a word
int hash(char* word) {
    int hash = 0;
    while (*word) {
        hash = (hash + *word) % HASH_SIZE;
        word++;
    }
    return hash;
}

// Function to insert a word into the hash table with its index
void insert_hash_table(hash_entry* hash_table[], char* word, int index) {
    int h = hash(word);
    hash_entry* entry = (hash_entry*) malloc(sizeof(hash_entry));
    entry->word = strdup(word);
    entry->index = index;
    entry->next = hash_table[h];
    hash_table[h] = entry;
}

// Function to search the hash table and return 1 if the word is found
int search_hash_table(hash_entry* hash_table[], char* word) {
    int h = hash(word);
    hash_entry* entry = hash_table[h];
    while (entry) {
        if (strcmp(entry->word, word) == 0) {
            return 1; // Word found
        }
        entry = entry->next;
    }
    return 0; // Word not found
}

// Function to process a document and store words in stack and hash table
void process_doc(char* document, dllStack* stack, hash_entry* hash_table[]) {
    char* word = strtok(document, " ,.!?\n");
    int index = 0;

    while (word != NULL) {
        push(stack, word);
        insert_hash_table(hash_table, word, index);
        word = strtok(NULL, " ,.!?\n");
        index++;
    }
}

// Function to calculate similarity percentage between documents
float similarity_calc(dllStack* stack2, hash_entry* hash_table[]) {
    int common_words = 0;
    node* current = stack2->head;

    while (current != NULL) {
        if (search_hash_table(hash_table, current->word)) {
            common_words++;
        }
        current = current->next;
    }

    return (float)common_words / MAX_WORDS * 100;
}

// Function to determine plagiarism level based on similarity percentage
int plagiarism(float similarity) {
    if (similarity >= 60) {
        return 3; // level 3 (above 60%)
    } else if (similarity >= 40) {
        return 2; // level 2 (40-60%)
    } else if (similarity >= 10) {
        return 1; // level 1 (10-40%)
    }
    return 0; // no plagiarism
}

// Function to read file content and return it as a string
char* read_file_content(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for content
    char *content = (char *)malloc(length + 1);
    fread(content, 1, length, file);
    content[length] = '\0'; // Null-terminate the content

    fclose(file);
    return content;
}

// Global variables for file paths and result display
GtkWidget *result_label;
char *file1_path = NULL;
char *file2_path = NULL;

// Function to update label with plagiarism result
void show_result(float similarity) {
    char result[100];
    sprintf(result, "Similarity: %.2f%%\n", similarity);
    gtk_label_set_text(GTK_LABEL(result_label), result);
}

// File selection callback
void on_file1_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Document 1", NULL, 
                        GTK_FILE_CHOOSER_ACTION_OPEN, 
                        "_Open", GTK_RESPONSE_ACCEPT, "_Cancel", GTK_RESPONSE_CANCEL, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        file1_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }
    gtk_widget_destroy(dialog);
}

void on_file2_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Document 2", NULL, 
                        GTK_FILE_CHOOSER_ACTION_OPEN, 
                        "_Open", GTK_RESPONSE_ACCEPT, "_Cancel", GTK_RESPONSE_CANCEL, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        file2_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }
    gtk_widget_destroy(dialog);
}

// Function to run plagiarism check when "Check" button is clicked
void on_check_button_clicked(GtkWidget *widget, gpointer data) {
    if (file1_path == NULL || file2_path == NULL) {
        gtk_label_set_text(GTK_LABEL(result_label), "Please select two files.");
        return;
    }

    hash_entry* hash_table1[HASH_SIZE];
    hash_entry* hash_table2[HASH_SIZE];
    memset(hash_table1, 0, sizeof(hash_table1));
    memset(hash_table2, 0, sizeof(hash_table2));

    dllStack stack1, stack2;
    init_stack(&stack1);
    init_stack(&stack2);

    char* content1 = read_file_content(file1_path);
    char* content2 = read_file_content(file2_path);

    if (content1 == NULL || content2 == NULL) {
        gtk_label_set_text(GTK_LABEL(result_label), "Error reading files.");
        return;
    }

    process_doc(content1, &stack1, hash_table1);
    process_doc(content2, &stack2, hash_table2);

    float similarity = similarity_calc(&stack2, hash_table1);
    show_result(similarity);

    free(content1);
    free(content2);
}

// Main function to initialize the GTK window and widgets
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Window setup
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Plagiarism Checker");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Grid for layout
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Buttons for file selection
    GtkWidget *file1_button = gtk_button_new_with_label("Select Document 1");
    g_signal_connect(file1_button, "clicked", G_CALLBACK(on_file1_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), file1_button, 0, 0, 1, 1);

    GtkWidget *file2_button = gtk_button_new_with_label("Select Document 2");
    g_signal_connect(file2_button, "clicked", G_CALLBACK(on_file2_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), file2_button, 1, 0, 1, 1);

    // "Check" button to trigger plagiarism check
    GtkWidget *check_button = gtk_button_new_with_label("Check Plagiarism");
    g_signal_connect(check_button, "clicked", G_CALLBACK(on_check_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), check_button, 0, 1, 2, 1);

    // Label to display result
    result_label = gtk_label_new("Select files to check plagiarism");
    gtk_grid_attach(GTK_GRID(grid), result_label, 0, 2, 2, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
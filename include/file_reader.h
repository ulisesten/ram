#ifndef __FILE_READER__
#define __FILE_READER__

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <gtksourceview/gtksource.h>

GtkWidget* create_notebook(GtkBuilder* builder);
GtkWidget* create_window_controls();
void       set_notebook(GtkWidget* notebook, char* filename, char* path, GtkBuilder* builder);
void       on_save_button_clicked(GtkTextBuffer* buffer, GString* location);
GtkWidget* create_tab(const gchar* title, GtkNotebook* notebook, GtkWidget* scrolled);
void       close_tab (GtkButton *button, gpointer   user_data);
void       setting_buffer(GtkSourceBuffer* buffer, GtkWidget* source_view, char* location);
void       on_buffer_change (GtkTextBuffer *textbuffer, gpointer user_data);
gboolean   on_key_press(GtkWidget* widget, GdkEventKey *event, gpointer user_data);

#endif
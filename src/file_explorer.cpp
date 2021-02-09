#include "file_explorer.h"
#include "file_reader.h"
#include <stdio.h>


GtkTreeSelection* create_file_explorer(GtkBuilder* builder){
    GError *error = NULL;
    file_browser fb;

    fb.tree_view  = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_view"));
    fb.selection  = gtk_tree_view_get_selection (GTK_TREE_VIEW(fb.tree_view));
    fb.column0    = gtk_tree_view_column_new    ();

    fb.renderer   = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start             (fb.column0, fb.renderer, FALSE);
    gtk_tree_view_column_set_attributes         (fb.column0, fb.renderer, "pixbuf", COLUMN_PIXBUF, NULL);
    
    fb.renderer   = gtk_cell_renderer_text_new  ();
    gtk_tree_view_column_pack_start             (fb.column0, fb.renderer, TRUE);
    gtk_tree_view_column_set_attributes         (fb.column0, fb.renderer, "text", COLUMN_STRING, NULL);


    fb.pixbuf_folder            = gdk_pixbuf_new_from_file("../assets/folder2_32.png", &error);
    fb.pixbuf_text_file         = gdk_pixbuf_new_from_file("../assets/text3_32.png"  , &error);

    if (error)
    {
        g_critical ("Could not load pixbuf: %s\n", error->message);
        g_error_free(error);
    }

    fb.tree_store = gtk_tree_store_new( 2, GDK_TYPE_PIXBUF, G_TYPE_STRING );

    setup_file_explorer(NULL, fb);
    
    g_object_unref(fb.pixbuf_folder);
    g_object_unref(fb.pixbuf_text_file);

    return (fb.selection);

}

void setup_file_explorer(GtkTreeViewColumn* column, file_browser fBrowser){
    
    //GDir* dir_contents;
    GError* gerr;
    gint pos = 0;
    GtkTreeIter iter, iter_child;
    gchar* folder_name;
    char* cwd = g_get_current_dir();
    PATH paths = NULL;
    int list_err = -1;

    printf("cwd %s\n", cwd);

    //traverse(cwd, 0);
    
    list_err = filling_list(&paths, cwd);
    if(list_err < 0 )
        printf("error al llenar lista.\n");

    walkList(paths);

    folder_name = g_path_get_basename(cwd);
    gtk_tree_view_column_set_title(fBrowser.column0, folder_name);

    initialize_tree_store(&fBrowser, &iter);
    fill_tree_store(paths, &iter, NULL, fBrowser, 0);
    
    set_tree_view(fBrowser);

}


void set_tree_view(file_browser fb){
                                                     
    gtk_tree_view_set_model(fb.tree_view, GTK_TREE_MODEL( fb.tree_store ));
    gtk_tree_view_append_column(fb.tree_view, fb.column0);
    
}


void initialize_tree_store(  file_browser* fb, GtkTreeIter * iter){
    gtk_tree_store_append(fb->tree_store, &(*iter), NULL);
}


void fill_tree_store(PATH paths, GtkTreeIter* iter, GtkTreeIter* parent, file_browser fb, int indent){

    GtkTreeIter new_iter;
        
    while(paths){

        if(paths->type == FOLDER_TYPE){

            gtk_tree_store_set (fb.tree_store, iter, COLUMN_PIXBUF, fb.pixbuf_folder, COLUMN_STRING, paths->dir, -1);

            bool branched = false;
            if( paths->branch ) {
                printf("branch in %s\n", paths->dir);
                gtk_tree_store_append(fb.tree_store, &new_iter, iter);
                branched = true;
                fill_tree_store( paths->branch , &new_iter, iter, fb, indent+1);

            } //else
             
            if(paths->next)
            if(parent)
                gtk_tree_store_append(fb.tree_store, iter, parent);
            else
                gtk_tree_store_append(fb.tree_store, iter, NULL);

            if (branched)
            {
                //fill_tree_store( paths->branch , &new_iter , iter, fb, indent+1);
            }
            

        } else {
            
            gtk_tree_store_set (fb.tree_store, iter, COLUMN_PIXBUF, fb.pixbuf_text_file, COLUMN_STRING, paths->dir, -1);


            if(paths->next)
                if(parent)
                    gtk_tree_store_append(fb.tree_store, iter, parent);
                else
                    gtk_tree_store_append(fb.tree_store, iter, NULL);
            
        }

        //if(paths->next)
            //gtk_tree_store_append(fb.tree_store, &(*parent), NULL);
        paths = paths->next;
        

    }

}


gboolean
  view_selection_func (GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path, gboolean path_currently_selected, gpointer userdata) {

    GtkTreeIter iter;
    GtkWidget* notebook = (GtkWidget*) userdata;

    if (gtk_tree_model_get_iter(model, &iter, path))
    {
        gchar *name;

        gtk_tree_model_get(model, &iter, COLUMN_STRING, &name, -1);

        if (!path_currently_selected) {

            gint pos = gtk_notebook_get_n_pages ((GtkNotebook *)notebook);
            set_notebook(notebook, name, pos);

        }
    
        g_free(name);

    }

    return TRUE;
}



/**To be erased*/
void walk_dir(PATH paths, GtkTreeIter* iter, file_browser fb){

    //gchar* dir = (char*)g_dir_read_name(contents);
    //struct stat sb;

    if(paths){

        gtk_list_store_append(fb.list_store, iter);

        if(paths->type == FOLDER_TYPE){

            gtk_list_store_set (fb.list_store, iter, COLUMN_PIXBUF, fb.pixbuf_folder, COLUMN_STRING, paths->dir, -1);
            walk_dir(paths->next, iter, fb);

            GDir* new_dir_contents;
            GtkTreeIter iter_child;
            gchar* next_dir;
            GError* gerr;

            new_dir_contents = g_dir_open( g_strconcat( g_get_current_dir(),"/",paths->dir,"/", NULL), 0, &gerr);

            next_dir = (gchar*)g_dir_read_name(new_dir_contents);

            if(next_dir) {
                PATH new_path_list = NULL;
                //fillingList(&new_path_list, new_dir_contents, "");
                walkList(new_path_list);
                walk_dir(new_path_list, &iter_child, fb);
            }
            
        } else {

            gtk_list_store_set (fb.list_store, iter, COLUMN_PIXBUF, fb.pixbuf_text_file, COLUMN_STRING, paths->dir, -1);
            walk_dir(paths->next, iter, fb);

        }
    }

}




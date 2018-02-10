/*
    Copyright © 2017 Felipe Ferreira da Silva <ferreiradaselva@protonmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtk/gtk.h>
#include "handlers.h"

static void tree_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	chandler *handler = user_data;
	gchar *iter_path = NULL;
	gchar *file_path = NULL;
	GFile *file = NULL;
	GFileInfo *file_info = NULL;
	GFileType file_type;
	GtkTreeIter iter;
	iter_path = gtk_tree_path_to_string(path);
	if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(handler->sidebar.tree_store), &iter, iter_path)) {
		gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
			&iter,
			2, &file_path,
			-1);
		if (file_path) {
			file = g_file_new_for_path(file_path);
			if (file) {
				file_info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
				if (file_info) {
					file_type = g_file_info_get_file_type(file_info);
					if (file_type == G_FILE_TYPE_REGULAR) {
						cdocument *document = new_document(handler, file_path);
						add_view_for_document(handler, document);
					}
				}
				g_object_unref(G_OBJECT(file));
			}
			g_free(file_path);
		}
	}
	g_free(iter_path);
}

static void tree_view_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	chandler *handler = user_data;
	gboolean valid = FALSE;
	GtkTreeIter child;
	valid = gtk_tree_model_iter_children(GTK_TREE_MODEL(handler->sidebar.tree_store), &child, iter);
	while (valid) {
		sidebar_update_iter_children(handler, child);
		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(handler->sidebar.tree_store), &child);
	}
}

static void tree_view_row_collapsed(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	chandler *handler = user_data;
	gboolean valid = FALSE;
	GtkTreeIter child;
	valid = gtk_tree_model_iter_children(GTK_TREE_MODEL(handler->sidebar.tree_store), &child, iter);
	while (valid) {
		gtk_tree_store_remove(handler->sidebar.tree_store, &child);
		valid = gtk_tree_model_iter_children(GTK_TREE_MODEL(handler->sidebar.tree_store), &child, iter);
	}
	sidebar_update_iter_children(handler, *iter);
}

void sidebar_update_iter_children(chandler *handler, GtkTreeIter iter)
{
	GFileInfo *file_info = NULL;
	gchar *path = NULL;
	GFile *file = NULL;
	GFileType file_type;
	GDir *dir = NULL;
	const gchar *name = NULL;
	GString *full_path = NULL;
	gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
		&iter,
		2, &path,
		-1);
	file = g_file_new_for_path(path);
	if (file) {
		file_info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
		if (file_info) {
			file_type = g_file_info_get_file_type(file_info);
			if (file_type == G_FILE_TYPE_DIRECTORY) {
				dir = g_dir_open(path, 0, NULL);
			}
		}
		g_object_unref(G_OBJECT(file));
	}
	if (dir) {
		full_path = g_string_new("");
		name = g_dir_read_name(dir);
		while (name) {
			full_path = g_string_append(full_path, path);
			full_path = g_string_append(full_path, "/");
			full_path = g_string_append(full_path, name);
			sidebar_add_iter(handler, &iter, full_path->str);
			full_path = g_string_assign(full_path, "");
			name = g_dir_read_name(dir);
		}
		g_string_free(full_path, TRUE);
	}
	g_free(path);
}

GtkTreeIter sidebar_add_iter(chandler *handler, GtkTreeIter *parent, gchar *path)
{
	GtkTreeIter iter;
	GFileInfo *file_info = NULL;
	GIcon *icon = NULL;
	const char *content_type = NULL;
	GFile *file = g_file_new_for_path(path);
	gchar *basename = g_path_get_basename(path);
	gtk_tree_store_append(GTK_TREE_STORE(handler->sidebar.tree_store),
		&iter,
		parent);
	gtk_tree_store_set(GTK_TREE_STORE(handler->sidebar.tree_store),
		&iter,
		1, basename,
		2, path,
		-1);
	g_free(basename);
	if (file) {
		file_info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
		if (file_info) {
			icon = g_file_info_get_icon(file_info);
			if (!icon) {
				content_type = g_file_info_get_content_type(file_info);
				icon = g_content_type_get_icon(content_type);
			}
		}
	}
	if (!icon) {
		icon = g_icon_new_for_string("text-x-generic-template", NULL);
	}
	gtk_tree_store_set(GTK_TREE_STORE(handler->sidebar.tree_store),
			&iter,
			0, icon,
			-1);
	g_object_unref(G_OBJECT(icon));
	if (file) {
		g_object_unref(G_OBJECT(file));
	}
	return iter;
}

void init_sidebar(chandler *handler)
{
	GtkWidget *scrolled_window = NULL;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	/* Sidebar revealer */
	handler->sidebar.revealer = gtk_revealer_new();
	gtk_widget_set_name(handler->sidebar.revealer, "revealer_tree_view");
	gtk_container_add(GTK_CONTAINER(handler->session.box), handler->sidebar.revealer);
	gtk_widget_set_hexpand(handler->sidebar.revealer, FALSE);
	gtk_widget_set_vexpand(handler->sidebar.revealer, TRUE);
	gtk_widget_set_halign(handler->sidebar.revealer, GTK_ALIGN_END);
	gtk_widget_set_valign(handler->sidebar.revealer, GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler->sidebar.revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_LEFT);
	/* Box */
	handler->sidebar.box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(handler->sidebar.revealer), handler->sidebar.box);
	gtk_widget_set_hexpand(handler->sidebar.box, TRUE);
	gtk_widget_set_vexpand(handler->sidebar.box, TRUE);
	gtk_widget_set_halign(handler->sidebar.box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->sidebar.box, GTK_ALIGN_FILL);
	/* Scrolled window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(handler->sidebar.box), scrolled_window);
	gtk_widget_set_hexpand(scrolled_window, TRUE);
	gtk_widget_set_vexpand(scrolled_window, TRUE);
	gtk_widget_set_halign(scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_valign(scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(scrolled_window, SIDEBAR_TREE_VIEW_MIN_WIDTH, -1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(scrolled_window)), GTK_STYLE_CLASS_RIGHT);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(scrolled_window)), GTK_STYLE_CLASS_SIDEBAR);
	/* Tree model */
	handler->sidebar.tree_store = gtk_tree_store_new(3, G_TYPE_ICON, G_TYPE_STRING, G_TYPE_STRING);
	/* Tree view */
	handler->sidebar.tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(handler->sidebar.tree_store));
	gtk_widget_set_name(handler->sidebar.tree_view, "tree_view");
	gtk_container_add(GTK_CONTAINER(scrolled_window), handler->sidebar.tree_view);
	gtk_widget_set_hexpand(handler->sidebar.tree_view, TRUE);
	gtk_widget_set_vexpand(handler->sidebar.tree_view, TRUE);
	gtk_widget_set_halign(handler->sidebar.tree_view, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->sidebar.tree_view, GTK_ALIGN_FILL);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(handler->sidebar.tree_view), FALSE);
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(handler->sidebar.tree_view), 1);
	g_signal_connect(handler->sidebar.tree_view, "row-expanded", G_CALLBACK(tree_view_row_expanded), handler);
	g_signal_connect(handler->sidebar.tree_view, "row-collapsed", G_CALLBACK(tree_view_row_collapsed), handler);
	g_signal_connect(handler->sidebar.tree_view, "row-activated", G_CALLBACK(tree_view_row_activated), handler);
	/* Column */
	column = gtk_tree_view_column_new();
	gtk_tree_view_insert_column(GTK_TREE_VIEW(handler->sidebar.tree_view), column, -1);
	gtk_tree_view_column_set_clickable(column, FALSE);
	/* Cell renderer */
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer,
		"gicon", 0,
		NULL);
	/* Cell renderer */
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
		"text", 1,
		NULL);
}

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

void session_update_lists(chandler *handler, csession *session)
{
	GString *name = NULL;
	gchar *path = NULL;
	GFile *file = NULL;
	GList *list_iter = NULL;
	GtkWidget *page = NULL;
	GtkTreeIter tree_iter;
	gboolean valid = FALSE;
	cview *view = NULL;
	/* Clear current list of file names in views */
	list_iter = session->view_file_names;
	while (list_iter) {
		g_string_free(list_iter->data, TRUE);
		list_iter = g_list_next(list_iter);
	}
	g_list_free(session->view_file_names);
	session->view_file_names = NULL;
	/* Update list of file names in views */
	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), 0);
	while (page) {
		view = g_object_get_data(G_OBJECT(page), "view");
		file = gtk_source_file_get_location(view->document->source_file);
		path = g_file_get_path(file);
		name = g_string_new(path);
		session->view_file_names = g_list_append(session->view_file_names, name);
		g_free(path);
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook),
			gtk_notebook_page_num(GTK_NOTEBOOK(handler->session.notebook), page) + 1);
	}
	/* Clear current list of folders in sidebar */
	list_iter = session->folders;
	while (list_iter) {
		g_string_free(list_iter->data, TRUE);
		list_iter = g_list_next(list_iter);
	}
	g_list_free(session->folders);
	session->folders = NULL;
	/* Update list of folders in sidebar */
	valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(handler->sidebar.tree_store), &tree_iter);
	while (valid) {
		gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
			&tree_iter,
			2, &path,
			-1);
		name = g_string_new(path);
		session->folders = g_list_append(session->folders, name);
		g_free(path);
		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(handler->sidebar.tree_store), &tree_iter);
	}
	handler->current_session = session;
}

void session_open(chandler *handler, csession *session)
{
	GString *name = NULL;
	GList *name_iter = NULL;
	cdocument *document = NULL;
	/* Add views */
	name_iter = session->view_file_names;
	while (name_iter) {
		name = name_iter->data;
		document = new_document(handler, name->str);
		add_view_for_document(handler, document);
		name_iter = g_list_next(name_iter);
	}
	/* Add folders */
	name_iter = session->folders;
	while (name_iter) {
		name = name_iter->data;
		sidebar_add_iter(handler, NULL, name->str);
		name_iter = g_list_next(name_iter);
	}
	handler->current_session = session;
}

void session_close(chandler *handler)
{
	cview *view = NULL;
	gtk_tree_store_clear(handler->sidebar.tree_store);
	while (gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook)) > 0) {
		view = get_nth_view(handler, 0);
		close_view(handler, view, FALSE);
	}
}

static void notebook_page_removed(GtkWidget *widget, GtkWidget *child, gint page_num, gpointer user_data)
{
	chandler *handler = user_data;
	update_view_status(handler, NULL);
}

static void notebook_switch_page(GtkWidget *widget, GtkWidget *page, guint page_num, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = g_object_get_data(G_OBJECT(page), "view");
	update_view_status(handler, view);
}

void init_session(chandler *handler)
{
	/* Box */
	handler->session.paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_stack_add_named(GTK_STACK(handler->window.stack), handler->session.paned, "session");
	/* Box notebook and status bar */
	handler->session.box_vertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler->session.box_vertical, "box_vertical");
	gtk_paned_pack1(GTK_PANED(handler->session.paned),
		handler->session.box_vertical,
		TRUE,
		FALSE);
	gtk_widget_set_hexpand(handler->session.box_vertical, TRUE);
	gtk_widget_set_vexpand(handler->session.box_vertical, TRUE);
	gtk_widget_set_halign(handler->session.box_vertical, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->session.box_vertical, GTK_ALIGN_FILL);
	/* Notebook */
	handler->session.notebook = gtk_notebook_new();
	gtk_widget_set_name(handler->session.notebook, "notebook");
	gtk_container_add(GTK_CONTAINER(handler->session.box_vertical), handler->session.notebook);
	gtk_widget_set_hexpand(handler->session.notebook, TRUE);
	gtk_widget_set_vexpand(handler->session.notebook, TRUE);
	gtk_widget_set_halign(handler->session.notebook, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->session.notebook, GTK_ALIGN_FILL);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(handler->session.notebook), FALSE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(handler->session.notebook), TRUE);
	g_signal_connect(handler->session.notebook, "switch-page", G_CALLBACK(notebook_switch_page), handler);
	g_signal_connect(handler->session.notebook, "page-removed", G_CALLBACK(notebook_page_removed), handler);
}

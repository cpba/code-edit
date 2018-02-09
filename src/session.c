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
	handler->session.box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_stack_add_named(GTK_STACK(handler->window.stack), handler->session.box, "session");
	/* Box notebook and sidebar */
	handler->session.box_notebook_and_sidebar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(handler->session.box_notebook_and_sidebar, "box_notebook_and_sidebar");
	gtk_container_add(GTK_CONTAINER(handler->session.box), handler->session.box_notebook_and_sidebar);
	gtk_widget_set_hexpand(handler->session.box_notebook_and_sidebar, TRUE);
	gtk_widget_set_vexpand(handler->session.box_notebook_and_sidebar, TRUE);
	gtk_widget_set_halign(handler->session.box_notebook_and_sidebar, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->session.box_notebook_and_sidebar, GTK_ALIGN_FILL);
	/* Notebook */
	handler->session.notebook = gtk_notebook_new();
	gtk_widget_set_name(handler->session.notebook, "notebook");
	gtk_container_add(GTK_CONTAINER(handler->session.box_notebook_and_sidebar), handler->session.notebook);
	gtk_widget_set_hexpand(handler->session.notebook, TRUE);
	gtk_widget_set_vexpand(handler->session.notebook, TRUE);
	gtk_widget_set_halign(handler->session.notebook, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->session.notebook, GTK_ALIGN_FILL);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(handler->session.notebook), FALSE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(handler->session.notebook), TRUE);
	g_signal_connect(handler->session.notebook, "switch-page", G_CALLBACK(notebook_switch_page), handler);
	g_signal_connect(handler->session.notebook, "page-removed", G_CALLBACK(notebook_page_removed), handler);
}

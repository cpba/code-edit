/*
    Copyright © 2018 Felipe Ferreira da Silva <ferreiradaselva@protonmail.com>

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

void window_remove_session(chandler *handler, csession *session)
{
	GList *children = NULL;
	GtkWidget *row = NULL;
	g_key_file_remove_group(handler->key_file_sessions, session->name->str, NULL);
	row = gtk_widget_get_parent(session->box);
	gtk_widget_destroy(row);
	g_string_free(session->name, TRUE);
	g_slice_free1(sizeof(csession), session);
	children = gtk_container_get_children(GTK_CONTAINER(handler->select_session.list_box));
	if (g_list_length(children) < 1) {
		window_new_session(handler, DEFAULT_SESSION_NAME);
	}
	g_list_free(children);
}

static void button_remove_session_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	csession *session = g_object_get_data(G_OBJECT(widget), "session");
	window_remove_session(handler, session);
}

static void list_box_sessions_row_activated(GtkWidget *widget, GtkListBoxRow *row, gpointer user_data)
{
	chandler *handler = user_data;
	GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
	GtkWidget *child = gtk_bin_get_child(GTK_BIN(row));
	csession *session = g_object_get_data(G_OBJECT(child), "session");
	gint index = gtk_list_box_row_get_index(row);
	if (index < g_list_length(children) - 1) {
		window_open_session(handler, session);
		window_go_to_session(handler);
	}
	g_list_free(children);
}

void init_select_session(chandler *handler)
{
	GtkWidget *scrolled_window = NULL;
	GtkWidget *box = NULL;
	GtkWidget *frame = NULL;
	/* Scrolled window select-session */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_stack_add_named(GTK_STACK(handler->window.stack), scrolled_window, "select-session");
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	/* Box scrolled window */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(box, "box_select_session");
	gtk_container_add(GTK_CONTAINER(scrolled_window), box);
	gtk_widget_set_size_request(box, 400, -1);
	/* Box select-session */
	handler->select_session.box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler->select_session.box, "box_select_session");
	gtk_container_add(GTK_CONTAINER(box), handler->select_session.box);
	gtk_widget_set_hexpand(handler->select_session.box, FALSE);
	gtk_widget_set_halign(handler->select_session.box, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_start(handler->select_session.box, MAJOR_SPACING);
	gtk_widget_set_margin_end(handler->select_session.box, MAJOR_SPACING);
	gtk_widget_set_size_request(handler->select_session.box, LIST_BOX_SESSIONS_MIN_WIDTH, -1);
	/* Entry search session */
	handler->select_session.search_entry = gtk_search_entry_new();
	gtk_widget_set_name(handler->select_session.search_entry, "search_entry_session");
	gtk_container_add(GTK_CONTAINER(handler->select_session.box), handler->select_session.search_entry);
	gtk_widget_set_hexpand(handler->select_session.search_entry, TRUE);
	gtk_widget_set_vexpand(handler->select_session.search_entry, FALSE);
	gtk_widget_set_halign(handler->select_session.search_entry, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->select_session.search_entry, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_top(handler->select_session.search_entry, MAJOR_SPACING);
	/* Frame list box */
	frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(handler->select_session.box), frame);
	gtk_widget_set_hexpand(frame, TRUE);
	gtk_widget_set_vexpand(frame, FALSE);
	gtk_widget_set_halign(frame, GTK_ALIGN_FILL);
	gtk_widget_set_valign(frame, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_top(frame, MAJOR_SPACING);
	/* List box */
	handler->select_session.list_box = gtk_list_box_new();
	gtk_widget_set_name(handler->select_session.list_box, "list_box_sessions");
	gtk_container_add(GTK_CONTAINER(frame), handler->select_session.list_box);
	gtk_widget_set_hexpand(handler->select_session.list_box, TRUE);
	gtk_widget_set_vexpand(handler->select_session.list_box, TRUE);
	gtk_widget_set_halign(handler->select_session.list_box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->select_session.list_box, GTK_ALIGN_FILL);
	gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(handler->select_session.list_box), FALSE);
	g_signal_connect(handler->select_session.list_box, "row-activated", G_CALLBACK(list_box_sessions_row_activated), handler);
}

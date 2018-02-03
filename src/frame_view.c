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

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "handlers.h"

void entry_search_search_changed(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gtk_source_search_settings_set_search_text(handler->handler_frame_view.source_search_settings, gtk_entry_get_text(GTK_ENTRY(widget)));
}

void notebook_page_removed(GtkWidget *widget, GtkWidget *child, gint page_num, gpointer user_data)
{
	chandler *handler = user_data;
	update_view_status(handler, NULL);
}

void notebook_switch_page(GtkWidget *widget, GtkWidget *page, guint page_num, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = g_object_get_data(G_OBJECT(page), "view");
	update_view_status(handler, view);
}

void init_frame_view(chandler *handler)
{
	chandler_window *handler_window = &handler->handler_window;
	chandler_frame_view *handler_frame_view = &handler->handler_frame_view;
	GtkWidget *box = NULL;
	GtkWidget *separator = NULL;
	GtkSizeGroup *size_group = NULL;
	/* Box view */
	handler_frame_view->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler_frame_view->box, "box_view");
	gtk_container_add(GTK_CONTAINER(handler_window->box_frames), handler_frame_view->box);
	gtk_widget_set_hexpand(handler_frame_view->box, TRUE);
	gtk_widget_set_vexpand(handler_frame_view->box, TRUE);
	gtk_widget_set_halign(handler_frame_view->box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->box, GTK_ALIGN_FILL);
	/* Notebook */
	handler_frame_view->notebook = gtk_notebook_new();
	gtk_widget_set_name(handler_frame_view->notebook, "notebook");
	gtk_container_add(GTK_CONTAINER(handler_frame_view->box), handler_frame_view->notebook);
	gtk_widget_set_hexpand(handler_frame_view->notebook, TRUE);
	gtk_widget_set_vexpand(handler_frame_view->notebook, TRUE);
	gtk_widget_set_halign(handler_frame_view->notebook, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->notebook, GTK_ALIGN_FILL);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(handler_frame_view->notebook), FALSE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(handler_frame_view->notebook), TRUE);
	g_signal_connect(handler_frame_view->notebook, "switch-page", G_CALLBACK(notebook_switch_page), handler);
	g_signal_connect(handler_frame_view->notebook, "page-removed", G_CALLBACK(notebook_page_removed), handler);
	/* Revealer search and replace */
	handler_frame_view->revealer_search_and_replace = gtk_revealer_new();
	gtk_container_add(GTK_CONTAINER(handler_frame_view->box), handler_frame_view->revealer_search_and_replace);
	gtk_widget_set_hexpand(handler_frame_view->revealer_search_and_replace, TRUE);
	gtk_widget_set_vexpand(handler_frame_view->revealer_search_and_replace, FALSE);
	gtk_widget_set_halign(handler_frame_view->revealer_search_and_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->revealer_search_and_replace, GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler_frame_view->revealer_search_and_replace), GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);
	/* Box separator and search and replace */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(box, "box_separator_and_search_and_replace");
	gtk_container_add(GTK_CONTAINER(handler_frame_view->revealer_search_and_replace), box);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, TRUE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_FILL);
	/* Separator */
	separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(box), separator);
	gtk_widget_set_hexpand(separator, TRUE);
	gtk_widget_set_vexpand(separator, FALSE);
	gtk_widget_set_halign(separator, GTK_ALIGN_FILL);
	gtk_widget_set_valign(separator, GTK_ALIGN_START);
	/* Size group */
	size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	/* Box search and replace */
	handler_frame_view->box_search_and_replace = gtk_box_new(GTK_ORIENTATION_VERTICAL, MINOR_SPACING);
	gtk_widget_set_name(handler_frame_view->box_search_and_replace, "box_search_and_replace");
	gtk_container_add(GTK_CONTAINER(box), handler_frame_view->box_search_and_replace);
	gtk_widget_set_hexpand(handler_frame_view->box_search_and_replace, TRUE);
	gtk_widget_set_vexpand(handler_frame_view->box_search_and_replace, FALSE);
	gtk_widget_set_halign(handler_frame_view->box_search_and_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->box_search_and_replace, GTK_ALIGN_END);
	gtk_container_set_border_width(GTK_CONTAINER(handler_frame_view->box_search_and_replace), MEDIUM_SPACING);
	/* Box search */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MINOR_SPACING);
	gtk_widget_set_name(box, "box_search");
	gtk_container_add(GTK_CONTAINER(handler_frame_view->box_search_and_replace), box);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_FILL);
	/* Entry search */
	handler_frame_view->entry_search = gtk_search_entry_new();
	gtk_widget_set_name(handler_frame_view->entry_search, "entry_search");
	gtk_container_add(GTK_CONTAINER(box), handler_frame_view->entry_search);
	gtk_widget_set_hexpand(handler_frame_view->entry_search, TRUE);
	gtk_widget_set_vexpand(handler_frame_view->entry_search, FALSE);
	gtk_widget_set_halign(handler_frame_view->entry_search, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->entry_search, GTK_ALIGN_CENTER);
	g_signal_connect(handler_frame_view->entry_search, "search-changed", G_CALLBACK(entry_search_search_changed), handler);
	/* Button search */
	handler_frame_view->button_search = gtk_button_new_with_label("Search");
	gtk_widget_set_name(handler_frame_view->button_search, "button_search");
	gtk_container_add(GTK_CONTAINER(box), handler_frame_view->button_search);
	gtk_widget_set_hexpand(handler_frame_view->button_search, FALSE);
	gtk_widget_set_vexpand(handler_frame_view->button_search, FALSE);
	gtk_widget_set_halign(handler_frame_view->button_search, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->button_search, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group), handler_frame_view->button_search);
	/* Button search all */
	handler_frame_view->button_search_all = gtk_button_new_with_label("Search All");
	gtk_widget_set_name(handler_frame_view->button_search_all, "button_search_all");
	gtk_container_add(GTK_CONTAINER(box), handler_frame_view->button_search_all);
	gtk_widget_set_hexpand(handler_frame_view->button_search_all, FALSE);
	gtk_widget_set_vexpand(handler_frame_view->button_search_all, FALSE);
	gtk_widget_set_halign(handler_frame_view->button_search_all, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->button_search_all, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group), handler_frame_view->button_search_all);
	/* Box replace */
	handler_frame_view->box_replace = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MINOR_SPACING);
	gtk_widget_set_name(box, "box_replace");
	gtk_container_add(GTK_CONTAINER(handler_frame_view->box_search_and_replace), handler_frame_view->box_replace);
	gtk_widget_set_hexpand(handler_frame_view->box_replace, TRUE);
	gtk_widget_set_vexpand(handler_frame_view->box_replace, FALSE);
	gtk_widget_set_halign(handler_frame_view->box_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->box_replace, GTK_ALIGN_FILL);
	/* Entry replace */
	handler_frame_view->entry_replace = gtk_entry_new();
	gtk_widget_set_name(handler_frame_view->entry_replace, "entry_replace");
	gtk_container_add(GTK_CONTAINER(handler_frame_view->box_replace), handler_frame_view->entry_replace);
	gtk_widget_set_hexpand(handler_frame_view->entry_replace, TRUE);
	gtk_widget_set_vexpand(handler_frame_view->entry_replace, FALSE);
	gtk_widget_set_halign(handler_frame_view->entry_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->entry_replace, GTK_ALIGN_CENTER);
	/* Button replace */
	handler_frame_view->button_replace = gtk_button_new_with_label("Replace");
	gtk_widget_set_name(handler_frame_view->button_replace, "button_replace");
	gtk_container_add(GTK_CONTAINER(handler_frame_view->box_replace), handler_frame_view->button_replace);
	gtk_widget_set_hexpand(handler_frame_view->button_replace, FALSE);
	gtk_widget_set_vexpand(handler_frame_view->button_replace, FALSE);
	gtk_widget_set_halign(handler_frame_view->button_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->button_replace, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group), handler_frame_view->button_replace);
	/* Button replace all */
	handler_frame_view->button_replace_all = gtk_button_new_with_label("Replace All");
	gtk_widget_set_name(handler_frame_view->button_replace_all, "button_replace_all");
	gtk_container_add(GTK_CONTAINER(handler_frame_view->box_replace), handler_frame_view->button_replace_all);
	gtk_widget_set_hexpand(handler_frame_view->button_replace_all, FALSE);
	gtk_widget_set_vexpand(handler_frame_view->button_replace_all, FALSE);
	gtk_widget_set_halign(handler_frame_view->button_replace_all, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_view->button_replace_all, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group), handler_frame_view->button_replace_all);
	/* Search settings */
	handler_frame_view->source_search_settings = gtk_source_search_settings_new();
	handler_frame_view->source_completion_words = gtk_source_completion_words_new(NULL, NULL);
}

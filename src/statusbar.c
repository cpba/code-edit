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

void update_statusbar(chandler *handler, cview *view)
{
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	GtkSourceLanguage *source_language = NULL;
	if (current_page > -1) {
		GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), current_page);
		if (!view) {
			view = g_object_get_data(G_OBJECT(page), "view");
		}
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_statusbar.revealer_statusbar), TRUE);
		if (view->document->encoding) {
			gtk_button_set_label(GTK_BUTTON(handler->handler_statusbar.button_encoding),
				gtk_source_encoding_get_charset(view->document->encoding));
		}
		source_language = gtk_source_buffer_get_language(GTK_SOURCE_BUFFER(view->document->source_buffer));
		if (source_language) {
			gtk_button_set_label(GTK_BUTTON(handler->handler_statusbar.button_language),
				gtk_source_language_get_name(GTK_SOURCE_LANGUAGE(source_language)));
		} else {
			gtk_button_set_label(GTK_BUTTON(handler->handler_statusbar.button_language), "Plain Text");
		}
	} else {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_statusbar.revealer_statusbar), FALSE);
	}
}

static void tree_view_source_langauge_activated(GtkWidget *widget, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	chandler *handler = user_data;
	GtkTreeIter tree_iter;
	gchar *id = NULL;
	gchar *name = NULL;
	GtkSourceLanguageManager *source_language_manager = gtk_source_language_manager_get_default();
	GtkSourceLanguage *source_language = NULL;
	cview *view = get_current_view(handler);
	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(widget))), &tree_iter, path)) {
		gtk_tree_model_get (GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(widget))),
			&tree_iter,
			0, &id,
			1, &name,
			-1);
		if (g_strcmp0(id, "plain-text") == 0) {
			gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(view->document->source_buffer), NULL);
		} else {
			source_language = gtk_source_language_manager_get_language(GTK_SOURCE_LANGUAGE_MANAGER(source_language_manager), id);
			gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(view->document->source_buffer), GTK_SOURCE_LANGUAGE(source_language));
		}
		gtk_button_set_label(GTK_BUTTON(handler->handler_statusbar.button_language), name);
		g_free(id);
		g_free(name);
	}
}

void init_statusbar(chandler *handler)
{
	chandler_statusbar *handler_statusbar = &handler->handler_statusbar;
	GtkWidget *popover = NULL;
	GtkWidget *scrolled_window = NULL;
	GtkWidget *box = NULL;
	GtkWidget *tree_view = NULL;
	GtkListStore *list_store = NULL;
	GtkTreeIter tree_iter;
	GtkWidget *entry = NULL;
	GtkCellRenderer *cell_renderer = NULL;
	GtkTreeViewColumn *tree_view_column = NULL;
	GtkSourceLanguage *source_language = NULL;
	GtkSourceLanguageManager *source_language_manager = gtk_source_language_manager_get_default();
	const gchar *id = NULL;
	const gchar * const *language_ids = gtk_source_language_manager_get_language_ids(GTK_SOURCE_LANGUAGE_MANAGER(source_language_manager));
	/* Revealer status bar */
	handler_statusbar->revealer_statusbar = gtk_revealer_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->revealer_statusbar), "revealer_statusbar");
	gtk_container_add(GTK_CONTAINER(handler->handler_window.box), GTK_WIDGET(handler_statusbar->revealer_statusbar));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->revealer_statusbar), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->revealer_statusbar), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->revealer_statusbar), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->revealer_statusbar), GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler_statusbar->revealer_statusbar), GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);
	/* Statusbar */
	handler_statusbar->action_bar = gtk_action_bar_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->action_bar), "action_bar");
	gtk_container_add(GTK_CONTAINER(handler_statusbar->revealer_statusbar), GTK_WIDGET(handler_statusbar->action_bar));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->action_bar), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->action_bar), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->action_bar), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->action_bar), GTK_ALIGN_FILL);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_statusbar->action_bar))), GTK_STYLE_CLASS_FLAT);
	/* Button language */
	handler_statusbar->button_language = gtk_menu_button_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->button_language), "button_language");
	gtk_action_bar_pack_end(GTK_ACTION_BAR(handler_statusbar->action_bar), GTK_WIDGET(handler_statusbar->button_language));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->button_language), FALSE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->button_language), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->button_language), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->button_language), GTK_ALIGN_CENTER);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_statusbar->button_language))), GTK_STYLE_CLASS_FLAT);
	/* Popover language */
	popover = gtk_popover_new(GTK_WIDGET(handler_statusbar->button_language));
	gtk_menu_button_set_popover(GTK_MENU_BUTTON(handler_statusbar->button_language), GTK_WIDGET(popover));
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, MINOR_SPACING);
	gtk_container_add(GTK_CONTAINER(popover), GTK_WIDGET(box));
	gtk_widget_set_hexpand(GTK_WIDGET(box), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(box), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_FILL);
	gtk_container_set_border_width(GTK_CONTAINER(box), MEDIUM_SPACING);
	entry = gtk_search_entry_new();
	gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(entry));
	gtk_widget_set_hexpand(GTK_WIDGET(box), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(box), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(scrolled_window));
	gtk_widget_set_hexpand(GTK_WIDGET(box), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(box), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_FILL);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), -1, LIST_VIEW_LANGUAGE_MIN_HEIGHT);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);
	list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(GTK_LIST_STORE(list_store), &tree_iter);
	gtk_list_store_set(GTK_LIST_STORE(list_store),
		&tree_iter,
		0, "plain-text",
		1, "Plain Text",
		-1);
	while (language_ids[0] != NULL) {
		id = language_ids[0];
		source_language = gtk_source_language_manager_get_language(GTK_SOURCE_LANGUAGE_MANAGER(source_language_manager), id);
		gtk_list_store_append(GTK_LIST_STORE(list_store), &tree_iter);
		gtk_list_store_set(GTK_LIST_STORE(list_store),
			&tree_iter,
			0, id,
			1, gtk_source_language_get_name(GTK_SOURCE_LANGUAGE(source_language)),
			-1);
		language_ids++;
	}
	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
	gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(tree_view));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), FALSE);
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view), TRUE);
	g_signal_connect(tree_view, "row-activated", G_CALLBACK(tree_view_source_langauge_activated), handler);
	cell_renderer = gtk_cell_renderer_text_new();
	tree_view_column = gtk_tree_view_column_new_with_attributes("Name", cell_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), GTK_TREE_VIEW_COLUMN(tree_view_column));
	gtk_tree_view_column_set_visible(GTK_TREE_VIEW_COLUMN(tree_view_column), TRUE);
	gtk_widget_show_all(GTK_WIDGET(box));
	/* Button encoding */
	handler_statusbar->button_encoding = gtk_menu_button_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->button_encoding), "button_encoding");
	gtk_action_bar_pack_end(GTK_ACTION_BAR(handler_statusbar->action_bar), GTK_WIDGET(handler_statusbar->button_encoding));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->button_encoding), FALSE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->button_encoding), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->button_encoding), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->button_encoding), GTK_ALIGN_CENTER);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_statusbar->button_encoding))), GTK_STYLE_CLASS_FLAT);
}

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
#include <gtksourceview/gtksource.h>
#include "handlers.h"

static void tree_view_source_encoding_activated(GtkWidget *widget, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	chandler *handler = user_data;
	GtkTreeIter tree_iter;
	GtkWidget *popover = NULL;
	gchar *charset = NULL;
	const GtkSourceEncoding *encoding = NULL;
	cview *view = get_current_view(handler);
	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(widget))), &tree_iter, path)) {
		gtk_tree_model_get (GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(widget))),
			&tree_iter,
			0, &charset,
			-1);
		if (charset) {
			encoding = gtk_source_encoding_get_from_charset(charset);
		}
		if (encoding) {
			view->document->encoding = encoding;
		}
		gtk_button_set_label(GTK_BUTTON(handler->handler_dialog_save.button_encoding), charset);
		g_free(charset);
	}
	popover = GTK_WIDGET(gtk_menu_button_get_popover(GTK_MENU_BUTTON(handler->handler_dialog_save.button_encoding)));
	gtk_widget_hide(popover);
}

static void file_chooser_dialog_save_show(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	if (view) {
		if (view->document->encoding) {
			gtk_button_set_label(GTK_BUTTON(handler->handler_dialog_save.button_encoding),
				gtk_source_encoding_get_charset(view->document->encoding));
		} else {
			gtk_button_set_label(GTK_BUTTON(handler->handler_dialog_save.button_encoding),
				gtk_source_encoding_get_charset(gtk_source_encoding_get_utf8()));
		}
	} else {
		gtk_button_set_label(GTK_BUTTON(handler->handler_dialog_save.button_encoding),
			gtk_source_encoding_get_charset(gtk_source_encoding_get_utf8()));
	}
}

void init_file_chooser_save(chandler *handler, gchar *title, gchar *button)
{
	chandler_dialog_save *handler_dialog_save = &handler->handler_dialog_save;
	GtkWidget *popover = NULL;
	GtkWidget *scrolled_window = NULL;
	GtkWidget *extra_widget = NULL;
	GtkWidget *box = NULL;
	GtkWidget *tree_view = NULL;
	GtkListStore *list_store = NULL;
	GtkTreeIter tree_iter;
	GtkWidget *entry = NULL;
	GSList *encodings = NULL;
	GSList *encoding_iter = NULL;
	GtkCellRenderer *cell_renderer = NULL;
	GtkTreeViewColumn *tree_view_column = NULL;
	handler_dialog_save->dialog = gtk_file_chooser_dialog_new(title,
		GTK_WINDOW(handler->handler_window.window),
		GTK_FILE_CHOOSER_ACTION_SAVE,
		button, GTK_RESPONSE_OK,
		NULL);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(handler_dialog_save->dialog), TRUE);
	g_signal_connect(handler_dialog_save->dialog, "show", G_CALLBACK(file_chooser_dialog_save_show), handler);
	extra_widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_widget_set_name(extra_widget, "extra_widget");
	gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(handler_dialog_save->dialog), extra_widget);
	/* Button encoding */
	handler_dialog_save->button_encoding = gtk_menu_button_new();
	gtk_widget_set_name(handler_dialog_save->button_encoding, "button_encoding");
	gtk_container_add(GTK_CONTAINER(extra_widget), handler_dialog_save->button_encoding);
	gtk_widget_set_hexpand(handler_dialog_save->button_encoding, FALSE);
	gtk_widget_set_vexpand(handler_dialog_save->button_encoding, FALSE);
	gtk_widget_set_halign(handler_dialog_save->button_encoding, GTK_ALIGN_START);
	gtk_widget_set_valign(handler_dialog_save->button_encoding, GTK_ALIGN_CENTER);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_dialog_save->button_encoding), GTK_STYLE_CLASS_FLAT);
	gtk_button_set_label(GTK_BUTTON(handler_dialog_save->button_encoding), "Encoding");
	/* Popover encoding */
	popover = gtk_popover_new(handler_dialog_save->button_encoding);
	gtk_menu_button_set_popover(GTK_MENU_BUTTON(handler_dialog_save->button_encoding), popover);
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, MINOR_SPACING);
	gtk_container_add(GTK_CONTAINER(popover), box);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_FILL);
	gtk_container_set_border_width(GTK_CONTAINER(box), MEDIUM_SPACING);
	entry = gtk_search_entry_new();
	gtk_container_add(GTK_CONTAINER(box), entry);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(box), scrolled_window);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(scrolled_window, -1, LIST_VIEW_MIN_HEIGHT);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);
	list_store = gtk_list_store_new(1, G_TYPE_STRING);
	encodings = gtk_source_encoding_get_all();
	encoding_iter = encodings;
	while (encoding_iter) {
		gtk_list_store_append(GTK_LIST_STORE(list_store), &tree_iter);
		gtk_list_store_set(GTK_LIST_STORE(list_store),
			&tree_iter,
			0, gtk_source_encoding_get_charset(encoding_iter->data),
			-1);
		encoding_iter = encoding_iter->next;
	}
	g_slist_free(encodings);
	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
	gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), FALSE);
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view), TRUE);
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(tree_view), GTK_ENTRY(entry));
	g_signal_connect(tree_view, "row-activated", G_CALLBACK(tree_view_source_encoding_activated), handler);
	cell_renderer = gtk_cell_renderer_text_new();
	tree_view_column = gtk_tree_view_column_new_with_attributes("Name", cell_renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), GTK_TREE_VIEW_COLUMN(tree_view_column));
	gtk_tree_view_column_set_visible(GTK_TREE_VIEW_COLUMN(tree_view_column), TRUE);
	gtk_widget_show_all(extra_widget);
	gtk_widget_show_all(box);
}

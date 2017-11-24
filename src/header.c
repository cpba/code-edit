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

void update_headerbar(chandler *handler, cview *view)
{
	if (!view) {
		view = get_current_view(handler);
	}
	if (view) {
		GFile *file = gtk_source_file_get_location(view->document->source_file);
		if (file) {
			gchar *basename = g_file_get_basename(file);
			gchar *path = g_file_get_path(file);
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), basename);
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), path);
			g_free(basename);
			g_free(path);
		} else {
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), "Untitled");
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), NULL);
		}
	} else {
		gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), PROGRAM_NAME);
		gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), NULL);
	}
}

static void button_new_document_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cdocument *document = new_document(handler, NULL);
	add_view_for_document(handler, document);
}

static void button_open_document_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gint response = 0;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open",
		GTK_WINDOW(handler->handler_window.window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"Open", GTK_RESPONSE_OK,
		NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);
	response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response == GTK_RESPONSE_OK) {
		GSList *file_names = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
		GSList *file_name_iter = file_names;
		while (file_name_iter) {
			gchar *file_name = file_name_iter->data;
			cdocument *document = new_document(handler, file_name);
			add_view_for_document(handler, document);
			g_free(file_name);
			file_name_iter = file_name_iter->next;
		}
		g_slist_free(file_names);
	}
	gtk_widget_destroy(dialog);
}

static void button_save_as_document_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	GtkWidget *scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), current_page);
	cview *view = g_object_get_data(G_OBJECT(scrolled_window), "view");
	if (view) {
		gint response = 0;
		gchar *file_name = NULL;
		init_file_chooser_save(handler, "Save As", "Save");
		response = gtk_dialog_run(GTK_DIALOG(handler->handler_dialog_save.dialog));
		if (response == GTK_RESPONSE_OK) {
			file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(handler->handler_dialog_save.dialog));
			save_document(view->document, file_name);
			g_free(file_name);
		}
		gtk_widget_destroy(handler->handler_dialog_save.dialog);
	}
}

static void button_save_document_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	GtkWidget *scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), current_page);
	cview *view = g_object_get_data(G_OBJECT(scrolled_window), "view");
	GFile *file = NULL;
	if (view) {
		file = gtk_source_file_get_location(view->document->source_file);
		if (!file) {
			gint response = 0;
			gchar *file_name = NULL;
			init_file_chooser_save(handler, "Save", "Save");
			response = gtk_dialog_run(GTK_DIALOG(handler->handler_dialog_save.dialog));
			if (response == GTK_RESPONSE_OK) {
				file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(handler->handler_dialog_save.dialog));
				save_document(view->document, file_name);
				g_free(file_name);
			}
			gtk_widget_destroy(handler->handler_dialog_save.dialog);
		} else {
			save_document(view->document, NULL);
		}
	}
}

static void button_preferences_toggled(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	/* Show preferences frame */
}

void init_header(chandler *handler)
{
	chandler_window *handler_window = &handler->handler_window;
	chandler_header *handler_header = &handler->handler_header;
	/* Header bar */
	handler_header->header_bar = gtk_header_bar_new();
	gtk_window_set_titlebar(GTK_WINDOW(handler_window->window), handler_header->header_bar);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(handler_header->header_bar), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(handler_header->header_bar), PROGRAM_NAME);
	/* Button new document */
	handler_header->button_new_document = gtk_button_new_from_icon_name("tab-new-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler_header->header_bar), handler_header->button_new_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_new_document), "circular");
	g_signal_connect(handler_header->button_new_document, "clicked", G_CALLBACK(button_new_document_clicked), handler);
	/* Button open document */
	handler_header->button_open_document = gtk_button_new_from_icon_name("document-open-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler_header->header_bar), handler_header->button_open_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_open_document), "circular");
	g_signal_connect(handler_header->button_open_document, "clicked", G_CALLBACK(button_open_document_clicked), handler);
	/* Button save document */
	handler_header->button_save_document = gtk_button_new_from_icon_name("document-save-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler_header->header_bar), handler_header->button_save_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_save_document), "circular");
	g_signal_connect(handler_header->button_save_document, "clicked", G_CALLBACK(button_save_document_clicked), handler);
	/* Button save as document */
	handler_header->button_save_as_document = gtk_button_new_from_icon_name("document-save-as-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler_header->header_bar), handler_header->button_save_as_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_save_as_document), "circular");
	g_signal_connect(handler_header->button_save_as_document, "clicked", G_CALLBACK(button_save_as_document_clicked), handler);
	/* Button preferences */
	handler_header->button_preferences = gtk_button_new_from_icon_name("preferences-system-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(handler_header->header_bar), handler_header->button_preferences);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_preferences), "circular");
}

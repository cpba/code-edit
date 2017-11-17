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

static void button_new_document_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cdocument *document = new_document(handler, NULL);
	add_view_for_document(handler, document);
}

static void button_save_document_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	GtkWidget *scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), current_page);
	cview *view = g_object_get_data(G_OBJECT(scrolled_window), "view");
	if (view->document->file) {
		g_print("Save existing file.\n");
	} else {
		g_print("Save file.\n");
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
	gtk_window_set_titlebar(GTK_WINDOW(handler_window->window), GTK_WIDGET(handler_header->header_bar));
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(handler_header->header_bar), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(handler_header->header_bar), PROGRAM_NAME);
	/* Button new document */
	handler_header->button_new_document = gtk_button_new_from_icon_name("tab-new-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler_header->header_bar), GTK_WIDGET(handler_header->button_new_document));
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_header->button_new_document))), "circular");
	g_signal_connect(handler_header->button_new_document, "clicked", G_CALLBACK(button_new_document_clicked), handler);
	/* Button save document */
	handler_header->button_save_document = gtk_button_new_from_icon_name("document-save-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler_header->header_bar), GTK_WIDGET(handler_header->button_save_document));
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_header->button_save_document))), "circular");
	g_signal_connect(handler_header->button_save_document, "clicked", G_CALLBACK(button_save_document_clicked), handler);
	/* Button preferences */
	handler_header->button_preferences = gtk_button_new_from_icon_name("preferences-system-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(handler_header->header_bar), GTK_WIDGET(handler_header->button_preferences));
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_header->button_preferences))), "circular");
}

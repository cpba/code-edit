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

cview *get_current_view(chandler *handler)
{
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	cview *view = NULL;
	if (current_page > -1) {
		GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), current_page);
		view = g_object_get_data(G_OBJECT(page), "view");
	}
	return view;
}

void update_document_views_status(chandler *handler, cdocument *document)
{
	cview *view = NULL;
	GList *view_iter = document->views;
	GString *text = NULL;
	GFile *file = NULL;
	gchar *basename = NULL;
	gchar *path = NULL;
	while (view_iter) {
		view = view_iter->data;
		/* Update tab label */
		if (!document->source_file_loader && gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(document->source_buffer))) {
			text = g_string_new("• ");
		} else {
			text = g_string_new("");
		}
		if (gtk_source_file_get_location(document->source_file)) {
			basename = g_file_get_basename(gtk_source_file_get_location(document->source_file));
		} else {
			basename = NULL;
		}
		if (basename) {
			text = g_string_append(text, basename);
			g_free(basename);
		} else {
			text = g_string_append(text, "<i>Untitled</i>");
		}
		gtk_label_set_markup(GTK_LABEL(view->label_tab), text->str);
		g_string_free(text, TRUE);
		/* Update if document is editable */
		if (document->source_file_loader || document->source_file_saver) {
			gtk_text_view_set_editable(GTK_TEXT_VIEW(view->source_view), FALSE);
		} else {
			gtk_text_view_set_editable(GTK_TEXT_VIEW(view->source_view), TRUE);
		}
		view_iter = view_iter->next;
	}
	view = get_current_view(handler);
	if (view) {
		file = gtk_source_file_get_location(view->document->source_file);
		if (file) {
			basename = g_file_get_basename(file);
			path = g_file_get_path(file);
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), basename);
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), path);
			g_free(basename);
			g_free(path);
		} else {
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), "Untitled");
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), NULL);
		}
	}
}

static void document_load_progress(goffset current_num_bytes, goffset total_num_bytes, gpointer user_data)
{
	cdocument *document = user_data;
	cview *view = NULL;
	gdouble percentage = (gdouble)current_num_bytes / (gdouble)total_num_bytes;
	GList *element = g_list_first(document->views);
	while (element) {
		view = element->data;
		gtk_revealer_set_reveal_child(GTK_REVEALER(view->revealer_progress_bar), TRUE);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(view->progress_bar), percentage);
		element = g_list_next(element);
	}
}

static void document_save_progress(goffset current_num_bytes, goffset total_num_bytes, gpointer user_data)
{
	cdocument *document = user_data;
	cview *view = NULL;
	gdouble percentage = (gdouble)current_num_bytes / (gdouble)total_num_bytes;
	GList *element = g_list_first(document->views);
	while (element) {
		view = element->data;
		gtk_revealer_set_reveal_child(GTK_REVEALER(view->revealer_progress_bar), TRUE);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(view->progress_bar), percentage);
		element = g_list_next(element);
	}
}

static void document_async_ready(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
	cdocument *document = user_data;
	chandler *handler = document->handler;
	cview *view = NULL;
	GList *element = g_list_first(document->views);
	document->encoding = gtk_source_file_get_encoding(document->source_file);
	if (document->source_file_loader) {
		gtk_source_file_loader_load_finish(GTK_SOURCE_FILE_LOADER(source_object), res, NULL);
		g_object_unref(document->source_file_loader);
		document->source_file_loader = NULL;
	}
	if (document->source_file_saver) {
		gtk_source_file_saver_save_finish(GTK_SOURCE_FILE_SAVER(source_object), res, NULL);
		g_object_unref(document->source_file_saver);
		document->source_file_saver = NULL;
	}
	while (element) {
		view = element->data;
		gtk_revealer_set_reveal_child(GTK_REVEALER(view->revealer_progress_bar), FALSE);
		element = g_list_next(element);
	}
	update_document_views_status(handler, document);
	update_statusbar(handler, NULL);
}

static void source_buffer_changed(GtkTextBuffer *text_buffer, gpointer user_data)
{
	chandler *handler = user_data;
	cdocument *document = g_object_get_data(G_OBJECT(text_buffer), "document");
	update_document_views_status(handler, document);
}

static cdocument *get_document_by_file_name(chandler *handler, gchar *file_name)
{
	gchar *path = NULL;
	cdocument *result = NULL;
	cdocument *document = NULL;
	GList *document_iter = handler->documents;
	while (document_iter && !result) {
		document = document_iter->data;
		path = NULL;
		if (gtk_source_file_get_location(document->source_file)) {
			path = g_file_get_path(gtk_source_file_get_location(document->source_file));
		}
		if (path) {
			if (g_strcmp0(file_name, path) == 0) {
				result = document;
			}
			g_free(path);
		}
		document_iter = document_iter->next;
	}
	return result;
}

void free_document(cdocument *document)
{
	if (document->source_file) {
		g_object_unref(G_OBJECT(document->source_file));
	}
	g_object_unref(G_OBJECT(document->source_buffer));
	free(document);
}

void save_document(cdocument *document, gchar *file_name)
{
	GFile *file = NULL;
	if (!document->source_file_saver && !document->source_file_loader) {
		if (file_name) {
			file = g_file_new_for_path(file_name);
			gtk_source_file_set_location(document->source_file, file);
		}
		document->source_file_saver = gtk_source_file_saver_new(document->source_buffer, document->source_file);
		gtk_source_file_saver_save_async(document->source_file_saver,
			G_PRIORITY_LOW,
			NULL,
			document_save_progress,
			document,
			NULL,
			document_async_ready,
			document);
		gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(document->source_buffer), FALSE);
	}
}

cdocument *new_document(chandler *handler, gchar *file_name)
{
	GtkSourceLanguageManager *source_language_manager = gtk_source_language_manager_get_default();
	GtkSourceLanguage *source_language = NULL;
	GFile *file = NULL;
	gboolean result_uncertain = FALSE;
	gchar *content_type = NULL;
	cdocument *document = get_document_by_file_name(handler, file_name);
	if (!document) {
		document = malloc(sizeof(cdocument));
		document->handler = handler;
		document->encoding = NULL;
		document->views = NULL;
		document->source_file_loader = NULL;
		document->source_file_saver = NULL;
		document->source_buffer = gtk_source_buffer_new(NULL);
		g_object_set_data(G_OBJECT(document->source_buffer), "document", document);
		g_signal_connect(document->source_buffer, "changed", G_CALLBACK(source_buffer_changed), handler);
		g_signal_connect(document->source_buffer, "modified-changed", G_CALLBACK(source_buffer_changed), handler);
		document->source_file = gtk_source_file_new();
		if (file_name) {
			file = g_file_new_for_path(file_name);
			content_type = g_content_type_guess(file_name, NULL, 0, &result_uncertain);
			if (result_uncertain) {
				g_free(content_type);
				content_type = NULL;
			}
			source_language = gtk_source_language_manager_guess_language(source_language_manager, file_name, content_type);
			if (content_type) {
				g_free(content_type);
			}
			gtk_source_buffer_set_language(document->source_buffer, source_language);
			gtk_source_file_set_location(document->source_file, file);
			document->source_file_loader = gtk_source_file_loader_new(document->source_buffer, document->source_file);
			gtk_source_file_loader_load_async(document->source_file_loader,
				G_PRIORITY_LOW,
				NULL,
				document_load_progress,
				document,
				NULL,
				document_async_ready,
				document);
		}
		handler->documents = g_list_append(handler->documents, document);
	}
	return document;
}

static void button_close_tab_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = g_object_get_data(G_OBJECT(widget), "view");
	close_view(handler, view);
	update_statusbar(handler, NULL);
}

void close_view(chandler *handler, cview *view)
{
	gboolean close = FALSE;
	gint views_count = g_list_length(view->document->views);
	if (views_count > 1) {
		close = TRUE;
	} else {
		if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(view->document->source_buffer))) {
			gint response = 0;
			GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(handler->handler_window.window),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_OK_CANCEL,
				"Close the tab without saving the modifications?");
			response = gtk_dialog_run(GTK_DIALOG(dialog));
			if (response == GTK_RESPONSE_OK) {
				close = TRUE;
			}
			gtk_widget_destroy(dialog);
		} else {
			close = TRUE;
		}
	}
	if (close) {
		view->document->views = g_list_remove(view->document->views, view);
		gtk_widget_destroy(view->box);
	}
	if (g_list_length(view->document->views) == 0) {
		handler->documents = g_list_remove(handler->documents, view->document);
		free_document(view->document);
	}
	if (close) {
		free(view);
	}
}

void add_view_for_document(chandler *handler, cdocument *document)
{
	cview *view = malloc(sizeof(cview));
	gint page_index = 0;
	view->document = document;
	view->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	g_object_set_data(G_OBJECT(view->box), "view", view);
	/* Revealer info bar */
	view->revealer_progress_bar = gtk_revealer_new();
	gtk_container_add(GTK_CONTAINER(view->box), view->revealer_progress_bar);
	gtk_widget_set_hexpand(view->revealer_progress_bar, TRUE);
	gtk_widget_set_vexpand(view->revealer_progress_bar, FALSE);
	gtk_widget_set_halign(view->revealer_progress_bar, GTK_ALIGN_FILL);
	gtk_widget_set_valign(view->revealer_progress_bar, GTK_ALIGN_START);
	gtk_revealer_set_transition_type(GTK_REVEALER(view->revealer_progress_bar), GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
	/* Progress bar */
	view->progress_bar = gtk_progress_bar_new();
	gtk_container_add(GTK_CONTAINER(view->revealer_progress_bar), view->progress_bar);
	gtk_widget_set_hexpand(view->progress_bar, TRUE);
	gtk_widget_set_vexpand(view->progress_bar, FALSE);
	gtk_widget_set_halign(view->progress_bar, GTK_ALIGN_FILL);
	gtk_widget_set_valign(view->progress_bar, GTK_ALIGN_CENTER);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(view->progress_bar), 0.5);
	/* Source view */
	view->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(view->box), view->scrolled_window);
	gtk_widget_set_hexpand(view->scrolled_window, TRUE);
	gtk_widget_set_vexpand(view->scrolled_window, TRUE);
	gtk_widget_set_halign(view->scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_valign(view->scrolled_window, GTK_ALIGN_FILL);
	view->source_view = gtk_source_view_new_with_buffer(GTK_SOURCE_BUFFER(document->source_buffer));
	gtk_container_add(GTK_CONTAINER(view->scrolled_window), view->source_view);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view->source_view), TRUE);
	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(view->source_view), TRUE);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(view->source_view), TRUE);
	/* Tab */
	view->box_tab = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	/* Label */
	view->label_tab = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(view->box_tab), view->label_tab);
	gtk_widget_set_hexpand(view->label_tab, TRUE);
	gtk_widget_set_vexpand(view->label_tab, TRUE);
	gtk_widget_set_halign(view->label_tab, GTK_ALIGN_FILL);
	gtk_widget_set_valign(view->label_tab, GTK_ALIGN_FILL);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(view->label_tab)), GTK_STYLE_CLASS_DIM_LABEL);
	gtk_label_set_use_markup(GTK_LABEL(view->label_tab), TRUE);
	gtk_label_set_markup(GTK_LABEL(view->label_tab), "");
	/* Button close */
	view->button_close_tab = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_container_add(GTK_CONTAINER(view->box_tab), view->button_close_tab);
	gtk_widget_set_hexpand(view->button_close_tab, FALSE);
	gtk_widget_set_vexpand(view->button_close_tab, TRUE);
	gtk_widget_set_halign(view->button_close_tab, GTK_ALIGN_END);
	gtk_widget_set_valign(view->button_close_tab, GTK_ALIGN_FILL);
	gtk_button_set_relief(GTK_BUTTON(view->button_close_tab), GTK_RELIEF_NONE);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(view->button_close_tab)), "circular");
	g_object_set_data(G_OBJECT(view->button_close_tab), "view", view);
	g_signal_connect(view->button_close_tab, "clicked", G_CALLBACK(button_close_tab_clicked), handler);
	/* Add to notebook */
	page_index = gtk_notebook_append_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), view->box, view->box_tab);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(handler->handler_frame_view.notebook), view->box, TRUE);
	gtk_widget_show_all(handler->handler_frame_view.notebook);
	gtk_widget_show_all(view->box_tab);
	/* Update document */
	document->views = g_list_append(document->views, view);
	update_document_views_status(handler, document);
	/* Show page */
	gtk_notebook_set_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), page_index);
	update_statusbar(handler, NULL);
}

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
#include <string.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include "handlers.h"

void update_statusbar(chandler *handler, cview *view)
{
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	if (current_page > -1) {
		GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), current_page);
		if (!view) {
			view = g_object_get_data(G_OBJECT(page), "view");
		}
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_statusbar.revealer_statusbar), TRUE);
		if (view->document->encoding) {
			gtk_button_set_label(GTK_BUTTON(handler->handler_statusbar.label_encoding),
				gtk_source_encoding_get_charset(view->document->encoding));
			gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_statusbar.revealer_encoding), TRUE);
		}
	} else {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_statusbar.revealer_encoding), FALSE);
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_statusbar.revealer_statusbar), FALSE);
	}
}

static void document_load_progress(goffset current_num_bytes, goffset total_num_bytes, gpointer user_data)
{
	cdocument *document = user_data;
	cview *view = NULL;
	double percentage = (double)current_num_bytes / (double)total_num_bytes;
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
	gtk_source_file_loader_load_finish(GTK_SOURCE_FILE_LOADER(source_object), res, NULL);
	document->encoding = gtk_source_file_get_encoding(GTK_SOURCE_FILE(document->source_file));
	g_object_unref(G_OBJECT(document->source_file_loader));
	document->source_file_loader = NULL;
	while (element) {
		view = element->data;
		gtk_revealer_set_reveal_child(GTK_REVEALER(view->revealer_progress_bar), FALSE);
		element = g_list_next(element);
	}
	update_statusbar(handler, NULL);
}

void free_document(cdocument *document)
{
	if (document->file) {
		g_object_unref(G_OBJECT(document->file));
	}
	if (document->source_file) {
		g_object_unref(G_OBJECT(document->source_file));
	}
	g_object_unref(G_OBJECT(document->source_buffer));
	free(document);
}

cdocument *new_document(chandler *handler, gchar *file_name)
{
	GtkSourceLanguageManager *source_language_manager = gtk_source_language_manager_get_default();
	cdocument *document = malloc(sizeof(cdocument));
	document->handler = handler;
	document->encoding = NULL;
	document->source_buffer = gtk_source_buffer_new(NULL);
	document->source_language = gtk_source_language_manager_get_language(GTK_SOURCE_LANGUAGE_MANAGER(source_language_manager), "c");
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(document->source_buffer), document->source_language);
	document->source_file = gtk_source_file_new();
	if (file_name) {
		document->file = g_file_new_for_path(file_name);
		gtk_source_file_set_location(GTK_SOURCE_FILE(document->source_file), G_FILE(document->file));
		document->source_file_loader = gtk_source_file_loader_new(GTK_SOURCE_BUFFER(document->source_buffer), GTK_SOURCE_FILE(document->source_file));
		gtk_source_file_loader_load_async(GTK_SOURCE_FILE_LOADER(document->source_file_loader),
			G_PRIORITY_LOW,
			NULL,
			document_load_progress,
			document,
			NULL,
			document_async_ready,
			document);
	} else {
		document->file = NULL;
	}
	document->views = NULL;
	handler->documents = g_list_append(handler->documents, document);
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
			gtk_widget_destroy(GTK_WIDGET(dialog));
		} else {
			close = TRUE;
		}
	}
	if (close) {
		view->document->views = g_list_remove(view->document->views, view);
		gtk_widget_destroy(GTK_WIDGET(view->box));
	}
	if (g_list_length(view->document->views) == 0) {
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
	gtk_container_add(GTK_CONTAINER(view->box), GTK_WIDGET(view->revealer_progress_bar));
	gtk_widget_set_hexpand(GTK_WIDGET(view->revealer_progress_bar), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(view->revealer_progress_bar), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(view->revealer_progress_bar), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(view->revealer_progress_bar), GTK_ALIGN_START);
	gtk_revealer_set_transition_type(GTK_REVEALER(view->revealer_progress_bar), GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
	/* Progress bar */
	view->progress_bar = gtk_progress_bar_new();
	gtk_container_add(GTK_CONTAINER(view->revealer_progress_bar), GTK_WIDGET(view->progress_bar));
	gtk_widget_set_hexpand(GTK_WIDGET(view->progress_bar), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(view->progress_bar), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(view->progress_bar), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(view->progress_bar), GTK_ALIGN_CENTER);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(view->progress_bar), 0.5);
	/* Source view */
	view->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(view->box), GTK_WIDGET(view->scrolled_window));
	gtk_widget_set_hexpand(GTK_WIDGET(view->scrolled_window), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(view->scrolled_window), TRUE);
	gtk_widget_set_halign(GTK_WIDGET(view->scrolled_window), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(view->scrolled_window), GTK_ALIGN_FILL);
	view->source_view = gtk_source_view_new_with_buffer(GTK_SOURCE_BUFFER(document->source_buffer));
	gtk_container_add(GTK_CONTAINER(view->scrolled_window), GTK_WIDGET(view->source_view));
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(document->source_buffer), document->source_language);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view->source_view), TRUE);
	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(view->source_view), TRUE);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(view->source_view), TRUE);
	/* Tab */
	view->box_tab = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	/* Label */
	view->label_tab = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(view->box_tab), GTK_WIDGET(view->label_tab));
	gtk_widget_set_hexpand(GTK_WIDGET(view->label_tab), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(view->label_tab), TRUE);
	gtk_widget_set_halign(GTK_WIDGET(view->label_tab), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(view->label_tab), GTK_ALIGN_FILL);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(view->label_tab))), GTK_STYLE_CLASS_DIM_LABEL);
	gtk_label_set_use_markup(GTK_LABEL(view->label_tab), TRUE);
	if (document->file) {
		gchar *basename = g_file_get_basename(G_FILE(document->file));
		gtk_label_set_markup(GTK_LABEL(view->label_tab), basename);
		g_free(basename);
	} else {
		gtk_label_set_markup(GTK_LABEL(view->label_tab), "<i>Untitled</i>");
	}
	/* Button close */
	view->button_close_tab = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_container_add(GTK_CONTAINER(view->box_tab), GTK_WIDGET(view->button_close_tab));
	gtk_widget_set_hexpand(GTK_WIDGET(view->button_close_tab), FALSE);
	gtk_widget_set_vexpand(GTK_WIDGET(view->button_close_tab), TRUE);
	gtk_widget_set_halign(GTK_WIDGET(view->button_close_tab), GTK_ALIGN_END);
	gtk_widget_set_valign(GTK_WIDGET(view->button_close_tab), GTK_ALIGN_FILL);
	gtk_button_set_relief(GTK_BUTTON(view->button_close_tab), GTK_RELIEF_NONE);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(view->button_close_tab))), "circular");
	g_object_set_data(G_OBJECT(view->button_close_tab), "view", view);
	g_signal_connect(view->button_close_tab, "clicked", G_CALLBACK(button_close_tab_clicked), handler);
	/* Add to notebook */
	page_index = gtk_notebook_append_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), GTK_WIDGET(view->box), GTK_WIDGET(view->box_tab));
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(handler->handler_frame_view.notebook), GTK_WIDGET(view->box), TRUE);
	gtk_widget_show_all(GTK_WIDGET(handler->handler_frame_view.notebook));
	gtk_widget_show_all(GTK_WIDGET(view->box_tab));
	/* Update document */
	document->views = g_list_append(document->views, view);
	/* Show page */
	gtk_notebook_set_current_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), page_index);
}

static void activate_show_about(GSimpleAction *simple, GVariant *parameter, gpointer user_data)
{
	/* Show about frame */
}

static void activate_quit(GSimpleAction *simple, GVariant *parameter, gpointer user_data)
{
	gtk_main_quit();
}

static void application_activate(GtkApplication *application, gpointer user_data)
{
	chandler *handler = user_data;
	GMenu *menu = NULL;
	/* Action group */
	const GActionEntry action_entries[] = {
		{"about", activate_show_about},
		{"quit", activate_quit}};
	GSimpleActionGroup *action_group = g_simple_action_group_new();
	g_action_map_add_action_entries(G_ACTION_MAP(action_group), action_entries, G_N_ELEMENTS(action_entries), NULL);
	/* Initialize handlers */
	init_window(handler);
	init_header(handler);
	init_frame_view(handler);
	init_frame_tree_view(handler);
	init_statusbar(handler);
	gtk_widget_insert_action_group(GTK_WIDGET(handler->handler_window.window), "default", G_ACTION_GROUP(action_group));
	/* Menu */
	menu = g_menu_new();
	g_menu_append(menu, "About", "about");
	g_menu_append(menu, "Quit", "quit");
	gtk_application_set_app_menu(application, G_MENU_MODEL(menu));
	/* Document list */
	handler->documents = NULL;
	/* Show */
	gtk_window_present(GTK_WINDOW(handler->handler_window.window));
	gtk_widget_show_all(GTK_WIDGET(handler->handler_window.window));
}

static void application_shutdown(GtkApplication *application, gpointer user_data)
{
	chandler *handler = user_data;
	/* Save session */
}

int main(int argc, char **args)
{
	chandler *handler = malloc(sizeof(chandler));
	memset(handler, 0, sizeof(chandler));
	gtk_init(NULL, NULL);
	handler->application = gtk_application_new("app.code", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(handler->application, "activate", G_CALLBACK(application_activate), handler);
	g_signal_connect(handler->application, "shutdown", G_CALLBACK(application_shutdown), handler);
	g_application_run(G_APPLICATION(handler->application), 0, NULL);
	free(handler);
	return EXIT_SUCCESS;
}

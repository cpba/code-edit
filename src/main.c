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

cdocument *new_document(chandler *handler, gchar *file_name)
{
	GtkSourceLanguageManager *source_language_manager = gtk_source_language_manager_get_default();
	cdocument *document = malloc(sizeof(cdocument));
	document->source_buffer = gtk_source_buffer_new(NULL);
	document->source_language = gtk_source_language_manager_get_language(GTK_SOURCE_LANGUAGE_MANAGER(source_language_manager), "c");
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(document->source_buffer), document->source_language);
	if (file_name) {
		document->file_name = g_string_new(file_name);
	} else {
		document->file_name = NULL;
	}
	document->views = NULL;
	handler->documents = g_list_append(handler->documents, document);
	return document;
}

cview *new_view(chandler *handler, cdocument *document)
{
	cview *view = malloc(sizeof(cview));
	GtkWidget *box_tab = NULL;
	view->document = document;
	view->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	g_object_set_data(G_OBJECT(view->scrolled_window), "view", view);
	view->source_view = gtk_source_view_new_with_buffer(GTK_SOURCE_BUFFER(document->source_buffer));
	gtk_container_add(GTK_CONTAINER(view->scrolled_window), GTK_WIDGET(view->source_view));
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(document->source_buffer), document->source_language);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view->source_view), TRUE);
	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(view->source_view), TRUE);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(view->source_view), TRUE);
	/* Tab */
	box_tab = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	view->label_tab = gtk_label_new("");
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(view->label_tab))), GTK_STYLE_CLASS_DIM_LABEL);
	gtk_label_set_use_markup(GTK_LABEL(view->label_tab), TRUE);
	if (document->file_name) {
		gchar *basename = g_path_get_basename(document->file_name->str);
		gtk_label_set_markup(GTK_LABEL(view->label_tab), basename);
		g_free(basename);
	} else {
		gtk_label_set_markup(GTK_LABEL(view->label_tab), "<i>Untitled</i>");
	}
	gtk_container_add(GTK_CONTAINER(box_tab), GTK_WIDGET(view->label_tab));
	gtk_widget_set_hexpand(GTK_WIDGET(view->label_tab), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(view->label_tab), TRUE);
	gtk_widget_set_halign(GTK_WIDGET(view->label_tab), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(view->label_tab), GTK_ALIGN_FILL);
	/* Button close */
	view->button_close_tab = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_container_add(GTK_CONTAINER(box_tab), GTK_WIDGET(view->button_close_tab));
	gtk_widget_set_hexpand(GTK_WIDGET(view->button_close_tab), FALSE);
	gtk_widget_set_vexpand(GTK_WIDGET(view->button_close_tab), TRUE);
	gtk_widget_set_halign(GTK_WIDGET(view->button_close_tab), GTK_ALIGN_END);
	gtk_widget_set_valign(GTK_WIDGET(view->button_close_tab), GTK_ALIGN_FILL);
	gtk_button_set_relief(GTK_BUTTON(view->button_close_tab), GTK_RELIEF_NONE);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(view->button_close_tab))), "circular");
	/* Add to notebook */
	gtk_notebook_append_page(GTK_NOTEBOOK(handler->handler_frame_view.notebook), GTK_WIDGET(view->scrolled_window), GTK_WIDGET(box_tab));
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(handler->handler_frame_view.notebook), GTK_WIDGET(view->scrolled_window), TRUE);
	gtk_widget_show_all(GTK_WIDGET(handler->handler_frame_view.notebook));
	gtk_widget_show_all(GTK_WIDGET(box_tab));
	/* Update document */
	document->views = g_list_append(document->views, view);
	return view;
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

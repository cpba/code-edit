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
#include <gtk/gtk.h>
#include <libgit2-glib/ggit.h>
#include "handlers.h"

void window_new(gpointer user_data)
{
	chandler *handler = user_data;
	cdocument *document = new_document(handler, NULL);
	add_view_for_document(handler, document);
}

void window_open(gpointer user_data)
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

void window_save_as(gpointer user_data)
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

void window_save(gpointer user_data)
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

void window_toggle_search_bar(gpointer user_data)
{
	chandler *handler = user_data;
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace), TRUE);
		gtk_widget_grab_focus(handler->handler_frame_view.entry_search);
	}
}

void window_toggle_search_and_replace_bar(gpointer user_data)
{
	chandler *handler = user_data;
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace), TRUE);
		gtk_widget_grab_focus(handler->handler_frame_view.entry_search);
	}
}

static void init_accels(chandler *handler)
{
	GtkAccelGroup *accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(handler->handler_window.window), GTK_ACCEL_GROUP(accel_group));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_N,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_new), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_O,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_open), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_S,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_save), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_F,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_toggle_search_bar), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_R,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_toggle_search_and_replace_bar), handler, NULL));
	
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
	init_accels(handler);
	gtk_widget_insert_action_group(handler->handler_window.window, "default", G_ACTION_GROUP(action_group));
	/* Menu */
	menu = g_menu_new();
	g_menu_append(menu, "About", "about");
	g_menu_append(menu, "Quit", "quit");
	gtk_application_set_app_menu(application, G_MENU_MODEL(menu));
	/* Document list */
	handler->documents = NULL;
	/* Show */
	gtk_window_present(GTK_WINDOW(handler->handler_window.window));
	gtk_widget_show_all(handler->handler_window.window);
	update_view_status(handler, NULL);
}

static void application_shutdown(GtkApplication *application, gpointer user_data)
{
	chandler *handler = user_data;
	//save_config(handler);
}

int main(void)
{
	chandler *handler = malloc(sizeof(chandler));
	memset(handler, 0, sizeof(chandler));
	gtk_init(NULL, NULL);
	ggit_init();
	handler->application = gtk_application_new("app.code", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(handler->application, "activate", G_CALLBACK(application_activate), handler);
	g_signal_connect(handler->application, "shutdown", G_CALLBACK(application_shutdown), handler);
	g_application_run(G_APPLICATION(handler->application), 0, NULL);
	free(handler);
	return EXIT_SUCCESS;
}

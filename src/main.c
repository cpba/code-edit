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

#include <string.h>
#include <gtk/gtk.h>
#include <libgit2-glib/ggit.h>
#include "handlers.h"

static void init_accels(chandler *handler)
{
	GtkAccelGroup *accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(handler->window.window), GTK_ACCEL_GROUP(accel_group));
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
		GDK_KEY_W,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_close), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_Q,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_quit), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_F3,
		0,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_search_next), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_F3,
		GDK_SHIFT_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_search_previous), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_F,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_show_search_bar), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_R,
		GDK_CONTROL_MASK,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_show_search_and_replace_bar), handler, NULL));
	gtk_accel_group_connect(GTK_ACCEL_GROUP(accel_group),
		GDK_KEY_F9,
		0,
		0,
		g_cclosure_new_swap(G_CALLBACK(window_toggle_tree_view), handler, NULL));
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
	GString *key_file_path = NULL;
	/* Action group */
	const GActionEntry action_entries[] = {
		{"about", activate_show_about},
		{"quit", activate_quit}};
	GSimpleActionGroup *action_group = g_simple_action_group_new();
	g_action_map_add_action_entries(G_ACTION_MAP(action_group), action_entries, G_N_ELEMENTS(action_entries), NULL);
	/* Initialize handlers */
	init_window(handler);
	init_header(handler);
	init_select_session(handler);
	init_session(handler);
	init_search_and_replace(handler);
	init_statusbar(handler);
	init_sidebar_files(handler);
	init_preferences(handler);
	init_accels(handler);
	gtk_widget_insert_action_group(handler->window.window, "default", G_ACTION_GROUP(action_group));
	/* Menu */
	menu = g_menu_new();
	g_menu_append(menu, TEXT_ABOUT, "about");
	g_menu_append(menu, TEXT_QUIT, "quit");
	gtk_application_set_app_menu(application, G_MENU_MODEL(menu));
	/* Document list */
	handler->documents = NULL;
	/* Key-file config */
	handler->key_file_sessions = g_key_file_new();
	if (g_get_user_config_dir()) {
		key_file_path = g_string_new(g_get_user_config_dir());
		key_file_path = g_string_append(key_file_path, SESSIONS_FILE_NAME);
	}
	if (key_file_path) {
		g_key_file_load_from_file(handler->key_file_sessions, key_file_path->str, G_KEY_FILE_NONE, NULL);
		g_string_free(key_file_path, TRUE);
	}
	window_update_sessions(handler);
	/* Show */
	gtk_window_present(GTK_WINDOW(handler->window.window));
	gtk_widget_show_all(handler->window.window);
	update_view_status(handler, NULL);
	window_go_to_select_session(handler);
	gtk_stack_set_transition_type(GTK_STACK(handler->window.stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler->statusbar.revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);
}

static void application_shutdown(GtkApplication *application, gpointer user_data)
{
	chandler *handler = user_data;
	g_key_file_free(handler->key_file_sessions);
	g_slice_free1(sizeof(chandler), handler);
}

int main(void)
{
	chandler *handler = NULL;;
	gtk_init(NULL, NULL);
	handler = g_slice_alloc0(sizeof(chandler));
	ggit_init();
	handler->application = gtk_application_new("app.code", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(handler->application, "activate", G_CALLBACK(application_activate), handler);
	g_signal_connect(handler->application, "shutdown", G_CALLBACK(application_shutdown), handler);
	g_application_run(G_APPLICATION(handler->application), 0, NULL);
	return EXIT_SUCCESS;
}

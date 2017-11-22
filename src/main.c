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

int main(void)
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

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

void load_session(chandler *handler, gchar *name)
{
	gchar **strings = NULL;
	gint i = 0;
	gsize length = 0;
	cdocument *document = NULL;
	cview *view = NULL;
	handler->session_name = g_string_assign(handler->session_name, name);
	strings = g_key_file_get_string_list(handler->key_file_config,
		name,
		"views",
		&length,
		NULL);
	while (i < length) {
		document = new_document(handler, strings[i]);
		add_view_for_document(handler, document);
		i++;
	}
}

void save_session(chandler *handler)
{
	gchar *config_dir = NULL;
	GString *key_file_path = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	gint page_index = 0;
	cview *view = NULL;
	GFile *file = NULL;
	gchar **strings = NULL;
	GString *list = NULL;
	if (page_count > 0) {
		strings = malloc(page_count * sizeof(gchar *));
	}
	while (page_index < page_count) {
		view = get_nth_view(handler, page_index);
		file = gtk_source_file_get_location(view->document->source_file);
		if (file) {
			strings[page_index] = g_file_get_path(file);
		} else {
			strings[page_index] = NULL;
		}
		page_index++;
	}
	g_key_file_set_string_list(handler->key_file_config,
		handler->session_name->str,
		"views",
		strings,
		page_count);
	config_dir = g_get_user_config_dir();
	if (config_dir) {
		key_file_path = g_string_new(config_dir);
		key_file_path = g_string_append(key_file_path, CONFIGURATION_FILE_NAME);
	}
	if (key_file_path) {
		g_key_file_save_to_file(handler->key_file_config, key_file_path->str, NULL);
		g_string_free(key_file_path, TRUE);
	}
}

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

#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#define PROGRAM_NAME "Code"
#define SIDEBAR_TREE_VIEW_MIN_WIDTH 160
#define LIST_VIEW_LANGUAGE_MIN_HEIGHT 260
#define MINOR_SPACING 6
#define MEDIUM_SPACING 12
#define MAJOR_SPACING 18

typedef struct cdocument cdocument;
typedef struct cview cview;
typedef struct chandler_header chandler_header;
typedef struct chandler_frame_tree_view chandler_frame_tree_view;
typedef struct chandler_frame_view chandler_frame_view;
typedef struct chandler_frame_preferences chandler_frame_preferences;
typedef struct chandler_statusbar chandler_statusbar;
typedef struct chandler_window chandler_window;
typedef struct chandler chandler;

struct cdocument {
	gboolean preview;
	GtkSourceFile *source_file;
	GtkSourceFileLoader *source_file_loader;
	GtkSourceFileSaver *source_file_saver;
	const GtkSourceEncoding *encoding;
	GtkSourceBuffer *source_buffer;
	GList *views;
	chandler *handler;
};

struct cview {
	cdocument *document;
	GtkWidget *box;
	GtkWidget *revealer_progress_bar;
	GtkWidget *progress_bar;
	GtkWidget *scrolled_window;
	GtkWidget *source_view;
	GtkWidget *box_tab;
	GtkWidget *label_tab;
	GtkWidget *button_close_tab;
};

struct chandler_header {
	chandler *handler;
	GtkWidget *header_bar;
	GtkWidget *button_new_document;
	GtkWidget *button_open_document;
	GtkWidget *button_save_document;
	GtkWidget *button_save_as_document;
	GtkWidget *button_preferences;
};

struct chandler_frame_tree_view {
	GtkWidget *revealer;
	GtkWidget *box;
	GtkWidget *scrolled_window;
	GtkWidget *tree_view;
	GtkWidget *popover_folder;
	GtkWidget *popover_file;
	GtkWidget *popover_rename;
};

struct chandler_frame_view {
	GtkWidget *box;
	GtkWidget *notebook;
	GtkWidget *revealer_search_and_replace;
	GtkWidget *box_search_and_replace;
	GtkWidget *entry_search;
	GtkWidget *entry_replace;
	GtkWidget *entry_file_pattern;
	GtkWidget *button_use_regex;
	GtkWidget *button_match_case;
	GtkWidget *button_only_in_selection;
	GtkWidget *button_search;
	GtkWidget *button_search_all;
	GtkWidget *button_replace;
	GtkWidget *button_replace_all;
};

struct chandler_frame_preferences {
	GtkWidget *box;
	GtkWidget *stack_switcher;
	GtkWidget *stack;
	GtkSizeGroup *size_group_input;
};

struct chandler_statusbar {
	GtkWidget *revealer_statusbar;
	GtkWidget *action_bar;
	GtkWidget *button_encoding;
	GtkWidget *button_language;
};

struct chandler_window {
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *box_frames;
};

struct chandler {
	GtkApplication *application;
	GList *documents;
	chandler_window handler_window;
	chandler_header handler_header;
	chandler_statusbar handler_statusbar;
	chandler_frame_preferences handler_frame_preferences;
	chandler_frame_view handler_frame_view;
	chandler_frame_tree_view handler_frame_tree_view;
};

/* Statusbar */
void update_statusbar_encoding(chandler *handler, cview *view);
void update_statusbar_language(chandler *handler, cview *view);
void update_statusbar(chandler *handler, cview *view);

/* Document */
cview *get_current_view(chandler *handler);
void update_document_views_status(chandler *handler, cdocument *document);
void free_document(cdocument *document);
void save_document(cdocument *document, gchar *file_name);
cdocument *new_document(chandler *handler, gchar *file_name);
void close_view(chandler *handler, cview *view);
void add_view_for_document(chandler *handler, cdocument *document);

/* Initialization */
void init_frame_tree_view(chandler *handler);
void init_frame_view(chandler *handler);
void init_header(chandler *handler);
void init_statusbar(chandler *handler);
void init_window(chandler *handler);

#endif

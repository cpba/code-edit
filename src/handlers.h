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
#define CONFIGURATION_FILE_NAME "/code.conf"
#define SESSIONS_FILE_NAME "/code-sessions.conf"
#define DEFAULT_SESSION_NAME "Default"
#define WINDOW_VIEW_MIN_WIDTH 320
#define WINDOW_VIEW_MIN_HEIGHT 320
#define SIDEBAR_TREE_VIEW_MIN_WIDTH 200
#define LIST_VIEW_MIN_HEIGHT 260
#define LIST_BOX_SESSIONS_MIN_WIDTH 600
#define SHOW_PROGRESS_BAR_AFTER 0.5
#define MINOR_SPACING 6
#define MEDIUM_SPACING 12
#define MAJOR_SPACING 18
#define HEADER_BAR_TEXT_SELECT_SESSION "Select Session"

typedef struct cdocument cdocument;
typedef struct cview cview;
typedef struct csession csession;
typedef struct chandler_dialog_save chandler_dialog_save;
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
	GDateTime *operation_start;
	GCancellable *cancellable;
	gboolean cancelled;
	GtkSourceFileLoader *source_file_loader;
	GtkSourceFileSaver *source_file_saver;
	const GtkSourceEncoding *encoding;
	GtkSourceBuffer *source_buffer;
	GtkSourceSearchContext *source_search_context;
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

struct csession {
	GString *name;
	GtkWidget *box;
	GtkWidget *label;
};

struct chandler_dialog_save {
	GtkWidget *dialog;
	GtkWidget *button_encoding;
	GtkWidget *button_newline;
};

struct chandler_header {
	chandler *handler;
	GtkWidget *header_bar;
	GtkWidget *revealer_session;
	GtkWidget *stack_extra;
	GtkWidget *button_open_menu;
	GtkWidget *button_new_document;
	GtkWidget *button_open_document;
	GtkWidget *button_save_document;
	GtkWidget *button_save_as_document;
	GtkWidget *button_preferences;
	GtkWidget *button_session_selection_mode;
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
	GtkWidget *entry_file_pattern;
	GtkWidget *button_use_regex;
	GtkWidget *button_match_case;
	GtkWidget *button_only_in_selection;
	GtkWidget *button_search;
	GtkWidget *button_search_all;
	GtkWidget *box_replace;
	GtkWidget *entry_replace;
	GtkWidget *button_replace;
	GtkWidget *button_replace_all;
	GtkSourceSearchSettings *source_search_settings;
	GtkSourceCompletionWords *source_completion_words;
	gint iter_search_offset;
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
	GtkWidget *button_language;
	GtkWidget *button_repo_branch;
};

struct chandler_window {
	GtkWidget *window;
	GtkWidget *stack;
	/* Select session */
	GtkWidget *box_select_session;
	GtkWidget *search_entry_session;
	GtkWidget *list_box_sessions;
	/* Session */
	GtkWidget *box_session;
	GtkWidget *box_frames;
};

struct chandler {
	GtkApplication *application;
	GList *documents;
	GKeyFile *key_file_config;
	GKeyFile *key_file_sessions;
	csession *current_session;
	chandler_window handler_window;
	chandler_header handler_header;
	chandler_statusbar handler_statusbar;
	chandler_frame_preferences handler_frame_preferences;
	chandler_frame_view handler_frame_view;
	chandler_frame_tree_view handler_frame_tree_view;
	chandler_dialog_save handler_dialog_save;
	chandler_dialog_save handler_dialog_save_as;
};

void window_update_sessions(chandler *handler);
void window_open_session(chandler *handler, csession *session);
void window_save_session(chandler *handler, csession *session);
csession *window_new_session(chandler *handler, gchar *name);

/* Header */
void update_headerbar(chandler *handler, cview *view);

/* Statusbar */
void update_statusbar_language(chandler *handler, cview *view);
void update_statusbar_repository_branch(chandler *handler, cview *view);
void update_statusbar(chandler *handler, cview *view);

/* Document */
cview *get_nth_view(chandler *handler, gint index);
cview *get_current_view(chandler *handler);
void update_view_status(chandler *handler, cview *view);
void update_document_views_status(chandler *handler, cdocument *document);
void free_document(chandler *handler, cdocument *document);
void save_document(cdocument *document, gchar *file_name);
cdocument *new_document(chandler *handler, gchar *file_name);
void close_view(chandler *handler, cview *view);
void add_view_for_document(chandler *handler, cdocument *document);

/* Actions */
void window_go_to_select_session(gpointer user_data);
void window_go_to_session(gpointer user_data);
void window_new(gpointer user_data);
void window_open(gpointer user_data);
void window_save_as(gpointer user_data);
void window_save(gpointer user_data);
void window_close(gpointer user_data);
void window_search_next(gpointer user_data);
void window_search_previous(gpointer user_data);
void window_toggle_search_bar(gpointer user_data);
void window_toggle_search_and_replace_bar(gpointer user_data);
void window_toggle_tree_view(gpointer user_data);

/* Initialization */
void init_frame_tree_view(chandler *handler);
void init_frame_view(chandler *handler);
void init_header(chandler *handler);
void init_statusbar(chandler *handler);
void init_file_chooser_save(chandler *handler, gchar *title, gchar *button);
void init_window(chandler *handler);

#endif

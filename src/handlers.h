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

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "texts.h"

#define PROGRAM_NAME "Code"
#define PROGRAM_ICON_NAME "text-editor"
#define CONFIGURATION_FILE_NAME "/code.conf"
#define SESSIONS_FILE_NAME "/code-sessions.conf"
#define WINDOW_VIEW_MIN_WIDTH 320
#define WINDOW_VIEW_MIN_HEIGHT 320
#define SIDEBAR_TREE_VIEW_MIN_WIDTH 200
#define LIST_VIEW_MIN_HEIGHT 260
#define LIST_BOX_SESSIONS_MIN_WIDTH 600
#define SHOW_PROGRESS_BAR_AFTER 0.5
#define MINOR_SPACING 6
#define MEDIUM_SPACING 12
#define MAJOR_SPACING 18

typedef struct cdocument cdocument;
typedef struct cview cview;
typedef struct csession csession;
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
	GtkTextIter iter_insert;
	GtkTextIter search_match_start;
	GtkTextIter search_match_end;
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
	gchar *id;
	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *entry;
	GList *view_file_names;
	GList *folders;
};

struct chandler {
	GtkApplication *application;
	GList *documents;
	GList *views;
	GKeyFile *key_file;
	GList *sessions;
	csession *current_session;
	struct {
		GtkWidget *window;
		GtkWidget *stack;
	} window;
	struct {
		chandler *handler;
		GtkWidget *header_bar;
		GtkWidget *stack_session;
		GtkWidget *stack_extra;
		GtkWidget *button_add_session;
		GtkWidget *button_select_session;
		GtkWidget *button_new_document;
		GtkWidget *button_open_document;
		GtkWidget *button_save_document;
		GtkWidget *button_save_as_document;
		GtkWidget *button_preferences;
		GtkWidget *button_session_selection_mode;
	} header;
	struct {
		GtkWidget *box;
		GtkWidget *scrolled_window;
		GtkWidget *search_bar;
		GtkWidget *search_entry;
		GtkWidget *list_box;
		GList *search_words;
		GtkSizeGroup *size_group;
		struct {
			GtkWidget *popover;
			GtkWidget *entry;
			GtkWidget *button_delete_session;
		} edit;
	} select_session;
	struct {
		GtkWidget *paned;
		GtkWidget *box_vertical;
		GtkWidget *notebook;
		gint iter_search_offset;
	} session;
	struct {
		GtkWidget *overlay;
		GtkWidget *box;
		GtkTreeStore *tree_store;
		GtkWidget *tree_view;
		GtkWidget *button_add_folder_to_session;
		struct {
			GtkWidget *popover;
		} root_selected;
		struct {
			GtkWidget *popover;
		} regular_selected;
		struct {
			GtkWidget *popover;
		} folder_selected;
		struct {
			GtkWidget *popover;
			GtkWidget *entry;
			GtkWidget *button;
		} rename;
		struct {
			GtkWidget *popover;
			GtkWidget *entry;
			GtkWidget *button;
		} name;
		
	} sidebar;
	struct {
		GtkWidget *revealer;
		GtkWidget *entry_search;
		GtkWidget *entry_file_pattern;
		GtkWidget *button_use_regex;
		GtkWidget *button_case_sensitive;
		GtkWidget *button_at_word_boundaries;
		GtkWidget *button_wrap_around;
		GtkWidget *button_search_next;
		GtkWidget *button_search_previous;
		GtkWidget *box_replace;
		GtkWidget *entry_replace;
		GtkWidget *button_replace;
		GtkWidget *button_replace_all;
		GtkSourceSearchSettings *source_search_settings;
		GtkSourceCompletionWords *source_completion_words;
	} search_and_replace;
	struct {
		GtkWidget *dialog;
		GtkWidget *button_encoding;
		GtkWidget *button_newline;
	} save;
	struct {
		GtkWidget *dialog;
		GtkWidget *button_encoding;
		GtkWidget *button_newline;
	} save_as;
	struct {
		GtkWidget *revealer;
		GtkWidget *box;
		GtkWidget *stack_switcher;
		GtkWidget *stack;
		GtkWidget *switch_show_sidebar;
		GtkWidget *switch_show_status_bar;
		GtkWidget *switch_show_overview_map;
		GtkWidget *switch_show_line_numbers;
		GtkWidget *switch_show_grid_pattern;
		GtkWidget *switch_show_right_margin;
		GtkWidget *spin_button_right_margin_position;
		GtkWidget *switch_automatic_indentation;
		GtkWidget *switch_show_newline;
		GtkWidget *switch_show_non_breaking_space;
		GtkWidget *switch_show_spaces;
		GtkWidget *switch_show_tabs;
		GtkWidget *toggle_button_show_trailing_tabs_and_spaces;
		GtkWidget *toggle_button_show_inside_text_tabs_and_spaces;
		GtkWidget *toggle_button_show_leading_tabs_and_spaces;
		GtkWidget *switch_insert_spaces_instead_of_tabs;
		GtkWidget *spin_button_tab_width;
		GtkWidget *switch_allow_text_wrapping;
		GtkWidget *switch_allow_split_words_over_lines;
		GtkWidget *switch_highlight_current_line;
		GtkWidget *switch_highlight_matching_brackets;
		GtkWidget *font_button;
		GtkWidget *list_box_highlight;
	} preferences;
	struct {
		GtkSizeGroup *size_group;
		GtkWidget *revealer;
		GtkWidget *action_bar;
		GtkWidget *button_language;
		GtkWidget *button_repo_branch;
	} statusbar;
};

/* Header */
void update_headerbar(chandler *handler, cview *view);

/* Statusbar */
void update_statusbar_language(chandler *handler, cview *view);
void update_statusbar_repository_branch(chandler *handler, cview *view);
void update_statusbar(chandler *handler, cview *view);

/* Sidebar */
void sidebar_update_iter_children(chandler *handler, GtkTreeIter iter);
GtkTreeIter sidebar_add_iter(chandler *handler, GtkTreeIter *parent, gchar *path);
void sidebar_open_selected(chandler *handler);
void sidebar_rename_selected(chandler *handler);
void sidebar_duplicate_selected(chandler *handler);
void sidebar_delete_selected(chandler *handler);
void sidebar_remove_folder_from_session(chandler *handler);

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

/* Preferences */
void preferences_default(chandler *handler);
void preferences_save(chandler *handler);
void preferences_load(chandler *handler);

/* Select session */
gchar *select_session_new_id(chandler *handler);
void select_session_load(chandler *handler, csession *session);
csession *select_session_new_session(chandler *handler, gchar *name, gchar *id, gint index);
void select_session_load_sessions(chandler *handler);
void select_session_save(chandler *handler);

/* Session */
void session_update_lists(chandler *handler, csession *session);
void session_open(chandler *handler, csession *session);
void session_close(chandler *handler);

/* Actions */
void window_show_about(chandler *handler);
void window_quit(chandler *handler);
void window_go_to_select_session(gpointer user_data);
void window_go_to_session(gpointer user_data);
void window_new(gpointer user_data);
void window_open(gpointer user_data);
void window_save_as(gpointer user_data);
void window_save(gpointer user_data);
void window_close(gpointer user_data);
void window_save_insert_iter(chandler *handler);
void window_search_here(chandler *handler);
void window_search_next(chandler *handler);
void window_search_previous(chandler *handler);
void window_hide_search_bar_and_replace_bar(chandler *handler);
void window_show_search_bar(chandler *handler);
void window_show_search_and_replace_bar(chandler *handler);
void window_toggle_sidebar(chandler *handler);

/* Initialization */
void init_file_chooser_save(chandler *handler, gchar *title, gchar *button);
void init_preferences(chandler *handler);
void init_sidebar(chandler *handler);
void init_search_and_replace(chandler *handler);
void init_session(chandler *handler);
void init_select_session(chandler *handler);
void init_statusbar(chandler *handler);
void init_header(chandler *handler);
void init_window(chandler *handler);

#endif

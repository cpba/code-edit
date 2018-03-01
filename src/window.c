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

#include <gtk/gtk.h>
#include "handlers.h"

void window_show_about(chandler *handler)
{
	gchar *authors[] = {
		"Felipe Ferreira da Silva <ferreiradaselva@protonmail.com>",
		NULL
	};
	gtk_show_about_dialog(GTK_WINDOW(handler->window.window),
		"program-name", PROGRAM_NAME,
		"logo-icon-name", PROGRAM_ICON_NAME,
		"title", TEXT_ABOUT,
		"license-type", GTK_LICENSE_GPL_3_0,
		"copyright", "Copyright © 2017 Felipe Ferreira da Silva",
		"authors", authors,
		NULL);
}

void window_quit(chandler *handler)
{
	gtk_window_close(GTK_WINDOW(handler->window.window));
}

void window_go_to_preferences(gpointer user_data)
{
	chandler *handler = user_data;
	gtk_stack_set_visible_child_name(GTK_STACK(handler->header.stack_session), "preferences");
	gtk_stack_set_visible_child_name(GTK_STACK(handler->header.stack_extra), "session");
	gtk_stack_set_visible_child_name(GTK_STACK(handler->window.stack), "preferences");
	window_update(handler, NULL);
}

void window_go_to_select_session(gpointer user_data)
{
	chandler *handler = user_data;
	gint response = 0;
	gboolean go_to_select_session = TRUE;
	GtkWidget *dialog = NULL;
	if (session_has_modified_document(handler)) {
		dialog = gtk_message_dialog_new(GTK_WINDOW(handler->window.window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_OK_CANCEL,
			TEXT_CLOSE_SESSION_WITHOUT_SAVING_THE_MODIFICATIONS);
		response = gtk_dialog_run(GTK_DIALOG(dialog));
		if (response != GTK_RESPONSE_OK) {
			go_to_select_session = FALSE;
		}
		gtk_widget_destroy(dialog);
	}
	if (go_to_select_session) {
		gtk_list_box_unselect_all(GTK_LIST_BOX(handler->select_session.list_box));
		if (handler->current_session) {
			session_update_lists(handler, handler->current_session);
			handler->current_session = NULL;
		}
		gtk_stack_set_visible_child_name(GTK_STACK(handler->header.stack_session), "select-session");
		gtk_stack_set_visible_child_name(GTK_STACK(handler->header.stack_extra), "select-session");
		gtk_stack_set_visible_child_name(GTK_STACK(handler->window.stack), "select-session");
		gtk_header_bar_set_title(GTK_HEADER_BAR(handler->header.header_bar), TEXT_SELECT_SESSION);
		gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->header.header_bar), NULL);
	}
}

void window_go_to_session(gpointer user_data)
{
	chandler *handler = user_data;
	window_update_preferences(handler);
	gtk_stack_set_visible_child_name(GTK_STACK(handler->header.stack_session), "session");
	gtk_stack_set_visible_child_name(GTK_STACK(handler->header.stack_extra), "session");
	gtk_stack_set_visible_child_name(GTK_STACK(handler->window.stack), "session");
	window_update(handler, NULL);
}

void window_new(gpointer user_data)
{
	chandler *handler = user_data;
	cdocument *document = new_document(handler, NULL);
	if (document) {
		document_add_view(handler, document);
	}
}

void window_open(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = NULL;
	gint response = 0;
	GtkWidget *dialog = NULL;
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		dialog = gtk_file_chooser_dialog_new(TEXT_OPEN,
			GTK_WINDOW(handler->window.window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			TEXT_OPEN, GTK_RESPONSE_OK,
			NULL);
		gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);
		view = get_current_view(handler);
		if (view) {
			GFile *file = gtk_source_file_get_location(view->document->source_file);
			if (file) {
				GFile *parent = g_file_get_parent(file);
				if (parent) {
					gchar *current_folder = g_file_get_path(parent);
					if (current_folder) {
						gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), current_folder);
						g_free(current_folder);
					}
				}
			}
		}
		response = gtk_dialog_run(GTK_DIALOG(dialog));
		if (response == GTK_RESPONSE_OK) {
			GSList *file_names = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
			GSList *file_name_iter = file_names;
			while (file_name_iter) {
				gchar *file_name = file_name_iter->data;
				cdocument *document = new_document(handler, file_name);
				if (document) {
					document_add_view(handler, document);
				}
				g_free(file_name);
				file_name_iter = file_name_iter->next;
			}
			g_slist_free(file_names);
		}
		gtk_widget_destroy(dialog);
	}
}

void window_save_as(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	gint response = 0;
	gchar *file_name = NULL;
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		if (view) {
			init_file_chooser_save(handler, TEXT_SAVE_AS, TEXT_SAVE);
			response = gtk_dialog_run(GTK_DIALOG(handler->save.dialog));
			if (response == GTK_RESPONSE_OK) {
				file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(handler->save.dialog));
				document_save(view->document, file_name);
				g_free(file_name);
			}
			gtk_widget_destroy(handler->save.dialog);
		}
	}
}

void window_save(gpointer user_data)
{
	chandler *handler = user_data;
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(handler->session.notebook));
	GtkWidget *scrolled_window = NULL;
	cview *view = NULL;
	GFile *file = NULL;
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), current_page);
		if (scrolled_window) {
			view = g_object_get_data(G_OBJECT(scrolled_window), "view");
		}
		if (view) {
			file = gtk_source_file_get_location(view->document->source_file);
			if (!file) {
				gint response = 0;
				gchar *file_name = NULL;
				init_file_chooser_save(handler, TEXT_SAVE, TEXT_SAVE);
				response = gtk_dialog_run(GTK_DIALOG(handler->save.dialog));
				if (response == GTK_RESPONSE_OK) {
					file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(handler->save.dialog));
					document_save(view->document, file_name);
					g_free(file_name);
				}
				gtk_widget_destroy(handler->save.dialog);
			} else {
				document_save(view->document, NULL);
			}
		}
	}
}

void window_close(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	cdocument *document = NULL;
	if (view) {
		document = view->document;
		view_close(handler, view, TRUE);
		if (!document->views) {
			document_free(handler, document);
		}
	}
}

void window_search_here(chandler *handler)
{
	cview *view = get_current_view(handler);
	gboolean found = FALSE;
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		if (view) {
			found = gtk_source_search_context_forward2(view->document->source_search_context,
				&view->document->iter_insert,
				&view->document->search_match_start,
				&view->document->search_match_end,
				NULL);
			if (found) {
				gtk_text_buffer_select_range(GTK_TEXT_BUFFER(view->document->source_buffer),
					&view->document->search_match_start,
					&view->document->search_match_end);
				gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
					&view->document->search_match_start,
					0.25,
					TRUE,
					0.5,
					0.5);
			} else {
				gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(view->document->source_buffer),
					&view->document->iter_insert);
				gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
					&view->document->iter_insert,
					0.25,
					TRUE,
					0.5,
					0.5);
			}
		}
	}
}

void window_search_next(chandler *handler)
{
	cview *view = get_current_view(handler);
	gboolean found = FALSE;
	GtkTextIter iter_beginning;
	GtkTextMark *text_mark = NULL;
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		if (view) {
			if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(view->document->source_buffer))) {
				text_mark = gtk_text_buffer_get_selection_bound(GTK_TEXT_BUFFER(view->document->source_buffer));
			} else {
				text_mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(view->document->source_buffer));
			}
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(view->document->source_buffer), &iter_beginning, text_mark);
			found = gtk_source_search_context_forward2(view->document->source_search_context,
				&iter_beginning,
				&view->document->search_match_start,
				&view->document->search_match_end,
				NULL);
			if (found) {
				gtk_text_buffer_select_range(GTK_TEXT_BUFFER(view->document->source_buffer),
					&view->document->search_match_start,
					&view->document->search_match_end);
				gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
					&view->document->search_match_start,
					0.25,
					TRUE,
					0.5,
					0.5);
			}
		}
	}
}

void window_search_previous(chandler *handler)
{
	cview *view = get_current_view(handler);
	gboolean found = FALSE;
	GtkTextIter iter_beginning;
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		if (view) {
			GtkTextMark *text_mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(view->document->source_buffer));
			text_mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(view->document->source_buffer));
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(view->document->source_buffer), &iter_beginning, text_mark);
			found = gtk_source_search_context_backward2(view->document->source_search_context,
				&iter_beginning,
				&view->document->search_match_start,
				&view->document->search_match_end,
				NULL);
			if (found) {
				gtk_text_buffer_select_range(GTK_TEXT_BUFFER(view->document->source_buffer),
					&view->document->search_match_start,
					&view->document->search_match_end);
				gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
					&view->document->search_match_start,
					0.25,
					TRUE,
					0.5,
					0.5);
			}
		}
	}
}

void window_show_search_bar(chandler *handler)
{
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		gtk_widget_hide(handler->search_and_replace.box_replace);
		if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->search_and_replace.revealer))) {
			gtk_revealer_set_reveal_child(GTK_REVEALER(handler->search_and_replace.revealer), TRUE);
		}
		gtk_widget_grab_focus(handler->search_and_replace.entry_search);
	}
}

void window_show_search_and_replace_bar(chandler *handler)
{
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		gtk_widget_show_all(handler->search_and_replace.box_replace);
		if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->search_and_replace.revealer))) {
			gtk_revealer_set_reveal_child(GTK_REVEALER(handler->search_and_replace.revealer), TRUE);
		}
		gtk_widget_grab_focus(handler->search_and_replace.entry_search);
	}
}

void window_hide_search_bar_and_replace_bar(chandler *handler)
{
	cview *view = get_current_view(handler);
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->search_and_replace.revealer), FALSE);
		if (view) {
			gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(view->document->source_buffer),
				&view->document->iter_insert);
			gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
				&view->document->iter_insert,
				0.25,
				TRUE,
				0.5,
				0.5);
		}
	}
}

void window_toggle_sidebar(chandler *handler)
{
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		if (gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_sidebar))) {
			gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_sidebar), FALSE);
		} else {
			gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_sidebar), TRUE);
		}
		window_update_preferences(handler);
	}
}

void window_update_preferences(chandler *handler)
{
	cview *view = NULL;
	GList *view_iter = handler->views;
	GtkSourceSpaceDrawer *source_space_drawer = NULL;
	GtkSourceSpaceLocationFlags locations = GTK_SOURCE_SPACE_LOCATION_NONE;
	GtkSourceSpaceTypeFlags types = GTK_SOURCE_SPACE_TYPE_NONE;
	if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_sidebar))) {
		gtk_widget_show(handler->sidebar.box);
	} else {
		gtk_widget_hide(handler->sidebar.box);
	}
	if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_statusbar))) {
		gtk_widget_show(handler->statusbar.revealer);
	} else {
		gtk_widget_hide(handler->statusbar.revealer);
	}
	if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_statusbar))) {
		gtk_widget_show(handler->statusbar.revealer);
	} else {
		gtk_widget_hide(handler->statusbar.revealer);
	}
	if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_newline))) {
		types = types | GTK_SOURCE_SPACE_TYPE_NEWLINE;
	}
	if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_tabs))) {
		types = types | GTK_SOURCE_SPACE_TYPE_TAB;
	}
	if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_spaces))) {
		types = types | GTK_SOURCE_SPACE_TYPE_SPACE;
	}
	if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_non_breaking_space))) {
		types = types | GTK_SOURCE_SPACE_TYPE_NBSP;
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_leading_tabs_and_spaces))) {
		locations = locations | GTK_SOURCE_SPACE_LOCATION_LEADING;
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces))) {
		locations = locations | GTK_SOURCE_SPACE_LOCATION_INSIDE_TEXT;
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_trailing_tabs_and_spaces))) {
		locations = locations | GTK_SOURCE_SPACE_LOCATION_TRAILING;
	}
	while (view_iter) {
		view = view_iter->data;
		gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view->source_view),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_line_numbers)));
		gtk_source_view_set_background_pattern(GTK_SOURCE_VIEW(view->source_view),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_grid_pattern)));
		gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(view->source_view),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_show_right_margin)));
		gtk_source_view_set_right_margin_position(GTK_SOURCE_VIEW(view->source_view),
			gtk_spin_button_get_value(GTK_SPIN_BUTTON(handler->preferences.spin_button_right_margin_position)));
		gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(view->source_view),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_automatic_indentation)));
		source_space_drawer = gtk_source_view_get_space_drawer(GTK_SOURCE_VIEW(view->source_view));
		gtk_source_space_drawer_set_matrix(source_space_drawer, NULL);
		gtk_source_space_drawer_set_types_for_locations(source_space_drawer,
			locations,
			types);
		gtk_source_space_drawer_set_enable_matrix(source_space_drawer, TRUE);
		gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view->source_view),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_insert_spaces_instead_of_tabs)));
		gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view->source_view),
			gtk_spin_button_get_value(GTK_SPIN_BUTTON(handler->preferences.spin_button_tab_width)));
		if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_allow_text_wrapping))) {
			if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_allow_split_words_over_lines))) {
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_CHAR);
			} else {
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_WORD);
			}
		} else {
			gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_NONE);
		}
		gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view->source_view),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_highlight_current_line)));
		gtk_source_buffer_set_highlight_matching_brackets(GTK_SOURCE_BUFFER(view->document->source_buffer),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_highlight_matching_brackets)));
		gtk_text_view_set_monospace(GTK_TEXT_VIEW(view->source_view),
			gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_use_monospace_font)));
		gtk_source_buffer_set_style_scheme(GTK_SOURCE_BUFFER(view->document->source_buffer),
			gtk_source_style_scheme_chooser_get_style_scheme(GTK_SOURCE_STYLE_SCHEME_CHOOSER(handler->preferences.style_scheme_chooser)));
		view_iter = g_list_next(view_iter);
	}
}

void window_update(chandler *handler, cview *view)
{
	headerbar_update(handler, view);
	statusbar_update(handler, view);
}

static gboolean window_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	chandler *handler = user_data;
	gboolean stop_propagate = FALSE;
	if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "select-session") == 0) {
		stop_propagate = gtk_search_bar_handle_event(GTK_SEARCH_BAR(handler->select_session.search_bar), (GdkEvent *)event);
		if (stop_propagate) {
			gtk_widget_hide(handler->header.button_add_session);
		}
	} else if (g_strcmp0(gtk_stack_get_visible_child_name(GTK_STACK(handler->window.stack)), "session") == 0) {
		if (gtk_widget_has_focus(handler->sidebar.tree_view)) {
			stop_propagate = gtk_search_bar_handle_event(GTK_SEARCH_BAR(handler->sidebar.search_bar), (GdkEvent *)event);
			if (stop_propagate) {
				gtk_widget_hide(handler->header.button_add_session);
			}
		}
	} 
	return stop_propagate;
}

static gboolean window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	chandler *handler = user_data;
	gboolean stop_propagate = FALSE;
	if (session_has_modified_document(handler)) {
		gint response = 0;
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(handler->window.window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_OK_CANCEL,
			TEXT_CLOSE_SESSION_WITHOUT_SAVING_THE_MODIFICATIONS);
		window_go_to_session(handler);
		response = gtk_dialog_run(GTK_DIALOG(dialog));
		if (response != GTK_RESPONSE_OK) {
			stop_propagate = TRUE;
		}
		gtk_widget_destroy(dialog);
	}
	return stop_propagate;
}

static void window_destroy(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	GString *key_file_path = NULL;
	GList *document_iter = NULL;
	if (handler->current_session) {
		session_update_lists(handler, handler->current_session);
	}
	select_session_save(handler);
	document_iter = handler->documents;
	while (document_iter) {
		document_free(handler, document_iter->data);
		document_iter = handler->documents;
	}
	preferences_save(handler);
	if (g_get_user_config_dir()) {
		key_file_path = g_string_new(g_get_user_config_dir());
		key_file_path = g_string_append(key_file_path, CONFIGURATION_FILE_NAME);
		g_key_file_save_to_file(handler->key_file, key_file_path->str, NULL);
	}
}

void init_window(chandler *handler)
{
	/* Window */
	handler->window.window = gtk_application_window_new(handler->application);
	gtk_window_set_icon_name(GTK_WINDOW(handler->window.window), "text-editor");
	g_signal_connect(handler->window.window, "delete-event", G_CALLBACK(window_delete_event), handler);
	g_signal_connect(handler->window.window, "key-press-event", G_CALLBACK(window_key_press_event), handler);
	g_signal_connect(handler->window.window, "destroy", G_CALLBACK(window_destroy), handler);
	/* Stack main */
	handler->window.stack = gtk_stack_new();
	gtk_widget_set_name(handler->window.stack, "stack");
	gtk_container_add(GTK_CONTAINER(handler->window.window), handler->window.stack);
	gtk_widget_set_hexpand(handler->window.stack, TRUE);
	gtk_widget_set_vexpand(handler->window.stack, TRUE);
	gtk_widget_set_halign(handler->window.stack, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->window.stack, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(handler->window.stack, WINDOW_VIEW_MIN_WIDTH, WINDOW_VIEW_MIN_HEIGHT);
	gtk_stack_set_transition_type(GTK_STACK(handler->window.stack), GTK_STACK_TRANSITION_TYPE_NONE);
}

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

void window_open_session(chandler *handler, csession *session)
{
	gchar **strings = NULL;
	gint i = 0;
	gsize length = 0;
	cdocument *document = NULL;
	/* Add pages */
	strings = g_key_file_get_string_list(handler->key_file_sessions,
		session->name->str,
		"views",
		&length,
		NULL);
	if (strings && length > 0) {
		i = 0;
		while (i < length) {
			if (strings[i]) {
				document = new_document(handler, strings[i]);
				add_view_for_document(handler, document);
			}
			i++;
		}
		g_strfreev(strings);
	}
	handler->current_session = session;
}

void window_save_session(chandler *handler, csession *session)
{
	GString *key_file_path = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->handler_frame_view.notebook));
	gint page_index = 0;
	cview *view = NULL;
	GFile *file = NULL;
	const gchar **strings = NULL;
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
	g_key_file_set_string_list(handler->key_file_sessions,
		session->name->str,
		"views",
		strings,
		page_count);
	if (g_get_user_config_dir()) {
		key_file_path = g_string_new(g_get_user_config_dir());
		key_file_path = g_string_append(key_file_path, SESSIONS_FILE_NAME);
	}
	if (key_file_path) {
		g_key_file_save_to_file(handler->key_file_sessions, key_file_path->str, NULL);
		g_string_free(key_file_path, TRUE);
	}
}

void window_remove_session(chandler *handler, csession *session)
{
	GList *children = NULL;
	GtkWidget *row = NULL;
	g_key_file_remove_group(handler->key_file_sessions, session->name->str, NULL);
	row = gtk_widget_get_parent(session->box);
	gtk_widget_destroy(row);
	g_string_free(session->name, TRUE);
	free(session);
	children = gtk_container_get_children(GTK_CONTAINER(handler->handler_window.list_box_sessions));
	if (g_list_length(children) < 1) {
		window_new_session(handler, DEFAULT_SESSION_NAME);
	}
	g_list_free(children);
}

csession *window_new_session(chandler *handler, gchar *name)
{
	csession *session = malloc(sizeof(csession));
	GString *label_string = NULL;
	session->name = g_string_new(name);
	session->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_set_border_width(GTK_CONTAINER(session->box), MEDIUM_SPACING);
	g_object_set_data(G_OBJECT(session->box), "session", session);
	/* Label */
	session->label = gtk_label_new(name);
	gtk_container_add(GTK_CONTAINER(session->box), session->label);
	gtk_widget_set_hexpand(session->label, TRUE);
	gtk_widget_set_vexpand(session->label, FALSE);
	gtk_widget_set_halign(session->label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(session->label, GTK_ALIGN_CENTER);
	gtk_label_set_xalign(GTK_LABEL(session->label), 0.0f);
	gtk_label_set_ellipsize(GTK_LABEL(session->label), PANGO_ELLIPSIZE_END);
	label_string = g_string_new("<b>");
	label_string = g_string_append(label_string, name);
	label_string = g_string_append(label_string, "</b>");
	gtk_label_set_use_markup(GTK_LABEL(session->label), TRUE);
	gtk_label_set_markup(GTK_LABEL(session->label), label_string->str);
	g_string_free(label_string, TRUE);
	/* List box */
	gtk_list_box_insert(GTK_LIST_BOX(handler->handler_window.list_box_sessions), session->box, -1);
	gtk_widget_show_all(handler->handler_window.list_box_sessions);
	return session;
}

void window_update_sessions(chandler *handler)
{
	csession *session = NULL;
	gint i = 0;
	gsize length = 0;
	gchar **strings = NULL;
	GList *children = NULL;
	GList *children_iter = NULL;
	GtkWidget *session_box = NULL;
	GtkWidget *row = NULL;
	GtkWidget *icon = NULL;
	/* Clear list box */
	children = gtk_container_get_children(GTK_CONTAINER(handler->handler_window.list_box_sessions));
	children_iter = children;
	while (children_iter) {
		row = children_iter->data;
		session_box = gtk_bin_get_child(GTK_BIN(row));
		session = g_object_get_data(G_OBJECT(session_box), "session");
		if (session) {
			g_string_free(session->name, TRUE);
			free(session);
		}
		gtk_widget_destroy(row);
		children_iter = g_list_next(children_iter);
	}
	/* Add sessions */
	strings = g_key_file_get_groups(handler->key_file_sessions, &length);
	while (i < length) {
		session = window_new_session(handler, strings[i]);
		i++;
	}
	g_strfreev(strings);
	/* Default session */
	children = gtk_container_get_children(GTK_CONTAINER(handler->handler_window.list_box_sessions));
	if (g_list_length(children) < 1) {
		window_new_session(handler, DEFAULT_SESSION_NAME);
	}
	/* Add session button */
	icon = gtk_image_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_list_box_insert(GTK_LIST_BOX(handler->handler_window.list_box_sessions), icon, -1);
	gtk_widget_set_margin_top(icon, MEDIUM_SPACING);
	gtk_widget_set_margin_bottom(icon, MEDIUM_SPACING);
	gtk_widget_set_margin_start(icon, MEDIUM_SPACING);
	gtk_widget_set_margin_end(icon, MEDIUM_SPACING);
	row = gtk_widget_get_parent(icon);
	gtk_list_box_row_set_selectable(GTK_LIST_BOX_ROW(row), FALSE);
	gtk_widget_show_all(icon);
	g_list_free(children);
}

void window_go_to_select_session(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = NULL;
	handler->current_session = NULL;
	gtk_list_box_unselect_all(GTK_LIST_BOX(handler->handler_window.list_box_sessions));
	/* Remove current notebook pages */
	while (gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->handler_frame_view.notebook)) > 0) {
		view = get_nth_view(handler, 0);
		close_view(handler, view);
	}
	window_update_sessions(handler);
	gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_header.revealer_session), FALSE);
	gtk_stack_set_visible_child_name(GTK_STACK(handler->handler_header.stack_extra), "select-session");
	gtk_stack_set_visible_child_name(GTK_STACK(handler->handler_window.stack), "select-session");
	gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), HEADER_BAR_TEXT_SELECT_SESSION);
	gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), NULL);
}

void window_go_to_session(gpointer user_data)
{
	chandler *handler = user_data;
	gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_header.revealer_session), TRUE);
	gtk_stack_set_visible_child_name(GTK_STACK(handler->handler_header.stack_extra), "session");
	gtk_stack_set_visible_child_name(GTK_STACK(handler->handler_window.stack), "session");
	update_view_status(handler, NULL);
}

void window_new(gpointer user_data)
{
	chandler *handler = user_data;
	cdocument *document = new_document(handler, NULL);
	add_view_for_document(handler, document);
}

void window_open(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = NULL;
	gint response = 0;
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open",
		GTK_WINDOW(handler->handler_window.window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"Open", GTK_RESPONSE_OK,
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
	cview *view = get_current_view(handler);
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

void window_close(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	if (view) {
		close_view(handler, view);
	}
}

static void button_remove_session_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	csession *session = g_object_get_data(G_OBJECT(widget), "session");
	window_remove_session(handler, session);
}

void window_search_here(chandler *handler)
{
	cview *view = get_current_view(handler);
	gboolean found = FALSE;
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

void window_search_next(chandler *handler)
{
	cview *view = get_current_view(handler);
	gboolean found = FALSE;
	GtkTextIter iter_beginning;
	GtkTextMark *text_mark = NULL;
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

void window_search_previous(chandler *handler)
{
	cview *view = get_current_view(handler);
	gboolean found = FALSE;
	GtkTextIter iter_beginning;
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

void window_show_search_bar(chandler *handler)
{
	gtk_widget_hide(GTK_WIDGET(handler->handler_frame_view.box_replace));
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace), TRUE);
	}
	gtk_widget_grab_focus(handler->handler_frame_view.entry_search);
}

void window_show_search_and_replace_bar(chandler *handler)
{
	gtk_widget_show_all(GTK_WIDGET(handler->handler_frame_view.box_replace));
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace), TRUE);
	}
	gtk_widget_grab_focus(handler->handler_frame_view.entry_search);
}

void window_hide_search_bar_and_replace_bar(chandler *handler)
{
	cview *view = get_current_view(handler);
	gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace), FALSE);
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

void window_toggle_tree_view(chandler *handler)
{
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_tree_view.revealer))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_tree_view.revealer), TRUE);
	} else if (gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_tree_view.revealer))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_tree_view.revealer), FALSE);
	}
}

static void list_box_sessions_row_activated(GtkWidget *widget, GtkListBoxRow *row, gpointer user_data)
{
	chandler *handler = user_data;
	GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
	GtkWidget *child = gtk_bin_get_child(GTK_BIN(row));
	csession *session = g_object_get_data(G_OBJECT(child), "session");
	gint index = gtk_list_box_row_get_index(row);
	if (index < g_list_length(children) - 1) {
		window_open_session(handler, session);
		window_go_to_session(handler);
	}
	g_list_free(children);
}

static void window_destroy(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	if (handler->current_session) {
		window_save_session(handler, handler->current_session);
	}
}

void init_window(chandler *handler)
{
	chandler_window *handler_window = &handler->handler_window;
	GtkWidget *scrolled_window = NULL;
	GtkWidget *box = NULL;
	GtkWidget *frame = NULL;
	/* Window */
	handler_window->window = gtk_application_window_new(handler->application);
	gtk_window_set_icon_name(GTK_WINDOW(handler_window->window), "text-editor");
	g_signal_connect(handler_window->window, "destroy", G_CALLBACK(window_destroy), handler);
	/* Stack main */
	handler_window->stack = gtk_stack_new();
	gtk_widget_set_name(handler_window->stack, "stack");
	gtk_container_add(GTK_CONTAINER(handler_window->window), handler_window->stack);
	gtk_widget_set_hexpand(handler_window->stack, TRUE);
	gtk_widget_set_vexpand(handler_window->stack, TRUE);
	gtk_widget_set_halign(handler_window->stack, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_window->stack, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(handler_window->stack, WINDOW_VIEW_MIN_WIDTH, WINDOW_VIEW_MIN_HEIGHT);
	gtk_stack_set_transition_type(GTK_STACK(handler_window->stack), GTK_STACK_TRANSITION_TYPE_NONE);
	/* Scrolled window select-session */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_stack_add_named(GTK_STACK(handler_window->stack), scrolled_window, "select-session");
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	/* Box scrolled window */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(box, "box_select_session");
	gtk_container_add(GTK_CONTAINER(scrolled_window), box);
	gtk_widget_set_size_request(box, 400, -1);
	/* Box select-session */
	handler_window->box_select_session = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler_window->box_select_session, "box_select_session");
	gtk_container_add(GTK_CONTAINER(box), handler_window->box_select_session);
	gtk_widget_set_hexpand(handler_window->box_select_session, FALSE);
	gtk_widget_set_halign(handler_window->box_select_session, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_start(handler_window->box_select_session, MAJOR_SPACING);
	gtk_widget_set_margin_end(handler_window->box_select_session, MAJOR_SPACING);
	gtk_widget_set_size_request(handler_window->box_select_session, LIST_BOX_SESSIONS_MIN_WIDTH, -1);
	/* Entry search session */
	handler_window->search_entry_session = gtk_search_entry_new();
	gtk_widget_set_name(handler_window->search_entry_session, "search_entry_session");
	gtk_container_add(GTK_CONTAINER(handler_window->box_select_session), handler_window->search_entry_session);
	gtk_widget_set_hexpand(handler_window->search_entry_session, TRUE);
	gtk_widget_set_vexpand(handler_window->search_entry_session, FALSE);
	gtk_widget_set_halign(handler_window->search_entry_session, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_window->search_entry_session, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_top(handler_window->search_entry_session, MAJOR_SPACING);
	/* Frame list box */
	frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(handler_window->box_select_session), frame);
	gtk_widget_set_hexpand(frame, TRUE);
	gtk_widget_set_vexpand(frame, FALSE);
	gtk_widget_set_halign(frame, GTK_ALIGN_FILL);
	gtk_widget_set_valign(frame, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_top(frame, MAJOR_SPACING);
	/* List box */
	handler_window->list_box_sessions = gtk_list_box_new();
	gtk_widget_set_name(handler_window->list_box_sessions, "list_box_sessions");
	gtk_container_add(GTK_CONTAINER(frame), handler_window->list_box_sessions);
	gtk_widget_set_hexpand(handler_window->list_box_sessions, TRUE);
	gtk_widget_set_vexpand(handler_window->list_box_sessions, TRUE);
	gtk_widget_set_halign(handler_window->list_box_sessions, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_window->list_box_sessions, GTK_ALIGN_FILL);
	gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(handler_window->list_box_sessions), FALSE);
	g_signal_connect(handler_window->list_box_sessions, "row-activated", G_CALLBACK(list_box_sessions_row_activated), handler);
	/* Box session */
	handler_window->box_session = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler_window->box_session, "box_session");
	gtk_stack_add_named(GTK_STACK(handler_window->stack), handler_window->box_session, "session");
	/* Box frames */
	handler_window->box_frames = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(handler_window->box_frames, "box_frames");
	gtk_container_add(GTK_CONTAINER(handler_window->box_session), handler_window->box_frames);
	gtk_widget_set_hexpand(handler_window->box_frames, TRUE);
	gtk_widget_set_vexpand(handler_window->box_frames, TRUE);
	gtk_widget_set_halign(handler_window->box_frames, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_window->box_frames, GTK_ALIGN_FILL);
}

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

void window_go_to_select_session(gpointer user_data)
{
	chandler *handler = user_data;
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

void window_search_next(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	if (view) {
		gboolean found = FALSE;
		GtkTextIter iter_beginning;
		GtkTextIter iter_start;
		GtkTextIter iter_end;
		GtkTextMark *text_mark = NULL;
		if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(view->document->source_buffer))) {
			text_mark = gtk_text_buffer_get_selection_bound(GTK_TEXT_BUFFER(view->document->source_buffer));
		} else {
			text_mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(view->document->source_buffer));
		}
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(view->document->source_buffer), &iter_beginning, text_mark);
		found = gtk_source_search_context_forward2(view->document->source_search_context,
			&iter_beginning,
			&iter_start,
			&iter_end,
			NULL);
		if (found) {
			gtk_text_buffer_select_range(GTK_TEXT_BUFFER(view->document->source_buffer),
				&iter_start,
				&iter_end);
			gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
				&iter_start,
				0.25,
				TRUE,
				0.5,
				0.5);
		}
	}
}

void window_search_previous(gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	if (view) {
		gboolean found = FALSE;
		GtkTextIter iter_beginning;
		GtkTextIter iter_start;
		GtkTextIter iter_end;
		GtkTextMark *text_mark = NULL;
		text_mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(view->document->source_buffer));
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(view->document->source_buffer), &iter_beginning, text_mark);
		found = gtk_source_search_context_backward2(view->document->source_search_context,
			&iter_beginning,
			&iter_start,
			&iter_end,
			NULL);
		if (found) {
			gtk_text_buffer_select_range(GTK_TEXT_BUFFER(view->document->source_buffer),
				&iter_start,
				&iter_end);
			gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
				&iter_start,
				0.25,
				TRUE,
				0.5,
				0.5);
		}
	}
}

void window_toggle_search_bar(gpointer user_data)
{
	chandler *handler = user_data;
	gtk_widget_hide(GTK_WIDGET(handler->handler_frame_view.box_replace));
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace), TRUE);
	}
	gtk_widget_grab_focus(handler->handler_frame_view.entry_search);
}

void window_toggle_search_and_replace_bar(gpointer user_data)
{
	chandler *handler = user_data;
	gtk_widget_show_all(GTK_WIDGET(handler->handler_frame_view.box_replace));
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_view.revealer_search_and_replace), TRUE);
	}
	gtk_widget_grab_focus(handler->handler_frame_view.entry_search);
}

void window_toggle_tree_view(gpointer user_data)
{
	chandler *handler = user_data;
	if (!gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_tree_view.revealer))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_tree_view.revealer), TRUE);
	} else if (gtk_revealer_get_child_revealed(GTK_REVEALER(handler->handler_frame_tree_view.revealer))) {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->handler_frame_tree_view.revealer), FALSE);
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
	gtk_widget_set_size_request(handler_window->box_select_session, 400, -1);
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

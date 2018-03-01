/*
    Copyright © 2018 Felipe Ferreira da Silva <ferreiradaselva@protonmail.com>

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

csession *get_session_by_id(chandler *handler, gchar *id)
{
	csession *result = NULL;
	csession *session = NULL;
	GtkWidget *row = NULL;
	GtkWidget *child = NULL;
	GList *children = gtk_container_get_children(GTK_CONTAINER(handler->select_session.list_box));
	GList *children_iter = children;
	while (children_iter && !result) {
		row = children_iter->data;
		child = gtk_bin_get_child(GTK_BIN(row));
		session = g_object_get_data(G_OBJECT(child), "session");
		if (g_strcmp0(session->id, id) == 0) {
			result = session;
		}
		children_iter = g_list_next(children_iter);
	}
	return result;
}

static gchar *get_random_id(void)
{
	gchar *id = g_uuid_string_random();
	gint i = 0;
	while (id[i] != '\0') {
		if (!g_ascii_isalnum(id[i])) {
			id[i] = '_';
		}
		i++;
	}
	return id;
}

gchar *select_session_new_id(chandler *handler)
{
	gchar *id = get_random_id();
	csession *session = get_session_by_id(handler, id);
	while (session) {
		g_free(id);
		id = get_random_id();
		session = get_session_by_id(handler, id);
	}
	return id;
}

csession *select_session_get_nth(chandler *handler, gint index)
{
	csession *session = NULL;
	gint i = 0;
	GtkWidget *session_box = NULL;
	GtkWidget *row = NULL;
	GList *children = gtk_container_get_children(GTK_CONTAINER(handler->select_session.list_box));
	GList *children_iter = children;
	while (children_iter && !session) {
		if (i == index) {
			row = children_iter->data;
			session_box = gtk_bin_get_child(GTK_BIN(row));
			session = g_object_get_data(G_OBJECT(session_box), "session");
		}
		children_iter = g_list_next(children_iter);
		i++;
	}
	return session;
}

static void select_session_save_session(chandler *handler, csession *session)
{
	GString *key_name = NULL;
	GString *name = NULL;
	gint i = 0;
	GList *list_iter = NULL;
	gchar **strings = NULL;
	gint count = 0;
	/* Views */
	key_name = g_string_new("views_");
	key_name = g_string_append(key_name, session->id);
	count = g_list_length(session->view_file_names);
	if (count > 0) {
		strings = g_malloc0((count + 1) * sizeof(gchar *));
		i = 0;
		list_iter = session->view_file_names;
		while (list_iter) {
			name = list_iter->data;
			strings[i] = g_strdup(name->str);
			i++;
			list_iter = g_list_next(list_iter);
		}
		g_key_file_set_string_list(handler->key_file,
			"sessions",
			key_name->str,
			(const gchar * const *)strings,
			count);
		g_strfreev(strings);
	} else {
		g_key_file_remove_key(handler->key_file,
			"sessions",
			key_name->str,
			NULL);
	}
	g_string_free(key_name, TRUE);
	/* Folders */
	key_name = g_string_new("folders_");
	key_name = g_string_append(key_name, session->id);
	count = g_list_length(session->folders);
	if (count > 0) {
		strings = g_malloc0((count + 1) * sizeof(gchar *));
		i = 0;
		list_iter = session->folders;
		while (list_iter) {
			name = list_iter->data;
			strings[i] = g_strdup(name->str);
			i++;
			list_iter = g_list_next(list_iter);
		}
		g_key_file_set_string_list(handler->key_file,
			"sessions",
			key_name->str,
			(const gchar * const *)strings,
			count);
		g_strfreev(strings);
	} else {
		g_key_file_remove_key(handler->key_file,
			"sessions",
			key_name->str,
			NULL);
	}
	g_string_free(key_name, TRUE);
}

void select_session_save(chandler *handler)
{
	csession *session = NULL;
	gchar **strings = NULL;
	gint i = 0;
	GList *children = gtk_container_get_children(GTK_CONTAINER(handler->select_session.list_box));
	gint length = g_list_length(children);
	if (handler->current_session) {
		g_key_file_set_string(handler->key_file,
			"preferences",
			"session",
			handler->current_session->id);
	} else {
		g_key_file_remove_key(handler->key_file,
			"preferences",
			"session",
			NULL);
	}
	g_key_file_remove_group(handler->key_file, "sessions", NULL);
	if (length > 0) {
		strings = g_malloc0(length * 2 * sizeof(gchar *));
		while (i < length) {
			session = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(g_list_nth_data(children, i)))), "session");
			select_session_save_session(handler, session);
			strings[i * 2] = g_strdup(session->name->str);
			strings[i * 2 + 1] = g_strdup(session->id);
			i++;
		}
		g_key_file_set_string_list(handler->key_file,
			"sessions",
			"list",
			(const gchar * const *)strings,
			length * 2);
		g_strfreev(strings);
	}
}

void select_session_load(chandler *handler, csession *session)
{
	gchar **strings = NULL;
	GString *file_name = NULL;
	GString *key_name = NULL;
	gint i = 0;
	gsize length = 0;
	/* View file names */
	key_name = g_string_new("views_");
	key_name = g_string_append(key_name, session->id);
	strings = g_key_file_get_string_list(handler->key_file,
		"sessions",
		key_name->str,
		&length,
		NULL);
	if (strings) {
		i = 0;
		while (i < length) {
			file_name = g_string_new(strings[i]);
			session->view_file_names = g_list_append(session->view_file_names, file_name);
			i++;
		}
		g_strfreev(strings);
	}
	g_string_free(key_name, TRUE);
	/* Folders */
	key_name = g_string_new("folders_");
	key_name = g_string_append(key_name, session->id);
	strings = g_key_file_get_string_list(handler->key_file,
		"sessions",
		key_name->str,
		&length,
		NULL);
	if (strings) {
		i = 0;
		while (i < length) {
			file_name = g_string_new(strings[i]);
			session->folders = g_list_append(session->folders, file_name);
			i++;
		}
		g_strfreev(strings);
	}
	g_string_free(key_name, TRUE);
}

static void box_session_destroy(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	GList *iter = NULL;
	csession *session = g_object_get_data(G_OBJECT(widget), "session");
	g_string_free(session->name, TRUE);
	g_free(session->id);
	/* View file names */
	iter = session->view_file_names;
	while (iter) {
		g_string_free(iter->data, TRUE);
		iter = g_list_next(iter);
	}
	g_list_free(session->view_file_names);
	/* Folders */
	iter = session->folders;
	while (iter) {
		g_string_free(iter->data, TRUE);
		iter = g_list_next(iter);
	}
	g_list_free(session->folders);
	g_slice_free1(sizeof(csession), session);
}

csession *select_session_new_session(chandler *handler, gchar *name, gchar *id, gint index)
{
	csession *session = g_slice_alloc0(sizeof(csession));
	GString *label_string = NULL;
	session->name = g_string_new(name);
	session->id = g_strdup(id);
	session->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_set_border_width(GTK_CONTAINER(session->box), MEDIUM_SPACING);
	gtk_size_group_add_widget(handler->select_session.size_group, session->box);
	g_object_set_data(G_OBJECT(session->box), "session", session);
	g_signal_connect(session->box, "destroy", G_CALLBACK(box_session_destroy), handler);
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
	g_string_free(label_string, TRUE);
	/* List box */
	gtk_list_box_insert(GTK_LIST_BOX(handler->select_session.list_box), session->box, index);
	gtk_widget_show_all(handler->select_session.list_box);
	return session;
}

void select_session_load_sessions(chandler *handler)
{
	csession *session = NULL;
	csession *current_session = NULL;
	gint i = 0;
	gsize length = 0;
	gchar *id = NULL;
	gchar *current_session_id = NULL;
	gchar **strings = NULL;
	GList *children = NULL;
	current_session_id = g_key_file_get_string(handler->key_file,
		"preferences",
		"session",
		NULL);
	/* Add sessions */
	strings = g_key_file_get_string_list(handler->key_file,
		"sessions",
		"list",
		&length,
		NULL);
	if (strings) {
		i = 0;
		while (i < length && !id) {
			if (i + 1 < length) {
				session = select_session_new_session(handler, strings[i], strings[i + 1], -1);
				select_session_load(handler, session);
				if (g_strcmp0(session->id, current_session_id) == 0) {
					current_session = session;
				}
			}
			i = i + 2;
		}
		g_strfreev(strings);
	}
	/* Default session */
	children = gtk_container_get_children(GTK_CONTAINER(handler->select_session.list_box));
	if (length < 2) {
		id = select_session_new_id(handler);
		select_session_new_session(handler, TEXT_DEFAULT_SESSION_NAME, id, -1);
		g_free(id);
	}
	g_list_free(children);
	if (current_session_id) {
		session_clear(handler);
		session_open(handler, current_session);
		window_go_to_session(handler);
		g_free(current_session_id);
	} else {
		session_clear(handler);
		window_go_to_session(handler);
	}
}

static void button_delete_session_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	GtkAllocation allocation;
	GList *children = NULL;
	gchar *id = NULL;
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(handler->select_session.list_box));
	if (row) {
		gtk_widget_get_allocation(GTK_WIDGET(row), &allocation);
		gtk_list_box_unselect_row(GTK_LIST_BOX(handler->select_session.list_box), row);
		gtk_popover_set_relative_to(GTK_POPOVER(handler->select_session.edit.popover), handler->select_session.list_box);
		gtk_popover_set_pointing_to(GTK_POPOVER(handler->select_session.edit.popover), &allocation);
		gtk_popover_popdown(GTK_POPOVER(handler->select_session.edit.popover));
		gtk_widget_destroy(GTK_WIDGET(row));
	}
	/* Default session */
	children = gtk_container_get_children(GTK_CONTAINER(handler->select_session.list_box));
	if (g_list_length(children) == 0) {
		id = select_session_new_id(handler);
		select_session_new_session(handler, TEXT_DEFAULT_SESSION_NAME, id, -1);
		g_free(id);
	}
	g_list_free(children);
}

static void entry_session_name_activate(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gtk_popover_popdown(GTK_POPOVER(handler->select_session.edit.popover));
}

static void entry_session_name_changed(GtkEditable *editable, gpointer user_data)
{
	chandler *handler = user_data;
	csession *session = NULL;
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(handler->select_session.list_box));
	GtkWidget *child = NULL;
	if (row) {
		child = gtk_bin_get_child(GTK_BIN(row));
		session = g_object_get_data(G_OBJECT(child), "session");
		gtk_label_set_text(GTK_LABEL(session->label), gtk_entry_get_text(GTK_ENTRY(handler->select_session.edit.entry)));
		session->name = g_string_assign(session->name, gtk_entry_get_text(GTK_ENTRY(handler->select_session.edit.entry)));
	}
}

static void search_entry_search_stop_search(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gtk_widget_show_all(handler->header.button_add_session);
}

static void search_entry_search_changed(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gchar *word_tmp = NULL;
	gchar *word = NULL;
	gint search_text_len = 0;
	gchar *word_c = NULL;
	gchar *word_start = NULL;
	gsize word_len = 0;
	gunichar unichar = 0;
	gchar *search_text = g_strdup_printf("%s ", gtk_entry_get_text(GTK_ENTRY(widget)));
	GList *word_iter = handler->select_session.search_words;
	while (word_iter) {
		g_free(word_iter->data);
		word_iter->data = NULL;
		word_iter = word_iter->next;
	}
	g_list_free(handler->select_session.search_words);
	handler->select_session.search_words = NULL;
	search_text_len = g_utf8_strlen(search_text, -1);
	word_c = search_text;
	while (word_c - search_text <= search_text_len) {
		unichar = g_utf8_get_char(word_c);
		if (g_unichar_isgraph(unichar)) {
			if (word_start == NULL) {
				word_start = word_c;
				word_len = word_len + 1;
			} else {
				word_len = word_len + 1;
			}
		} else {
			if (word_len > 0) {
				word_tmp = g_malloc(sizeof(gchar) * (word_c - word_start) + 1);
				word_tmp = g_utf8_strncpy(word_tmp, word_start, word_len);
				word = g_utf8_strdown(word_tmp, -1);
				handler->select_session.search_words = g_list_append(handler->select_session.search_words, word);
				g_free(word_tmp);
			}
			word_start = NULL;
			word_len = 0;
		}
		word_c = g_utf8_next_char(word_c);
	}
	gtk_list_box_invalidate_filter(GTK_LIST_BOX(handler->select_session.list_box));
	g_free(search_text);
}

static void list_box_sessions_row_activated(GtkWidget *widget, GtkListBoxRow *row, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *child = gtk_bin_get_child(GTK_BIN(row));
	csession *session = g_object_get_data(G_OBJECT(child), "session");
	session_clear(handler);
	session_open(handler, session);
	window_go_to_session(handler);
}

static gboolean list_box_filter(GtkListBoxRow *row, gpointer user_data)
{
	chandler *handler = user_data;
	gboolean show = TRUE;
	GtkWidget *box = gtk_bin_get_child(GTK_BIN(row));
	csession *session = g_object_get_data(G_OBJECT(box), "session");
	GList *word_iter = NULL;
	gchar *word = NULL;
	const gchar *text = NULL;
	gchar *text_tmp = NULL;
	word_iter = handler->select_session.search_words;
	while (word_iter && show) {
		word = word_iter->data;
		text = gtk_label_get_text(GTK_LABEL(session->label));
		if (text) {
			text_tmp = g_utf8_strdown(text, -1);
			if (!g_strstr_len(text_tmp, -1, word)) {
				/* One of the search words is missing */
				show = FALSE;
			}
			g_free(text_tmp);
		}
		word_iter = word_iter->next;
	}
	return show;
}

static gboolean list_box_popup_menu(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	csession *session = NULL;
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(handler->select_session.list_box));
	GtkAllocation allocation;
	GtkWidget *child = NULL;
	if (row) {
		gtk_widget_get_allocation(GTK_WIDGET(row), &allocation);
		child = gtk_bin_get_child(GTK_BIN(row));
		session = g_object_get_data(G_OBJECT(child), "session");
		gtk_entry_set_text(GTK_ENTRY(handler->select_session.edit.entry), session->name->str);
		gtk_popover_set_relative_to(GTK_POPOVER(handler->select_session.edit.popover), handler->select_session.list_box);
		gtk_popover_set_pointing_to(GTK_POPOVER(handler->select_session.edit.popover), &allocation);
		gtk_popover_set_position(GTK_POPOVER(handler->select_session.edit.popover), GTK_POS_BOTTOM);
		gtk_popover_popup(GTK_POPOVER(handler->select_session.edit.popover));
	}
	return TRUE;
}

static gboolean list_box_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	chandler *handler = user_data;
	csession *session = NULL;
	GtkListBoxRow *row = gtk_list_box_get_row_at_y(GTK_LIST_BOX(handler->select_session.list_box), event->y);
	GtkAllocation allocation;
	GtkWidget *child = NULL;
	if (row) {
		gtk_list_box_select_row(GTK_LIST_BOX(handler->select_session.list_box), row);
		gtk_widget_grab_focus(GTK_WIDGET(row));
	}
	row = gtk_list_box_get_selected_row(GTK_LIST_BOX(handler->select_session.list_box));
	if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
		if (row) {
			gtk_widget_get_allocation(GTK_WIDGET(row), &allocation);
			child = gtk_bin_get_child(GTK_BIN(row));
			session = g_object_get_data(G_OBJECT(child), "session");
			gtk_entry_set_text(GTK_ENTRY(handler->select_session.edit.entry), session->name->str);
			gtk_popover_set_relative_to(GTK_POPOVER(handler->select_session.edit.popover), handler->select_session.list_box);
			gtk_popover_set_pointing_to(GTK_POPOVER(handler->select_session.edit.popover), &allocation);
			gtk_popover_set_position(GTK_POPOVER(handler->select_session.edit.popover), GTK_POS_BOTTOM);
			gtk_popover_popup(GTK_POPOVER(handler->select_session.edit.popover));
		}
	}
	return FALSE;
}

void init_select_session(chandler *handler)
{
	GtkWidget *box = NULL;
	GtkWidget *label = NULL;
	GtkWidget *label_placeholder = NULL;
	handler->select_session.size_group = gtk_size_group_new(GTK_SIZE_GROUP_VERTICAL);
	/* Box select-session */
	handler->select_session.box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler->select_session.box, "box_select_session");
	gtk_stack_add_named(GTK_STACK(handler->window.stack), handler->select_session.box, "select-session");
	gtk_widget_set_size_request(handler->select_session.box, LIST_BOX_SESSIONS_MIN_WIDTH, -1);
	/* Search bar */
	handler->select_session.search_bar = gtk_search_bar_new();
	gtk_container_add(GTK_CONTAINER(handler->select_session.box), handler->select_session.search_bar);
	gtk_widget_set_hexpand(handler->select_session.search_bar, TRUE);
	gtk_widget_set_vexpand(handler->select_session.search_bar, FALSE);
	gtk_widget_set_halign(handler->select_session.search_bar, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->select_session.search_bar, GTK_ALIGN_FILL);
	gtk_search_bar_set_show_close_button(GTK_SEARCH_BAR(handler->select_session.search_bar), TRUE);
	handler->select_session.search_entry = gtk_search_entry_new();
	gtk_container_add(GTK_CONTAINER(handler->select_session.search_bar), handler->select_session.search_entry);
	gtk_widget_set_hexpand(handler->select_session.search_entry, FALSE);
	gtk_widget_set_vexpand(handler->select_session.search_entry, TRUE);
	gtk_widget_set_halign(handler->select_session.search_entry, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->select_session.search_entry, GTK_ALIGN_CENTER);
	gtk_search_bar_connect_entry(GTK_SEARCH_BAR(handler->select_session.search_bar), GTK_ENTRY(handler->select_session.search_entry));
	g_signal_connect(handler->select_session.search_entry, "stop-search", G_CALLBACK(search_entry_search_stop_search), handler);
	g_signal_connect(handler->select_session.search_entry, "search-changed", G_CALLBACK(search_entry_search_changed), handler);
	/* Scrolled window select-session */
	handler->select_session.scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(handler->select_session.box), handler->select_session.scrolled_window);
	gtk_widget_set_hexpand(handler->select_session.scrolled_window, TRUE);
	gtk_widget_set_vexpand(handler->select_session.scrolled_window, TRUE);
	gtk_widget_set_halign(handler->select_session.scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->select_session.scrolled_window, GTK_ALIGN_FILL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(handler->select_session.scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	/* List box */
	handler->select_session.list_box = gtk_list_box_new();
	gtk_widget_set_name(handler->select_session.list_box, "list_box_sessions");
	gtk_container_add(GTK_CONTAINER(handler->select_session.scrolled_window), handler->select_session.list_box);
	gtk_widget_set_hexpand(handler->select_session.list_box, TRUE);
	gtk_widget_set_vexpand(handler->select_session.list_box, TRUE);
	gtk_widget_set_halign(handler->select_session.list_box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->select_session.list_box, GTK_ALIGN_FILL);
	gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(handler->select_session.list_box), FALSE);
	gtk_list_box_set_filter_func(GTK_LIST_BOX(handler->select_session.list_box), list_box_filter, handler, NULL);
	g_signal_connect(handler->select_session.list_box, "row-activated", G_CALLBACK(list_box_sessions_row_activated), handler);
	g_signal_connect(handler->select_session.list_box, "button-press-event", G_CALLBACK(list_box_button_press_event), handler);
	g_signal_connect(handler->select_session.list_box, "popup-menu", G_CALLBACK(list_box_popup_menu), handler);
	/* List placeholder */
	label_placeholder = gtk_label_new(TEXT_SELECT_SESSION_NO_SEARCH_RESULT);
	gtk_widget_set_hexpand(label_placeholder, TRUE);
	gtk_widget_set_vexpand(label_placeholder, FALSE);
	gtk_widget_set_halign(label_placeholder, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label_placeholder, GTK_ALIGN_CENTER);
	gtk_label_set_xalign(GTK_LABEL(label_placeholder), 0.5);
	gtk_label_set_ellipsize(GTK_LABEL(label_placeholder), PANGO_ELLIPSIZE_END);
	gtk_label_set_use_markup(GTK_LABEL(label_placeholder), TRUE);
	gtk_label_set_markup(GTK_LABEL(label_placeholder), TEXT_SELECT_SESSION_NO_SEARCH_RESULT);
	gtk_widget_set_margin_top(label_placeholder, MEDIUM_SPACING);
	gtk_widget_set_margin_bottom(label_placeholder, MEDIUM_SPACING);
	gtk_widget_set_margin_start(label_placeholder, MEDIUM_SPACING);
	gtk_widget_set_margin_end(label_placeholder, MEDIUM_SPACING);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(label_placeholder)), GTK_STYLE_CLASS_DIM_LABEL);
	gtk_widget_show_all(label_placeholder);
	gtk_list_box_set_placeholder(GTK_LIST_BOX(handler->select_session.list_box), label_placeholder);
	/* Popover edit */
	handler->select_session.edit.popover = gtk_popover_new(handler->select_session.list_box);
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(handler->select_session.edit.popover), box);
	gtk_container_set_border_width(GTK_CONTAINER(box), MEDIUM_SPACING);
	/* Label session name */
	label = gtk_label_new(TEXT_SESSION_NAME);
	gtk_container_add(GTK_CONTAINER(box), label);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_vexpand(label, FALSE);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	/* Entry session name */
	handler->select_session.edit.entry = gtk_entry_new();
	gtk_widget_set_name(handler->select_session.edit.entry, "entry_session_name");
	gtk_container_add(GTK_CONTAINER(box), handler->select_session.edit.entry);
	gtk_widget_set_hexpand(handler->select_session.edit.entry, TRUE);
	gtk_widget_set_vexpand(handler->select_session.edit.entry, FALSE);
	gtk_widget_set_halign(handler->select_session.edit.entry, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->select_session.edit.entry, GTK_ALIGN_START);
	g_signal_connect(handler->select_session.edit.entry, "activate", G_CALLBACK(entry_session_name_activate), handler);
	g_signal_connect(handler->select_session.edit.entry, "changed", G_CALLBACK(entry_session_name_changed), handler);
	/* Button delete session */
	handler->select_session.edit.button_delete_session = gtk_button_new_with_label(TEXT_DELETE_SESSION);
	gtk_widget_set_name(handler->select_session.edit.button_delete_session, "button_delete_session");
	gtk_container_add(GTK_CONTAINER(box), handler->select_session.edit.button_delete_session);
	gtk_widget_set_hexpand(handler->select_session.edit.button_delete_session, TRUE);
	gtk_widget_set_vexpand(handler->select_session.edit.button_delete_session, FALSE);
	gtk_widget_set_halign(handler->select_session.edit.button_delete_session, GTK_ALIGN_END);
	gtk_widget_set_valign(handler->select_session.edit.button_delete_session, GTK_ALIGN_START);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(handler->select_session.edit.button_delete_session)), GTK_STYLE_CLASS_DESTRUCTIVE_ACTION);
	g_signal_connect(handler->select_session.edit.button_delete_session, "clicked", G_CALLBACK(button_delete_session_clicked), handler);
	gtk_widget_show_all(box);
}

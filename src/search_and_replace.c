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

static void entry_search_search_changed(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	if (view) {
		gtk_source_search_settings_set_search_text(handler->search_and_replace.source_search_settings, gtk_entry_get_text(GTK_ENTRY(widget)));
		window_search_here(handler);
	}
}

static gboolean entry_search_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	chandler *handler = user_data;
	if (event->keyval == GDK_KEY_Escape) {
		window_hide_search_bar_and_replace_bar(handler);
	}
	return FALSE;
}

static void entry_search_grab_focus(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	if (view) {
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(view->document->source_buffer), &view->document->iter_insert,
			gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(view->document->source_buffer)));
		gtk_source_search_settings_set_search_text(handler->search_and_replace.source_search_settings, gtk_entry_get_text(GTK_ENTRY(widget)));
	}
}

static void button_search_next_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	window_search_next(handler);
}

static void button_search_previous_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	window_search_previous(handler);
}

static void button_use_regex_toggled(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_source_search_settings_set_regex_enabled(handler->search_and_replace.source_search_settings, TRUE);
	} else {
		gtk_source_search_settings_set_regex_enabled(handler->search_and_replace.source_search_settings, FALSE);
	}
}

static void button_case_sensitive_toggled(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_source_search_settings_set_case_sensitive(handler->search_and_replace.source_search_settings, TRUE);
	} else {
		gtk_source_search_settings_set_case_sensitive(handler->search_and_replace.source_search_settings, FALSE);
	}
}

static void button_at_word_boundaries_toggled(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_source_search_settings_set_at_word_boundaries(handler->search_and_replace.source_search_settings, TRUE);
	} else {
		gtk_source_search_settings_set_at_word_boundaries(handler->search_and_replace.source_search_settings, FALSE);
	}
}

static void button_wrap_around_toggled(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_source_search_settings_set_wrap_around(handler->search_and_replace.source_search_settings, TRUE);
	} else {
		gtk_source_search_settings_set_wrap_around(handler->search_and_replace.source_search_settings, FALSE);
	}
}

static gboolean entry_replace_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	if (event->keyval == GDK_KEY_Escape) {
		window_hide_search_bar_and_replace_bar(handler);
		if (view) {
			gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(view->document->source_buffer),
				&view->document->iter_insert);
			gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
				&view->document->iter_insert,
				0.25,
				TRUE,
				0.5,
				0.5);
			gtk_widget_grab_focus(view->source_view);
		}
	}
	return FALSE;
}

static void button_replace_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	GtkTextIter match_start;
	GtkTextIter match_end;
	if (gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(view->document->source_buffer), &match_start, &match_end)) {
		gtk_source_search_context_replace2(view->document->source_search_context,
			&match_start,
			&match_end,
			gtk_entry_get_text(GTK_ENTRY(handler->search_and_replace.entry_replace)),
			-1,
			NULL);
		gtk_text_buffer_select_range(GTK_TEXT_BUFFER(view->document->source_buffer),
			&match_start,
			&match_end);
		gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->source_view),
			&match_start,
			0.25,
			TRUE,
			0.5,
			0.5);
		window_search_next(handler);
	}
	
}

static void button_replace_all_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	cview *view = get_current_view(handler);
	gtk_source_search_context_replace_all(view->document->source_search_context,
		gtk_entry_get_text(GTK_ENTRY(handler->search_and_replace.entry_replace)),
		-1,
		NULL);
}

void init_search_and_replace(chandler *handler)
{
	GtkWidget *box = NULL;
	GtkWidget *box_search_and_replace = NULL;
	GtkWidget *box_search = NULL;
	GtkWidget *box_buttons = NULL;
	GtkWidget *box_buttons_search_side = NULL;
	GtkWidget *box_buttons_replace_side = NULL;
	GtkWidget *separator = NULL;
	GtkWidget *icon = NULL;
	GtkSizeGroup *size_group_box_side = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	GtkSizeGroup *size_group_buttons = NULL;
	/* Revealer search and replace */
	handler->search_and_replace.revealer = gtk_revealer_new();
	gtk_container_add(GTK_CONTAINER(handler->session.box), handler->search_and_replace.revealer);
	gtk_widget_set_hexpand(handler->search_and_replace.revealer, TRUE);
	gtk_widget_set_vexpand(handler->search_and_replace.revealer, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.revealer, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.revealer, GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler->search_and_replace.revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);
	/* Box separator and search and replace */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(box, "box_separator_and_search_and_replace");
	gtk_container_add(GTK_CONTAINER(handler->search_and_replace.revealer), box);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, TRUE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_FILL);
	/* Separator */
	separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(box), separator);
	gtk_widget_set_hexpand(separator, TRUE);
	gtk_widget_set_vexpand(separator, FALSE);
	gtk_widget_set_halign(separator, GTK_ALIGN_FILL);
	gtk_widget_set_valign(separator, GTK_ALIGN_START);
	/* Box search and replace*/
	box_search_and_replace = gtk_box_new(GTK_ORIENTATION_VERTICAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box), box_search_and_replace);
	gtk_widget_set_hexpand(box_search_and_replace, TRUE);
	gtk_widget_set_vexpand(box_search_and_replace, TRUE);
	gtk_widget_set_halign(box_search_and_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_search_and_replace, GTK_ALIGN_FILL);
	gtk_container_set_border_width(GTK_CONTAINER(box_search_and_replace), MEDIUM_SPACING);
	/* Box search */
	box_search = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MINOR_SPACING);
	gtk_widget_set_name(box_search, "box_search");
	gtk_container_add(GTK_CONTAINER(box_search_and_replace), box_search);
	gtk_widget_set_hexpand(box_search, TRUE);
	gtk_widget_set_vexpand(box_search, FALSE);
	gtk_widget_set_halign(box_search, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_search, GTK_ALIGN_FILL);
	/* Entry search */
	handler->search_and_replace.entry_search = gtk_search_entry_new();
	gtk_widget_set_name(handler->search_and_replace.entry_search, "entry_search");
	gtk_container_add(GTK_CONTAINER(box_search), handler->search_and_replace.entry_search);
	gtk_widget_set_hexpand(handler->search_and_replace.entry_search, TRUE);
	gtk_widget_set_vexpand(handler->search_and_replace.entry_search, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.entry_search, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.entry_search, GTK_ALIGN_CENTER);
	g_signal_connect(handler->search_and_replace.entry_search, "key-press-event", G_CALLBACK(entry_search_key_press_event), handler);
	g_signal_connect(handler->search_and_replace.entry_search, "search-changed", G_CALLBACK(entry_search_search_changed), handler);
	g_signal_connect(handler->search_and_replace.entry_search, "grab-focus", G_CALLBACK(entry_search_grab_focus), handler);
	/* Box buttons side */
	box_buttons_search_side = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MINOR_SPACING);
	gtk_widget_set_name(box_buttons_search_side, "box_buttons_search_side");
	gtk_container_add(GTK_CONTAINER(box_search), box_buttons_search_side);
	gtk_widget_set_hexpand(box_buttons_search_side, FALSE);
	gtk_widget_set_vexpand(box_buttons_search_side, TRUE);
	gtk_widget_set_halign(box_buttons_search_side, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_buttons_search_side, GTK_ALIGN_FILL);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_box_side), box_buttons_search_side);
	/* Size group */
	size_group_buttons = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	/* Button search next */
	handler->search_and_replace.button_search_next = gtk_button_new_with_label(TEXT_NEXT);
	gtk_widget_set_name(handler->search_and_replace.button_search_next, "button_search_next");
	gtk_container_add(GTK_CONTAINER(box_buttons_search_side), handler->search_and_replace.button_search_next);
	gtk_widget_set_hexpand(handler->search_and_replace.button_search_next, FALSE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_search_next, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_search_next, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_search_next, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_search_next);
	g_signal_connect(handler->search_and_replace.button_search_next, "clicked", G_CALLBACK(button_search_next_clicked), handler);
	/* Button search previous */
	handler->search_and_replace.button_search_previous = gtk_button_new_with_label(TEXT_PREVIOUS);
	gtk_widget_set_name(handler->search_and_replace.button_search_previous, "button_search_previous");
	gtk_container_add(GTK_CONTAINER(box_buttons_search_side), handler->search_and_replace.button_search_previous);
	gtk_widget_set_hexpand(handler->search_and_replace.button_search_previous, FALSE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_search_previous, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_search_previous, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_search_previous, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_search_previous);
	g_signal_connect(handler->search_and_replace.button_search_previous, "clicked", G_CALLBACK(button_search_previous_clicked), handler);
	/* Box buttons */
	box_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(box_buttons_search_side), box_buttons);
	gtk_widget_set_hexpand(box_buttons, FALSE);
	gtk_widget_set_vexpand(box_buttons, TRUE);
	gtk_widget_set_halign(box_buttons, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_buttons, GTK_ALIGN_FILL);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(box_buttons)), GTK_STYLE_CLASS_LINKED);
	/* Size group */
	size_group_buttons = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	/* Button use regular expressions */
	handler->search_and_replace.button_use_regex = gtk_toggle_button_new();
	gtk_widget_set_name(handler->search_and_replace.button_use_regex, "button_use_regex");
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->search_and_replace.button_use_regex);
	gtk_widget_set_hexpand(handler->search_and_replace.button_use_regex, FALSE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_use_regex, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_use_regex, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_use_regex, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_use_regex);
	g_signal_connect(handler->search_and_replace.button_use_regex, "toggled", G_CALLBACK(button_use_regex_toggled), handler);
	icon = gtk_image_new_from_icon_name("format-text-underline-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(handler->search_and_replace.button_use_regex), icon);
	/* Button search at word boundaries */
	handler->search_and_replace.button_at_word_boundaries = gtk_toggle_button_new();
	gtk_widget_set_name(handler->search_and_replace.button_at_word_boundaries, "button_at_word_boundaries");
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->search_and_replace.button_at_word_boundaries);
	gtk_widget_set_hexpand(handler->search_and_replace.button_at_word_boundaries, FALSE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_at_word_boundaries, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_at_word_boundaries, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_at_word_boundaries, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_at_word_boundaries);
	g_signal_connect(handler->search_and_replace.button_at_word_boundaries, "toggled", G_CALLBACK(button_at_word_boundaries_toggled), handler);
	icon = gtk_image_new_from_icon_name("object-flip-horizontal-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(handler->search_and_replace.button_at_word_boundaries), icon);
	/* Button case sensitive */
	handler->search_and_replace.button_case_sensitive = gtk_toggle_button_new();
	gtk_widget_set_name(handler->search_and_replace.button_case_sensitive, "button_case_sensitive");
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->search_and_replace.button_case_sensitive);
	gtk_widget_set_hexpand(handler->search_and_replace.button_case_sensitive, FALSE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_case_sensitive, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_case_sensitive, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_case_sensitive, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_case_sensitive);
	g_signal_connect(handler->search_and_replace.button_case_sensitive, "toggled", G_CALLBACK(button_case_sensitive_toggled), handler);
	icon = gtk_image_new_from_icon_name("insert-text-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(handler->search_and_replace.button_case_sensitive), icon);
	/* Button wrap around */
	handler->search_and_replace.button_wrap_around = gtk_toggle_button_new();
	gtk_widget_set_name(handler->search_and_replace.button_wrap_around, "button_wrap_around");
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->search_and_replace.button_wrap_around);
	gtk_widget_set_hexpand(handler->search_and_replace.button_wrap_around, FALSE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_wrap_around, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_wrap_around, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_wrap_around, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_wrap_around);
	g_signal_connect(handler->search_and_replace.button_wrap_around, "toggled", G_CALLBACK(button_wrap_around_toggled), handler);
	icon = gtk_image_new_from_icon_name("view-wrapped-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(handler->search_and_replace.button_wrap_around), icon);
	/* Box replace */
	handler->search_and_replace.box_replace = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MINOR_SPACING);
	gtk_widget_set_name(box, "box_replace");
	gtk_container_add(GTK_CONTAINER(box_search_and_replace), handler->search_and_replace.box_replace);
	gtk_widget_set_hexpand(handler->search_and_replace.box_replace, TRUE);
	gtk_widget_set_vexpand(handler->search_and_replace.box_replace, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.box_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.box_replace, GTK_ALIGN_FILL);
	/* Entry replace */
	handler->search_and_replace.entry_replace = gtk_entry_new();
	gtk_widget_set_name(handler->search_and_replace.entry_replace, "entry_replace");
	gtk_container_add(GTK_CONTAINER(handler->search_and_replace.box_replace), handler->search_and_replace.entry_replace);
	gtk_widget_set_hexpand(handler->search_and_replace.entry_replace, TRUE);
	gtk_widget_set_vexpand(handler->search_and_replace.entry_replace, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.entry_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.entry_replace, GTK_ALIGN_CENTER);
	g_signal_connect(handler->search_and_replace.entry_replace, "key-press-event", G_CALLBACK(entry_replace_key_press_event), handler);
	/* Size group */
	size_group_buttons = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	/* Box buttons side */
	box_buttons_replace_side = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MINOR_SPACING);
	gtk_widget_set_name(box_buttons_replace_side, "box_buttons_replace_side");
	gtk_container_add(GTK_CONTAINER(handler->search_and_replace.box_replace), box_buttons_replace_side);
	gtk_widget_set_hexpand(box_buttons_replace_side, FALSE);
	gtk_widget_set_vexpand(box_buttons_replace_side, TRUE);
	gtk_widget_set_halign(box_buttons_replace_side, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_buttons_replace_side, GTK_ALIGN_FILL);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_box_side), box_buttons_replace_side);
	/* Button replace */
	handler->search_and_replace.button_replace = gtk_button_new_with_label(TEXT_REPLACE);
	gtk_widget_set_name(handler->search_and_replace.button_replace, "button_replace");
	gtk_container_add(GTK_CONTAINER(box_buttons_replace_side), handler->search_and_replace.button_replace);
	gtk_widget_set_hexpand(handler->search_and_replace.button_replace, TRUE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_replace, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_replace, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_replace, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_replace);
	g_signal_connect(handler->search_and_replace.button_replace, "clicked", G_CALLBACK(button_replace_clicked), handler);
	/* Button replace all */
	handler->search_and_replace.button_replace_all = gtk_button_new_with_label(TEXT_REPLACE_ALL);
	gtk_widget_set_name(handler->search_and_replace.button_replace_all, "button_replace_all");
	gtk_container_add(GTK_CONTAINER(box_buttons_replace_side), handler->search_and_replace.button_replace_all);
	gtk_widget_set_hexpand(handler->search_and_replace.button_replace_all, TRUE);
	gtk_widget_set_vexpand(handler->search_and_replace.button_replace_all, FALSE);
	gtk_widget_set_halign(handler->search_and_replace.button_replace_all, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->search_and_replace.button_replace_all, GTK_ALIGN_CENTER);
	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group_buttons), handler->search_and_replace.button_replace_all);
	g_signal_connect(handler->search_and_replace.button_replace_all, "clicked", G_CALLBACK(button_replace_all_clicked), handler);
	/* Search settings */
	handler->search_and_replace.source_search_settings = gtk_source_search_settings_new();
	handler->search_and_replace.source_completion_words = gtk_source_completion_words_new(NULL, NULL);
}

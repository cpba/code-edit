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

static gboolean switch_show_sidebar_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	printf("state\n");
	if (state) {
		gtk_widget_show_all(handler->sidebar.overlay);
	} else {
		gtk_widget_hide(handler->sidebar.overlay);
	}
	return FALSE;
}

static gboolean switch_show_status_bar_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	gtk_revealer_set_reveal_child(GTK_REVEALER(handler->statusbar.revealer), state);
	return FALSE;
}

static gboolean switch_show_overview_map_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	return FALSE;
}

static gboolean switch_show_line_numbers_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view->source_view), state);
		page_count--;
	}
	return FALSE;
}

static gboolean switch_show_grid_pattern_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		gtk_source_view_set_background_pattern(GTK_SOURCE_VIEW(view->source_view), state);
		page_count--;
	}
	return FALSE;
}

static gboolean switch_show_right_margin_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(view->source_view), state);
		page_count--;
	}
	return FALSE;
}

static gboolean switch_automatic_indentation_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(view->source_view), state);
		page_count--;
	}
	return FALSE;
}

static void preferences_update_show_indicators(chandler *handler)
{
	GtkWidget *page = NULL;
	cview *view = NULL;
	GtkSourceSpaceDrawer *source_space_drawer = NULL;
	GtkSourceSpaceLocationFlags locations = GTK_SOURCE_SPACE_LOCATION_NONE;
	GtkSourceSpaceTypeFlags types = GTK_SOURCE_SPACE_TYPE_NONE;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
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
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		source_space_drawer = gtk_source_view_get_space_drawer(GTK_SOURCE_VIEW(view->source_view));
		gtk_source_space_drawer_set_matrix(source_space_drawer, NULL);
		gtk_source_space_drawer_set_types_for_locations(source_space_drawer,
			locations,
			types);
		gtk_source_space_drawer_set_enable_matrix(source_space_drawer, TRUE);
		page_count--;
	}
}

static gboolean switch_show_tabs_and_spaces_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	preferences_update_show_indicators(user_data);
	return FALSE;
}

static void toggle_button_show_tabs_and_spaces_location_toggled(GtkSwitch *widget, gpointer user_data)
{
	preferences_update_show_indicators(user_data);
}

static gboolean switch_insert_spaces_instead_of_tabs_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view->source_view), state);
		page_count--;
	}
	return FALSE;
}

static gboolean switch_allow_text_wrapping_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		if (state) {
			if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_allow_split_words_over_lines))) {
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_CHAR);
			} else {
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_WORD);
			}
		} else {
			gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_NONE);
		}
		page_count--;
	}
	return FALSE;
}

static gboolean switch_allow_split_words_over_lines_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		if (gtk_switch_get_active(GTK_SWITCH(handler->preferences.switch_allow_text_wrapping))) {
			if (state) {
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_CHAR);
			} else {
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_WORD);
			}
		} else {
			gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->source_view), GTK_WRAP_NONE);
		}
		page_count--;
	}
	return FALSE;
}

static gboolean switch_highlight_current_line_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	chandler *handler = user_data;
	GtkWidget *page = NULL;
	cview *view = NULL;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(handler->session.notebook));
	while (page_count > 0) {
		page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(handler->session.notebook), page_count - 1);
		view = g_object_get_data(G_OBJECT(page), "view");
		gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view->source_view), state);
		page_count--;
	}
	return FALSE;
}

gboolean spin_button_draw_unit_characters(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(widget));
	PangoContext *pango_context = gtk_widget_create_pango_context(widget);
	PangoLayout *pango_layout = gtk_entry_get_layout(GTK_ENTRY(widget));
	GdkRectangle rectangle;
	PangoRectangle ink_rectangle;
	PangoRectangle logical_rectangle;
	GdkRGBA color;
	gint x;
	gint y;
	gtk_style_context_get_color(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(widget)), gtk_widget_get_state_flags(widget), &color);
	gtk_entry_get_layout_offsets(GTK_ENTRY(widget), &x, &y);
	gtk_entry_get_text_area(GTK_ENTRY(widget), &rectangle);
	pango_layout_set_text(pango_layout, text, -1);
	pango_layout_get_extents(pango_layout, &ink_rectangle, &logical_rectangle);
	pango_extents_to_pixels(&ink_rectangle, &logical_rectangle);
	if (text[0] != '\0') {
		if (text[0] == '1' && text[1] == '\0') {
			pango_layout_set_text(pango_layout, TEXT_UNIT_CHARACTER, -1);
		} else {
			pango_layout_set_text(pango_layout, TEXT_UNIT_CHARACTERS, -1);
		}
	} else {
		pango_layout_set_text(pango_layout, "", -1);
	}
	cairo_set_source_rgba(cr, color.red, color.green, color.blue, color.alpha * 0.5);
	cairo_save(cr);
	cairo_rectangle(cr, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
	cairo_clip(cr);
	cairo_translate(cr, x + ink_rectangle.width, y);
	pango_cairo_show_layout(cr, pango_layout);
	cairo_restore(cr);
	pango_layout_set_text(pango_layout, text, -1);
	g_object_unref(pango_context);
	return FALSE;
}

static void scrolled_window_list_box_highlight_size_allocate(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data)
{
	gtk_widget_set_size_request(widget, -1, allocation->width);
}

void init_preferences(chandler *handler)
{
	GtkWidget *scrolled_window = NULL;
	GtkWidget *box = NULL;
	GtkWidget *box_centered = NULL;
	GtkWidget *box_horizontal = NULL;
	GtkWidget *box_buttons = NULL;
	GtkWidget *frame = NULL;
	GtkWidget *label = NULL;
	GtkSizeGroup *size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	/* Scrolled window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_stack_add_named(GTK_STACK(handler->window.stack), scrolled_window, "preferences");
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	/* Box */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(scrolled_window), box);
	gtk_container_set_border_width(GTK_CONTAINER(box), MAJOR_SPACING);
	/* Box centered */
	box_centered = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(box), box_centered);
	gtk_widget_set_hexpand(box_centered, FALSE);
	gtk_widget_set_vexpand(box_centered, TRUE);
	gtk_widget_set_halign(box_centered, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box_centered, GTK_ALIGN_FILL);
	/* Show files tree */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_SIDEBAR);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_sidebar = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_sidebar);
	gtk_widget_set_hexpand(handler->preferences.switch_show_sidebar, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_sidebar, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_sidebar, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_sidebar, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_sidebar, "state-set", G_CALLBACK(switch_show_sidebar_state_set), handler);
	/* Show status bar */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_STATUS_BAR);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_status_bar = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_status_bar);
	gtk_widget_set_hexpand(handler->preferences.switch_show_status_bar, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_status_bar, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_status_bar, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_status_bar, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_status_bar, "state-set", G_CALLBACK(switch_show_status_bar_state_set), handler);
	/* Show overview map */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_OVERVIEW_MAP);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_overview_map = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_overview_map);
	gtk_widget_set_hexpand(handler->preferences.switch_show_overview_map, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_overview_map, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_overview_map, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_overview_map, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_overview_map, "state-set", G_CALLBACK(switch_show_overview_map_state_set), handler);
	/* Show line numbers */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_LINE_NUMBERS);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_line_numbers = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_line_numbers);
	gtk_widget_set_hexpand(handler->preferences.switch_show_line_numbers, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_line_numbers, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_line_numbers, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_line_numbers, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_line_numbers, "state-set", G_CALLBACK(switch_show_line_numbers_state_set), handler);
	/* Show grid pattern */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_GRID_PATTERN);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_grid_pattern = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_grid_pattern);
	gtk_widget_set_hexpand(handler->preferences.switch_show_grid_pattern, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_grid_pattern, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_grid_pattern, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_grid_pattern, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_grid_pattern, "state-set", G_CALLBACK(switch_show_grid_pattern_state_set), handler);
	/* Show right margin */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_RIGHT_MARGIN);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_right_margin = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_right_margin);
	gtk_widget_set_hexpand(handler->preferences.switch_show_right_margin, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_right_margin, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_right_margin, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_right_margin, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_right_margin, "state-set", G_CALLBACK(switch_show_right_margin_state_set), handler);
	/* Right margin position */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_RIGHT_MARGIN_POSITION);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.spin_button_right_margin_position = gtk_spin_button_new_with_range(1.0, 1000.0, 1.0);
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.spin_button_right_margin_position);
	gtk_widget_set_hexpand(handler->preferences.spin_button_right_margin_position, TRUE);
	gtk_widget_set_vexpand(handler->preferences.spin_button_right_margin_position, TRUE);
	gtk_widget_set_halign(handler->preferences.spin_button_right_margin_position, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.spin_button_right_margin_position, GTK_ALIGN_FILL);
	g_signal_connect_after(handler->preferences.spin_button_right_margin_position, "draw", G_CALLBACK(spin_button_draw_unit_characters), handler);
	/* Invisible characters */
	label = gtk_label_new(TEXT_INVISIBLE_CHARACTERS);
	gtk_container_add(GTK_CONTAINER(box_centered), label);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_vexpand(label, FALSE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(label, MAJOR_SPACING);
	gtk_label_set_xalign(GTK_LABEL(label), 0.5);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	gtk_label_set_markup(GTK_LABEL(label), TEXT_INVISIBLE_CHARACTERS);
	/* Show newline */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_NEWLINE);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_newline = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_newline);
	gtk_widget_set_hexpand(handler->preferences.switch_show_newline, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_newline, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_newline, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_newline, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_newline, "state-set", G_CALLBACK(switch_show_tabs_and_spaces_state_set), handler);
	/* Show non breaking space */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_NON_BREAKING_SPACES);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_non_breaking_space = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_non_breaking_space);
	gtk_widget_set_hexpand(handler->preferences.switch_show_non_breaking_space, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_non_breaking_space, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_non_breaking_space, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_non_breaking_space, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_non_breaking_space, "state-set", G_CALLBACK(switch_show_tabs_and_spaces_state_set), handler);
	/* Show tabs */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_TABS);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_tabs = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_tabs);
	gtk_widget_set_hexpand(handler->preferences.switch_show_tabs, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_tabs, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_tabs, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_tabs, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_tabs, "state-set", G_CALLBACK(switch_show_tabs_and_spaces_state_set), handler);
	/* Show spaces */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_SHOW_SPACES);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_show_spaces = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_spaces);
	gtk_widget_set_hexpand(handler->preferences.switch_show_spaces, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_spaces, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_spaces, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_spaces, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_show_spaces, "state-set", G_CALLBACK(switch_show_tabs_and_spaces_state_set), handler);
	/* Tabs and spaces to show */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_TABS_AND_SPACES_TO_SHOW);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	box_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(box_horizontal), box_buttons);
	gtk_widget_set_hexpand(box_buttons, TRUE);
	gtk_widget_set_vexpand(box_buttons, TRUE);
	gtk_widget_set_halign(box_buttons, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_buttons, GTK_ALIGN_FILL);
	handler->preferences.toggle_button_show_leading_tabs_and_spaces = gtk_toggle_button_new_with_label(TEXT_LEADING);
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->preferences.toggle_button_show_leading_tabs_and_spaces);
	gtk_widget_set_hexpand(handler->preferences.toggle_button_show_leading_tabs_and_spaces, TRUE);
	gtk_widget_set_vexpand(handler->preferences.toggle_button_show_leading_tabs_and_spaces, TRUE);
	gtk_widget_set_halign(handler->preferences.toggle_button_show_leading_tabs_and_spaces, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.toggle_button_show_leading_tabs_and_spaces, GTK_ALIGN_CENTER);
	g_signal_connect(handler->preferences.toggle_button_show_leading_tabs_and_spaces, "toggled", G_CALLBACK(toggle_button_show_tabs_and_spaces_location_toggled), handler);
	handler->preferences.toggle_button_show_inside_text_tabs_and_spaces = gtk_toggle_button_new_with_label(TEXT_INSIDE_TEXT);
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->preferences.toggle_button_show_inside_text_tabs_and_spaces);
	gtk_widget_set_hexpand(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, TRUE);
	gtk_widget_set_vexpand(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, TRUE);
	gtk_widget_set_halign(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, GTK_ALIGN_CENTER);
	g_signal_connect(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, "toggled", G_CALLBACK(toggle_button_show_tabs_and_spaces_location_toggled), handler);
	handler->preferences.toggle_button_show_trailing_tabs_and_spaces = gtk_toggle_button_new_with_label(TEXT_TRAILING);
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->preferences.toggle_button_show_trailing_tabs_and_spaces);
	gtk_widget_set_hexpand(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, TRUE);
	gtk_widget_set_vexpand(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, TRUE);
	gtk_widget_set_halign(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, GTK_ALIGN_CENTER);
	g_signal_connect(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, "toggled", G_CALLBACK(toggle_button_show_tabs_and_spaces_location_toggled), handler);
	/* Text */
	label = gtk_label_new(TEXT_TEXT);
	gtk_container_add(GTK_CONTAINER(box_centered), label);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_vexpand(label, FALSE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(label, MAJOR_SPACING);
	gtk_label_set_xalign(GTK_LABEL(label), 0.5);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	gtk_label_set_markup(GTK_LABEL(label), TEXT_TEXT);
	/* Automatic indentation */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_AUTOMATIC_INDENTATION);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_automatic_indentation = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_automatic_indentation);
	gtk_widget_set_hexpand(handler->preferences.switch_automatic_indentation, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_automatic_indentation, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_automatic_indentation, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_automatic_indentation, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_automatic_indentation, "state-set", G_CALLBACK(switch_automatic_indentation_state_set), handler);
	/* Insert spaces instead of tabs */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_INSERT_SPACES_INSTEAD_OF_TABS);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_insert_spaces_instead_of_tabs = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_insert_spaces_instead_of_tabs);
	gtk_widget_set_hexpand(handler->preferences.switch_insert_spaces_instead_of_tabs, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_insert_spaces_instead_of_tabs, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_insert_spaces_instead_of_tabs, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_insert_spaces_instead_of_tabs, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_insert_spaces_instead_of_tabs, "state-set", G_CALLBACK(switch_insert_spaces_instead_of_tabs_state_set), handler);
	/* Tab width */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_TAB_WIDTH);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.spin_button_tab_width = gtk_spin_button_new_with_range(1.0, 24.0, 1.0);
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.spin_button_tab_width);
	gtk_widget_set_hexpand(handler->preferences.spin_button_tab_width, TRUE);
	gtk_widget_set_vexpand(handler->preferences.spin_button_tab_width, TRUE);
	gtk_widget_set_halign(handler->preferences.spin_button_tab_width, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.spin_button_tab_width, GTK_ALIGN_FILL);
	g_signal_connect_after(handler->preferences.spin_button_tab_width, "draw", G_CALLBACK(spin_button_draw_unit_characters), handler);
	/* Allow text wrapping */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_ALLOW_TEXT_WRAPPING);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_allow_text_wrapping = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_allow_text_wrapping);
	gtk_widget_set_hexpand(handler->preferences.switch_allow_text_wrapping, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_allow_text_wrapping, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_allow_text_wrapping, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_allow_text_wrapping, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_allow_text_wrapping, "state-set", G_CALLBACK(switch_allow_text_wrapping_state_set), handler);
	/* Allow split words over lines */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_ALLOW_SPLIT_WORDS_OVER_LINES);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_allow_split_words_over_lines = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_allow_split_words_over_lines);
	gtk_widget_set_hexpand(handler->preferences.switch_allow_split_words_over_lines, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_allow_split_words_over_lines, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_allow_split_words_over_lines, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_allow_split_words_over_lines, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_allow_split_words_over_lines, "state-set", G_CALLBACK(switch_allow_split_words_over_lines_state_set), handler);
	/* Theming */
	label = gtk_label_new(TEXT_THEMING);
	gtk_container_add(GTK_CONTAINER(box_centered), label);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_vexpand(label, FALSE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(label, MAJOR_SPACING);
	gtk_label_set_xalign(GTK_LABEL(label), 0.5);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	gtk_label_set_markup(GTK_LABEL(label), TEXT_THEMING);
	/* Highlight current line */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_HIGHLIGHT_CURRENT_LINE);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_highlight_current_line = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_highlight_current_line);
	gtk_widget_set_hexpand(handler->preferences.switch_highlight_current_line, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_highlight_current_line, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_highlight_current_line, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_highlight_current_line, GTK_ALIGN_FILL);
	g_signal_connect(handler->preferences.switch_highlight_current_line, "state-set", G_CALLBACK(switch_highlight_current_line_state_set), handler);
	/* Highlight matching brackets */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_HIGHLIGHT_MATCHING_BRACKETS);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_highlight_matching_brackets = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_highlight_matching_brackets);
	gtk_widget_set_hexpand(handler->preferences.switch_highlight_matching_brackets, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_highlight_matching_brackets, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_highlight_matching_brackets, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_highlight_matching_brackets, GTK_ALIGN_FILL);
	/* Font */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_FONT);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.font_button = gtk_font_button_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.font_button);
	gtk_widget_set_hexpand(handler->preferences.font_button, TRUE);
	gtk_widget_set_vexpand(handler->preferences.font_button, TRUE);
	gtk_widget_set_halign(handler->preferences.font_button, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.font_button, GTK_ALIGN_FILL);
	/* Highlight */
	label = gtk_label_new(TEXT_HIGHLIGHT);
	gtk_container_add(GTK_CONTAINER(box_centered), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_widget_set_margin_top(label, MEDIUM_SPACING);
	gtk_size_group_add_widget(size_group, label);
	frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(box_centered), frame);
	gtk_widget_set_hexpand(frame, TRUE);
	gtk_widget_set_vexpand(frame, FALSE);
	gtk_widget_set_halign(frame, GTK_ALIGN_FILL);
	gtk_widget_set_valign(frame, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_top(frame, MINOR_SPACING);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(frame), scrolled_window);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	handler->preferences.list_box_highlight = gtk_list_box_new();
	gtk_widget_set_name(handler->preferences.list_box_highlight, "list_box_sessions");
	gtk_container_add(GTK_CONTAINER(scrolled_window), handler->preferences.list_box_highlight);
	gtk_widget_set_hexpand(handler->preferences.list_box_highlight, TRUE);
	gtk_widget_set_vexpand(handler->preferences.list_box_highlight, TRUE);
	gtk_widget_set_halign(handler->preferences.list_box_highlight, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.list_box_highlight, GTK_ALIGN_FILL);
	gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(handler->preferences.list_box_highlight), FALSE);
	g_signal_connect(scrolled_window, "size-allocate", G_CALLBACK(scrolled_window_list_box_highlight_size_allocate), handler);
}

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

void preferences_default(chandler *handler)
{
	GtkSourceStyleSchemeManager *style_scheme_manager = gtk_source_style_scheme_manager_get_default();
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_sidebar), FALSE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_statusbar), TRUE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_overview_map), FALSE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_line_numbers), TRUE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_grid_pattern), FALSE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_right_margin), TRUE);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(handler->preferences.spin_button_right_margin_position), 80);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_automatic_indentation), FALSE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_insert_spaces_instead_of_tabs), FALSE);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(handler->preferences.spin_button_tab_width), 8);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_allow_text_wrapping), FALSE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_allow_split_words_over_lines), FALSE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_highlight_current_line), TRUE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_highlight_matching_brackets), TRUE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_use_monospace_font), TRUE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_non_breaking_space), TRUE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_newline), FALSE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_spaces), TRUE);
	gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_tabs), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_leading_tabs_and_spaces), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_trailing_tabs_and_spaces), TRUE);
	gtk_source_style_scheme_chooser_set_style_scheme(GTK_SOURCE_STYLE_SCHEME_CHOOSER(handler->preferences.style_scheme_chooser),
		gtk_source_style_scheme_manager_get_scheme(style_scheme_manager, "classic"));
}

void preferences_save(chandler *handler)
{
	g_key_file_set_boolean(handler->key_file, "preferences", "show_sidebar",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_sidebar)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_statusbar",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_statusbar)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_line_numbers",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_line_numbers)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_grid_pattern",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_grid_pattern)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_right_margin",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_right_margin)));
	g_key_file_set_integer(handler->key_file, "preferences", "right_margin_position",
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(handler->preferences.spin_button_right_margin_position)));
	g_key_file_set_boolean(handler->key_file, "preferences", "automatic_indentation",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_automatic_indentation)));
	g_key_file_set_boolean(handler->key_file, "preferences", "insert_spaces_instead_of_tabs",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_insert_spaces_instead_of_tabs)));
	g_key_file_set_integer(handler->key_file, "preferences", "insert_spaces_instead_of_tabs",
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(handler->preferences.spin_button_tab_width)));
	g_key_file_set_boolean(handler->key_file, "preferences", "allow_text_wrapping",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_allow_text_wrapping)));
	g_key_file_set_boolean(handler->key_file, "preferences", "allow_split_words_over_lines",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_allow_split_words_over_lines)));
	g_key_file_set_boolean(handler->key_file, "preferences", "highlight_current_line",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_highlight_current_line)));
	g_key_file_set_boolean(handler->key_file, "preferences", "highlight_matching_brackets",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_highlight_matching_brackets)));
	g_key_file_set_boolean(handler->key_file, "preferences", "use_monospace_font",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_use_monospace_font)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_non_breaking_space",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_non_breaking_space)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_newline",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_newline)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_spaces",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_spaces)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_tabs",
		gtk_switch_get_state(GTK_SWITCH(handler->preferences.switch_show_tabs)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_leading_tabs_and_spaces",
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_leading_tabs_and_spaces)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_inside_text_tabs_and_spaces",
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces)));
	g_key_file_set_boolean(handler->key_file, "preferences", "show_inside_text_tabs_and_spaces",
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces)));
	g_key_file_set_string(handler->key_file, "preferences", "style_scheme",
		gtk_source_style_scheme_get_id(GTK_SOURCE_STYLE_SCHEME(gtk_source_style_scheme_chooser_get_style_scheme(GTK_SOURCE_STYLE_SCHEME_CHOOSER(handler->preferences.style_scheme_chooser)))));
}

void preferences_load(chandler *handler)
{
	gchar *value = NULL;
	GtkSourceStyleSchemeManager *style_scheme_manager = gtk_source_style_scheme_manager_get_default();
	/* Show sidebar */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_sidebar", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_sidebar),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_sidebar", NULL));
	}
	/* Show statusbar */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_statusbar", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_statusbar),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_statusbar", NULL));
	}
	/* Show overview map */
	/* TODO */
	/* Show line numbers */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_line_numbers", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_line_numbers),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_line_numbers", NULL));
	}
	/* Show grid pattern */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_grid_pattern", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_grid_pattern),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_grid_pattern", NULL));
	}
	/* Show right margin */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_right_margin", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_right_margin),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_right_margin", NULL));
	}
	/* Right margin position */
	if (g_key_file_has_key(handler->key_file, "preferences", "right_margin_position", NULL)) {
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(handler->preferences.spin_button_right_margin_position),
			g_key_file_get_integer(handler->key_file, "preferences", "right_margin_position", NULL));
	}
	/* Automatic indentation */
	if (g_key_file_has_key(handler->key_file, "preferences", "automatic_indentation", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_automatic_indentation),
			g_key_file_get_boolean(handler->key_file, "preferences", "automatic_indentation", NULL));
	}
	/* Insert spaces instead of tabs */
	if (g_key_file_has_key(handler->key_file, "preferences", "automatic_indentation", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_insert_spaces_instead_of_tabs),
			g_key_file_get_boolean(handler->key_file, "preferences", "insert_spaces_instead_of_tabs", NULL));
	}
	/* Allow text wrapping */
	if (g_key_file_has_key(handler->key_file, "preferences", "allow_text_wrapping", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_allow_text_wrapping),
			g_key_file_get_boolean(handler->key_file, "preferences", "allow_text_wrapping", NULL));
	}
	/* Allow split words over lines */
	if (g_key_file_has_key(handler->key_file, "preferences", "allow_split_words_over_lines", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_allow_split_words_over_lines),
			g_key_file_get_boolean(handler->key_file, "preferences", "allow_split_words_over_lines", NULL));
	}
	/* Highlight current line */
	if (g_key_file_has_key(handler->key_file, "preferences", "highlight_current_line", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_highlight_current_line),
			g_key_file_get_boolean(handler->key_file, "preferences", "highlight_current_line", NULL));
	}
	/* Highlight matching brackets */
	if (g_key_file_has_key(handler->key_file, "preferences", "highlight_matching_brackets", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_highlight_matching_brackets),
			g_key_file_get_boolean(handler->key_file, "preferences", "highlight_matching_brackets", NULL));
	}
	/* Use monospace font */
	if (g_key_file_has_key(handler->key_file, "preferences", "use_monospace_font", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_use_monospace_font),
			g_key_file_get_boolean(handler->key_file, "preferences", "use_monospace_font", NULL));
	}
	/* Show non breaking space */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_non_breaking_space", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_non_breaking_space),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_non_breaking_space", NULL));
	}
	/* Show newline */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_newline", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_newline),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_newline", NULL));
	}
	/* Show spaces */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_spaces", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_spaces),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_spaces", NULL));
	}
	/* Show tabs */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_tabs", NULL)) {
		gtk_switch_set_state(GTK_SWITCH(handler->preferences.switch_show_tabs),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_tabs", NULL));
	}
	/* Show leading tabs and spaces */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_leading_tabs_and_spaces", NULL)) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_leading_tabs_and_spaces),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_leading_tabs_and_spaces", NULL));
	}
	/* Show inside text tabs and spaces */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_inside_text_tabs_and_spaces", NULL)) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_inside_text_tabs_and_spaces", NULL));
	}
	/* Show trailing tabs and space */
	if (g_key_file_has_key(handler->key_file, "preferences", "show_trailing_tabs_and_spaces", NULL)) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(handler->preferences.toggle_button_show_trailing_tabs_and_spaces),
			g_key_file_get_boolean(handler->key_file, "preferences", "show_trailing_tabs_and_spaces", NULL));
	}
	/* Style scheme */
	if (g_key_file_has_key(handler->key_file, "preferences", "style_scheme", NULL)) {
		value = g_key_file_get_string(handler->key_file, "preferences", "style_scheme", NULL);
		gtk_source_style_scheme_chooser_set_style_scheme(GTK_SOURCE_STYLE_SCHEME_CHOOSER(handler->preferences.style_scheme_chooser),
			gtk_source_style_scheme_manager_get_scheme(style_scheme_manager, value));
		g_free(value);
	}
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
	handler->preferences.switch_show_statusbar = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_show_statusbar);
	gtk_widget_set_hexpand(handler->preferences.switch_show_statusbar, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_show_statusbar, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_show_statusbar, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_show_statusbar, GTK_ALIGN_FILL);
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
	handler->preferences.toggle_button_show_inside_text_tabs_and_spaces = gtk_toggle_button_new_with_label(TEXT_INSIDE_TEXT);
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->preferences.toggle_button_show_inside_text_tabs_and_spaces);
	gtk_widget_set_hexpand(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, TRUE);
	gtk_widget_set_vexpand(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, TRUE);
	gtk_widget_set_halign(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.toggle_button_show_inside_text_tabs_and_spaces, GTK_ALIGN_CENTER);
	handler->preferences.toggle_button_show_trailing_tabs_and_spaces = gtk_toggle_button_new_with_label(TEXT_TRAILING);
	gtk_container_add(GTK_CONTAINER(box_buttons), handler->preferences.toggle_button_show_trailing_tabs_and_spaces);
	gtk_widget_set_hexpand(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, TRUE);
	gtk_widget_set_vexpand(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, TRUE);
	gtk_widget_set_halign(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.toggle_button_show_trailing_tabs_and_spaces, GTK_ALIGN_CENTER);
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
	/* Use monospace font */
	box_horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(box_centered), box_horizontal);
	gtk_widget_set_hexpand(box_horizontal, FALSE);
	gtk_widget_set_vexpand(box_horizontal, FALSE);
	gtk_widget_set_halign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box_horizontal, GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(box_horizontal, MEDIUM_SPACING);
	gtk_box_set_homogeneous(GTK_BOX(box_horizontal), TRUE);
	label = gtk_label_new(TEXT_USE_MONOSPACE_FONT);
	gtk_container_add(GTK_CONTAINER(box_horizontal), label);
	gtk_widget_set_hexpand(label, FALSE);
	gtk_widget_set_vexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_widget_set_valign(label, GTK_ALIGN_FILL);
	gtk_label_set_xalign(GTK_LABEL(label), 1.0);
	gtk_size_group_add_widget(size_group, label);
	handler->preferences.switch_use_monospace_font = gtk_switch_new();
	gtk_container_add(GTK_CONTAINER(box_horizontal), handler->preferences.switch_use_monospace_font);
	gtk_widget_set_hexpand(handler->preferences.switch_use_monospace_font, FALSE);
	gtk_widget_set_vexpand(handler->preferences.switch_use_monospace_font, TRUE);
	gtk_widget_set_halign(handler->preferences.switch_use_monospace_font, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->preferences.switch_use_monospace_font, GTK_ALIGN_FILL);
	/* Style scheme */
	label = gtk_label_new(TEXT_STYLE_SCHEME);
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
	handler->preferences.style_scheme_chooser = gtk_source_style_scheme_chooser_widget_new();
	gtk_widget_set_name(handler->preferences.style_scheme_chooser, "style_scheme_chooser");
	gtk_container_add(GTK_CONTAINER(scrolled_window), handler->preferences.style_scheme_chooser);
	gtk_widget_set_hexpand(handler->preferences.style_scheme_chooser, TRUE);
	gtk_widget_set_vexpand(handler->preferences.style_scheme_chooser, TRUE);
	gtk_widget_set_halign(handler->preferences.style_scheme_chooser, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->preferences.style_scheme_chooser, GTK_ALIGN_FILL);
	g_signal_connect(scrolled_window, "size-allocate", G_CALLBACK(scrolled_window_list_box_highlight_size_allocate), handler);
}

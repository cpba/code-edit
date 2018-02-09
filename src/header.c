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

void update_headerbar(chandler *handler, cview *view)
{
	if (!view) {
		view = get_current_view(handler);
	}
	if (view) {
		GFile *file = gtk_source_file_get_location(view->document->source_file);
		if (file) {
			gchar *path = g_file_get_path(file);
			gchar *basename = g_path_get_basename(path);
			gchar *dirname = g_path_get_dirname(path);
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->header.header_bar), basename);
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->header.header_bar), dirname);
			g_free(basename);
			g_free(dirname);
			g_free(path);
		} else {
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->header.header_bar), TEXT_UNTITLED);
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->header.header_bar), NULL);
		}
		gtk_widget_show(handler->header.button_save_document);
		gtk_widget_show(handler->header.button_save_as_document);
	} else {
		gtk_header_bar_set_title(GTK_HEADER_BAR(handler->header.header_bar), PROGRAM_NAME);
		gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->header.header_bar), NULL);
		gtk_widget_hide(handler->header.button_save_document);
		gtk_widget_hide(handler->header.button_save_as_document);
	}
}

static void button_new_document_clicked(GtkWidget *widget, gpointer user_data)
{
	window_new(user_data);
}

static void button_open_menu_clicked(GtkWidget *widget, gpointer user_data)
{
	window_go_to_select_session(user_data);
}

static void button_open_document_clicked(GtkWidget *widget, gpointer user_data)
{
	window_open(user_data);
}

static void button_save_as_document_clicked(GtkWidget *widget, gpointer user_data)
{
	window_save_as(user_data);
}

static void button_save_document_clicked(GtkWidget *widget, gpointer user_data)
{
	window_save(user_data);
}

static void button_preferences_toggled(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_stack_set_visible_child_name(GTK_STACK(handler->window.stack), "preferences");
	} else {
		gtk_stack_set_visible_child_name(GTK_STACK(handler->window.stack), "session");
	}
}

static void button_session_selection_mode_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(handler->select_session.list_box), TRUE);
}

void init_header(chandler *handler)
{
	GtkWidget *box = NULL;
	GtkWidget *icon = NULL;
	/* Header bar */
	handler->header.header_bar = gtk_header_bar_new();
	gtk_window_set_titlebar(GTK_WINDOW(handler->window.window), handler->header.header_bar);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(handler->header.header_bar), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(handler->header.header_bar), PROGRAM_NAME);
	/* Stack session */
	handler->header.revealer_session = gtk_revealer_new();
	gtk_widget_set_name(handler->header.revealer_session, "header_revealer_session");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler->header.header_bar), handler->header.revealer_session);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler->header.revealer_session), GTK_STACK_TRANSITION_TYPE_NONE);
	/* Box session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(handler->header.revealer_session), box);
	/* Button session */
	handler->header.button_open_menu = gtk_button_new_from_icon_name("open-menu-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->header.button_open_menu, "button_open_menu");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_open_menu);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_open_menu), "circular");
	g_signal_connect(handler->header.button_open_menu, "clicked", G_CALLBACK(button_open_menu_clicked), handler);
	/* Button new document */
	handler->header.button_new_document = gtk_button_new_from_icon_name("tab-new-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->header.button_new_document, "button_new_document");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_new_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_new_document), "circular");
	g_signal_connect(handler->header.button_new_document, "clicked", G_CALLBACK(button_new_document_clicked), handler);
	/* Button open document */
	handler->header.button_open_document = gtk_button_new_from_icon_name("document-open-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->header.button_open_document, "button_open_document");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_open_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_open_document), "circular");
	g_signal_connect(handler->header.button_open_document, "clicked", G_CALLBACK(button_open_document_clicked), handler);
	/* Button save document */
	handler->header.button_save_document = gtk_button_new_from_icon_name("document-save-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->header.button_save_document, "button_save_document");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_save_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_save_document), "circular");
	g_signal_connect(handler->header.button_save_document, "clicked", G_CALLBACK(button_save_document_clicked), handler);
	/* Button save as document */
	handler->header.button_save_as_document = gtk_button_new_from_icon_name("document-save-as-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->header.button_save_as_document, "button_save_as_document");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_save_as_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_save_as_document), "circular");
	g_signal_connect(handler->header.button_save_as_document, "clicked", G_CALLBACK(button_save_as_document_clicked), handler);
	/* Stack extra */
	handler->header.stack_extra = gtk_stack_new();
	gtk_widget_set_name(handler->header.stack_extra, "header_stack_extra");
	gtk_header_bar_pack_end(GTK_HEADER_BAR(handler->header.header_bar), handler->header.stack_extra);
	gtk_stack_set_transition_type(GTK_STACK(handler->header.stack_extra), GTK_STACK_TRANSITION_TYPE_NONE);
	gtk_stack_set_hhomogeneous(GTK_STACK(handler->header.stack_extra), FALSE);
	/* Box select-session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_stack_add_named(GTK_STACK(handler->header.stack_extra), box, "select-session");
	/* Button selection-mode */
	handler->header.button_session_selection_mode = gtk_button_new_from_icon_name("object-select-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->header.button_session_selection_mode, "button_session_selection_mode");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_session_selection_mode);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_session_selection_mode), "circular");
	g_signal_connect(handler->header.button_session_selection_mode, "clicked", G_CALLBACK(button_session_selection_mode_clicked), handler);
	/* Box session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_stack_add_named(GTK_STACK(handler->header.stack_extra), box, "session");
	/* Button preferences */
	handler->header.button_preferences = gtk_toggle_button_new();
	gtk_widget_set_name(handler->header.button_preferences, "button_preferences");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_preferences);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_preferences), "circular");
	g_signal_connect(handler->header.button_preferences, "clicked", G_CALLBACK(button_preferences_toggled), handler);
	icon = gtk_image_new_from_icon_name("preferences-system-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(handler->header.button_preferences), icon);
}

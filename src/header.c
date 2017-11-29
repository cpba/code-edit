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

void update_headerbar(chandler *handler, cview *view)
{
	if (!view) {
		view = get_current_view(handler);
	}
	if (view) {
		GFile *file = gtk_source_file_get_location(view->document->source_file);
		if (file) {
			GString *subtitle = NULL;
			GFile *home = NULL;
			GFile *parent = g_file_get_parent(file);
			gchar *basename = g_file_get_basename(file);
			gchar *path = NULL;
			const gchar *home_dir = g_get_home_dir();
			home = g_file_new_for_path(home_dir);
			if (home && parent) {
				path = g_file_get_relative_path(home, parent);
			}
			if (path) {
				subtitle = g_string_new("~/");
				subtitle = g_string_append(subtitle, path);
			} else {
				path = g_file_get_path(parent);
			}
			if (!subtitle && path) {
				subtitle = g_string_new(path);
			} else {
				path = g_file_get_path(file);
			}
			if (!subtitle && path) {
				subtitle = g_string_new(path);
			}
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), basename);
			if (subtitle) {
				gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), subtitle->str);
			} else {
				gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), NULL);
			}
			g_free(basename);
			g_free(path);
			if (subtitle) {
				g_string_free(subtitle, TRUE);
			}
			if (parent) {
				g_object_unref(G_OBJECT(parent));
			}
			if (home) {
				g_object_unref(G_OBJECT(home));
			}
		} else {
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), "Untitled");
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), NULL);
		}
		gtk_widget_show(handler->handler_header.button_save_document);
		gtk_widget_show(handler->handler_header.button_save_as_document);
	} else {
		gtk_header_bar_set_title(GTK_HEADER_BAR(handler->handler_header.header_bar), PROGRAM_NAME);
		gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->handler_header.header_bar), NULL);
		gtk_widget_hide(handler->handler_header.button_save_document);
		gtk_widget_hide(handler->handler_header.button_save_as_document);
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
	/* Show preferences frame */
}

void init_header(chandler *handler)
{
	chandler_window *handler_window = &handler->handler_window;
	chandler_header *handler_header = &handler->handler_header;
	GtkWidget *box = NULL;
	/* Header bar */
	handler_header->header_bar = gtk_header_bar_new();
	gtk_window_set_titlebar(GTK_WINDOW(handler_window->window), handler_header->header_bar);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(handler_header->header_bar), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(handler_header->header_bar), PROGRAM_NAME);
	/* Stack session */
	handler_header->revealer_session = gtk_revealer_new();
	gtk_widget_set_name(handler_header->revealer_session, "header_revealer_session");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler_header->header_bar), handler_header->revealer_session);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler_header->revealer_session), GTK_STACK_TRANSITION_TYPE_NONE);
	/* Box session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_container_add(GTK_CONTAINER(handler_header->revealer_session), box);
	/* Button session */
	handler_header->button_open_menu = gtk_button_new_from_icon_name("open-menu-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler_header->button_open_menu, "button_open_menu");
	gtk_container_add(GTK_CONTAINER(box), handler_header->button_open_menu);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_open_menu), "circular");
	g_signal_connect(handler_header->button_open_menu, "clicked", G_CALLBACK(button_open_menu_clicked), handler);
	/* Button new document */
	handler_header->button_new_document = gtk_button_new_from_icon_name("tab-new-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler_header->button_new_document, "button_new_document");
	gtk_container_add(GTK_CONTAINER(box), handler_header->button_new_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_new_document), "circular");
	g_signal_connect(handler_header->button_new_document, "clicked", G_CALLBACK(button_new_document_clicked), handler);
	/* Button open document */
	handler_header->button_open_document = gtk_button_new_from_icon_name("document-open-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler_header->button_open_document, "button_open_document");
	gtk_container_add(GTK_CONTAINER(box), handler_header->button_open_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_open_document), "circular");
	g_signal_connect(handler_header->button_open_document, "clicked", G_CALLBACK(button_open_document_clicked), handler);
	/* Button save document */
	handler_header->button_save_document = gtk_button_new_from_icon_name("document-save-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler_header->button_save_document, "button_save_document");
	gtk_container_add(GTK_CONTAINER(box), handler_header->button_save_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_save_document), "circular");
	g_signal_connect(handler_header->button_save_document, "clicked", G_CALLBACK(button_save_document_clicked), handler);
	/* Button save as document */
	handler_header->button_save_as_document = gtk_button_new_from_icon_name("document-save-as-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler_header->button_save_as_document, "button_save_as_document");
	gtk_container_add(GTK_CONTAINER(box), handler_header->button_save_as_document);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_save_as_document), "circular");
	g_signal_connect(handler_header->button_save_as_document, "clicked", G_CALLBACK(button_save_as_document_clicked), handler);
	/* Stack extra */
	handler_header->stack_extra = gtk_stack_new();
	gtk_widget_set_name(handler_header->stack_extra, "header_stack_extra");
	gtk_header_bar_pack_end(GTK_HEADER_BAR(handler_header->header_bar), handler_header->stack_extra);
	gtk_stack_set_transition_type(GTK_STACK(handler_header->stack_extra), GTK_STACK_TRANSITION_TYPE_NONE);
	gtk_stack_set_hhomogeneous(GTK_STACK(handler_header->stack_extra), FALSE);
	/* Box select-session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_stack_add_named(GTK_STACK(handler_header->stack_extra), box, "select-session");
	/* Box session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_stack_add_named(GTK_STACK(handler_header->stack_extra), box, "session");
	/* Button preferences */
	handler_header->button_preferences = gtk_button_new_from_icon_name("preferences-system-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler_header->button_preferences, "button_preferences");
	gtk_container_add(GTK_CONTAINER(box), handler_header->button_preferences);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_header->button_preferences), "circular");
	g_signal_connect(handler_header->button_preferences, "clicked", G_CALLBACK(button_preferences_toggled), handler);
	
}

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

void headerbar_update(chandler *handler, cview *view)
{
	GFile *file = NULL;
	gchar *path = NULL;
	gchar *basename = NULL;
	gchar *dirname = NULL;
	gboolean untitled = FALSE;
	if (!view) {
		view = get_current_view(handler);
	}
	if (view) {
		file = gtk_source_file_get_location(view->document->source_file);
		if (file) {
			path = g_file_get_path(file);
			basename = g_path_get_basename(path);
			dirname = g_path_get_dirname(path);
		}
		if (basename && dirname) {
			if (g_strcmp0(dirname, g_get_tmp_dir()) == 0) {
				untitled = TRUE;
			}
		} else {
			untitled = TRUE;
		}
		if (!untitled) {
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->header.header_bar), basename);
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->header.header_bar), dirname);
		} else {
			gtk_header_bar_set_title(GTK_HEADER_BAR(handler->header.header_bar), TEXT_UNTITLED);
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(handler->header.header_bar), NULL);
		}
		if (basename) {
			g_free(basename);
		}
		if (dirname) {
			g_free(dirname);
		}
		if (path) {
			g_free(path);
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

static void button_select_session_clicked(GtkWidget *widget, gpointer user_data)
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

static void button_add_session_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gchar *id = select_session_new_id(handler);
	csession *session = select_session_new_session(handler, TEXT_UNTITLED_SESSION_NAME, id, 0);
	GtkWidget *row = gtk_widget_get_parent(session->box);
	GtkAllocation allocation;
	gtk_container_check_resize(GTK_CONTAINER(handler->select_session.list_box));
	gtk_list_box_select_row(GTK_LIST_BOX(handler->select_session.list_box), GTK_LIST_BOX_ROW(row));
	gtk_entry_set_text(GTK_ENTRY(handler->select_session.edit.entry), session->name->str);
	gtk_widget_get_allocation(GTK_WIDGET(row), &allocation);
	gtk_popover_set_pointing_to(GTK_POPOVER(handler->select_session.edit.popover), &allocation);
	gtk_popover_popup(GTK_POPOVER(handler->select_session.edit.popover));
	g_free(id);
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
	handler->header.stack_session = gtk_stack_new();
	gtk_widget_set_name(handler->header.stack_session, "header_stack_session");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(handler->header.header_bar), handler->header.stack_session);
	gtk_stack_set_transition_type(GTK_STACK(handler->header.stack_session), GTK_STACK_TRANSITION_TYPE_NONE);
	/* Box session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_stack_add_named(GTK_STACK(handler->header.stack_session), box, "select-session");
	/* Button add session */
	handler->header.button_add_session = gtk_button_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_add_session);
	gtk_widget_set_hexpand(handler->header.button_add_session, TRUE);
	gtk_widget_set_vexpand(handler->header.button_add_session, FALSE);
	gtk_widget_set_halign(handler->header.button_add_session, GTK_ALIGN_START);
	gtk_widget_set_valign(handler->header.button_add_session, GTK_ALIGN_CENTER);
	g_signal_connect(handler->header.button_add_session, "clicked", G_CALLBACK(button_add_session_clicked), handler);
	/* Box session */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MEDIUM_SPACING);
	gtk_stack_add_named(GTK_STACK(handler->header.stack_session), box, "session");
	/* Button select session */
	handler->header.button_select_session = gtk_button_new_from_icon_name("go-previous-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->header.button_select_session, "button_select_session");
	gtk_container_add(GTK_CONTAINER(box), handler->header.button_select_session);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->header.button_select_session), "circular");
	g_signal_connect(handler->header.button_select_session, "clicked", G_CALLBACK(button_select_session_clicked), handler);
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

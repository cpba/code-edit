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

void sidebar_update_iter_with_dummy_child(chandler *handler, GtkTreeIter iter)
{
	gchar *path = NULL;
	GtkTreeIter child;
	gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
		&iter,
		2, &path,
		-1);
	while (gtk_tree_model_iter_children(GTK_TREE_MODEL(handler->sidebar.tree_store), &child, &iter)) {
		gtk_tree_store_remove(handler->sidebar.tree_store, &child);
	}
	/* Add dummy */
	gtk_tree_store_append(handler->sidebar.tree_store,
		&child,
		&iter);
}

void sidebar_update_iter_children(chandler *handler, GtkTreeIter iter)
{
	GtkTreeIter child;
	gchar *path = NULL;
	GDir *dir = NULL;
	const gchar *name = NULL;
	GString *full_path = NULL;
	gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
		&iter,
		2, &path,
		-1);
	dir = g_dir_open(path, 0, NULL);
	if (dir) {
		full_path = g_string_new("");
		name = g_dir_read_name(dir);
		while (name) {
			full_path = g_string_append(full_path, path);
			full_path = g_string_append(full_path, "/");
			full_path = g_string_append(full_path, name);
			sidebar_add_iter(handler, &iter, full_path->str);
			full_path = g_string_assign(full_path, "");
			name = g_dir_read_name(dir);
		}
		g_string_free(full_path, TRUE);
	}
	g_free(path);
	if (gtk_tree_model_iter_children(GTK_TREE_MODEL(handler->sidebar.tree_store), &child, &iter)) {
		gtk_tree_store_remove(handler->sidebar.tree_store, &child);
	}
}

GtkTreeIter sidebar_add_iter(chandler *handler, GtkTreeIter *parent, gchar *path)
{
	GtkTreeIter iter;
	GFileInfo *file_info = NULL;
	GIcon *icon = NULL;
	GFileType file_type;
	const gchar *content_type = NULL;
	GFile *file = g_file_new_for_path(path);
	gchar *basename = g_path_get_basename(path);
	gtk_tree_store_append(handler->sidebar.tree_store,
		&iter,
		parent);
	gtk_tree_store_set(handler->sidebar.tree_store,
		&iter,
		1, basename,
		2, path,
		-1);
	g_free(basename);
	if (file) {
		file_info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
		if (file_info) {
			icon = g_file_info_get_icon(file_info);
			if (!icon) {
				content_type = g_file_info_get_content_type(file_info);
				icon = g_content_type_get_icon(content_type);
			}
			file_type = g_file_info_get_file_type(file_info);
			if (file_type == G_FILE_TYPE_DIRECTORY) {
				sidebar_update_iter_with_dummy_child(handler, iter);
			}
		}
	}
	if (!icon) {
		icon = g_icon_new_for_string("text-x-generic-template", NULL);
	}
	gtk_tree_store_set(handler->sidebar.tree_store,
			&iter,
			0, icon,
			-1);
	g_object_unref(G_OBJECT(icon));
	if (file) {
		g_object_unref(G_OBJECT(file));
	}
	return iter;
}

void sidebar_open_selected(chandler *handler)
{
	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(handler->sidebar.tree_view));
	gchar *file_path = NULL;
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(tree_selection, NULL, &iter)) {
		gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
			&iter,
			2, &file_path,
			-1);
		add_view_for_document(handler,
			new_document(handler, file_path));
	}
}

void sidebar_rename_selected(chandler *handler)
{
	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(handler->sidebar.tree_view));
	gchar *file_path = NULL;
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(tree_selection, NULL, &iter)) {
		gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
			&iter,
			2, &file_path,
			-1);
		/* TODO */
	}
}

void sidebar_duplicate_selected(chandler *handler)
{
	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(handler->sidebar.tree_view));
	gchar *file_path = NULL;
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(tree_selection, NULL, &iter)) {
		gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
			&iter,
			2, &file_path,
			-1);
		/* TODO */
	}
}

void sidebar_delete_selected(chandler *handler)
{
	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(handler->sidebar.tree_view));
	gchar *file_path = NULL;
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(tree_selection, NULL, &iter)) {
		gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
			&iter,
			2, &file_path,
			-1);
		/* TODO */
	}
}

void sidebar_remove_folder_from_session(chandler *handler)
{
	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(handler->sidebar.tree_view));
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(tree_selection, NULL, &iter)) {
		gtk_tree_store_remove(handler->sidebar.tree_store, &iter);
	}
}

static void init_popover_folder_selected(chandler *handler)
{
	GMenu *menu;
	/* Menu */
	menu = g_menu_new();
	g_menu_insert(menu, -1, TEXT_RENAME, "win.rename-selected");
	g_menu_insert(menu, -1, TEXT_DUPLICATE, "win.duplicate-selected");
	g_menu_insert(menu, -1, TEXT_DELETE, "win.delete-selected");
	/* Popover */
	handler->sidebar.folder_selected.popover = gtk_popover_new_from_model(handler->sidebar.tree_view, G_MENU_MODEL(menu));
	gtk_popover_set_position(GTK_POPOVER(handler->sidebar.folder_selected.popover), GTK_POS_LEFT);
}

static void init_popover_regular_selected(chandler *handler)
{
	GMenu *menu;
	/* Menu */
	menu = g_menu_new();
	g_menu_insert(menu, -1, TEXT_OPEN, "win.open-selected");
	g_menu_insert(menu, -1, TEXT_RENAME, "win.rename-selected");
	g_menu_insert(menu, -1, TEXT_DUPLICATE, "win.duplicate-selected");
	g_menu_insert(menu, -1, TEXT_DELETE, "win.delete-selected");
	/* Popover */
	handler->sidebar.regular_selected.popover = gtk_popover_new_from_model(handler->sidebar.tree_view, G_MENU_MODEL(menu));
	gtk_popover_set_position(GTK_POPOVER(handler->sidebar.regular_selected.popover), GTK_POS_LEFT);
}

static void init_popover_root_selected(chandler *handler)
{
	GMenu *menu;
	GMenu *menu_section_file;
	GMenu *menu_section_session;
	/* Menu */
	menu = g_menu_new();
	/* Section session */
	menu_section_session = g_menu_new();
	g_menu_insert(menu_section_session, -1, TEXT_REMOVE_FOLDER_FROM_SESSION, "win.remove-folder-from-session");
	g_menu_insert_section(menu, -1, NULL, G_MENU_MODEL(menu_section_session));
	/* Section file */
	menu_section_file = g_menu_new();
	g_menu_insert(menu_section_file, -1, TEXT_RENAME, "rename");
	g_menu_insert_section(menu, -1, NULL, G_MENU_MODEL(menu_section_file));
	/* Popover */
	handler->sidebar.root_selected.popover = gtk_popover_new_from_model(handler->sidebar.tree_view, G_MENU_MODEL(menu));
	gtk_popover_set_position(GTK_POPOVER(handler->sidebar.root_selected.popover), GTK_POS_LEFT);
}

static void tree_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	chandler *handler = user_data;
	gchar *iter_path = NULL;
	gchar *file_path = NULL;
	GFile *file = NULL;
	GFileInfo *file_info = NULL;
	GFileType file_type;
	GtkTreeIter iter;
	iter_path = gtk_tree_path_to_string(path);
	if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(handler->sidebar.tree_store), &iter, iter_path)) {
		gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
			&iter,
			2, &file_path,
			-1);
		if (file_path) {
			file = g_file_new_for_path(file_path);
			if (file) {
				file_info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
				if (file_info) {
					file_type = g_file_info_get_file_type(file_info);
					if (file_type == G_FILE_TYPE_REGULAR) {
						add_view_for_document(handler,
							new_document(handler, file_path));
					}
				}
				g_object_unref(G_OBJECT(file));
			}
			g_free(file_path);
		}
	}
	g_free(iter_path);
}

static void tree_view_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	chandler *handler = user_data;
	gboolean valid = FALSE;
	GtkTreeIter child;
	sidebar_update_iter_children(handler, *iter);
	valid = gtk_tree_model_iter_children(GTK_TREE_MODEL(handler->sidebar.tree_store), &child, iter);
	while (valid) {
		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(handler->sidebar.tree_store), &child);
	}
	gtk_tree_view_columns_autosize(tree_view);
}

static void tree_view_row_collapsed(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	chandler *handler = user_data;
	sidebar_update_iter_with_dummy_child(handler, *iter);
	gtk_tree_view_columns_autosize(tree_view);
}

static gboolean tree_view_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	chandler *handler = user_data;
	gchar *file_path = NULL;
	GFile *file = NULL;
	GFileInfo *file_info = NULL;
	GFileType file_type;
	GtkTreeIter iter;
	GdkRectangle rect;
	GtkTreePath *path = NULL;
	if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), event->x, event->y, &path, NULL, NULL, NULL)) {
			gchar *path_string = gtk_tree_path_to_string(path);
			gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(handler->sidebar.tree_store), &iter, path_string);
			g_free(path_string);
			gtk_tree_view_get_cell_area(GTK_TREE_VIEW(widget),
				path,
				NULL,
				&rect);
			if (gtk_tree_store_iter_depth(handler->sidebar.tree_store, &iter) == 0) {
				gtk_popover_set_pointing_to(GTK_POPOVER(handler->sidebar.root_selected.popover), &rect);
				gtk_popover_popup(GTK_POPOVER(handler->sidebar.root_selected.popover));
			} else {
				gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
					&iter,
					2, &file_path,
					-1);
				if (file_path) {
					file = g_file_new_for_path(file_path);
					if (file) {
						file_info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
						if (file_info) {
							file_type = g_file_info_get_file_type(file_info);
							if (file_type == G_FILE_TYPE_REGULAR) {
								gtk_popover_set_pointing_to(GTK_POPOVER(handler->sidebar.regular_selected.popover), &rect);
								gtk_popover_popup(GTK_POPOVER(handler->sidebar.regular_selected.popover));
							} else {
								gtk_popover_set_pointing_to(GTK_POPOVER(handler->sidebar.folder_selected.popover), &rect);
								gtk_popover_popup(GTK_POPOVER(handler->sidebar.folder_selected.popover));
							}
						}
						g_object_unref(G_OBJECT(file));
					}
					g_free(file_path);
				}
			}
		}
	}
	return FALSE;
}

static gboolean tree_view_popup_menu(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(handler->sidebar.tree_view));
	gchar *file_path = NULL;
	GFile *file = NULL;
	GFileInfo *file_info = NULL;
	GFileType file_type;
	GtkTreeIter iter;
	GdkRectangle rect;
	GtkTreePath *path = NULL;
	if (gtk_tree_selection_get_selected(tree_selection, NULL, &iter)) {
		path = gtk_tree_model_get_path(GTK_TREE_MODEL(handler->sidebar.tree_store), &iter);
		gtk_tree_view_get_cell_area(GTK_TREE_VIEW(widget),
			path,
			NULL,
			&rect);
		if (gtk_tree_store_iter_depth(handler->sidebar.tree_store, &iter) == 0) {
			gtk_popover_set_pointing_to(GTK_POPOVER(handler->sidebar.root_selected.popover), &rect);
			gtk_popover_popup(GTK_POPOVER(handler->sidebar.root_selected.popover));
		} else {
			gtk_tree_model_get(GTK_TREE_MODEL(handler->sidebar.tree_store),
					&iter,
					2, &file_path,
					-1);
				if (file_path) {
					file = g_file_new_for_path(file_path);
					if (file) {
						file_info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
						if (file_info) {
							file_type = g_file_info_get_file_type(file_info);
							if (file_type == G_FILE_TYPE_REGULAR) {
								gtk_popover_set_pointing_to(GTK_POPOVER(handler->sidebar.regular_selected.popover), &rect);
								gtk_popover_popup(GTK_POPOVER(handler->sidebar.regular_selected.popover));
							} else {
								gtk_popover_set_pointing_to(GTK_POPOVER(handler->sidebar.folder_selected.popover), &rect);
								gtk_popover_popup(GTK_POPOVER(handler->sidebar.folder_selected.popover));
							}
						}
						g_object_unref(G_OBJECT(file));
					}
					g_free(file_path);
				}
		}
	}
	return TRUE;
}

static void button_add_folder_to_session_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gint response = 0;
	GtkWidget *dialog = gtk_file_chooser_dialog_new(TEXT_ADD_FOLDER_TO_SESSION,
		GTK_WINDOW(handler->window.window),
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
		TEXT_ADD_FOLDER_TO_SESSION_OK, GTK_RESPONSE_OK,
		NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);
	response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response == GTK_RESPONSE_OK) {
		GSList *file_names = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
		GSList *file_name_iter = file_names;
		while (file_name_iter) {
			sidebar_add_iter(handler, NULL, file_name_iter->data);
			g_free(file_name_iter->data);
			file_name_iter = file_name_iter->next;
		}
		g_slist_free(file_names);
	}
	gtk_widget_destroy(dialog);
}

void init_sidebar(chandler *handler)
{
	GtkWidget *box = NULL;
	GtkWidget *scrolled_window = NULL;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	/* Overlay */
	handler->sidebar.overlay = gtk_overlay_new();
	gtk_paned_pack2(GTK_PANED(handler->session.paned),
		handler->sidebar.overlay,
		FALSE,
		FALSE);
	gtk_widget_set_hexpand(handler->sidebar.overlay, TRUE);
	gtk_widget_set_vexpand(handler->sidebar.overlay, TRUE);
	gtk_widget_set_halign(handler->sidebar.overlay, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->sidebar.overlay, GTK_ALIGN_FILL);
	/* Button add root */
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_hexpand(box, FALSE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_END);
	gtk_widget_set_valign(box, GTK_ALIGN_END);
	gtk_container_set_border_width(GTK_CONTAINER(box), MAJOR_SPACING);
	gtk_overlay_add_overlay(GTK_OVERLAY(handler->sidebar.overlay), box);
	handler->sidebar.button_add_folder_to_session = gtk_button_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->sidebar.button_add_folder_to_session, "button_add_folder_to_session");
	gtk_container_add(GTK_CONTAINER(box), handler->sidebar.button_add_folder_to_session);
	gtk_widget_set_tooltip_text(handler->sidebar.button_add_folder_to_session, TEXT_TOOLTIP_ADD_FOLDER_TO_SESSION);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->sidebar.button_add_folder_to_session), "circular");
	g_signal_connect(handler->sidebar.button_add_folder_to_session, "clicked", G_CALLBACK(button_add_folder_to_session_clicked), handler);
	/* Scrolled window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(handler->sidebar.overlay), scrolled_window);
	gtk_widget_set_hexpand(scrolled_window, TRUE);
	gtk_widget_set_vexpand(scrolled_window, TRUE);
	gtk_widget_set_halign(scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_valign(scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(scrolled_window, SIDEBAR_TREE_VIEW_MIN_WIDTH, -1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(scrolled_window)), GTK_STYLE_CLASS_RIGHT);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(scrolled_window)), GTK_STYLE_CLASS_SIDEBAR);
	/* Tree model */
	handler->sidebar.tree_store = gtk_tree_store_new(3, G_TYPE_ICON, G_TYPE_STRING, G_TYPE_STRING);
	/* Tree view */
	handler->sidebar.tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(handler->sidebar.tree_store));
	gtk_widget_set_name(handler->sidebar.tree_view, "tree_view");
	gtk_container_add(GTK_CONTAINER(scrolled_window), handler->sidebar.tree_view);
	gtk_widget_set_hexpand(handler->sidebar.tree_view, TRUE);
	gtk_widget_set_vexpand(handler->sidebar.tree_view, TRUE);
	gtk_widget_set_halign(handler->sidebar.tree_view, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->sidebar.tree_view, GTK_ALIGN_FILL);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(handler->sidebar.tree_view), FALSE);
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(handler->sidebar.tree_view), 1);
	g_signal_connect(handler->sidebar.tree_view, "row-expanded", G_CALLBACK(tree_view_row_expanded), handler);
	g_signal_connect(handler->sidebar.tree_view, "row-collapsed", G_CALLBACK(tree_view_row_collapsed), handler);
	g_signal_connect(handler->sidebar.tree_view, "row-activated", G_CALLBACK(tree_view_row_activated), handler);
	g_signal_connect(handler->sidebar.tree_view, "button-press-event", G_CALLBACK(tree_view_button_press_event), handler);
	g_signal_connect(handler->sidebar.tree_view, "popup-menu", G_CALLBACK(tree_view_popup_menu), handler);
	/* Column */
	column = gtk_tree_view_column_new();
	gtk_tree_view_insert_column(GTK_TREE_VIEW(handler->sidebar.tree_view), column, -1);
	gtk_tree_view_column_set_clickable(column, FALSE);
	/* Cell renderer */
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer,
		"gicon", 0,
		NULL);
	/* Cell renderer */
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
		"text", 1,
		NULL);
	/* Popovers */
	init_popover_root_selected(handler);
	init_popover_regular_selected(handler);
	init_popover_folder_selected(handler);
}

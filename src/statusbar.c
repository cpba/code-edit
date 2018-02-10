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
#include <libgit2-glib/ggit.h>
#include "handlers.h"

void update_statusbar_language(chandler *handler, cview *view)
{
	GtkSourceLanguage *source_language = NULL;
	if (view) {
		source_language = gtk_source_buffer_get_language(view->document->source_buffer);
	}
	if (source_language) {
		gtk_button_set_label(GTK_BUTTON(handler->statusbar.document.button_language),
			gtk_source_language_get_name(source_language));
	} else {
		gtk_button_set_label(GTK_BUTTON(handler->statusbar.document.button_language), "Plain Text");
	}
}

void update_statusbar_repository_branch(chandler *handler, cview *view)
{
	GgitRepository *repo = NULL;
	GgitBranch *branch = NULL;
	GError *error = NULL;
	const gchar *branch_name = NULL;
	GFile *file_repo = NULL;
	GFile *file = NULL;
	if (view) {
		file = gtk_source_file_get_location(view->document->source_file);
	}
	if (file) {
		file_repo = ggit_repository_discover(file, &error);
		if (file_repo && !error) {
			repo = ggit_repository_open(file_repo, &error);
		}
	}
	if (repo && !error) {
		GgitRef *ref = ggit_repository_get_head(repo, &error);
		if (ref && !error) {
			if (ggit_ref_is_branch(ref)) {
				branch = GGIT_BRANCH(ref);
				branch_name = ggit_branch_get_name(branch, &error);
			}
		}
	}
	if (branch_name && !error) {
		gtk_widget_show_all(handler->statusbar.document.button_repo_branch);
		gtk_button_set_label(GTK_BUTTON(handler->statusbar.document.button_repo_branch), branch_name);
	} else {
		gtk_widget_hide(handler->statusbar.document.button_repo_branch);
	}
}

void update_statusbar(chandler *handler, cview *view)
{
	if (!view) {
		view = get_current_view(handler);
	}
	if (view) {
		update_statusbar_language(handler, view);
		update_statusbar_repository_branch(handler, view);
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->statusbar.document.revealer), TRUE);
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->statusbar.sidebar.revealer), TRUE);
	} else {
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->statusbar.document.revealer), FALSE);
		gtk_revealer_set_reveal_child(GTK_REVEALER(handler->statusbar.sidebar.revealer), FALSE);
	}
}

static void tree_view_source_language_activated(GtkWidget *widget, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	chandler *handler = user_data;
	GtkTreeIter tree_iter;
	GtkWidget *popover = NULL;
	gchar *id = NULL;
	gchar *name = NULL;
	GtkSourceLanguageManager *source_language_manager = gtk_source_language_manager_get_default();
	GtkSourceLanguage *source_language = NULL;
	cview *view = get_current_view(handler);
	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(widget))), &tree_iter, path)) {
		gtk_tree_model_get (GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(widget))),
			&tree_iter,
			0, &id,
			1, &name,
			-1);
		if (g_strcmp0(id, "plain-text") == 0) {
			gtk_source_buffer_set_language(view->document->source_buffer, NULL);
		} else {
			source_language = gtk_source_language_manager_get_language(source_language_manager, id);
			gtk_source_buffer_set_language(view->document->source_buffer, source_language);
		}
		gtk_button_set_label(GTK_BUTTON(handler->statusbar.document.button_language), name);
		g_free(id);
		g_free(name);
	}
	popover = GTK_WIDGET(gtk_menu_button_get_popover(GTK_MENU_BUTTON(handler->statusbar.document.button_language)));
	gtk_widget_hide(popover);
}

static void init_statusbar_document(chandler *handler)
{
	GtkWidget *popover = NULL;
	GtkWidget *scrolled_window = NULL;
	GtkWidget *box = NULL;
	GtkWidget *image = NULL;
	GtkWidget *tree_view = NULL;
	GtkListStore *list_store = NULL;
	GtkTreeIter tree_iter;
	GtkWidget *entry = NULL;
	GtkCellRenderer *cell_renderer = NULL;
	GtkTreeViewColumn *tree_view_column = NULL;
	GtkSourceLanguage *source_language = NULL;
	GtkSourceLanguageManager *source_language_manager = gtk_source_language_manager_get_default();
	const gchar *id = NULL;
	const gchar * const *language_ids = gtk_source_language_manager_get_language_ids(source_language_manager);
	/* Revealer status bar */
	handler->statusbar.document.revealer = gtk_revealer_new();
	gtk_widget_set_name(handler->statusbar.document.revealer, "revealer");
	gtk_container_add(GTK_CONTAINER(handler->session.box_vertical), handler->statusbar.document.revealer);
	gtk_widget_set_hexpand(handler->statusbar.document.revealer, TRUE);
	gtk_widget_set_vexpand(handler->statusbar.document.revealer, FALSE);
	gtk_widget_set_halign(handler->statusbar.document.revealer, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->statusbar.document.revealer, GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler->statusbar.document.revealer), GTK_REVEALER_TRANSITION_TYPE_NONE);
	/* Statusbar */
	handler->statusbar.document.action_bar = gtk_action_bar_new();
	gtk_widget_set_name(handler->statusbar.document.action_bar, "action_bar");
	gtk_container_add(GTK_CONTAINER(handler->statusbar.document.revealer), handler->statusbar.document.action_bar);
	gtk_widget_set_hexpand(handler->statusbar.document.action_bar, TRUE);
	gtk_widget_set_vexpand(handler->statusbar.document.action_bar, FALSE);
	gtk_widget_set_halign(handler->statusbar.document.action_bar, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->statusbar.document.action_bar, GTK_ALIGN_FILL);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->statusbar.document.action_bar), GTK_STYLE_CLASS_FLAT);
	gtk_size_group_add_widget(handler->statusbar.size_group, handler->statusbar.document.action_bar);
	/* Button language */
	handler->statusbar.document.button_language = gtk_menu_button_new();
	gtk_widget_set_name(handler->statusbar.document.button_language, "button_language");
	gtk_action_bar_pack_end(GTK_ACTION_BAR(handler->statusbar.document.action_bar), handler->statusbar.document.button_language);
	gtk_widget_set_hexpand(handler->statusbar.document.button_language, FALSE);
	gtk_widget_set_vexpand(handler->statusbar.document.button_language, FALSE);
	gtk_widget_set_halign(handler->statusbar.document.button_language, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->statusbar.document.button_language, GTK_ALIGN_CENTER);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->statusbar.document.button_language), GTK_STYLE_CLASS_FLAT);
	gtk_button_set_label(GTK_BUTTON(handler->statusbar.document.button_language), "Plain Text");
	/* Popover language */
	popover = gtk_popover_new(handler->statusbar.document.button_language);
	gtk_menu_button_set_popover(GTK_MENU_BUTTON(handler->statusbar.document.button_language), popover);
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, MINOR_SPACING);
	gtk_container_add(GTK_CONTAINER(popover), box);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_FILL);
	gtk_container_set_border_width(GTK_CONTAINER(box), MEDIUM_SPACING);
	entry = gtk_search_entry_new();
	gtk_container_add(GTK_CONTAINER(box), entry);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(box), scrolled_window);
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, FALSE);
	gtk_widget_set_halign(box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(box, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(scrolled_window, -1, LIST_VIEW_MIN_HEIGHT);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);
	list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(GTK_LIST_STORE(list_store), &tree_iter);
	gtk_list_store_set(GTK_LIST_STORE(list_store),
		&tree_iter,
		0, "plain-text",
		1, "Plain Text",
		-1);
	while (language_ids[0] != NULL) {
		id = language_ids[0];
		if (g_strcmp0(id, "def") != 0) {
			source_language = gtk_source_language_manager_get_language(source_language_manager, id);
			gtk_list_store_append(GTK_LIST_STORE(list_store), &tree_iter);
			gtk_list_store_set(GTK_LIST_STORE(list_store),
				&tree_iter,
				0, id,
				1, gtk_source_language_get_name(source_language),
				-1);
		}
		language_ids++;
	}
	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
	gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), FALSE);
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view), TRUE);
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(tree_view), GTK_ENTRY(entry));
	g_signal_connect(tree_view, "row-activated", G_CALLBACK(tree_view_source_language_activated), handler);
	cell_renderer = gtk_cell_renderer_text_new();
	tree_view_column = gtk_tree_view_column_new_with_attributes("Name", cell_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), GTK_TREE_VIEW_COLUMN(tree_view_column));
	gtk_tree_view_column_set_visible(GTK_TREE_VIEW_COLUMN(tree_view_column), TRUE);
	gtk_widget_show_all(box);
	/* Button repository branch */
	handler->statusbar.document.button_repo_branch = gtk_menu_button_new();
	gtk_widget_set_name(handler->statusbar.document.button_repo_branch, "button_repo_branch");
	gtk_action_bar_pack_end(GTK_ACTION_BAR(handler->statusbar.document.action_bar), handler->statusbar.document.button_repo_branch);
	gtk_widget_set_hexpand(handler->statusbar.document.button_repo_branch, FALSE);
	gtk_widget_set_vexpand(handler->statusbar.document.button_repo_branch, FALSE);
	gtk_widget_set_halign(handler->statusbar.document.button_repo_branch, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->statusbar.document.button_repo_branch, GTK_ALIGN_CENTER);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->statusbar.document.button_repo_branch), GTK_STYLE_CLASS_FLAT);
	gtk_button_set_label(GTK_BUTTON(handler->statusbar.document.button_repo_branch), "Branch");
	gtk_button_set_always_show_image(GTK_BUTTON(handler->statusbar.document.button_repo_branch), TRUE);
	image = gtk_image_new_from_icon_name("gitg-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(handler->statusbar.document.button_repo_branch), image);
}

static void button_add_root_clicked(GtkWidget *widget, gpointer user_data)
{
	chandler *handler = user_data;
	gint response = 0;
	GtkWidget *dialog = gtk_file_chooser_dialog_new(TEXT_ADD_ROOT_TO_SIDEBAR,
		GTK_WINDOW(handler->window.window),
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
		TEXT_ADD_ROOT_TO_SIDEBAR_OK, GTK_RESPONSE_OK,
		NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);
	response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response == GTK_RESPONSE_OK) {
		GSList *file_names = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
		GSList *file_name_iter = file_names;
		while (file_name_iter) {
			gchar *file_name = file_name_iter->data;
			GtkTreeIter iter = sidebar_add_iter(handler, NULL, file_name);
			sidebar_update_iter_children(handler, iter);
			g_free(file_name);
			file_name_iter = file_name_iter->next;
		}
		g_slist_free(file_names);
	}
	gtk_widget_destroy(dialog);
}

static void init_statusbar_sidebar(chandler *handler)
{
	/* Revealer status bar */
	handler->statusbar.sidebar.revealer = gtk_revealer_new();
	gtk_widget_set_name(handler->statusbar.sidebar.revealer, "revealer");
	gtk_container_add(GTK_CONTAINER(handler->sidebar.box), handler->statusbar.sidebar.revealer);
	gtk_widget_set_hexpand(handler->statusbar.sidebar.revealer, TRUE);
	gtk_widget_set_vexpand(handler->statusbar.sidebar.revealer, FALSE);
	gtk_widget_set_halign(handler->statusbar.sidebar.revealer, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->statusbar.sidebar.revealer, GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler->statusbar.sidebar.revealer), GTK_REVEALER_TRANSITION_TYPE_NONE);
	/* Statusbar */
	handler->statusbar.sidebar.action_bar = gtk_action_bar_new();
	gtk_widget_set_name(handler->statusbar.sidebar.action_bar, "action_bar");
	gtk_container_add(GTK_CONTAINER(handler->statusbar.sidebar.revealer), handler->statusbar.sidebar.action_bar);
	gtk_widget_set_hexpand(handler->statusbar.sidebar.action_bar, TRUE);
	gtk_widget_set_vexpand(handler->statusbar.sidebar.action_bar, FALSE);
	gtk_widget_set_halign(handler->statusbar.sidebar.action_bar, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->statusbar.sidebar.action_bar, GTK_ALIGN_FILL);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->statusbar.sidebar.action_bar), GTK_STYLE_CLASS_FLAT);
	gtk_size_group_add_widget(handler->statusbar.size_group, handler->statusbar.sidebar.action_bar);
	/* Button add root */
	handler->statusbar.sidebar.button_add_root = gtk_button_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(handler->statusbar.sidebar.button_add_root, "button_add_root");
	gtk_action_bar_pack_end(GTK_ACTION_BAR(handler->statusbar.sidebar.action_bar), handler->statusbar.sidebar.button_add_root);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler->statusbar.sidebar.button_add_root), "circular");
	g_signal_connect(handler->statusbar.sidebar.button_add_root, "clicked", G_CALLBACK(button_add_root_clicked), handler);
}

void init_statusbar(chandler *handler)
{
	handler->statusbar.size_group = gtk_size_group_new(GTK_SIZE_GROUP_VERTICAL);
	init_statusbar_document(handler);
	init_statusbar_sidebar(handler);
}

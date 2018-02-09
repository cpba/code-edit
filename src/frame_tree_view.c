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

void init_frame_tree_view(chandler *handler)
{
	/* Sidebar revealer */
	handler->handler_frame_tree_view.revealer = gtk_revealer_new();
	gtk_widget_set_name(handler->handler_frame_tree_view.revealer, "revealer_tree_view");
	gtk_container_add(GTK_CONTAINER(handler->handler_window.box_frames), handler->handler_frame_tree_view.revealer);
	gtk_widget_set_hexpand(handler->handler_frame_tree_view.revealer, FALSE);
	gtk_widget_set_vexpand(handler->handler_frame_tree_view.revealer, TRUE);
	gtk_widget_set_halign(handler->handler_frame_tree_view.revealer, GTK_ALIGN_END);
	gtk_widget_set_valign(handler->handler_frame_tree_view.revealer, GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler->handler_frame_tree_view.revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_LEFT);
	/* Scrolled window */
	/* Box */
	handler_frame_tree_view->tree_view = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler_frame_tree_view->tree_view, "tree_view");
	gtk_container_add(GTK_CONTAINER(handler_frame_tree_view->scrolled_window), handler_frame_tree_view->tree_view);
	gtk_widget_set_hexpand(handler_frame_tree_view->tree_view, TRUE);
	gtk_widget_set_vexpand(handler_frame_tree_view->tree_view, TRUE);
	gtk_widget_set_halign(handler_frame_tree_view->tree_view, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_frame_tree_view->tree_view, GTK_ALIGN_FILL);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_frame_tree_view->tree_view), GTK_STYLE_CLASS_RIGHT);
	gtk_style_context_add_class(gtk_widget_get_style_context(handler_frame_tree_view->tree_view), GTK_STYLE_CLASS_SIDEBAR);
	handler->handler_frame_tree_view.scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_name(handler->handler_frame_tree_view.scrolled_window, "scrolled_window_tree_view");
	gtk_container_add(GTK_CONTAINER(handler->handler_frame_tree_view.revealer), handler->handler_frame_tree_view.scrolled_window);
	gtk_widget_set_hexpand(handler->handler_frame_tree_view.scrolled_window, TRUE);
	gtk_widget_set_vexpand(handler->handler_frame_tree_view.scrolled_window, TRUE);
	gtk_widget_set_halign(handler->handler_frame_tree_view.scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler->handler_frame_tree_view.scrolled_window, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(handler->handler_frame_tree_view.scrolled_window, SIDEBAR_TREE_VIEW_MIN_WIDTH, -1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(handler->handler_frame_tree_view.scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
}

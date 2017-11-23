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

void init_window(chandler *handler)
{
	chandler_window *handler_window = &handler->handler_window;
	/* Window */
	handler_window->window = gtk_application_window_new(handler->application);
	gtk_window_set_icon_name(GTK_WINDOW(handler_window->window), "text-editor");
	/* Box main */
	handler_window->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(handler_window->box, "box");
	gtk_container_add(GTK_CONTAINER(handler_window->window), handler_window->box);
	gtk_widget_set_hexpand(handler_window->box, TRUE);
	gtk_widget_set_vexpand(handler_window->box, TRUE);
	gtk_widget_set_halign(handler_window->box, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_window->box, GTK_ALIGN_FILL);
	gtk_widget_set_size_request(handler_window->box, 400, 400);
	/* Box frames */
	handler_window->box_frames = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(handler_window->box_frames, "box_frames");
	gtk_container_add(GTK_CONTAINER(handler_window->box), handler_window->box_frames);
	gtk_widget_set_hexpand(handler_window->box_frames, TRUE);
	gtk_widget_set_vexpand(handler_window->box_frames, TRUE);
	gtk_widget_set_halign(handler_window->box_frames, GTK_ALIGN_FILL);
	gtk_widget_set_valign(handler_window->box_frames, GTK_ALIGN_FILL);
}

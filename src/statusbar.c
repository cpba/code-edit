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

void init_statusbar(chandler *handler)
{
	chandler_statusbar *handler_statusbar = &handler->handler_statusbar;
	/* Revealer status bar */
	handler_statusbar->revealer_statusbar = gtk_revealer_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->revealer_statusbar), "revealer_statusbar");
	gtk_container_add(GTK_CONTAINER(handler->handler_window.box), GTK_WIDGET(handler_statusbar->revealer_statusbar));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->revealer_statusbar), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->revealer_statusbar), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->revealer_statusbar), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->revealer_statusbar), GTK_ALIGN_FILL);
	gtk_revealer_set_transition_type(GTK_REVEALER(handler_statusbar->revealer_statusbar), GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);
	/* Statusbar */
	handler_statusbar->statusbar = gtk_action_bar_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->statusbar), "statusbar");
	gtk_container_add(GTK_CONTAINER(handler_statusbar->revealer_statusbar), GTK_WIDGET(handler_statusbar->statusbar));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->statusbar), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->statusbar), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->statusbar), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->statusbar), GTK_ALIGN_FILL);
	gtk_revealer_set_reveal_child(GTK_REVEALER(handler_statusbar->revealer_statusbar), TRUE);
}

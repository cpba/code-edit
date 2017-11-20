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
	handler_statusbar->action_bar = gtk_action_bar_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->action_bar), "action_bar");
	gtk_container_add(GTK_CONTAINER(handler_statusbar->revealer_statusbar), GTK_WIDGET(handler_statusbar->action_bar));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->action_bar), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->action_bar), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->action_bar), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->action_bar), GTK_ALIGN_FILL);
	gtk_revealer_set_reveal_child(GTK_REVEALER(handler_statusbar->revealer_statusbar), TRUE);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_statusbar->action_bar))), GTK_STYLE_CLASS_FLAT);
	/* Revealer encoding */
	handler_statusbar->revealer_encoding = gtk_revealer_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->revealer_encoding), "revealer_encoding");
	gtk_action_bar_pack_end(GTK_ACTION_BAR(handler_statusbar->action_bar), GTK_WIDGET(handler_statusbar->revealer_encoding));
	gtk_revealer_set_transition_type(GTK_REVEALER(handler_statusbar->revealer_encoding), GTK_REVEALER_TRANSITION_TYPE_SLIDE_LEFT);
	/* Label encoding */
	handler_statusbar->label_encoding = gtk_button_new();
	gtk_widget_set_name(GTK_WIDGET(handler_statusbar->label_encoding), "label_encoding");
	gtk_container_add(GTK_CONTAINER(handler_statusbar->revealer_encoding), GTK_WIDGET(handler_statusbar->label_encoding));
	gtk_widget_set_hexpand(GTK_WIDGET(handler_statusbar->label_encoding), FALSE);
	gtk_widget_set_vexpand(GTK_WIDGET(handler_statusbar->label_encoding), FALSE);
	gtk_widget_set_halign(GTK_WIDGET(handler_statusbar->label_encoding), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(handler_statusbar->label_encoding), GTK_ALIGN_CENTER);
	gtk_widget_show_all(handler_statusbar->revealer_encoding);
	gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(handler_statusbar->label_encoding))), GTK_STYLE_CLASS_FLAT);
}

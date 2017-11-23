# Contributing

## Coding Style and Language

The coding style is similar to the Linux kernel, but flexible.

The project is writen in C and will be kept that way. No plugin system will be added. Preferably, a new feature should be discussed to decide on how to implement it in the project.

## Portability

Code is focused on GNU/Linux, but patches for BSD-based operating systems are welcome.

## Using GTK and GLib

Every widget must be declared as `GtkWidget` and casted to the specific type that a function takes (except if the function takes `GtkWidget`).

Variables of type `GObject` (or non-visual `Gtk` objects) and descendants should be declared as their specific type (example: `GFile`, `GtkTreeStore`).

The casting must be only made when necessary, if a function takes a different type than the variable.

Correct cases:

```c
GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
GtkWidget *button = gtk_button_new_with_label("Button");
gtk_container_add(GTK_CONTAINER(box), button);
gtk_box_set_homogeneous(GTK_BOX(box), TRUE);

GtkTreeStore *tree_store = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_STRING);
GFile *file = g_file_new_for_path(path);
```

Wrong cases:

```c
GtkBox *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); /* Should be GtkWidget */
GtkWidget *button = gtk_button_new_with_label("Button");
gtk_button_set_label(button, "Test"); /* Should cast to GtkButton */
gtk_container_add(box, GTK_WIDGET(button)); /* No need to cast to GtkWidget */
gtk_box_set_homogeneous(box, TRUE);

GtkTreeStore *tree_store = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_STRING);
GObject *file = g_file_new_for_path(path); /* Should be declared as GFile */
```

## Graphical User Interface Design

The GUI takes in consideration the GNOME Human Interface Guidelines.

### Tree view

- The tree view behave like a bookmark sidebar, and the bookmarked items are saved and restored in the next session.
- The items of the tree view are files or folders.
- The items must have icons.
- It must be possible to drag files to another folder.
- A sigle click on a file will open the file on a tab in preview mode.
- A double click on a file will effectively open the file.
- If the item is open in one of the tabs, then the item in the tree view must symbolize somehow.
- If the item (file or folder) is tracked by `git`, then the item in the tree view must symbolize the status (modified or untracked) somehow.
- The right click on an file must show a menu with:
  - Rename option.
  - Duplicate option.
  - Delete option.
  - The pertinent `git` options.
- The right click on an folder must show a menu with:
  - Rename option.
  - Duplicate option.
  - Delete option.
  - The pertinent `git` option.

## Search and Replace bar

- The search and replace bar must be toggleable with a smooth revealer animation.
- It must be possible to search on all files open or in the tree view and replace the results.
- It must be possible to search and replace using regular expressions.

## Status bar

- The status bar must be togglable.
- The status bar will have these:
  - A `git` status section.
  - Show the language of the document, and the language highlight must be switchable.
  - Show the tab width, and the tab width must be switchable.
  - Show if the text editor is in insert mode or override mode.
  - Show the line and column.

## Preferences

- The preferences frame must be like a sidebar, keeping the rest of the editor visible.

## Licensing

Contributions are accepted licensed under GPL-3 or LGPL.

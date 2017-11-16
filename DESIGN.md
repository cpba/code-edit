# Design

This is a simple design document that will change over time.

## Graphical User Interface

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

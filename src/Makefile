CC ?= gcc
CFLAGS ?= -g -m64 -Wall -Wshadow -Wstrict-prototypes

LDLIBS += `pkg-config --libs gtk+-3.0 gtksourceview-3.0 libgit2-glib-1.0` -ldl -lrt -lm
CFLAGS += `pkg-config --cflags gtk+-3.0 gtksourceview-3.0 libgit2-glib-1.0`

build:
	$(MAKE) clear
	$(CC) -Wall $(CFLAGS) -c session.c
	$(CC) -Wall $(CFLAGS) -c document.c
	$(CC) -Wall $(CFLAGS) -c frame_view.c
	$(CC) -Wall $(CFLAGS) -c frame_tree_view.c
	$(CC) -Wall $(CFLAGS) -c header.c
	$(CC) -Wall $(CFLAGS) -c statusbar.c
	$(CC) -Wall $(CFLAGS) -c file_chooser_save.c
	$(CC) -Wall $(CFLAGS) -c window.c
	$(CC) -Wall $(CFLAGS) -c main.c
	$(CC) -Wall -g -o ./code *.o $(LDLIBS)
	$(MAKE) clear

clear:
	rm -f ./*.o

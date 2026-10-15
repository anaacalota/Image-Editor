CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build:
	$(CC) $(CFLAGS) main.c load.c select.c save.c exit.c histogram.c equalize.c apply.c crop.c rotate.c -lm -o image_editor
	
pack:
	zip -FSr image_editor.zip README Makefile *.c *.h

clean:
	rm -f image_editor


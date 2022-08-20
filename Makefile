CC = gcc -std=c99 -Wall
PROGS = Ballz
ALLEGRO = -lallegro_image -lallegro_primitives  -lallegro_dialog  -lallegro_ttf -lallegro_font -lallegro_acodec -lallegro_audio -lallegro
CFLAGS = -I.
LFLAGS = $(ALLEGRO)

%.o:  %.c
	$(CC) $(CFLAGS) -c -lm $<

debug: CFLAGS += -g -D__DEBUG__

all debug: $(PROGS)

$(PROGS) : % : %.o Bolas.o Blocos.o
	$(CC) $(CFLAGS) -o  $@ $^ $(LFLAGS) -lm

clean:
	@echo "Limpando sujeira ..."
	@rm -f *% *.bak *~

purge: clean
	@echo "Limpando tudo ..."
	@rm -rf core *.o
	@rm -f $(PROGS)
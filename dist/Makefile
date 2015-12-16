CC=gcc
CFLAGS = -g -pthread
OBJDIR=obj

#vpath %.c src
#vpath %.h src
#main.h conf.h tasks.h escalonador.h workers.h balanceador.h

objects = $(addprefix $(OBJDIR)/, main.o conf.o tasks.o escalonador.o workers.o balanceador.o)

simulador: $(objects)
	$(CC) -o simulador $(objects) $(CFLAGS)

obj/%.o: src/%.c 
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -r $(objects) simulador EstatisticasPizza.txt EstatisticasBarras.txt TemposThreads.txt TotalTime.txt -f

LDFLAGS = -framework Foundation \
          -framework CoreFoundation \
          -framework ApplicationServices \
          -Ldeps/libuv

build: libuv.a queue

queue:
	$(CC) -o queue.o src/queue/queue.c $(LDFLAGS)

libuv.a:
	$(MAKE) -C deps/libuv libuv.a

clean:
	rm *.o
	rm deps/libuv/libuv.a

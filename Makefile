LDFLAGS = -framework Foundation \
          -framework CoreFoundation \
          -framework ApplicationServices \
          -Ldeps/libuv

build: queue

queue:
	$(CC) -o queue.o src/queue/queue.c

thread_simple:
	$(CC) -o simple_thread.o src/thread/simple.c

libuv.a:
	$(MAKE) -C deps/libuv libuv.a

clean:
	rm *.o
	rm deps/libuv/libuv.a

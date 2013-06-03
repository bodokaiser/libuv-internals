# libuv-internals

**libuv-internals** is a collection of simple code snippets which represent
different parts of the **libuv** architecture. For example the _queue_ snippet
shows how **QUEUE** works with a simple use-case.

## Content

### queue

**QUEUE** is a circularly linked list in libuv which is used to store tasks in
order. Its main idea is to have a array with two items. The first item points
to the previous list node while the second points to the next one. The last 
node points again to the beginning of the list. Through this set up it is 
possible by just following the pointers to navigate through the list. If you
now want to access a specific queue member you can calculate the memory address
of the struct which implements a queue node as property. This is possible
because a struct is always saved in one memory block and the position of a
property has a specific byte offset. Using this allows to create a pointer to
the beginning of the whole struct and we can access other properties.

### threads

To avoid blocking **libuv** executes work requests in a thread pool. Each time
intensive i/o should be done a task will be put in queue and then spawned as 
thread in which it can execute to the end without blocking the main thread. The
given examples show up basic thread management. It is recommend to read this
[document](https://computing.llnl.gov/tutorials/pthreads) to get an idea of the
basics.

### events

There are several mechanisms which are used by libuv to handle pending tasks in
a non-blocking way. For example async.c uses eventfd (on linux) and else 
socketpair to execute async callbacks. Also all notification for example about
new readable data in the socket must be published to libuv somehow. The events
section tries to get some concepts of that.

### streams

The stream component is the most platform dependent part of libuv. As I 
understood correctly so far the most simple implementation works by just
subscribing to a file discriptor with the event notification system of the os.
In case of OS X this is not possible for all all file discriptor types. This
causes libuv to create an extra thread polling manuelly the file state and then
using the async component to wake up the main thread. The created example tries
to the main idea of a readable file stream which will execute read callbacks
each time new data is added to the file.

## License

Copyright © 2013 Bodo Kaiser <i@bodokaiser.io>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

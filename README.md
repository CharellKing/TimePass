TimePass - TimePass is a posix-based library
====================
TimePass is being developed. I'll try my best to make it more perfect. Now TimePass has four parts(global, lock, model,
shm, sock, sys, time).
 
 * global: defines custom error codes, macros and global functions.
 * lock: encapsulate mutex, sem, condmutex, filelock, rwlock, rwsem. 
 * model: based on lock part, implement the produce-consumer problem between threads and processes. 
 * shm: implement shared memory's containers such as array, list, queue, shm, set, hashlist, hashmap, hashset and so on.
 * sock: encapsulate the socket
 * sys:  implement system functions
 * time: add timer for event's notice and watcher for watching program's efficiency.
 
Compile and Build
--------------------
### use cmake to compile and build
    sudo apt-get install cmake #for debian(ubuntu etc)
    sudo yum install cmake     #for redhat(fedora centos etc)

### only install shared librarys
    cd TimePass/src
    mkdir build
    cmake .. (CMAKE_PREFIX_INSALL="path")
    make
    sudo make install

### generate executable examples
    cd TimePass/example
    #the path must be same with above
    cmake .. (CMAKE_PREFIX_INSTALL="path")
    make
    sudo make install

### automake.sh
    #be fully compatible with bash, not others scripts. the argument is optional;
    sudo bash automake.sh [insall-path]
    (for convenience, execute automake.sh to finish above two steps)

### autoclean.sh

#installed path mush be correspondence between autoinstall.sh

sudo bash autoclean.sh [install-path]

(for convenience, execute autoclean.sh to clean all the intermediate files and installed files)

Improvment
--------------------
 * optimize the shared memory: the shared memory alloc a big memory, it will cause many memory-slices; to manage these
 sclies, I should make most use of these slices.
 
 * expand hash container: to expand hash container, all the data must rehash, so I should deal with moment of rehashing
   to prevent reducing the efficiency.   
    
Contact Me
-------------------
CharellkingQu@gmail.com
  

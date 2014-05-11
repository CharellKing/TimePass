TimePass-For-Eclipse
====================
    TimePass includes tree parts: "shm", "lock", "model", "time".
    The "shm" part is about some container which imitates STL and
bases on shared memory (ShmArray, ShmList, ShmDoublyList, ShmQueue,
ShmSet, ShmMultiset, ShmMap, ShmMultimap, ShmHashlist, ShmHashset,
ShmHashmap, ShmHashmultimap, ShmHashmultiset).
    The "lock" part implements some locks, the lock's sorts are
thread's lock which is implemented by sem and process'lock which
is implemented by mutex(FileLock, Mutex, SemMutex, RWMutex, RWSem). 
    The "model" mainly implements the producer-consumer problem between
threads and process.
    TimePass is based on GPL license.

Compile and Build
--------------------
    Add the dependent include directories.
    "Project Properties" => "C/C++ Gerneral" =>"Paths and Symbols"
=> "Includes" => "GNU C++" => Add"."
    
Run Example
--------------------
    Import a example in "TimePass/example".
    "Project Properties" => "C/C++ Gerneral" => "Paths and Symbols" =>
"Includes" => "GNU C++" => Add"../../src" => "Libraries" => Add "pthread",
"src", "rt" => "Libraries Paths" => "../../src/Debug"
    Building project is OK, but Running project will be fault, it shows
"can't find libsrc.so".
    edit /etc/ld.so.conf with root mode, and add "libsrc.so" file's path.
    
Contact Me
-------------------
CharellkingQu@gmail.com
  

TimePass-For-Eclipse
====================
TimePass includes four parts: "shm", "lock", "model", "time".<br/>
The "shm" part is about some container which imitates STL and bases on shared memory (ShmArray, ShmList, ShmDoublyList, ShmQueue, ShmSet, ShmMultiset, ShmMap, ShmMultimap, ShmHashlist, ShmHashset, ShmHashmap, ShmHashmultimap, ShmHashmultiset).<br/>
The "lock" part implements some locks, the lock's sorts are thread's lock which is implemented by sem and process'lock which is implemented by mutex(FileLock, Mutex, SemMutex, RWMutex, RWSem).<br/>
The "model" mainly implements the producer-consumer problem between threads and process.<br/>
The "time" has a timer which sets your program to do some actions at a interval time and a time to calculate your program's running time. <br/>
TimePass is based on GPL license.<br/>

Compile and Build
--------------------
Add the dependent include directories.<br/>
"Project Properties" => "C/C++ Gerneral" =>"Paths and Symbols" => "Includes" => "GNU C++" => Add"."<br/>
Build "src" project, generator libsrc.so in Debug directory.<br/>
    
Run Example
--------------------
Import a example in "TimePass/example".<br/>
"Project Properties" => "C/C++ Gerneral" => "Paths and Symbols" =>"Includes" => "GNU C++" => Add"../../src" => "Libraries" => Add "pthread", "src", "rt" => "Libraries Paths" => "../../src/Debug"<br/>
Building project is OK, but Running project will be fault, it shows "can't find libsrc.so". edit /etc/ld.so.conf with root mode, and add "libsrc.so" file's path and then execute "sudo ldconfig".<br/>
Build example project, gernerate executable file.<br/>
    
Contact Me
-------------------
CharellkingQu@gmail.com
  

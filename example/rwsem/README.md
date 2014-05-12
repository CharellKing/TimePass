RWSem
=======================================

Compile and Build example
---------------------------------------
Import rw_sem from TimePass/example/rw_sem.<br/>
"Project Properties" => "C/C++ Gerneral" => "Paths and Symbols" =>"Includes" => "GNU C++" => Add"../../src" => "Libraries" => Add "pthread", "src", "rt" => "Libraries Paths" => "../../src/Debug"<br/>
Building project is OK, but Running project will be fault, it shows "can't find libsrc.so". edit /etc/ld.so.conf with root mode, and add "libsrc.so" file's path and then execute "sudo ldconfig".<br/>
Build example project, gernerate executable file.<br/>

Run executable file
---------------------------------------
The executable file is in Debug directory. The read thread reads datas from a queue and the write thread write datas to the same queue <br/>

show help <br/>    
     ./rwsem -h      

create sem and queue <br/>
     ./rwsem -c [size]

destroy sem and queue <br/>
     ./rwsem -d

write data <br/>
     ./rwsem -w

read data <br/>
     ./rwsem -r


initialization:</br>
     ./rwsem -c 1024

in a terminal write datas:</br>
     ./rwsem -w

in other terminal read datas:</br>
     ./rwsem -r


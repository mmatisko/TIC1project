# TIC1project
TIC1 Project  
  
Fully argument version to easy and fast testing without changing source code    
Argument list with default values in []: (no need to use directed order)  
Project.exe (Program name...of course)  
-i \<input hash> [ahoj]  
-b \<biteSize> [32]    
-p \<probability of false positive> [0.005/5E-3]     
-c \<capacity of array(in M)> [10]  
-t \<thread number> [cpu threads]  
-l (logging) [false]  
-m \<multiplier of filter size>  
-n \<modulo of writing hash> [10] (tested just with 10/100/1000, should work with others like 20,50, but need to test)
  
Array of filters principle:  
Capacity (size of array) is divided by number of threads and multiplicated by multiplier (because of speed),  
to get final size of one array, if number of hash get size of filter, programme will create another filter.   
     
full example: Project.exe -i ahoj -b 56 -p 0.005(5E-3) -c 100 -m 4 -n -10 -t 4 -l     
  
Logging:   
every 10th M of filter IO operations - console and logFile   
every 5th K of array operations - console and logFile   
lenght of filling every filter in array marked as "lap" - to console only   


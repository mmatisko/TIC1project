# TIC1project
TIC1 Project  
  
Fully argument version to easy and fast testing without changing source code    
Argument order:  
<0-Program name> <1-input hash> <2-biteSize> <3-probability of false positive>  
<4-capacity of array(in M)> <5-thread number(optional)> <6-logging (optional)>  
    
Example: Project.exe -i ahoj -b 56 -p 0.005(5E-3) -c 100 [-t 4] [-l]  
  
Logging:  every 20th M of filter IO operations, every 5th K of array operations

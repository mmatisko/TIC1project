# TIC1project
TIC1 Project  
  
Fully argument version to easy and fast testing without changing source code    
Argument order:  
<0-Program name> <1-input hash> <2-biteSize> <3-falseFilterProbability>  
<4-predictedSize(in M)> <5-threads(optional)> <6-logging(optional)>  
    
Example: Project.exe -i ahoj 56 0.005(5E-3) 100 [-t 4] [-l]  
  
Logging:  every 20th M of filter IO operations, every 5th K of array operations

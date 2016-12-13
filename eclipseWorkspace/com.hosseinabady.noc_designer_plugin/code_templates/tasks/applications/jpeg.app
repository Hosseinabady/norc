NumberOfTasks   9
--
Task
ID         1
Name       ReadImage 
C          20    
To         20
Tmin       15
Tmax       25
E          1
PreTask    0   100
sucTask    2   100
--
Task
ID         2
Name       Converter
C          10    
To         20
Tmin       15
Tmax       25
E          1
PreTask    1   100
sucTask    3   100
--
Task
ID         3
Name       Blocking
C          30    
To         20
Tmin       15
Tmax       25
E          1
PreTask    2   100
sucTask    4   100
--
Task
ID         4
Name       DownSampler
C          30    
To         20
Tmin       15
Tmax       25
E          1
PreTask    3   100
sucTask    5   100
--
Task
ID         5
Name       FDCT
C          30    
To         20
Tmin       15
Tmax       25
E          1
PreTask    4   100
sucTask    6   100
--
Task
ID         6
Name       Quantizer
C          30    
To         20
Tmin       15
Tmax       25
E          1
PreTask    5   100
sucTask    7   100
--
--
Task
ID         7
Name       ZigZag
C          30    
To         20
Tmin       15
Tmax       25
E          1
PreTask    6   100
sucTask    8   100
--
--
Task
ID         8
Name       Huffman
C          30    
To         20
Tmin       15
Tmax       25
E          1
PreTask    7   100
sucTask    9   100
--
Task
ID         9
Name       WriteImage
C          30    
To         20
Tmin       15
Tmax       25
E          1
PreTask    8   100
sucTask    0   100
--
END    
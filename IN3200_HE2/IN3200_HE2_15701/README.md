# IN3200 Home exam 2
Candidate 15701
## How to compile
To compile ```MPI_main.c``` (Task 2.2) run:

```mpicc MPI_main.c -o MPI_main```
This creates the executable ```MPI_main```, which can be run by the command:

```mpirun -np $1 ./MPI_main $2 $3 $3```

where $1 is the numer of threads you would like to run it on.
The remaining variables are ```$2```: M, ```$3```: N and ```$4```: K.

Task 2.1 can be found in the file ```MPI_single_layer_convolution.c``` which is
included in ```MPI_main.c``` and run there.

## What the example code does
The example code in ```MPI_main.c``` will automatically generate an input array
and a kernel, which for M = 8, N = 8, K = 3 is the example depicted in the 
introductions of the exam.

For other valeus it will simply fill the left-most 
column of the kernel with ```1```, the right-most with ```-1``` and leave 
everything else as ```0```. (For a 1x1 kernel, the only value will be ```-1```,
as the right side is assigned after the left.) The input will always have its 
first four columns filled with ```4```.

## Additional notes
For the code to work porperly it is required that ```K < M``` and ```K < N```!

For low numbers of M, N and K the calculated times will most likely be printed 
as either ```"nan"``` or ```"-nan"```, sicne the RTC only measures time in whole seconds. 
This means a float division by zero is encountered.
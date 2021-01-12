#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
    int upper_limit, no_processes;
    printf("This is a program to calculate the value of pi\nplease enter the upper limit: ");
    scanf("%d", &upper_limit);
    printf("please enter the no of processes: ");
    scanf("%d", &no_processes);
    string command = "mpirun -n "+to_string(no_processes)+" ./Assignment4 "+to_string(upper_limit);
    system(command.c_str());
    return 0;
}

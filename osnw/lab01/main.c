#include<stdio.h>

extern void print_id();
extern void print_name();

void main(){
    printf("김도영\n");
    print_id();
    print_name();
}
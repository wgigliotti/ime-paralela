/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   time.c
 * Author: ricardo
 *
 * Created on March 8, 2019, 12:04 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

void nanosleep(long t) {
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = t;
    
    if(nanosleep(&tim , &tim2) < 0 ) {
        fprintf(stderr, "Nano sleep system call failed \n");
        return;
    }
}

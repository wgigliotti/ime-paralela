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
#include <time.h>
#include "sleep.h"

void nsleep(long miliseconds) {
    struct timespec req, rem;

    if(miliseconds > 999) {   
        req.tv_sec = (int)(miliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (miliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
    } else {   
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = miliseconds * 1000000;    /* Must be in range of 0 to 999999999 */
    }   

    nanosleep(&req , &rem);
}
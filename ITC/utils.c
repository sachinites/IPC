/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/14/2020 08:14:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Juniper Networks (https://csepracticals.wixsite.com/csepracticals), sachinites@gmail.com
 *        Company:  Juniper Networks
 *
 *        This file is part of the XXX distribution (https://github.com/sachinites) 
 *        Copyright (c) 2019 Abhishek Sagar.
 *        This program is free software: you can redistribute it and/or modify it under the terms of the GNU General 
 *        Public License as published by the Free Software Foundation, version 3.
 *        
 *        This program is distributed in the hope that it will be useful, but
 *        WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *        General Public License for more details.
 *
 *        visit website : https://csepracticals.wixsite.com/csepracticals for more courses and projects
 *                                  
 * =====================================================================================
 */
#include "utils.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

char*
hrs_min_sec_format(unsigned int seconds){

    static char time_f[16];
    unsigned int hrs = 0,
                 min =0, sec = 0;

    if(seconds > 3600){
        min = seconds/60;
        sec = seconds%60;
        hrs = min/60;
        min = min%60;
    }
    else{
        min = seconds/60;
        sec = seconds%60;
    }
    memset(time_f, 0, sizeof(time_f));
    sprintf(time_f, "%u::%u::%u", hrs, min, sec);
    return time_f;
}


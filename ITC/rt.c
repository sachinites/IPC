/*
 * =====================================================================================
 *
 *       Filename:  rt.c
 *
 *    Description:  This file implements the routines to work with simple Routing Table
 *
 *        Version:  1.0
 *        Created:  02/14/2020 07:35:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Juniper Networks (https://csepracticals.wixsite.com/csepracticals), sachinites@gmail.com
 *        Company:  Juniper Networks
 *
 *        This file is part of the Inter-Thread Communication distribution (https://github.com/sachinites) 
 *        Copynext (c) 2019 Abhishek Sagar.
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

#include "rt.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include <string.h>

struct rt_table_entry_{

    char dest[16];
    char mask;
    char gw[16];
    char oif[32];
    time_t last_updated_time;
    struct rt_table_entry_ *next;
    struct rt_table_entry_ *prev;
};

struct rt_table_ {

    char rt_table_name[32];
    struct rt_table_entry_ *head;
};


rt_table_t *
rt_create_new_rt_table(char *name){

    rt_table_t *rtable = calloc(1, sizeof(rt_table_t));
    strncpy(rtable->rt_table_name, name, 32);
    return rtable;
}


int /*0 on success, -1 on failure*/
rt_insert_new_entry(rt_table_t *rt,
        char *dest, char mask,
        char *gw, char *oif){

    rt_table_entry_t *rt_table_entry = rt_look_up_rt_table_entry(
        rt, dest, mask);
    
    if(rt_table_entry){
        printf("Error : Entry already exist for key %s/%d\n", dest, mask);
        return -1;
    }

    rt_table_entry = 
            calloc(1, sizeof(rt_table_entry_t));

   strncpy(rt_table_entry->dest, dest, 16);
   rt_table_entry->mask = mask;
   strncpy(rt_table_entry->gw, gw, 16);
   strncpy(rt_table_entry->oif, oif, 32);
   rt_table_entry->next = 0;
   rt_table_entry->prev = 0;
   time(&rt_table_entry->last_updated_time);

   /* Now insert the new rt_table_entry at the beginnig of the
    * list*/
   if(!rt->head){
        rt->head = rt_table_entry;
        return 0;
   }

   rt_table_entry->next = rt->head;
   rt_table_entry->prev = 0;
   rt->head->prev = rt_table_entry;
   rt->head = rt_table_entry;
   return 0;
}

int /*0 on success, -1 on failure*/
rt_delete_rt_entry(rt_table_t *rt,
        char *dest, char mask){

    rt_table_entry_t *rt_table_entry = rt_look_up_rt_table_entry(
        rt, dest, mask);

    if(!rt_table_entry)
        return -1;

    /*Now delete it*/
    if(rt->head == rt_table_entry){
        rt->head = rt_table_entry->next;
        if(rt->head)
            rt->head->prev = NULL;
        free(rt_table_entry);
        return 0;
    }

    if(!rt_table_entry->prev){
        if(rt_table_entry->next){
            rt_table_entry->next->prev = NULL;
            rt_table_entry->next = 0;
            return 0;
        }
        return 0;
    }
    if(!rt_table_entry->next){
        rt_table_entry->prev->next = NULL;
        rt_table_entry->prev = NULL;
        return 0;
    }

    rt_table_entry->prev->next = rt_table_entry->next;
    rt_table_entry->next->prev = rt_table_entry->prev;
    rt_table_entry->prev = 0;
    rt_table_entry->next = 0;
    free(rt_table_entry);
    return 0;
}

int /*0 on success, -1 on failure*/
rt_update_rt_entry(rt_table_t *rt,
        char *dest, char mask,
        char *new_gw, char *new_oif){

    rt_table_entry_t *rt_table_entry = rt_look_up_rt_table_entry(
        rt, dest, mask);

    if(!rt_table_entry)
        return -1;

    if(strncmp(rt_table_entry->dest, dest, 16) == 0 &&
       rt_table_entry->mask == mask &&
       strncmp(rt_table_entry->gw, new_gw, 16) == 0 &&
       strncmp(rt_table_entry->oif, new_oif, 32) == 0){    
       return -1;
    }

   strncpy(rt_table_entry->dest, dest, 16);
   rt_table_entry->mask = mask;
   strncpy(rt_table_entry->gw, new_gw, 16);
   strncpy(rt_table_entry->oif, new_oif, 32);
   time(&rt_table_entry->last_updated_time);
   return 0;
}

void
rt_display_rt_table(rt_table_t *rt){

    int i = 1;
    rt_table_entry_t *rt_table_entry = NULL;
    time_t curr_time = time(NULL);
    unsigned int uptime_in_seconds = 0;

    for(rt_table_entry = rt->head; rt_table_entry;
        rt_table_entry = rt_table_entry->next){

        uptime_in_seconds = (unsigned int)difftime(
            curr_time, rt_table_entry->last_updated_time);

        printf("%d. %-18s %-4d %-18s %s\n", i,
            rt_table_entry->dest,
            rt_table_entry->mask,
            rt_table_entry->gw,
            rt_table_entry->oif);
        printf("Last updated : %s\n", hrs_min_sec_format(uptime_in_seconds));
        i++;
    }
}

rt_table_entry_t *
rt_look_up_rt_table_entry(rt_table_t *rt, 
        char *dest, char mask){

    rt_table_entry_t *rt_table_entry = NULL;

    for(rt_table_entry = rt->head; rt_table_entry;
        rt_table_entry = rt_table_entry->next){

        if(strncmp(rt_table_entry->dest, dest, 16) == 0 &&
            rt_table_entry->mask == mask)
        return rt_table_entry;
    }
    return NULL;
}


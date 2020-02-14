/*
 * =====================================================================================
 *
 *       Filename:  rtm.c
 *
 *    Description:  This file implements the Routing Table Manager(Publisher Thread)
 *
 *        Version:  1.0
 *        Created:  02/14/2020 07:24:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Juniper Networks (https://csepracticals.wixsite.com/csepracticals), sachinites@gmail.com
 *        Company:  Juniper Networks
 *
 *        This file is part of the Inter-Thread Communication distribution (https://github.com/sachinites) 
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

#include <stdio.h>
#include <stdlib.h>

#include "rt.h"

rt_table_t *publisher_rt_table = NULL;

int
main(int argc, char **argv){

    
    for(;;){
        printf("Main Menu\n");
        printf("\t 1. Publisher : Display Routing Table\n");
        printf("\t 2. Create New Consumer\n");
        printf("\t 3. Kill the Consumer\n");
        printf("\t 4. Consumer : Subscribe to RT Entry\n");
        printf("\t 5. Consumer : Un-Subscribe to RT Entry\n");
        printf("\t 6. Consumer : Display Routing Table\n");
        printf("\t 7. Publisher : Create New RT Entry\n");
        printf("\t 8. Publisher : Update Existing RT Entry\n");
        printf("\t 9. Publisher : Delete RT entry\n");
        printf("\t10. exit\n");

        int choice;
        printf("Enter Choice : ");
        scanf("%d", &choice);


        switch(choice){
            case 1:
                if(!publisher_rt_table){
                    publisher_rt_table = 
                        rt_create_new_rt_table("Publisher-Table");
                    printf("Publisher New Routing Table Created\n");
                }
                else{
                    printf("Publisher RT Table Already Created\n");
                }
                break;
            default:
                break;
        }
    }
    return 0;
}

/*
 * Author : Krishang Patney 
 * StudentID : 2405690p
 * File : date.c
 * Assignment Title : SP Exerscise 1A
 * This is my own work as defined in the Academic Ethics agreement I have signed.
 */

#include "date.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



static int date_validation(int day, int month, int year);

struct date{
    int day;
    int month;
    int year;
};

/*
 * date_create creates a Date structure from `datestr`
 * `datestr' is expected to be of the form "dd/mm/yyyy"
 * returns pointer to Date structure if successful,
 *         NULL if not (syntax error)
 */
Date *date_create(char *datestr){
    Date *date = (Date *) malloc(sizeof(Date));

    if (date == NULL){
        date_destroy(date);
    }

    int new_day, new_year, new_month;
    sscanf(datestr, "%d/%d/%d", &new_day, &new_month, &new_year);
    
    if (date_validation(new_day, new_month, new_year) == 1){
        date -> day = new_day;
        date -> month = new_month;
        date -> year = new_year;
    } 
    
    return date;
};

// helper function for date validation
int date_validation(int day, int month, int year){
    if (year >= 1000 && year<= 9999){
        if (month >= 1 && month <= 12){
            if ((day >= 1 && day <= 31) && (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12))
                return 1;
            else if ((day >= 1 && day <= 30) && (month == 4 || month == 6 || month == 9 || month == 11))
                return 1;
            else if ((day >= 1 && day <= 28) && ((year % 4 == 0 && year % 100 != 0) || year%400 == 0 ))
                return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
    return 1;
}

/*
 * date_duplicate creates a duplicate of `d'
 * returns pointer to new Date structure if successful,
 *         NULL if not (memory allocation failure)
 */
Date *date_duplicate(Date *d){
    Date *date_dupe = (Date *) malloc(sizeof(Date));

    if (date_dupe != NULL){
        date_dupe -> day = d -> day;
        date_dupe -> month = d -> month;
        date_dupe -> year = d -> year;
    }

    return date_dupe;
};

/*
 * date_compare compares two dates, returning <0, 0, >0 if
 * date1<date2, date1==date2, date1>date2, respectively
 */
int date_compare(Date *date1,Date *date2){
    if(date1 -> year > date2 -> year ){
        return 1;
    }else if (date1 -> year < date2 -> year){
        return -1;
    }
    if (date1->year == date2-> year){
        if(date1 -> month > date2 -> month){
            return 1;
        }
        else if(date1 -> month < date2 -> month){
            return -1;
        }else if(date1 -> day > date2 -> day ){
                return 1;
            }
        else if (date1 -> day < date2 -> day ){
                return -1;
        }
        else{
            return 0;
        }
    }
    
    return -2;
};

/*
 * date_destroy returns any storage associated with `d' to the system
 */
void date_destroy(Date *d){
    if (d != NULL){
        free(d);
    }
}


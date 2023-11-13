/*
 * This file contains the skeleton for an LRU pager
 */

#include <stdio.h> 
#include <stdlib.h>
#include "simulator.h"



void pageit(Pentry q[MAXPROCESSES]) { 

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proc;
    int page;

    /* initialize timestamp array on first run */
    if (!initialized) {
        for (proc=0; proc < MAXPROCESSES; proc++) {
            for (page=0; page < MAXPROCPAGES; page++) {
                timestamps[proc][page] = 0;
            }
        }
        initialized = 1;
    }

    /* TODO: Implement LRU Paging */ // BELOW ARE THE NOTES FROM RECITATION 
    // Go through all ACTIVE processes
        // Calculate the wanted page based on PC and PAGESIZE
        // Update timestamp for that page
        // Check if the wanted page is already in memory
            // If the wanted page is swapped in => continue to the next process
            // If not => try to swap in the wanted page by using pagein()
                // If pagein() succeeds => continue to the next process
                // If pagein() fails => LRU kicks in
                    //get the swapped in least recently used page of the current process (local) using timestamp and pages array => pageout()

    for (proc = 0; proc < MAXPROCESSES; proc++) {
        if (q[proc].active) {                                                               // This checks if the process is active
            page = q[proc].pc / PAGESIZE;                                                   // We divide by PAGESIZE because the PC is in bytes
            timestamps[proc][page] = tick;
            if (!q[proc].pages[page]) {                                                     // This checks if the wanted page is already in memory
                if (!pagein(proc, page)) {                                                  // This checks if pagein() fails, then LRU kicks in
                    int lru_page = 0;
                    int lru_timestamp = tick;
                    for (int i = 0; i < MAXPROCPAGES; i++) {
                        if (q[proc].pages[i] && timestamps[proc][i] < lru_timestamp) {      // This gets the swapped in least recently used page of the current process (local) using timestamp and pages array
                            lru_page = i;
                            lru_timestamp = timestamps[proc][i];
                        }
                    }
                    pageout(proc, lru_page);
                }
            }
        }
    }

    /* advance time for next pageit iteration */
    tick++;
} 

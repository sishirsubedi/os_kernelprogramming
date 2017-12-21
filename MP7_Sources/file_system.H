/* 
    File: file_system.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 10/04/05

    Description: Simple File System.
    

*/

#ifndef _FILE_SYSTEM_H_                   // include file only once
#define _FILE_SYSTEM_H_

// file system can have max of 25 files
#define  FILESYSTEM_MAXFILE 25
#define  FILESYSTEM_BLOCK_SIZE  512
#define  FILESYSTEM_BLOCK_LENGTH  3
#define  FILESYSTEM_DATA_OFFSET  12
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "file.H"
#include "simple_disk.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARD DECLARATIONS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* F i l e S y s t e m  */
/*--------------------------------------------------------------------------*/


class File;
class FileSystem {

//friend class File; /* -- not sure if we need this; feel free to delete */

private:
     /* -- DEFINE YOUR FILE SYSTEM DATA STRUCTURES HERE. */
     
     // file system size 
     unsigned int filesystem_size;
     // how many files can this file system keep
     unsigned int filesystem_total_files;
     // information of each files in an 2d array
     unsigned int filesystem_file_info[FILESYSTEM_MAXFILE][FILESYSTEM_BLOCK_LENGTH];


     
public:

// here we define add two items related to disk and blocks	
// disk used by file system to read and write
SimpleDisk * filesystem_disk;
// to keep track of free blocks in the disk
unsigned int filesystem_free_block;


    FileSystem();
    /* Just initializes local data structures. Does not connect to disk yet. */
    
    bool Mount(SimpleDisk * _disk);
    /* Associates this file system with a disk. Limit to at most one file system per disk.
     Returns true if operation successful (i.e. there is indeed a file system on the disk.) */
    
    static bool Format(SimpleDisk * _disk, unsigned int _size);
    /* Wipes any file system from the disk and installs an empty file system of given size. */
    
    File * LookupFile(int _file_id);
    /* Find file with given id in file system. If found, return the initialized
     file object. Otherwise, return null. */
    
    bool CreateFile(int _file_id);
    /* Create file with given id in the file system. If file exists already,
     abort and return false. Otherwise, return true. */
    
    bool DeleteFile(int _file_id);
    /* Delete file with given id in the file system; free any disk block occupied by the file. */
   
    // check if file system has given file in its system
    bool CheckFile(int _file_id);
};
#endif

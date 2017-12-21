/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("This is from constructor of file system.\n");
   

   // here initilizing size, freeblock and
    //total file number as all zeros

        filesystem_size = 0;
        filesystem_free_block = 0;
        filesystem_total_files = 0;


   // just initilizing disk to null
        filesystem_disk = NULL;
       
    // making sure file associated with
    // this system points to this file system   
        File::file_system = this;
}


bool FileSystem::Mount(SimpleDisk * _disk) {


    Console::puts("This is from mount of file system\n");
    

        //assign the disk   
        filesystem_disk = _disk;
        // initialize the disk buffer of 512 size as all zeros

        unsigned char disk_buffer[FILESYSTEM_BLOCK_SIZE] = {0,};
        // read from the simple disk to filesystem disk
        filesystem_disk->read(0, disk_buffer);
        // copy size, free block, total files 
        //and file info from disk buffer to each variable


        // first copy the file size to file system file size
        memcpy(&filesystem_size, disk_buffer, 4);


        // copy the number of free blocks in the file system
      
        memcpy(&filesystem_free_block, disk_buffer+4, 4);

        // copy the how many total files are there in the system

        memcpy(&filesystem_total_files, disk_buffer+8, 4);


        // copy the meta data from the temp disk buffer

        memcpy(filesystem_file_info, disk_buffer + FILESYSTEM_DATA_OFFSET, 12 * FILESYSTEM_MAXFILE);


        return true;

}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) {
    Console::puts("This is from format of file system\n");
    
    // create the temp disk buffer of same size as block
    unsigned char disk_buffer[FILESYSTEM_BLOCK_SIZE] = {0,};


        // get temp size to transfer to block
        unsigned int temp_size = _size;


        memcpy(disk_buffer+0, &temp_size, 4); 

        // get temp block length to transfer				
        
        unsigned int temp_filesystem_block_length = FILESYSTEM_BLOCK_LENGTH; 				
        

        memcpy(disk_buffer+4, &temp_filesystem_block_length, 4);
        


       // get holder to transfer to third and remaining blocks
         unsigned int holder = 0; 								
        

        memcpy(disk_buffer+8, &holder, 4);
        

        memset(disk_buffer+12, 0, 500);				
        


        _disk->write(0, disk_buffer);

	
       
       // for each of the remaining block write the zeros disk buffer 

        for(unsigned int block_number = 1; block_number < FILESYSTEM_BLOCK_LENGTH; block_number++) {
                _disk->write(block_number, disk_buffer);
        }


        unsigned int temp_block_number; 


        for(unsigned int block_number = FILESYSTEM_BLOCK_LENGTH; block_number < _size / FILESYSTEM_BLOCK_SIZE - 1; block_number++) {
                unsigned int temp_block2 = block_number +1;

                memcpy(disk_buffer + 508, &temp_block2, 4);
                
                _disk->write(block_number, disk_buffer);

                temp_block_number = block_number;
        }
        // similarly take care of last block in the disk
        int last_block = -1;

        memcpy(disk_buffer + 508, &last_block, 4);

        _disk->write(temp_block_number, disk_buffer);
		

return true;
}

bool FileSystem::CheckFile(int _file_id) {


// check array in file system to find if the file id is present

        for(unsigned int counter = 0; counter < FILESYSTEM_MAXFILE; counter++) {
                
                if(filesystem_file_info[counter][0] == _file_id) {
                        
                        return true;
                }
        }
}


File * FileSystem::LookupFile(int _file_id) {
    Console::puts("This is from look up of filesystem \n");
    
    // check all the files in the file system and return in file is present
        File * temp_file ;    
        

        for(unsigned int counter = 0; counter < FILESYSTEM_MAXFILE; counter++) {
               
                if(filesystem_file_info[counter][0] == _file_id) {
                        
                        temp_file->file_id = _file_id;
                        
                        temp_file->file_size = filesystem_file_info[counter][1];
                        
                        temp_file->file_starting_block = filesystem_file_info[counter][2];
                        
                        return temp_file;
                }
}


}

bool FileSystem::CreateFile(int _file_id) {
    Console::puts("The is from creating file from file system\n");
   

        unsigned char tempdisk_buffer[FILESYSTEM_BLOCK_SIZE];

        unsigned int counter;

         // Check if the file id is already taken or file is already present
        for(counter = 0; counter < FILESYSTEM_MAXFILE; counter++) { 


                if(filesystem_file_info[counter][0] == _file_id){

                        return false;
        
        }
    }
        // find a empty location to put give file
     
        for(counter = 0; counter < FILESYSTEM_MAXFILE; counter++) {
     
                if(filesystem_file_info[counter][0] == 0){

                        break;
        }
    }

        filesystem_file_info[counter][0] = _file_id;
        
        // initialize size as zero
        filesystem_file_info[counter][1] = 0; 


        filesystem_file_info[counter][2] = 0;
        
        // increase the total number of files in the file system
        

        filesystem_total_files++;
return true; 

}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("The is from delete file from file system\n");
    
    unsigned int counter;

     // find the location of the file id in the file information in file system
        for(counter = 0; counter < FILESYSTEM_MAXFILE; counter++) {
               
                if(filesystem_file_info[counter][0] == _file_id){
                        
                        break;

                    }
        }

        // make a temp disk buffer with same size
        unsigned char tempdisk_buffer[FILESYSTEM_BLOCK_SIZE];
        
        //  make sure block is same
        
        unsigned int temp_block = filesystem_file_info[counter][2];

    // initilize holder for next block
        unsigned int temp_block2 = 0;

        while(temp_block != -1) {

        	// while end is not reached
        	// first read and then write 
        	// and empty and free the blocks

                filesystem_disk->read(temp_block, tempdisk_buffer);
                
                memcpy(&temp_block2, tempdisk_buffer+508, 4);

              
                memcpy(tempdisk_buffer+508, &filesystem_free_block, 4);
                
                filesystem_disk->write(temp_block, tempdisk_buffer);
                
                filesystem_free_block = temp_block;

                temp_block = temp_block2;
        }

// clear the filesystem information 2d array
        for (unsigned int i =0; i<3;i++){
        filesystem_file_info[counter][i] = 0;

          }
return true;
}

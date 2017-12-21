/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
FileSystem* File::file_system = 0;

unsigned int File::file_init=0;

File::File() {
    Console::puts("This is constructor of file\n");

    // intitilize the variables of the file
  
  	
  	//file_init =0;
  	
  	file_block = 0;
    
    file_id = file_init;

    file_init = file_init +1;
  
    file_position =0;      

        // check if file id is already created
    bool file_present = file_system->CheckFile(file_id);
     
    if(!file_present){
     file_system->CreateFile(file_id);
     file_size = 0;
     file_starting_block = 0;
       }
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char * _buf) {
    

    Console::puts("This is read from file\n");

    
    // -- we create a temp buffer of 512 size to keep read data from disk    
        unsigned char temp_buf[FILESYSTEM_BLOCK_SIZE];
     // to keep track of read characters
        unsigned int read_character = 0; 

        unsigned int counter = _n;
		
        while(counter) {

                file_system->filesystem_disk->read(file_block, temp_buf);
			
			// now temp buffer has the data
             // find out the position of the file in the block
                unsigned int file_offset = file_position % 508;
            
                unsigned int data_to_read;
             // check if given file can be fit in this block
             // if no then substract what can be fit and save in data to read   
                if (_n > 508 - file_offset)
                        data_to_read = 508 - file_offset;
                else
                        data_to_read = _n ;
                
   
                if(data_to_read > file_size - file_position)
                        data_to_read = file_size - file_position;
                // write the data from temp_buf to given buffer
                memcpy(_buf + read_character, temp_buf + file_offset, data_to_read);

                // add read characters
                read_character = read_character + data_to_read;
                // subtract remaining to read
                _n = _n - data_to_read;
                counter =_n;
                // change file position
                file_position = file_position + data_to_read;

               // if block is finished and it is still not end of file
                // then we need to get another block
                // this is not needed as kernel test fits in one block
 
        }
// return how many characters read in the given buffer		
return read_character;

}


void File::Write(unsigned int _n, const char * _buf) {
    
    Console::puts("This is write of the file\n");
    //Console::puti(file_id);
    //Console::puts("\n");
    unsigned char temp_buf[FILESYSTEM_BLOCK_SIZE]= {0,};
    unsigned int written_char = 0;
    unsigned int counter = _n;


    //unsigned int counter = _n;
		
        while(counter) {

              // get the position in block to read
                unsigned int file_offset = file_position % 508;

                // keep track of data to write
                unsigned int data_to_write;

                // calculate if data can be written in one block or not
                if (_n > 508 - file_offset)
                        data_to_write = 508 - file_offset;
                else
                        data_to_write = _n ;

                 // first write to temp buffer
                memcpy(temp_buf + file_offset, _buf + written_char, data_to_write);
                
                  // now write to disk
                file_system->filesystem_disk->write(file_block, temp_buf);
                
                // update written characters
                written_char = written_char + data_to_write;
                // decrease how much is left to write
                _n = _n - data_to_write;
                counter =_n;
                // change file position for next write
                file_position = file_position + data_to_write;

     
        }

}

void File::Reset() {
    Console::puts("This is reset of the file\n");
 
    file_position = 0;
    file_block = file_starting_block;
}

void File::Rewrite() {
    Console::puts("This is rewrite of the file\n");

  	file_position = 0;
    file_block = 0;

        unsigned char temp_buf[FILESYSTEM_BLOCK_SIZE]= {0,};
        unsigned int temp_block = file_starting_block;
        unsigned int temp_block2 = 0;

        while(temp_block != -1) {
            
                file_system->filesystem_disk->read(temp_block, temp_buf);

                // get next block address 
                memcpy(&temp_block2, temp_buf+508, 4);
                // free this block
                memcpy(temp_buf+508, &(file_system->filesystem_free_block), 4);
                // zero out this block
                file_system->filesystem_disk->write(temp_block, temp_buf);
                // mark this block as free
                file_system->filesystem_free_block = temp_block;

                // change the temp block
                temp_block = temp_block2;
        }
    file_starting_block = 0;
    file_size = 0;

 //Console::puts("erase content of file is done \n");

}


bool File::EoF() {
    Console::puts("This is EOF of the file\n");
   // just check if file size is maxed 
    // if so then end of file is reached
   if(file_size == file_position){
	return true;
        }
       else{
       	return false;
       }
   }

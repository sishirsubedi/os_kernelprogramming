#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"


PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{

   PageTable::kernel_mem_pool = _kernel_mem_pool;
   PageTable::process_mem_pool = _process_mem_pool; 
   PageTable::shared_size = _shared_size;


   // assert(false);
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{

   // here we need to initialize location for the page directory
    // so page directory will ask kernel memory pool to get frames.
	page_directory = (unsigned long *) (kernel_mem_pool->get_frames(1) * PAGE_SIZE);

// similarly page table aslo needs one frame from the kernel mem pool

  unsigned long  * page_table = (unsigned long *) (kernel_mem_pool->get_frames(1) * PAGE_SIZE);

 //Console::puts(" Location of page_directory : ");
 //Console::puti((unsigned long) page_directory);
  //Console::puts("\n");
   //assert(false);
   

  //Console::puts(" Location of page_table : ");
 //Console::puti((unsigned long) page_table);
  //Console::puts("\n");


//
// to initialize first page table with directly physical address 
// 1024 entried of each frame of 4kb size = 4096 kb = 4mb
 // so this section maps until 4mb memory directly to physical address
unsigned long page_initialization = 0;

for ( unsigned int i =0; i<512;i++){
 	page_table[i] = page_initialization | 1; // mark last 1 as 001 to mark read only and valid
 	page_initialization = page_initialization + PAGE_SIZE;
 	 //Console::puti((unsigned long) page_table[i]);
 }

 for ( unsigned int i =512; i<ENTRIES_PER_PAGE;i++){
 	page_table[i] = page_initialization | 3; // mark last 3 as 011 to mark supersor, read/write/, present
 	page_initialization = page_initialization + PAGE_SIZE;
 	 //Console::puti((unsigned long) page_table[i]);
 }

  // assign above create page table as the first entry in the page directory
  page_directory[0] =  (unsigned long) page_table;
  page_directory[0] =  page_directory[0] |3;// make these read/write and valid
//  Console::puts(" Location of page_directory[0] : ");
//  Console::puti((unsigned long) &page_directory[0]);
//   Console::puts("\n");

 //Console::puts(" value of page_directory[0] : ");
  //Console::puti((unsigned long) page_directory[0]);
   //Console::puts("\n");


unsigned long page_directory_initialization = 0;

for ( unsigned int j = 1 ; j<ENTRIES_PER_PAGE;j++){
	page_directory[j]= page_directory_initialization | 2;// mark NOT present

	// Console::puti((unsigned long) page_directory[j]);
    }

 //  Console::puts(" value of page_table[10] : ");
 // Console::puti((unsigned long) page_table[10]);
 //  Console::puts("\n");

 //  Console::puts(" value of page_directory[10] : ");
 //  Console::puti((unsigned long) page_directory[10]);
 //   Console::puts("\n");


}


void PageTable::load()
{
 current_page_table = this;
 write_cr3 ((unsigned long) page_directory); // write page directory location to register cr3 
 
 
   //assert(false);
   //Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{

paging_enabled = 1;


// change the cr0 register bit to mark that paging is enabled
unsigned long p_enable = read_cr0();
p_enable = p_enable | 0x80000000;
write_cr0(p_enable);

//write_cr0(read_cr0() | 0x80000000);

   //assert(false);
  // Console::puts("Enabled paging\n");
//Console::puts(" value of :(unsigned long) read_cr0():: ");
//Console::puti(temp);
//Console::puts("\n");

//for(;;);
}

void PageTable::handle_fault(REGS * _r)
{ 


// we can check what type of error is this by using _r->err_code 

// Console::puts("Handling page fault \n");

// if (_r->err_code & 2 ){
// 	Console::puts("current error is WRITE....");
// }
// else{
// 	Console::puts("current error is READ....");
// }

// if (_r->err_code & 1){
// 	Console::puts("current error is page fault....");
// 	//return;
// }
// else{
// 	Console::puts("current error is page not present....");
// }


// read the address of page table index that is faulted
unsigned long cr2_message = read_cr2();
// use this address to calculated page directory index and page table index
unsigned long page_directory_index = (cr2_message>>22);
unsigned long page_table_index = (cr2_message>>12);
 
//  Console::puts(" cr2 message: ");
//   Console::puti(cr2_message >> 22 );
//    Console::puts("\n");


//  unsigned long cr0_message = read_cr0();
 
//  Console::puts(" cr0 message: ");
//   Console::puti(cr0_message >> 22 );
//    Console::puts("\n");


// get the page directory of current page 
// as this is static we need to get page directory using cr3 register
unsigned long * temp_page_directory = (unsigned long * ) read_cr3(); //= current_page_table->page_directory;

// get the page table in that specific page directory causing page fault
unsigned long * temp_page_table = (unsigned long *) temp_page_directory[page_directory_index];

//  Console::puts(" temp page directory : ");
//   Console::puti((unsigned long) temp_page_directory);
//    Console::puts("\n");


//  Console::puts(" temp directory address : ");
//   Console::puti((unsigned long)   &temp_page_directory );
//    Console::puts("\n");
 //

// Console::puts(" temp_page_directory[cr2_message]: ");
//   Console::puti((unsigned long)   temp_page_directory[cr2_message] );
//    Console::puts("\n");
  


  
  //exit(0);




  //assert(false);
  //Console::puts("handled page fault\n");


//first check if the page directory is empty  or not
//if empty then we need to create new page table and add in it
if((temp_page_directory[page_directory_index] & 1 ) == 0 ){

   // Console::puts(" page table not in memory  \n");
    
   // get new frame for this new page table
   // this one should be from kernel pool as this is page table
    temp_page_table = (unsigned long *) (kernel_mem_pool->get_frames(1)*PAGE_SIZE);

 // add entries in this page table
    for ( unsigned int i =0; i<ENTRIES_PER_PAGE;i++){

     //get new frame for each entry of the pool
    	// this is for frame so, it is from process pool
     unsigned long new_frame = process_mem_pool->get_frames(1);

     // if new frame is zero that means cont frame pools do not have enough frames

     if(new_frame==0){
     	Console::puts(" Donot have enough frames in the pool  \n");
     	return;
     }
    
    //get the address of the frame to add to the new page entry
    unsigned long * page_to_enter = (unsigned long *) (new_frame * PAGE_SIZE);
    //add this new page entry to page table
 	temp_page_table[i] = (unsigned long) (page_to_enter) | 3; // mark last 3 as 011 to mark supersor, read/write/, present
 	//page_initialization = page_initialization + PAGE_SIZE;
 	 //Console::puti((unsigned long) page_table[i]);
 }


// once page table is filled , now add this page to page directory where fault had occured
temp_page_directory[page_directory_index] = (unsigned long) temp_page_table ;
temp_page_directory[page_directory_index] |= 3;
}


// check if the page table entry was creating the fault
// inthis case we need to get new frame and add its address to this page table index
if((temp_page_table[page_table_index] & 1 ) == 0) {

// this is same step as above - get one frame from process pool 
	// and add it to page index
unsigned long new_frame = process_mem_pool->get_frames(1);
unsigned long * page_to_enter = (unsigned long *) (new_frame * PAGE_SIZE);

temp_page_table[page_table_index] = (unsigned long) (page_to_enter) | 3;

}


}




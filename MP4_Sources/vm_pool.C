

/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "page_table.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table):
base_address (_base_address),
frame_pool(_frame_pool),
page_table(_page_table)
{


// _page_table->register_pool(this);


// initialize number of vmpools as zero
region_counter = 0;

// calculate maximum number of vmpools that can fit in one page
max_allowed = PageTable::PAGE_SIZE/sizeof(vmpool_info);

// get frame to store vmpool information
current_vmpool_info = (vmpool_info *)(PageTable::PAGE_SIZE * (frame_pool->get_frames(1)));


//pointer to point to previous and next vmpools
  // VMPool * vmnext = NULL;
   //VMPool * vmprev = NULL;


// if(VMPool::vmhead == NULL){
	
// 	VMPool::vmhead = this;
// 	VMPool::vmtail = this;

// 	vmnext = NULL;
//     vmprev = NULL;
// }
// else
// {
	
// 	VMPool::vmtail->vmnext = this;
// 	this->vmnext = VMPool::vmtail;

// 	VMPool::vmtail= this;
// }


page_table->register_pool(this);


}

unsigned long VMPool::allocate(unsigned long _size) {

// start from 0 
   unsigned long starting_address = 0;
	
unsigned long requested_size = _size;

if(region_counter ==0){

	// if this is the first vmpool then starting address will be same as base address

       starting_address = base_address;

}else{

// if this is not first then starting address will be where last region ends
starting_address = current_vmpool_info[region_counter-1].start_address + current_vmpool_info[region_counter-1].size;

	}

// initialize current vmpool start address and size	
    current_vmpool_info[region_counter].start_address = starting_address;
    current_vmpool_info[region_counter].size = _size;
// increase the number of regions
    region_counter = region_counter + 1;

    return starting_address;


    //assert(false);
    //Console::puts("Allocated region of memory.\n");
}

void VMPool::release(unsigned long _start_address) {
    //assert(false);
    //Console::puts("Released region of memory.\n");
	
    unsigned int releasing_region =0;

// check if the releasing region belongs to any allocated vmpools region
    for (unsigned int i =0; i<region_counter; i++){

     if(current_vmpool_info[i].start_address == _start_address){
     releasing_region = i; break;
     }
    }// couting how many page tables are present in this region
    // and then freeing these page tables
    for(unsigned int j =0; j<current_vmpool_info[releasing_region].size; j++){

    page_table->free_page(_start_address);
    _start_address = _start_address + PageTable::PAGE_SIZE;
    }
    
// get a new temp pool to point to current vm pool so that we can update it
    vmpool_info * temp_pool = current_vmpool_info;
    current_vmpool_info = (vmpool_info *)(PageTable::PAGE_SIZE * (frame_pool->get_frames(1)));

    unsigned int newindex = 0;
    // update new vmpool in place of old one
      for (unsigned int nondelete_index=0;nondelete_index<region_counter;++nondelete_index){
            if (nondelete_index!=releasing_region)
                current_vmpool_info[newindex]=temp_pool[nondelete_index];
                 newindex = newindex + 1;
        }
// release old vm pool
        frame_pool->release_frames((unsigned long)temp_pool/PageTable::PAGE_SIZE);

      page_table->load();

    

    }


bool VMPool::is_legitimate(unsigned long _address) {
    //assert(false);
    //Console::puts("Checked whether address is part of an allocated region.\n");


// check if given address is within range of which region in the vmpool
 if(!current_vmpool_info){
            if((current_vmpool_info[0].start_address <= _address) && (_address < (current_vmpool_info[0].start_address + current_vmpool_info[0].size))){
            
                return true;}
return false;}

    
}

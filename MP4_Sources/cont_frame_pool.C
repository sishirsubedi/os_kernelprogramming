/*
 File: ContFramePool.C
 
 Author:SISHIR SUBEDI
 Date  : JUNE 19,2017
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool* ContFramePool::head = NULL;
ContFramePool* ContFramePool::tail = NULL;



ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
 // design is 1 bitmap for each frame we do not need to multiply by 8
   // assert(_n_frames <= FRAME_SIZE * 8);
    
    base_frame_no = _base_frame_no; 
    nframes = _n_frames; 
    nFreeFrames = _n_frames; 
    info_frame_no = _info_frame_no; 
    n_info_frames = _n_info_frames;
    
    ContFramePool*  next = NULL;
    ContFramePool*  prev = NULL;

// If _info_frame_no is zero then we keep management info in the first
    //frame, else we use the provided frame to keep management info

/*
what this line does is assigns bitmap a location, and a location is determined by
the frame size, so if frame frame number is 0 then the bit map location is frame size,
and if its 512 then location is 512 time frame size.

idea frame starts at the multiple of framesize.

512 actually starts at 2MB, 4KB TIMES 512

 Bit map points to the start of that specifc frame
*/

if (info_frame_no == 0)
{
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE);

}
else 
{
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
     
}

// Number of frames must be "fill" the bitmap!
//    assert ((nframes % 8 ) == 0);
    
    

    for(int i=0; i < nframes; i++) 
    {
        bitmap[i] = FREE; 
    }
    

    if(_info_frame_no == 0) {
        bitmap[0] = ALLOCATED; 
        nFreeFrames--;
    }
	


   if ( ContFramePool::head == NULL){

   	  ContFramePool::head = this;
   	  ContFramePool::tail = this;

   	  next = NULL;
   	  prev = NULL;
   }
   else
   {

     //ContFramePool* temp = tail;

     ContFramePool::tail->next = this;
     this->prev= tail;

     ContFramePool::tail = this;


   }

	// if(ContFramePool::fplist==NULL){

	// 	ContFramePool::fplist = this;
	// 	this->next = NULL;
	// 	this->prev = NULL;
	// 	Console::puts(" current-1 "); Console::puti(ContFramePool::fplist->get_base_frame_no()); Console::puts("\n");
		
	// }
	// else{
	// 	ContFramePool::fplist->next = this;
	// 	ContFramePool::fplist->next->prev = ContFramePool::fplist;
	// 	ContFramePool::fplist = this; 

 //        Console::puts(" current "); Console::puti(ContFramePool::fplist->get_base_frame_no()); Console::puts("\n");
	// 	Console::puts(" previous "); Console::puti(ContFramePool::fplist->prev->get_base_frame_no()); Console::puts("\n");
	
	// 	}
    
    Console::puts("Frame Pool initialized\n");
}




unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
  //Console::puts(" current free frames = "); Console::puti(nFreeFrames); Console::puts("\n");
	
// Any frames left to allocate?
    assert(nFreeFrames >= _n_frames);

// Find a frame that is not being used and return its frame index.
    // Mark that frame as being used in the bitmap.

// requested_frames to track requested frames 
    unsigned int requested_frames = _n_frames;
// base frame no if there are n_frames available
    unsigned int frame_number = base_frame_no;  
// keep track of frame index from which number of frames are free
    unsigned int free_index = 0;
     
//start from 0 to check if there are contigious free frames with n frames
for(int start=0; start<nframes ; start++)
{
    if( bitmap[start]== FREE) // if found a free frame then start checking from this point
    {
        free_index =start;
        if((nframes- free_index)>=requested_frames) // make sure the difference is enough to cover n frames
        {  // start from second frame to find n free frames
                for(int second=1; second<=requested_frames; second++)
                {
                    if(bitmap[start+second] != FREE)
                  {
                        start=start+second;break; // move start to new start if found non free frame
                  }
               else if(second==requested_frames)
                {   // when we found there are enough n free frames
                        // start with HOS
                        bitmap[free_index]=HEAD_OF_SEQUENCE;nFreeFrames--;
                        frame_number =  free_index + base_frame_no;
                        //proceed to remaining frames in the pool
                        for (int i=1; i<requested_frames ; i++)
                        {
                            bitmap[free_index+i]= ALLOCATED;
                            start=nframes; nFreeFrames--;  
                        } 
       return(frame_number);
                    }

                }
        }
     else
     {
       Console::puts("Error, Insufficient space left\n");
       return(0);
      }
    }

}

return (0);

}
 // function to return base frame number for static function
 unsigned long   ContFramePool::get_base_frame_no(){
        return base_frame_no;
    }



void ContFramePool::mark_inaccessible(unsigned long _base_frame_no, unsigned long _n_frames)
{
    unsigned int block = _base_frame_no + _n_frames -1;
    //assert ((_base_frame_no > base_frame_no) && (block <= nframes)); 
  
    
// start from base frame no to check its free

    if ((_base_frame_no>=base_frame_no) && (_n_frames <= nframes))
    {
      int counter=0;

    if(bitmap[_base_frame_no]==FREE)
    {
	for(counter=1; counter<_n_frames; counter++)
	  {
	   if (bitmap[_base_frame_no+counter]!=FREE) 
	   {
     	     // if any of  these frames in the block are not free then 
	    // assert(false);
	    Console::puts("No Free Frames \n");
           }
	  }
    }
    else

    {
        Console::puts("No Free Frames \n");	
    }
   
    // if there are enough free frames in the block to mark inaccessible
    if(counter==_n_frames)
    {
 	for (int i=0;i<_n_frames;i++)
	{
	     bitmap[_base_frame_no+i]=HEAD_OF_SEQUENCE; // mark HOS to make in inaccessible
	}
       
    }
}
}

bool ContFramePool::check_frames(unsigned long _first_frame_no){
	

//Console::puts("frame no ");Console::puti(_frame_no); Console::puts("\n");

//Console::puts("--bitmap");Console::puti(bitmap[_frame_no]); Console::puts("\n");

if( base_frame_no <= _first_frame_no && _first_frame_no <= base_frame_no + nframes){
	return true;

}
return false;
}




void ContFramePool::release_this_frames(unsigned long  fnum  )
{
  //subtract base frame number to get frame to release
    unsigned int frame_to_release = fnum - base_frame_no;
    unsigned int COUNTER=0;
// check to find head of sequence and once head of sequence is found
    // then release following allocated frames
    if(bitmap[frame_to_release]==HEAD_OF_SEQUENCE)
    {
        bitmap[frame_to_release]=FREE;
        COUNTER++;
        frame_to_release++;
        nFreeFrames++;
        
        while (bitmap[frame_to_release]==ALLOCATED)
        {
            bitmap[frame_to_release]=FREE;
            frame_to_release++;
            COUNTER++;
            nFreeFrames++;
        }
       // Console::puts("Successfully Released all frames !\n")
 
    }


   // for(int start=_first_frame_no -  base_frame_no; start< nframes ; start++)
   // {
   // 	      bitmap[start]= FREE;
   //        nFreeFrames++;  

   //   // Console::puts("RELEASING correct frame \n") 
   // }
 
  }



void ContFramePool::release_frames(unsigned long _first_frame_no)
{
   //assert(false);

    ContFramePool* temp = head;

    if(head!=NULL){

    while (!(temp->check_frames(_first_frame_no))){

    	temp = temp->next;
    	//Console::puts("searching correct frame \n");

    	if(temp==NULL){
    		//Console::puts("DID NOT FIND correct frame \n");
    		break;
    	}

    }

    temp->release_this_frames(_first_frame_no);
    //Console::puts(" base_frame_no = "); Console::puti(temp->get_base_frame_no()); Console::puts("\n");
   // Console::puts(" no.of frames = "); Console::puti(temp->nframes); Console::puts("\n");


   }
  }




unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{   // since this is static function cant access n_frames
   // way out it to declare another variable
    unsigned int substitute_n_frames=0;
   // Console::puts("Bitmap started");
  // since we have equal number of frames in bitmap as frame size , so 4096
        if(_n_frames<4095)
        {
                substitute_n_frames=1;
        } // if more than one then will not fit in one frame number so error
        else if (_n_frames>4095 && _n_frames <= 8190){
        	substitute_n_frames =2;
        }
        else
        {
        Console::puts("ERROR- Needed_info_frames\n");
        }

return substitute_n_frames;
 
}


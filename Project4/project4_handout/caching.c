#include <stdio.h>
#include <stdlib.h>
#include "memory_system.h"

typedef struct Cache{
  int tag;
  int valid;
  int data; 
  //timemap to check 
  int age;    
}Caches;

static Caches cachaddr[32][2];
static int tlb[16];

void  
initialize() {
/* if there is any initialization you would like to have, do it here */
/*  This is called for you in the main program */

//initialize tlb & cache 

  for(int i=0; i<16; i++){
    tlb[i]=0;
  }

  for(int i=0; i<32; i++){
    for(int j=0; j<2; j++){
      cachaddr[i][j].tag=0;
      cachaddr[i][j].data=0;
      cachaddr[i][j].valid=0;
      cachaddr[i][j].age=0;  
    }
  }
}

/* You will implement the two functions below:
 *     * you may add as many additional functions as you like
 *     * you may add header files for your data structures
 *     * you MUST call the relevant log_entry() functions (described below)
 *          or you will not receive credit for all of your hard work!
 */

/*
Convert the incoming virtual address to a physical address. 
     * if virt_address too large, 
          log_entry(ILLEGALVIRTUAL,virt_address); 
          return -1
     * if PPN is in the TLB, 
    compute PA 
          log_entry(ADDRESS_FROM_TLB,PA);
          return PA
     * else use the page table function to get the PPN:
          PPN = get_page_table_entry(VPN) // provided function
          compute PA 
          log_entry(ADDRESS_FROM_PAGETABLE,PA);
          return PA
*/

int
get_physical_address(int virt_address) {

  int phys_address = 0;
  //check if virt_address too large
  int temp= virt_address>>18;
  if(temp>0)
  { 
    log_entry(ILLEGALVIRTUAL,virt_address); 
    return -1;
  }

  //convert
  int vpn = virt_address >> 9; //first 9
  int vpo = virt_address & 0x1ff; //last 9

  int vpn_tag = vpn >> 4; //first 5 of VPN
  int vpn_index = vpn & 0xf;//remaing 4 of VPN

  //check tlb, index, tag, validity
  int tlbaddr=tlb[vpn_index];
  int ppn;

  //if PPN is in the TLB
  if((tlbaddr>>12) == vpn_tag){
    if(((tlbaddr>>11) & 0x1)==1){ //valid

      ppn = tlbaddr & 0x7ff;
      phys_address= (ppn<<9) | vpo;
      log_entry(ADDRESS_FROM_TLB,phys_address);
    }
    else{//invalid, tag right

      ppn = get_page_table_entry(vpn);
      tlb[vpn_index]= (tlbaddr|0x800)|ppn;
      phys_address= (ppn<<9) | vpo;
      log_entry(ADDRESS_FROM_PAGETABLE,phys_address);
    }
  }
  //if PPN is in the page table
  else{//tag wrong
  	
    ppn = get_page_table_entry(vpn);
    tlb[vpn_index]= (vpn_tag<<12)|0x800|ppn;
    phys_address= (ppn<<9) | vpo; 
    log_entry(ADDRESS_FROM_PAGETABLE,phys_address);
  }
  return phys_address;
}

/*
   Use the incoming physical address to find the relevant byte. 
     * if data is in the cache, use the offset (last 2 bits of address)
          to compute the byte to be returned data
          log_entry(DATA_FROM_CACHE,byte);
          return byte 
     * else use the function get_long_word(phys_address) to get the 
          4 bytes of data where the relevant byte will be at the
          given offset (last 2 bits of address)
          log_entry(DATA_FROM_MEMORY,byte);
          return byte

NOTE: if the incoming physical address is too large, there is an
error in the way you are computing it...
*/

char 
get_byte(int phys_address) {

  //check if phys_address too large
  int check_length= phys_address>>20;
  if(check_length>0)
  { 
    log_entry(PHYSICALERROR,phys_address); 
    return 0;
  }

  int byte;

  //get the tag, index, location from phys_address
  int tag=phys_address>>7;
  int index=((0x1f<<2)&phys_address)>>2;  
  int byte_addr= phys_address&0x3;

  //calculate the location beacuse of little endian
  int fixbyte_addr= byte_addr*8;
  int moveVal=0xff<<fixbyte_addr;

    //first 13 is tag, next valid bit, next oldest bit, 4 bytes data
  if(cachaddr[index][0].tag==tag && cachaddr[index][1].tag !=tag){
      if(cachaddr[index][0].valid==1){ //valid

        byte=(cachaddr[index][0].data & moveVal)>>(fixbyte_addr);
        log_entry(DATA_FROM_CACHE,byte);
      }
      //else wont happen 
    }
    //valid 
  else if(cachaddr[index][1].tag==tag && cachaddr[index][0].tag !=tag){//2nd place check
      if(cachaddr[index][1].valid==1){ //valid

        byte=(cachaddr[index][1].data & moveVal)>>(fixbyte_addr);
        log_entry(DATA_FROM_CACHE,byte);
      }
      //first valid 2nd invalid but right tag(0), grab and put in 2nd
      else{

        byte=get_word(phys_address);
        cachaddr[index][1].data=byte;
        cachaddr[index][1].age=1;
        cachaddr[index][0].age=0;
        cachaddr[index][1].valid=1;
        byte=(cachaddr[index][1].data & moveVal)>>(fixbyte_addr);       
        log_entry(DATA_FROM_MEMORY,byte);
      }
  }
  //both invalid or only 1st is invalid
  else if(cachaddr[index][0].valid==0){

    byte=get_word(phys_address);
    cachaddr[index][0].tag=tag;
    cachaddr[index][0].data=byte;
    cachaddr[index][0].age=1;
    cachaddr[index][0].valid=1;
    byte=(byte&moveVal)>>(fixbyte_addr);
    log_entry(DATA_FROM_MEMORY,byte);
  }
  //2nd invalid but 1st is valid  
  else if(cachaddr[index][1].valid==0){

    byte=get_word(phys_address);
    cachaddr[index][1].tag=tag;
    cachaddr[index][1].data=byte;
    cachaddr[index][1].age=1;
    cachaddr[index][1].valid=1;
    cachaddr[index][0].age=0;
    byte=(byte&moveVal)>>(fixbyte_addr);
    log_entry(DATA_FROM_MEMORY,byte);
  }
  //else missed both and no invalid
  else{
    //which is older
    int old;
    if(cachaddr[index][0].age==0)
    	old=0;
    else
    	old=1;

    //replace old
    byte=get_word(phys_address);
    cachaddr[index][old].data=byte;
    cachaddr[index][old].age=1;//make new
    cachaddr[index][old].tag=tag;//append tag
    cachaddr[index][!old].age=0;
    byte=(byte&moveVal)>>(fixbyte_addr);
    log_entry(DATA_FROM_MEMORY,byte); 
  }   
  return byte;
}
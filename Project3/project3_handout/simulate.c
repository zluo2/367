#include <stdio.h>
#include <stdlib.h>


int Pop(int), Push(int), Call(int), Ret(int), Jump(int);
int IRmov(int), RRmov(int), RMmov(int), MRmov(int); //IRmov pass
int OPx(int);
int printall(int), printreg(int),printmem(int);

char SF = 0x00;
char ZF = 0x00;

extern char memory[] ;
extern int PC ;

long int regs[16];

char *regname[15] = {"%rax","%rcx","%rdx","%rbx","%rsp","%rbp",
                "%rsi","%rdi","%r8","%r9","%r10","%r11","%r12","%r13","%r14"};



void
execute(int PC) {
   int done = 0;
   char byte;
   char opcode ;

   while (!done) {
      byte = memory[PC];
      opcode = (byte >> 4)&0xf;
      switch (opcode) {

      	 case 0: printf("Halting at instruction 0x%x\n",PC);  //halt
      		       done = 1;
            break;
      	 case 1: PC++;  // nop
            break;
      	 case 2: PC = RRmov(PC);
      		  break;
      	 case 3: PC = IRmov(PC);
      		  break;
      	 case 4: PC = RMmov(PC);
      		  break;
      	 case 5: PC = MRmov(PC);
      		  break;
      	 case 6: PC = OPx(PC);
      		  break;
      	 case 7:  PC = Jump(PC);
      		  break;
      	 case 8: PC = Call(PC);
      		  break;
      	 case 9: PC = Ret(PC);
      		  break;
      	 case 10: PC = Push(PC);
      		  break;
      	 case 11: PC = Pop(PC);
      		  break;
         case 12: PC = printall(PC);
      		  break;
      	 case 13: PC = printreg(PC);
      		  break;
      	 case 14: PC = printmem(PC);
      		  break;
      	 default:
      	 	printf("# unknown op at PC=0x%x\n",PC); PC++;
      }
   }
}

//Single byte instruction 0xC0 – prints out the values stored in all 15 registers
int
printreg(int PC) {

  //get the register index 
  char byte = memory[PC+1];
  int num = (byte >> 4) & 0xf;

  printf("%s：0x%lx\n", regname[num], regs[num]);
  return PC+2;
}

//Two byte instruction 0xD0, 0xrA:F – print out the value of the register indicated by rA
int
printall (int PC) {

  for(int i=0; i<15; i++)
    printf("%d: 0x%lx\n", i, regs[i]);

  return PC+1;
}

//10 byte instruction 0xE0, 0xrA:F, 8 byte displacement – print out the 8 byte constant stored at address contents(rA) + displacement.
int
printmem(int PC) {

  //get the register index
   char byte = memory[PC+1];
   int num = (byte >> 4) & 0xf;
  //calcu the displacement 
   int displacement = 0;
   int count = 0;
   for (int i=PC+2;i<PC+10;i++)
   {
     displacement += ((memory[i] & 0xff)<<count);
     count += 8;
   }
   //calcu the newValue for the regs[num]+displacement in the memory
   int address = regs[num]+displacement;
   long int value = 0;
   int count1 = 0;
   for (int i= address; i<address+8; i++)
   {

   	 long int temp = memory[i] & 0xff;
   	 temp = temp << count1;
     value = value | temp;
     count1 += 8;
      
   }
  
   printf("0x%lx: 0x%lx\n",address,value); //need to solve the fffffff

   return PC + 10;
}

int
IRmov(int PC) {

  char byte = memory[PC+1];
  char temp1 = (byte >> 4);
  char temp2 = byte - (temp1 << 4);

  //get the fn and rB
  int f = temp1 & 0xf;
  int rB = temp2 & 0xf;
  //calcu the ValE
  long int value = 0;
  int count = 0;
  for (int i=PC+2;i<PC+10;i++)
  { 
    value += ((memory[i] & 0xff)<<count);
    count += 8;
  }
  //R[rB] = ValE
  regs[rB] = value;

  return PC+10;
}

//pass
int
RRmov(int PC) {

  char byte = memory[PC];
  char temp1 = (byte >> 4);
  char temp2 = byte - (temp1 << 4);
  // get the opreations fn
  int fn = temp2 & 0xf;

  char byte1 = memory[PC+1];
  char temp3 = (byte1 >> 4);
  char temp4 = byte1 - (temp3 << 4);
  //get the rA and rB
  int rA = temp3 & 0xf;
  int rB = temp4 & 0xf;

  switch(fn)
  {
    //rrmovq
    case 0:
      regs[rB] = regs[rA];
      break;
    //comvle
    case 1:
      if (ZF != 0x00 || SF != 0x00) 
        regs[rB] = regs[rA];
      break;
    //comvl
    case 2:
      if (ZF == 0x00 && SF !=0x00) 
        regs[rB] = regs[rA];
      break;
    //comve
    case 3:
      if (ZF != 0x00 && SF == 0x00) 
        regs[rB] = regs[rA];
      break;
    //comvne 
    case 4:
      if (ZF == 0x00) 
        regs[rB] = regs[rA];
      break;
    //comvge
    case 5:
      if (ZF != 0x00 && SF == 0x00) 
        regs[rB] = regs[rA];
      break;
    //comovg
    case 6:
      if (ZF == 0x00 && SF == 0x00) 
        regs[rB] = regs[rA];
      break;
  }

  return PC+2;
}

//pass 
int
RMmov(int PC) {

  char byte = memory[PC+1];
  char temp1 = (byte >> 4);
  char temp2 = byte - (temp1 << 4);
  //get the rA and rB
  int rA = temp1 & 0xf;
  int rB = temp2 & 0xf;
  //get the ValC from the memory
  long int value = 0;
  int count = 0;
  for (int i=PC+2;i<PC+10;i++)
  {
    long int temp = memory[i] & 0xff;
   	temp = temp << count;
    value = value | temp;
    count += 8;
  }
  //add = valE
  int address = regs[rB]+value;

 // memory[newValue] = regs[rA]; 
  //M[ValE] = ValA(R[rA])
  long int valA = regs[rA];
  for(int i=address; i<address+8; i++)
  {
    char temp = valA & 0xff;
    memory[i] = temp;
    valA = valA >> 8;
  }

  return PC+10;
}

//pass
int
MRmov(int PC) {

  char byte = memory[PC+1];
  char temp1 = (byte >> 4);
  char temp2 = byte - (temp1 << 4);
  //get the rA and rB
  int rA = temp1 & 0xf;
  int rB = temp2 & 0xf;
  //get the valC from the memory 
  int value = 0;
  int count = 0;
  for (int i=PC+2;i<PC+10;i++)
  {
    long int temp = memory[i] & 0xff;
   	temp = temp << count;
    value = value | temp;
    count += 8;
  }
  //address = ValE
  int address = regs[rB]+value;
  long int newValue = 0;
  int count1 = 0;
  for (int i= address; i<address+8; i++)
  {
    long int temp = memory[i] & 0xff;
   	temp = temp << count1;
    newValue = newValue | temp;
    count1 += 8;
  }
  //newValue is M[valE]
  regs[rA] = newValue;

  return PC+10;
}

//pass
int
Jump(int PC) {

//using for debug 
/*
  if (ZF == 0x00)
    printf("%s\n", "ZF == 0X00");
  else 
    printf("%s\n", "ZF != 0X00");

  if (SF == 0x00)
    printf("%s\n", "SF == 0X00");
  else
    printf("%s\n", "ZF == 0x00");
*/

  char byte = memory[PC];
  char temp1 = (byte >> 4);
  char temp2 = byte - (temp1 << 4);
  // get the opreations fn
  int fn = temp2 & 0xf;
//  printf("fn = %d\n",fn);

  //calcu the destination 
  int dest = 0;
  int count = 0;
  for (int i=PC+1;i<PC+9;i++)
  {
    dest += ((memory[i] & 0xff)<<count);
    count += 8;
  }

  switch (fn)
  {
    //jmp
    case 0:
      PC = dest;
      break;
    //jle
    case 1:
      if (ZF != 0x00 || SF != 0x00) 
      	//change the PC to the destination 
        PC = dest;
      else 
        PC += 9;
      break;
    //jl
    case 2:
      if (ZF == 0x00 && SF !=0x00) 
        PC = dest;
      else 
        PC += 9;
      break;
    //je
    case 3:
      if (ZF != 0x00 && SF == 0x00) 
        PC = dest;
      else 
        PC += 9;
      break;
    //jne 
    case 4:
      if (ZF == 0x00) 
        PC=dest;
      else 
        PC += 9;
      break;
    //jge
    case 5:
      if (ZF != 0x00 && SF == 0x00) 
        PC = dest;
      else 
        PC += 9;
      break;
    //jg
    case 6:
      if (ZF == 0x00 && SF == 0x00) 
        PC = dest;
      else 
        PC += 9;
      break;
  }

	return PC;
}

//Push address of next instruction onto stack
//Start executing instructions at Dest

int
Call(int PC) {
  //get the ValC
  long int valC = 0;
  int count = 0;
  for (int i=PC+1;i<PC+9;i++)
  {
    valC += ((memory[i] & 0xff)<<count);
    count += 8;
  }
  //valP <- PC +9
  int valP = PC + 9;
  PC = valC;
  //valE <-rsp-8
  regs[4] = regs[4]-8;
  //M[valE] = valP
  for(int i=regs[4]; i<regs[4]+8; i++)
  {
    char temp = valP & 0xff;
    memory[i] = temp;
    valP = valP >> 8;
  }

  return PC;
}

//Pop value from stack
//Use as address for next instruction

int
Ret(int PC) {

  //valA <-rsp
  int address = regs[4];
  //valM(newValue) <- M[valA]
  long int newValue = 0;
  int count = 0;
  for (int i= address; i<address+8; i++)
  {
    long int temp = memory[i] & 0xff;
   	temp = temp << count;
    newValue = newValue | temp;
    count += 8;
  }

  PC = newValue;
  //valE
  regs[4] = regs[4]+8;

  return PC;
}

//Decrement %rsp by 8
//Store word from rA to memory at %rsp

int
Push(int PC) {

  char byte = memory[PC+1];
  int rA = (byte >> 4) & 0xf;

  regs[4]-=8;
 
  long int valP = regs[rA];
  for(int i=regs[4]; i<regs[4]+8; i++)
  {
    char temp = valP & 0xff;
    memory[i] = temp;
    valP = valP >> 8;
  }

  PC += 2;
  return PC;
}

//Read word from memory at %rsp
//Save in rA
//Increment %rsp by 8

int
Pop(int PC) {

  char byte = memory[PC+1];
  int rA = (byte >> 4) & 0xf;

  //regs[rA] = memory[regs[4]];
  long int tempValue = 0;
  int count = 0;
  for (int i=regs[4];i<regs[4]+8;i++)
  {
    long int temp = memory[i] & 0xff;
   	temp = temp << count;
    tempValue = tempValue | temp;
    count += 8;
  }
  regs[rA] = tempValue;
  regs[4]+=8;

  PC += 2;
  return PC;
}

int
OPx(int PC) {

  //init CC
  ZF = 0x00;
  SF = 0x00;

  char byte = memory[PC];
  char temp1 = (byte >> 4);
  char temp2 = byte - (temp1 << 4);
  //get the opreations fn
  int fn = temp2 & 0xf;

  char byte2 = memory[PC+1];
  char temp3 = (byte2 >> 4);
  char temp4 = byte2 - (temp3 << 4);
  //get the fa and fb
  int rA = temp3 & 0xf;
  int rB = temp4 & 0xf;
  //get the value 
  long int valA = regs[rA];
  long int valB = regs[rB];

  switch (fn)
  { 
    //add
    case 0:
      valB += valA;
      regs[rB] = valB;
      break;
    //sub
    case 1:
      valB -= valA;
      regs[rB] = valB;
      break;
    //and
    case 2:
      valB = valA & valB;
      regs[rB] = valB;
      break;
    //xor
    case 3:
      valB = valA ^ valB;
      regs[rB] = valB;
      break;
  }

  //set the CC
  if (valB == 0) ZF = 0x01;
  if (valB < 0) SF = 0x01;

  PC = PC+2;
  return PC;
}


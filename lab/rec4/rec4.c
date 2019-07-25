#include <stdio.h>   /* printf  */
#include <string.h>  /* strncmp, strdup */
#include <stdlib.h>  /* atoi, strtol    */

extern long         func1 (long, long, long, long);
extern long         func2 (long);
extern long         func3 (long[], long);
extern long         func4 (long, long);
extern long         func5 (long);

/*
receives command line arguments for:
 #1 function to be tested
 #2 first argument
 (#3+: more arguments, or array values for the last argument)
*/
int main(int argc, char ** argv){
  /* check for minimum number of arguments */
  if (argc<3){
    printf("error - not enough arguments.\n\n\tusage: ./rec4 funcname arg1 arg2 arg3 ...\n\n");
    return 1;
  }
  
  /* read in first function argument (always a long) */
  long n;
  sscanf(argv[2],"%ld",&n);
  
  /* dispatch to the correct function */
  char* funcName = argv[1];
  
  
  if ( ! strncmp("func1",funcName,10)){
    long a = n;
    long b, c, d;
    sscanf(argv[3],"%ld",&b);
    sscanf(argv[4],"%ld",&c);
    sscanf(argv[5],"%ld",&d);
    printf("%ld\n",func1(a,b,c,d));
  }
  
  else if ( ! strncmp("func2", funcName,10)){
    printf("%ld\n",func2 (n));
  }
  
  else if ( ! strncmp("func3",funcName,10)){
    /* get the array items, build array. */
    long xs [n];
    for (int i=0; i<n; i++){
      sscanf(argv[i+3],"%ld",&xs[i]);
    }
    printf("%ld\n",func3(xs,n));
  }
  
  else if ( ! strncmp("func4",funcName,10)){
    long x = n;
    long y;
    sscanf(argv[3],"%ld",&y);
    printf("%ld\n",func4(x,y)); 
  }

  else if ( ! strncmp("func5", funcName,10)){
    printf("%ld\n",func5(n));
  }
  
  /* give a helpful message when the function is misspelled. */
  else {
    printf("error - unrecognized command '%s'.\n",argv[1]);
    return 2;
  }
}

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// initalize file[] to store the original file.
// total_length record the length of original file.
unsigned char *file = NULL;
int total_length = 0;

// mycmp compare two string by reading file[]. a and b are the first position that in the file[], if they
// are the same, we will look at the next position of character, until we get the finial result.
int mycmp(int a, int b) {
   int first = a;   
   int second = b;   
   int left_length = total_length;   
   // if the positions are the same, return 0.         
   if(first == second) 
      return 0; 
   // if file[first] equals file[second], get the next character and compare again, until we get the result.
   while (file[first] == file[second])  {
      first++;
      second++;
      left_length--;  
      if (first == total_length)  
         first = 0;  
      if (second == total_length)  
         second = 0;  
      if (left_length < 0)  
         return 0;           
   }  
   // if file[first] is larger than file[second], return 1, else return -1.
   if (file[first] > file[second]) {
      return 1; 
   }  
   else {
      return -1;  
   } 
      
}

// Use quciksort to sort file[]. We transfer file[] via array[].
void quicksort(int *array, int left, int right) {
    int i = left; 
    int j = right;
    int tmp;
    // use the middle element as pivot.
    int pivot = array[(left + right) / 2];
    
    /* partition */
    while (i <= j) {
       // compare two string, if pivot is bigger than array[i], go to the next element by i++.
       while (true) {
          if (mycmp(pivot, array[i]) == 1) {
             i++;
          }
          else 
            break;
       }
       // compare two string, if pivot is smaller than array[j], go to the forward element by j--.
       while (true) {
          if (mycmp(array[j], pivot) == 1) {
             j--;
          }
          else
            break;
       }
       if (i <= j) {
          tmp = array[i];
          array[i] = array[j];
          array[j] = tmp;
          i++;
          j--;
       }
    }    
      /* recursion */
    if (left < j)
       quicksort(array, left, j);
    if (i < right)
       quicksort(array, i, right);
}

int main(int argc, char **argv) {

   int start_position;
   int c;
   int i;
   // count the total length of original file.
   FILE *const input_file_len = fopen(argv[1], "r");
   fseek(input_file_len, -1, SEEK_END);
   total_length = ftell(input_file_len) + 1;
   fclose(input_file_len);
   // file[] store the original file in it.  
   file = malloc(total_length * sizeof(unsigned char));
   
   // start read input file, finish c table and record the total length of this input file.
   i = 0;
   FILE *const input_file = fopen(argv[1], "r");
   while ((c = getc(input_file)) != EOF) { 
      file[i] = c; 
      i++;       
   } 
   fclose(input_file);
   
   // file_bwt[] will store the final result(ascii code) of bwt file.
   unsigned char *file_bwt = malloc(total_length * sizeof(unsigned char));
   
   // We initalize the position for each character in file[]. So the position of character:file[0] is 0, the position of character:file[1] is 1 and so on. When we are sorting file[], we sort the elements of first_order, and read each value from first_order[], we know the bwt order.
   int *first_order = malloc(total_length * sizeof(int));
   for (i = 0; i < total_length; i++) {
      first_order[i] = i;
   }  
   // quciksort will sort the first_order[], and we can get the ascii code for each element in file_bwt[] from the elements of sorted first_order[].
   quicksort(first_order, 0, total_length - 1);
   
   for (i = 0; i < total_length; i++) {
      // out_pos record the position from file_in[]  to file_out[]. i.e. file_in[3] should be at file_out[6], then i will be 6 in this case, and (first_order[i] + total_length - 1) % total_length should be 3.
      int out_pos = (first_order[i] + total_length - 1) % total_length;
      file_bwt[i] = file[out_pos];
   } 
   
   // find the start position.
   for (i = 0; i < total_length; i++) {  
      if (first_order[i] == 0) {  
         start_position = i;    
      }           
   }  
   free(first_order);
   
  
   FILE *const output_file = fopen(argv[2], "w");
   // print the start position of bwt file.
   printf("%d\n", start_position + 1);
   fprintf(output_file, "%s", file_bwt); // write to bwt file.
   fclose(output_file);
    
   return EXIT_SUCCESS;
}

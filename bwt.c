#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//compare integer
int intcmp ( const void *a , const void *b )
{  
   int first = *(int *)a;
   int second = *(int *)b;
   // As we know, if the pwt position is 1, it means i is the last sentence in original file, so it is the biggest. Other number
   // follows the natural order.
   if (first != 1 && second != 1)
      return first - second; 
   else if (first == 1 && second != 1)
      return 1;
   else if (first != 1 && second == 1)
      return -1;   
} 

// get the occ value for search_c.
int occ(int search_c, int search_position, unsigned char *file_name, int **occ_c_table){
   
   FILE *const read_file_pwt = fopen(file_name, "r");
   int match_value = 0;    
   
   int i;   
   int block = search_position / 4096; // count the block.
   match_value = occ_c_table[block][search_c];
   
   int start = search_position;
   int end = (block + 1) * 4096;
   int rest_c;
   int nb_of_char_in_rest_block = 0;
   fseek(read_file_pwt, start, SEEK_SET);
   // count how many character left in the rest of this block.
   for (i = start; i < end; i++) {
      rest_c = getc(read_file_pwt);
      if (rest_c == search_c) {
         nb_of_char_in_rest_block++;
      }  
   }
   
   fclose(read_file_pwt);
   
   match_value = match_value - nb_of_char_in_rest_block;
   return match_value; // return occ.
}



int main(int argc, char **argv) {
   // search.
   if (argc == 3) {
      
      int i;
      
      //copy search string to search[]
      int length_of_search = strlen(argv[2]);
      int search[length_of_search];
      for (i = 0; i < length_of_search; i++) { 
         search[i] = argv[2][i];
      }
      
      int c;
      int block_length = 0;
      
      // get the total_length of the pwt file, so we know how many blocks we need to malloc.
      // Each block is a sibling c_table[], we read every 4096 characters and store the occurrence of each ascii code.
      FILE *const input_file_len = fopen(argv[1], "r");
      fseek(input_file_len, -1, SEEK_END);
      int total_length = ftell(input_file_len) + 1;
      fclose(input_file_len);
      
      int block = total_length / 4096 + 1;
      
      // occ_c_table[][] is a 2d array, first d is the number of blocks, second d is 256, which means the number of ascii code.
      int **occ_c_table = malloc(block * sizeof(int *));
      for(i = 0; i < block; i++) {
         occ_c_table[i] = malloc(256 * sizeof(int));
      }
      
      
      // c_table[] store the last result of C[].
      int c_table[256];
      for(i = 0; i < 256; i++) {
         c_table[i] = -1;
      }
      int cur_block = 0;
      FILE *const read_pwt = fopen(argv[1], "r");
      while ((c = getc(read_pwt)) != EOF) {
         c_table[c]++; 
         block_length++;
         
         if (block_length == 4096) {
            
            for (i = 0; i < 256; i++) {
               int tmp_occ = c_table[i] + 1;
               occ_c_table[cur_block][i] = tmp_occ;
               //printf("tmp_occ: %d \n", tmp_occ); 
            }  
            cur_block++;
            block_length = 0;   
         }       
      }
      fclose(read_pwt);
      // if there is sth left, then write into occ_c_table[][] now.
      if (block_length != 0) {
         for (i = 0; i < 256; i++) {
            int tmp_occ = c_table[i] + 1;
            occ_c_table[cur_block][i] = tmp_occ;
         }  
      }
      
      
      //set the value of c_table
      for(i = 0; i < 256; i++) {
         if (c_table[i] != -1) {
            c_table[i]++;
         } 
      }  
      // get the number of '['.
      int nb_of_braket = c_table[91];
      c_table[91] = 0;
        
      int tmp_next = nb_of_braket;
      for(i = 0; i < 256; i++) {
         if (c_table[i] != -1 && i != 91) {
            int tmp_tmp_next = tmp_next;
            tmp_next = c_table[i] + tmp_next;
            c_table[i] = tmp_tmp_next;  
         }
      } 
      
      // start search
      // Initialize first, last, cur_c and index_i. 
      length_of_search--;
     
      int cur_c = search[length_of_search];
      int first = c_table[cur_c] + 1;
      int last = -1;
      
      for(i = (cur_c + 1); i < 256; i++) {
         if (c_table[i] != -1) {
            last = c_table[i];
            break;
         }
      }
      if (last == -1) {
         FILE *const get_pwt_end = fopen(argv[1], "r");
         fseek(get_pwt_end, 0, SEEK_END);
         last = ftell(get_pwt_end);
      }
      
      // is found is true, that means the pwt file which we are searching now contains the search string.
      bool found = false;
      while ((first <= last) && ((length_of_search + 1) >= 2)) {
         cur_c = search[length_of_search - 1];
         first = c_table[cur_c] + occ(cur_c, first - 1, argv[1], occ_c_table) + 1;
         last = c_table[cur_c] + occ(cur_c, last, argv[1], occ_c_table);
         
         length_of_search--;
         if (last < first) {
            found = false;
            break;
         }
         else {
            found = true;
         }
         
      }
      if (last >= first) {
         found = true;    
      }
      
      if(!found) {
         return EXIT_FAILURE;
      }
      
      // if found the result, we start to decode from that position in pwt.
      // store all the results that not occurence twice.
      int last_pos_of_final_result[1000];
      for (i = 0; i < 1000; i++) {
         last_pos_of_final_result[i] = -1;
      } 
      // store the final results.
      int final_result[1000];
      for (i = 0; i < 1000; i++) {
         final_result[i] = -999;
      } 
      
      // store temp result position which is the position of last character of that sentence in pwt file.
      int result[1000];
      for (i = 0; i < 1000; i++) {
         result[i] = -999;
      }
      
      int result_i;
      for (result_i = first; result_i <= last; result_i++) {
      
         // find the pwt position of character of search[0] in pwt file.
         int find_pwt_pos = result_i - c_table[search[0]];
         
         
         FILE *const find_file_pwt = fopen(argv[1], "r");
       
         int nb_of_search = 0;
         int tmp_pwt_pos = 0;
         while ((c = getc(find_file_pwt)) != EOF) {
            tmp_pwt_pos++;
            if (c == search[0]) {
               nb_of_search++;
            }
            if (nb_of_search == find_pwt_pos) {
               find_pwt_pos = tmp_pwt_pos;
               break;
            }
         }
         fclose(find_file_pwt);
         
         
         // use forward search to decode last sentence.
         int pwt_file_place = find_pwt_pos;
         int next_c;
         int th_c; // th_c means it is the th_c th next_c in pwt file. so if next_c is 'e', th_c is 3, then it means, this 'e' is the third 'e' in pwt file.
         
         while (true) {
            
            bool too_big = true;
            for(i = 0; i < 256; i++) {
               if (c_table[i] >= pwt_file_place) {
                  too_big = false;
                  int w;
                  bool not_braket = false;
                  for ((w = i -1); w > 0; w--) {
                     if (c_table[w] != -1) {
                        not_braket = true;
                        next_c = w;
                        if (next_c == 91)
                           continue;
                        break;
                     }
                  }
                  if (!not_braket) {
                     next_c = 91;
                  }
                     
                  if (c_table[i] > pwt_file_place) {
                     th_c = pwt_file_place - c_table[next_c];
                  }
                        
                  else {
                     th_c = c_table[i] - c_table[next_c];
                  }   
                  break;
               }
                  
            } 
            // pwt_file_place is larger than the biggest value of c_table[], then we need to know what is the last ascii code.
            if (too_big) {
               for(i = 255; i > 0; i--) {
                  if (c_table[i] != -1) {
                     next_c = i;
                     th_c = pwt_file_place - c_table[next_c];
                     break;
                  }
               }   
            }   
               
             // if next_c is '[', it means we finished decoding this sentence, start to store this srntence.  
            if (next_c == 91) {
               for (i = 0; i < 1000; i++) {
                  if (last_pos_of_final_result[i] == pwt_file_place) {
                     break;
                  }
                  if (last_pos_of_final_result[i] == -1) {
                     last_pos_of_final_result[i] = pwt_file_place;
                     break;
                  }   
               }
               break;  
            }
            
            // find the next_c position in pwt file. 
            FILE *const find_file_pwt = fopen(argv[1], "r");
            
            //use occ_c_table[][] to count nb of next_c, so when we find the right block of the array, we only need to read 4096 chars in pwt file.
            int nb_of_search = 0;
            int occ_cur_nb = 0;
            int last_occ_nb = 0;
            int which_block = 0;
            
            while(true) {
               
               occ_cur_nb = occ_c_table[which_block][next_c];
               if (occ_cur_nb >= th_c) { // we find the right block caculate the start position and the end position of pwt file.
                  int pwt_start = which_block * 4096;
                  int pwt_end = (which_block + 1) * 4096;  
                  fseek(find_file_pwt, pwt_start, SEEK_SET); 
                  while ((c = getc(find_file_pwt)) != EOF && pwt_start < pwt_end) {
                     pwt_start++;
                     if (c == next_c) {
                        nb_of_search++;
                        if (nb_of_search + last_occ_nb == th_c) {
                           pwt_file_place = pwt_start;
                           break;
                        }
                     }
                        
                  }
                  if (nb_of_search + last_occ_nb == th_c)
                     break;
               }
               else {
                  last_occ_nb = occ_cur_nb;
                  which_block++;
               }
            }
   
            fclose(find_file_pwt);
         }
       
      }
      
      int nb_of_sentence = 0;
      for (i = 0; i < 1000; i++) {
         if (last_pos_of_final_result[i] != -1)
            nb_of_sentence++;
      }
      // sort last_pos_of_final_result[].
      qsort(last_pos_of_final_result, nb_of_sentence, sizeof(int), intcmp);
      
      int j;
      FILE *const read_file_pwt = fopen(argv[1], "r");
      for (j = 0; j < nb_of_sentence; j++) {
         
         int cur_result = 0;
         //clear final_result[]
         for (i = 0; i < 1000; i++) {
            final_result[i] = -999;
         }
         // store temp result in result[], because we use backward search, so the char we find is inverted order.
         int result[1000];
         for (i = 0; i < 1000; i++) {
            result[i] = -999;
         } 
        
         int pwt_file_place = last_pos_of_final_result[j] - 1;
         
         fseek(read_file_pwt, pwt_file_place, SEEK_SET); 
         c = getc(read_file_pwt);
         result[cur_result] = c; // put the first character into result[].
         
         
         
         while (true) {
            
            // Get the match table value.
            int search_block = pwt_file_place / 4096; // find out which block we need to read.
            int match_value = occ_c_table[search_block][c];
            
            int rest_c;
            int nb_of_char_in_rest_block = 0;
            int end = (pwt_file_place / 4096 + 1) * 4096;
            int start = pwt_file_place + 1;
         
            fseek(read_file_pwt, start, SEEK_SET);
            for (i = start; i < end; i++) {
               rest_c = getc(read_file_pwt);
               if (rest_c == c) {
                  nb_of_char_in_rest_block++;
               }  
            }
            match_value = match_value - nb_of_char_in_rest_block - 1 + c_table[c];

            // get the position of the next character.
            pwt_file_place = match_value;
            //get the ascii code of the next character.
            fseek(read_file_pwt, pwt_file_place, SEEK_SET);
            cur_result++;
            
            c = getc(read_file_pwt);
            //check if we finish to decode one sentence, when we meet first '[', we know a sentence is finished.
            if(c == 91 && cur_result != 1) {
               result[cur_result] = c;
               // To check if this '[' position has in last_pos_of_final_result[].
              
               // put the result[] into unsigned char result_check[], then use Boyer-Moore Aol to check if this sentence contains search string.
               int result_len = 0;
               for (i = 0; i < 1000; i++) {
                  if (result[i] != -999) 
                     result_len++;
                  else
                     break;
               }
               // copy the sentence with the right order.
               for (i = 0; i < result_len; i++) {
                  final_result[i] = result[result_len - 1 - i];
               }  
               
               // clear the result[] and reset the int cur_result.
               cur_result = 0;
               // print final_result.
               for (i = 0; i < 1000; i++) {
                  if (final_result[i] != -999)
                     printf("%c", final_result[i]);
               }
               
               break;
            } 
            result[cur_result] = c;
         }
         
      }
      
   }
   // decode sentences.
   else if (argc == 4) {
      int i;
      int length_of_start = strlen(argv[2]);
      int length_of_end = strlen(argv[3]);
      int start_position_decode = 0;
      int end_position_decode = 0;
      
      for (i = 0; i < length_of_start; i++) { 
         start_position_decode = start_position_decode * 10 + argv[2][i] - '0';
      }
      for (i = 0; i < length_of_end; i++) {
         end_position_decode = end_position_decode * 10 + argv[3][i] - '0';
      }
   
      int c;
      
      
      // get the total_length of the pwt file, so we know how many blocks we need to malloc.
      // Each block is a sibling c_table[], we read every 4096 characters and store the occurrence of each ascii code.
      FILE *const input_file_len = fopen(argv[1], "r");
      fseek(input_file_len, -1, SEEK_END);
      int total_length = ftell(input_file_len) + 1;
      fclose(input_file_len);
      
      int block_length = 0;
      int block = total_length / 4096 + 1;     
      
      // occ_c_table[][] is a 2d array, first d is the number of blocks, second d is 256, which means the number of ascii code.
      int **occ_c_table = malloc(block * sizeof(int *));
      for(i = 0; i < block; i++) {
         occ_c_table[i] = malloc(256 * sizeof(int));
      }
        
      // c_table[] store the last result of C[].
      int c_table[256];
      for(i = 0; i < 256; i++) {
         c_table[i] = -1;
      }
      int cur_block = 0;
      FILE *const read_pwt = fopen(argv[1], "r");
      while ((c = getc(read_pwt)) != EOF) {
         c_table[c]++; 
         block_length++;
         
         if (block_length == 4096) {
            
            for (i = 0; i < 256; i++) {
               int tmp_occ = c_table[i] + 1;
               occ_c_table[cur_block][i] = tmp_occ;
            }  
            cur_block++;
            block_length = 0;   
         }       
      }
      fclose(read_pwt);
      // if there is sth left, then write into occ_c_table[][] now.
      if (block_length != 0) {
         for (i = 0; i < 256; i++) {
            int tmp_occ = c_table[i] + 1;
            occ_c_table[cur_block][i] = tmp_occ;
         }  
      }
      
      
      //set the value of c_table
      for(i = 0; i < 256; i++) {
         if (c_table[i] != -1) {
            c_table[i]++;
         } 
      }  
      // get the number of '['.
      int nb_of_braket = c_table[91];
      c_table[91] = 0;
        
      int tmp_next = nb_of_braket;
      for(i = 0; i < 256; i++) {
         if (c_table[i] != -1 && i != 91) {
            int tmp_tmp_next = tmp_next;
            tmp_next = c_table[i] + tmp_next;
            c_table[i] = tmp_tmp_next;  
         }
      } 
      
      // store each result sentence in final_result[].
      int final_result[1000];
      for (i = 0; i < 1000; i++) {
         final_result[i] = -999;
      }
      // store all the results that not occurence twice.
      int last_pos_of_final_result[1000];
      for (i = 0; i < 1000; i++) {
         last_pos_of_final_result[i] = -1;
      }     
      
      int j;
      FILE *const read_file_pwt = fopen(argv[1], "r");
      for (j = start_position_decode; j < (end_position_decode + 1); j++) {
         bool have_existed = false;
         int cur_result = 0;
         //clear final_result[]
         for (i = 0; i < 1000; i++) {
            final_result[i] = -999;
         }
         // store temp result in result[], because we use backward search, so the char we find is inverted order.
         int result[1000];
         for (i = 0; i < 1000; i++) {
            result[i] = -999;
         } 
        
         
         int pwt_file_place;
         // We know that the start decode place is j, so find the char in that place.
         if (j >= nb_of_braket) {
            pwt_file_place = 0;
           
         }
         else {
            pwt_file_place = j;
         }
         fseek(read_file_pwt, pwt_file_place, SEEK_SET); 
         c = getc(read_file_pwt);
         result[cur_result] = c; // put the first character into result[].
         
         while (true) {
            // Get the match table value.
            int search_block = pwt_file_place / 4096; // find out which block we need to read.
            int match_value = occ_c_table[search_block][c];
            
            int rest_c;
            int nb_of_char_in_rest_block = 0;
            int end = (pwt_file_place / 4096 + 1) * 4096;
            int start = pwt_file_place + 1;
         
            fseek(read_file_pwt, start, SEEK_SET);
            for (i = start; i < end; i++) {
               rest_c = getc(read_file_pwt);
               if (rest_c == c) {
                  nb_of_char_in_rest_block++;
               }  
            }
            match_value = match_value - nb_of_char_in_rest_block - 1 + c_table[c];
            
            // get the position of the next character.
            pwt_file_place = match_value;
            
            //get the ascii code of the next character.
            fseek(read_file_pwt, pwt_file_place, SEEK_SET);
            cur_result++;
            
            c = getc(read_file_pwt);
            //check if we finish to decode one sentence, when we meet first '[', we know a sentence is finished.
            if(c == 91 && cur_result != 1) {
               result[cur_result] = c;
               // To check if this '[' position has in last_pos_of_final_result[].
            
               for (i = 0; i < 1000; i++) {
                  if (last_pos_of_final_result[i] != -1) {
                     if (last_pos_of_final_result[i] == pwt_file_place) {
                        have_existed = true;
                        break;
                     }
                     else
                        continue;
                  }
                  else if (last_pos_of_final_result[i] == -1) {
                     last_pos_of_final_result[i] = pwt_file_place;
                     break;
                  }
                     
               }
              
               if (have_existed) {
                  break;
               } 
               // put the result[] into unsigned char result_check[], then use Boyer-Moore Aol to check if this sentence contains search string.
               int result_len = 0;
               for (i = 0; i < 1000; i++) {
                  if (result[i] != -999) 
                     result_len++;
                  else
                     break;
               }
               // copy the sentence with the right order.
               for (i = 0; i < result_len; i++) {
                  final_result[i] = result[result_len - 1 - i];
               }  
               
               // clear the result[] and reset the int cur_result.
               cur_result = 0;
               // print final_result.
               for (i = 0; i < 1000; i++) {
                  if (final_result[i] != -999)
                     printf("%c", final_result[i]);
               }
               
               break;
            } 
            result[cur_result] = c;
         }
      }
      fclose(read_file_pwt); 
   }
   
   return EXIT_SUCCESS;
}

There are two C code files, bwt_encode.c is the code to encode the normal text file into bwt text file, so we can compresse the bwt file or do search on the bwt file. bwt.c file is the code to search the string based on the bwt text file.

Here is an example:

If you have a original normal file contain a string: #mississippi, where $ means the end of this file.
then first you run bwt_encode.c, and it will create a new bwt_text file whcih contains a bwt string: ipssm#pissii

Then you run bwt.c file to search the characters or words you want, type"si", the bwt.c is going to the bwt_text file to do the backword search, if the orignal file contains "si", then after you complie and run the bwt.c, it will print the whole sentence whcih contains si.

I post 100.txt file as a test file, you can download 3 files: 100.txt, bwt.c, bwt_encode.c
then bwt_encode.c has two arguments,
type in your terminal: gcc bwt_encode.c -o encode
it will create an exe called encode, then type: ./encode 100.txt 100.bwt
it will create a new text file called 100.bwt, open this 100.bwt file, you will see all the strings in 100.txt now re-ordered by bwt rules;

type: gcc bwt.c -o bwt
to create an exe called bwt, then run it by typing: ./bwt 100.bwt "of"
it will search the string "of" in the 100.bwt, then print the sentence in the 100.txt if exsits, you should see this as result:
"[7 allocation 8 with 94#8Taaffe7112/eor.00 of :-1/journals.0 of :-[6.0312 of[9#8Journal 6122.org-14 of[1.ejor properties 8#6 of[9 of[63 of[http4ResearchH00 of[7Tamiz7Journal 72/eor010 of[611.ejor production 933G:-1 of[0000112.html algorithms413Journal 7 of[9 of[000/j using 9R00 of[http430 of[61/0 of Endurance  average"

Congratulations!!! then now you can try other text file you like, or even change my code to fit your need.

#include <stdio.h>

int main()
{
  /*  unsigned char perm1[7][8] = {{, , , , , , , },
			       {, , , , , , , },
			       {, , , , , , , },
			       {, , , , , , , },
			       {, , , , , , , },
			       {, , , , , , , },
			       {, , , , , , , }};
  */
  /*PC-1
  unsigned char perm1[7][8] = {{8, 16, 24, 56, 52, 44, 36, 7},
			       {15, 23, 55, 51, 43, 35, 6, 14},
			       {22, 54, 50, 42, 34, 5, 13, 21},
			       {53, 49, 41, 33, 4, 12, 20, 28},
			       {48, 40, 32, 3, 11, 19, 27, 47},
			       {39, 31, 2, 10, 18, 26, 46, 38},
			       {30, 1, 9, 17, 25, 45, 37, 29}};
  */
  /*PC-2
  unsigned char perm1[7][8] = {{5, 24, 7, 16, 6, 10, 20, 18},
			       {0, 12, 3, 15, 23, 1, 9, 19},
			       {2, 0, 14, 22, 11, 0, 13, 4},
			       {0, 17, 21, 8, 47, 31, 27, 48},
			       {35, 41, 0, 46, 28, 0, 39, 32},
			       {25, 44, 0, 37, 34, 43, 29, 36},
			       {38, 45, 33, 26, 42, 0, 30, 40}};
  */
  /*IP
  unsigned char perm1[8][8] = {{40, 8, 48, 16, 56, 24, 64, 32},
			       {39, 7, 47, 15, 55, 23, 63, 31},
			       {38, 6, 46, 14, 54, 22, 62, 30},
			       {37, 5, 45, 13, 53, 21, 61, 29},
			       {36, 4, 44, 12, 52, 20, 60, 28},
			       {35, 3, 43, 11, 51, 19, 59, 27},
			       {34, 2, 42, 10, 50, 18, 58, 26},
			       {33, 1, 41,  9, 49, 17, 57, 25}};

  */
  /*E*/
  unsigned char perm1[4][8] = {{2, 3, 4, 5, 6, 9, 10, 11},
			       {12, 15, 16, 17, 18, 21, 22, 23},
			       {24, 27, 28, 29, 30, 33, 34, 35},
			       {36, 39, 40, 41, 42, 45, 46, 47}};
  unsigned char perm2[4][8] = {{48, 0, 0, 7, 8, 0, 0, 13},
			       {14, 0, 0, 19, 20, 0, 0, 25},
			       {26, 0, 0, 31, 32, 0, 0, 37},
			       {38, 0, 0, 43, 44, 0, 0, 1}};
  
  /*P
  unsigned char perm1[4][8] = {{9, 17, 23, 31, 13, 28, 2, 18},
			       {24, 16, 30, 6, 26, 20, 10, 1},
			       {8, 14, 25, 3, 4, 29, 11, 19},
			       {32, 12, 22, 7, 5, 27, 15, 21}};
  */  
  /*FP
  unsigned char perm1[8][8] = {{58, 50, 42, 34, 26, 18, 10, 2},
			       {60, 52, 44, 36, 28, 20, 12, 4},
			       {62, 54, 46, 38, 30, 22, 14, 6},
			       {64, 56, 48, 40, 32, 24, 16, 8},
			       {57, 49, 41, 33, 25, 17, 9, 1},
			       {59, 51, 43, 35, 27, 19, 11, 3},
			       {61, 53, 45, 37, 29, 21, 13, 5},
			       {63, 55, 47, 39, 31, 23, 15, 7}};
  */

  /*unsigned short result[8][256][4], pom[4], tmp;*/
    unsigned short result[4][256][3], pom[3], tmp;
  unsigned char blok, wart, bit, maska, i;

  for (blok = 1; blok < 5; blok++) {
    for (wart = 0; wart <= 255; wart++) {
      result[blok - 1][wart][0] = 0;
      result[blok - 1][wart][1] = 0;
      result[blok - 1][wart][2] = 0;
      /*result[blok - 1][wart][3] = 0;*/
      for (bit = 1; bit < 9; bit++) {
	pom[1] = pom[0] = pom[2] = pom[3] = 0;
	maska = 1;
	for (i = 0; i < 8 - bit; i++)
	  maska *= 2;
	maska &= wart;
	if (maska) {
	  if (perm1[blok - 1][bit - 1] == 0) {/*skip*/}
	  else if (perm1[blok - 1][bit - 1] <= 16) {
	    pom[0] = 1;
	    for (i = 0; i < 16 - perm1[blok - 1][bit - 1]; i++)
	      pom[0] *= 2;
	  } else if (perm1[blok - 1][bit - 1] <= 32) {
	    pom[1] = 1;
	    for (i = 0; i < 32 - perm1[blok - 1][bit - 1]; i++)
	      pom[1] *= 2;
	  } else if (perm1[blok - 1][bit - 1] <= 48) {
	    pom[2] = 1;
	    for (i = 0; i < 48 - perm1[blok - 1][bit - 1]; i++)
	      pom[2] *= 2;
	  } /*else {
	    pom[3] = 1;
	    for (i = 0; i < 64 - perm1[blok - 1][bit - 1]; i++)
	    pom[3] *= 2;
	  }*/
	  result[blok - 1][wart][0] |= pom[0];
	  result[blok - 1][wart][1] |= pom[1];
	  result[blok - 1][wart][2] |= pom[2];
	  /*  result[blok - 1][wart][3] |= pom[3];*/
	  pom[1] = pom[0] = pom[2] = pom[3] = 0;
	  if (perm2[blok - 1][bit - 1] == 0) {/*skip*/}
	  else if (perm2[blok - 1][bit - 1] <= 16) {
	    pom[0] = 1;
	    for (i = 0; i < 16 - perm2[blok - 1][bit - 1]; i++)
	      pom[0] *= 2;
	  } else if (perm2[blok - 1][bit - 1] <= 32) {
	    pom[1] = 1;
	    for (i = 0; i < 32 - perm1[blok - 1][bit - 1]; i++)
	      pom[1] *= 2;
	  } else if (perm2[blok - 1][bit - 1] <= 48) {
	    pom[2] = 1;
	    for (i = 0; i < 48 - perm2[blok - 1][bit - 1]; i++)
	      pom[2] *= 2;
	  }/* else {
	    pom[3] = 1;
	    for (i = 0; i < 64 - perm2[blok - 1][bit - 1]; i++)
	    pom[3] *= 2;
	  }*/
	  result[blok - 1][wart][0] |= pom[0];
	  result[blok - 1][wart][1] |= pom[1];
	  result[blok - 1][wart][2] |= pom[2];
	  /*  result[blok - 1][wart][3] |= pom[3];*/
	}
      }
      printf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \\\n ", result[blok - 1][wart][0]/256, result[blok - 1][wart][0]%256, result[blok - 1][wart][1]/256, result[blok - 1][wart][1]%256, result[blok - 1][wart][2]/256, result[blok - 1][wart][2]%256/*, result[blok - 1][wart][3]/256, result[blok - 1][wart][3]%256*/);
      if (wart == 255) break;
    }
    //printf("-----------------\n");
  }


  return 0;
}


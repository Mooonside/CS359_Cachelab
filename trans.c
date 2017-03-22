/*
*Project info :
	Yifeng Chen
	ID:5140309458 

 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
 #include <stdlib.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int row_start; 
	int col_start;
 	int i;
 	int j;
 	int buffer[8];
	if(M == 32){
                for(row_start = 0 ;row_start < M ;row_start+=8){
                        for(col_start = 0;col_start < N ; col_start+=8){
                            for(i =  row_start; i < row_start + 8 ; i++){
                                  buffer[1] = -1;
                                  for(j =col_start ; j <col_start + 8 ;j++){
                                          if(i != j)  B[j][i] = A[i][j];
                                          else{ 
                                                buffer[0] = A[i][j];
                                                buffer[1] = i;
                                          }
                                  }
                                  if (buffer[1] != -1)
                                          B[buffer[1]][buffer[1]] = buffer[0] ;
                            }
                        }
                }
        } else if (M ==64){
		for( row_start = 0;row_start<64;row_start+=8){
			for(col_start=0;col_start<64;col_start+=8){
				/*handle the upper A's 4*8*/
				for( i = row_start ;i < row_start +4 ;i++){
					/* read A's i th line into buffer*/
					for(j = 0;j <8 ;j++){
						buffer[j] = A[i][col_start+j];
					}
					/* save the buffer to B's  two columns*/
					for(j=0;j<4;j++){
						B[col_start+j][i] = buffer[j];
						B[col_start+j][i+4]= buffer[j+4];
					}
				}
				for(i = col_start  ; i < col_start +4 ;i++){
					/* read B's  upper right 's row(4) into buffer*/
					for(j=0;j<4;j++){
						buffer[ j ] = B[ i ][ row_start + 4 + j ];
					}
					/* change B's upper right 's row(4) into correct transpose value*/
					for(j=0;j<4;j++){
						B[ i ][row_start + 4 +j ] = A[row_start + 4 +j ][i];
					}
					for(j=0;j<4;j++)
						B[i+4][row_start+j] = buffer[j];
					for(j=4;j<8;j++)
						B[i+4][row_start+j] =  A[row_start + j][i+4];
				}
			}
		}	
        }else if (M==61){
                for(row_start = 0 ;row_start < 67 ;row_start+=16){
                        for(col_start = 0;col_start < 61 ; col_start+=16){
                            for(i =  row_start; ( i < row_start + 16 && i <67); i++){
                                  buffer[1] = -1;
                                  for(j =col_start ; (j <col_start + 16 && j <61);j++){
                                          if(i != j)  B[j][i] = A[i][j];
                                          else{ 
                                                buffer[0] = A[i][j];
                                                buffer[1] = i;
                                          }
                                  }
                                  if (buffer[1] != -1)
                                          B[buffer[1]][buffer[1]] = buffer[0] ;
                            }
                        }
                }
        }
}


/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    
}

char trans_name_BB[] = "elimiante B & B's conflicts";
void trans_BB(int M, int N, int A[N][M], int B[M][N])
{
	int buffer[8];
	int row_start;
	int col_start;
	int i;
	int j;
	for( row_start = 0;row_start<64;row_start+=8){
		for(col_start=0;col_start<64;col_start+=8){
			/*handle the upper A's 4*8*/
			for( i = row_start ;i < row_start +4 ;i++){
				/* read A's i th line into buffer*/
				for(j = 0;j <8 ;j++){
					buffer[j] = A[i][col_start+j];
				}
				/* save the buffer to B's  two columns*/
				for(j=0;j<4;j++){
					B[col_start+j][i] = buffer[j];
					B[col_start+j][i+4]= buffer[j+4];
				}
			}
			for(i = col_start  ; i < col_start +4 ;i++){
				/* read B's  upper right 's row(4) into buffer*/
				for(j=0;j<4;j++){
					buffer[ j ] = B[ i ][ row_start + 4 + j ];
				}
				/* change B's upper right 's row(4) into correct transpose value*/
				for(j=0;j<4;j++){
					B[ i ][row_start + 4 +j ] = A[row_start + 4 +j ][i];
				}
				/* read A's down right 's column into buffer*/ 
				for(j=0;j<4;j++){
					buffer[j+4] = A[row_start +4 + j][i+4];
				}
				/* save buffer to B's lower row entirely*/
				for(j=0;j<8;j++){
					B[i+4][row_start+j] = buffer[j];
				}
			}
		}
	}
}


char trans_name_AB[] = "eliminate A&B 's conflict misses";
void trans_AB(int M, int N, int A[N][M], int B[M][N])
{
	int block_row; 
	int block_col;
	int i;
	int j;
	int temp1;
	int temp2;
	int index1= -1;

	int index2 =-1;
	int index3= -1;
	for(block_col = 0 ;block_col < M/8 ;block_col++){
		for(block_row = 0;block_row < N/8 ; block_row++){
			if(block_row != block_col){
                            for(i =  block_row * 8; i < block_row * 8 + 8 ; i++){
                                  for(j = block_col * 8 ; j < block_col *8 + 8 ;j++){
					 B[j][i] = A[i][j];
                                  }
                            }
			}	
			else{
				for( i = block_row * 8 ; i<block_row *8+ 8;i++){
					for(j = block_col * 8;j < block_col*8 +8;j++){
						if(i == j){
							temp1 = A[i][j];
							index1 = i;
						}
						else if (i<4 && j == (i+4)){
							temp2 = A[i][j];
							index2 = i;
							index3 = j;
						}else if ( i>=4 && i ==(j+4)){
							temp2 = A[i][j];
							index2 = i;
							index3 = j;
						}
						else B[j][i] = A[i][j];
					}
					if(index1 != -1)
					{
						B[index1][index1]=temp1;
						index1= -1;
					}
					if(index2 != -1 && index3 != -1)
					{
						B[index3][index2]=temp2;
						index2= -1;
						index3 = -1;
					}
				}
			}	
		}
	}
}


/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
   registerTransFunction(trans_BB, trans_name_BB); 
   registerTransFunction(trans_AB, trans_name_AB); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


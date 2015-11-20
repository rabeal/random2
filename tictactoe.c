#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <float.h>



/*
	***********************
	**** Macros/Structs ***
	***********************
*/

#define BOARD_SIZE 3
#define MACHINE_PIECE 'X'
#define OPPONENT_PIECE 'O'
#define TIE '-'
#define EXPERIMENTS_NUM 400 // number of games
#define NUM_WEIGHTS 5
#define HIGH 30000
#define LOW 25000

struct Board
{
	char b[BOARD_SIZE][BOARD_SIZE];
};

struct Weights
{
	double w[NUM_WEIGHTS];
};

int opponent_was_first=0;


/*
	***********************
	****** Prototypes *****
	***********************
*/

void display_board(struct Board b1);
struct Board init_board();
struct Board transpose(struct Board b1);
char game_over(struct Board b1);
char row_match(struct Board b1);
void machine_move(struct Board *b1,char piece,int move_no,struct Weights w);
void opponent_move(struct Board *b1,char piece,int move_no);
void manual_board_selection(struct Board b1,int *row,int *col);
void block_or_win(struct Board *b1,char my_piece,char their_piece);
void near_victory(struct Board b1,char consider_piece,int *ans_row,int *ans_col);
struct Board copy_board(struct Board b1);
short went_first(char piece);
int corners_avail(struct Board b1);
int count_corners(struct Board b1,char piece);
int edges_avail(struct Board b1);
int count_edges(struct Board b1,char piece);
char center(struct Board b1);
void random_weights(struct Weights *w1);
void copy_weights(struct Weights from, struct Weights *to);


/*
	***********************
	******** Main *********
	***********************
*/

int main(int argc, char* argv[])
{
	struct Board b1;
	char winner='\0';
	int turn=0,move_machine=1,move_opponent=1,e,first;
	struct Weights w1,w2;
	char last_w_status='\0';
	int wins,losses,ties;
	int machine_first=0, opponent_first=0;
	
	srand(time(NULL));
	
	wins=losses=ties=0;
	
	for(e=0; e<NUM_WEIGHTS; e++) w1.w[e]=w2.w[e]=0.0;
	
	printf("MACHINE is '%c' and OPPONENT is '%c'\n\n",MACHINE_PIECE,OPPONENT_PIECE);
	
	random_weights(&w1);
	copy_weights(w1,&w2);
		
	for(e=0; e<EXPERIMENTS_NUM; e++)
	{
		b1=init_board();
		turn=opponent_was_first=0;
		move_machine=move_opponent=1;
		
		first=rand()%2;
		switch(first){
			case 0: printf("\n\n!!! Randomly, it was decided that MACHINE will start the game. !!!\n\n"); opponent_was_first=0; machine_first++; break;
			case 1: printf("\n\n!!! Randomly, it was decided that OPPONENT will start the game. !!!\n\n"); opponent_was_first=1; opponent_first++; break;
		}
		do
		{
			//printf("\n\nw1=[%f,%f,%f,%f,%f]\n\n",w1.w[0],w1.w[1],w1.w[2],w1.w[3],w1.w[4]);
			display_board(b1);
			
			if(first==0)
			{
				switch(turn%2)
				{
					case 0: 
						machine_move(&b1,MACHINE_PIECE,move_machine++,w1);
						printf("\nMACHINE finished move...\n");
						break;
					case 1:
						opponent_move(&b1,OPPONENT_PIECE,move_opponent++);
						printf("\nOPPONENT finished move...\n");
				}
			}else{
				switch(turn%2)
				{
					case 0: 
						opponent_move(&b1,OPPONENT_PIECE,move_opponent++);
						printf("\nOPPONENT finished move...\n");
						break;
					case 1:
						machine_move(&b1,MACHINE_PIECE,move_machine++,w1);
						printf("\nMACHINE finished move...\n");						
				}

			}
			turn++;
		}while((winner=game_over(b1))=='\0');
		printf("\n!!! Game Over !!!\n\n");
		display_board(b1);
		
		switch(winner)
		{
			case MACHINE_PIECE: printf("\n!!! MACHINE Wins !!!\n\n"); 
				last_w_status='w'; 
				copy_weights(w1,&w2);
				wins++;
				break;
			case OPPONENT_PIECE: printf("\n!!! OPPONENT Wins !!!\n\n"); 
				if(last_w_status=='w') copy_weights(w2,&w1);
				else random_weights(&w1);
				
				last_w_status='l';
				losses++;
				break;
			case TIE: printf("\n!!! The game was a draw !!!\n\n"); 
				if(last_w_status!='w') random_weights(&w1);
				else copy_weights(w2,&w1);
				
				last_w_status='t';
				ties++;
				break;
			default: printf("\n!!! Problem !!!\n\n");
		}
		
		printf("*******************\n****End of Game****\n*******************\n\n");
	}
	
	printf("\n\n!!! Total Games:%d, Machine first move:%d, Opponent first move:%d !!!\n!!! %.2f%% Machine Wins, %.2f%% Machine Losses, %.2f%% Draws !!!\n\n",EXPERIMENTS_NUM,machine_first,opponent_first,wins/(float)EXPERIMENTS_NUM,losses/(float)EXPERIMENTS_NUM,ties/(float)EXPERIMENTS_NUM);
	
	return 0;
}


/*
	***********************
	****** Functions ******
	***********************
*/

void copy_weights(struct Weights from, struct Weights *to)
{
	int i;
	
	for(i=0; i<NUM_WEIGHTS; i++)
		(*to).w[i]=from.w[i];
}

void random_weights(struct Weights *w1)
{
	double r,delta,l;
	int i;
	
	while((l=r=(int)(rand()%HIGH))<LOW);
	delta=r/NUM_WEIGHTS;
	
	for(i=0; i<NUM_WEIGHTS; i++)
	{
		l=l-delta;
		while(((*w1).w[i]=(int)(rand()%(int)(l+delta)))<=l);
	}
}

short went_first(char piece)
{
	short first=0;
	
	if(piece==OPPONENT_PIECE && opponent_was_first!=0) first=1;
	if(piece==MACHINE_PIECE && opponent_was_first==0) first=1;
	
	return first;
}

int corners_avail(struct Board b1)
{
	return count_corners(b1,'\0');
}

int count_corners(struct Board b1,char piece)
{
	int i,j,count=0;
	
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
		{
			if(b1.b[i][j]==piece)
			{
				if((i==0 && j==0) || (i==0 && j==BOARD_SIZE-1) || (j==0 && i==BOARD_SIZE-1) || (i==j && i==BOARD_SIZE-1))
					count++;
			}
		}
	return count;
}

int edges_avail(struct Board b1)
{
	return count_edges(b1,'\0');
}

int count_edges(struct Board b1,char piece)
{
	int i,j,count=-1*count_corners(b1,piece);
	
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
		{
			if(b1.b[i][j]==piece)
			{
				if(i==0 || j==0 || i==BOARD_SIZE-1 || j==BOARD_SIZE-1)
					count++;
			}
		}
	return count;
}

char center(struct Board b1)
{
	return b1.b[BOARD_SIZE/2][BOARD_SIZE/2];
}

void machine_move(struct Board *b1,char piece,int move_no,struct Weights w)
{
	struct Board b2=copy_board(*b1),b3;
	int i,j,f=0,mi,mj,a,b;
	int i_win,i_tie,have_center,center_avail,my_first_move,my_edges,my_corners,opp_edges,opp_corners,i_lose,more_corners,more_edges,multiple_wins,opp_multiple_wins; // features
	double v=0,vmax=0;
	char go;
	
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
		{
			if(b2.b[i][j]=='\0') // if empty, try
			{
				f++;
				
				my_first_move=went_first(piece);
				
				center_avail=((center(*b1)=='\0')?1:0);
				
				b2.b[i][j]=piece;
				
				go=game_over(b2);
				if(go==piece) { i_win=1; i_tie=0; }
				else if(go==TIE) { i_tie=1; i_win=0; }
				else i_win=i_tie=0;
				
				have_center=((center(b2)==MACHINE_PIECE)?1:0);
				my_edges=count_edges(b2,piece);
				my_corners=count_corners(b2,piece);
				opp_edges=count_edges(b2,OPPONENT_PIECE);
				opp_corners=count_corners(b2,OPPONENT_PIECE);
				
				multiple_wins=0;
				b3=copy_board(b2);
				for(a=0; a<BOARD_SIZE; a++)
					for(b=0; b<BOARD_SIZE; b++)
					{
						if(b3.b[a][b]=='\0')
						{
							b3.b[a][b]=piece;
							go=game_over(b3);
							if(go==piece) multiple_wins++;
							b3.b[a][b]='\0';
						}
					}
				multiple_wins=((multiple_wins>1)?1:0);
				
				b2.b[i][j]=OPPONENT_PIECE; 
				
				b3=copy_board(b2);
				for(a=0; a<BOARD_SIZE; a++)
					for(b=0; b<BOARD_SIZE; b++)
					{
						if(b3.b[a][b]=='\0')
						{
							b3.b[a][b]=OPPONENT_PIECE;
							go=game_over(b3);
							if(go==OPPONENT_PIECE) opp_multiple_wins++;
							b3.b[a][b]='\0';
						}
					}
				opp_multiple_wins=((opp_multiple_wins>1)?1:0);
				
				go=game_over(b2);
				if(go==OPPONENT_PIECE) i_lose=1;
				else i_lose=0;
				
				more_corners=((my_corners-opp_corners>0)?1:0);
				more_edges=((my_edges-opp_edges>0)?1:0);
				
				v=pow(2,10)*w.w[0]*(i_win+i_tie)+pow(2,9)*w.w[1]*i_lose+pow(2,8)*w.w[2]*multiple_wins+pow(2,6)*w.w[3]*opp_multiple_wins;
				//printf("v (%d,%d) is %f\n\n",i,j,v);
				
				
				if(f==1 || v>vmax) { vmax=v; mi=i; mj=j; }
				
				b2.b[i][j]='\0'; // restore empty space
			}
		}
	(*b1).b[mi][mj]=piece; // make best move
		
	/* // for manual testing
	manual_board_selection(*b1,&i,&j);
	(*b1).b[i][j]=piece; */
}

struct Board copy_board(struct Board b1)
{
	int i,j;
	struct Board b2;
	
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
			b2.b[i][j]=b1.b[i][j];
	return b2;
}

void near_victory(struct Board b1,char consider_piece,int *ans_row,int *ans_col)
{
	struct Board b2=copy_board(b1);
	struct Board b3,b4;
	int wins=0;
	char over='\0';
	int i,j,k,l,m,n,match,a,b;
	
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
		{
			if(b2.b[i][j]==0) // empty, try to add a piece and assess subsequent boards
				b2.b[i][j]=consider_piece; // 1st piece
			
			b4=copy_board(b2);
			for(m=0; m<BOARD_SIZE; m++)
			{
				for(n=0; n<BOARD_SIZE; n++)
					if(b2.b[m][n]==0) // 2nd move, then assess!
					{
						b4.b[m][n]=consider_piece; // 2nd piece
								
						// check horizontal
						over=row_match(b4);
						if(over==consider_piece) wins++;
						
						// check vertical
						b3=transpose(b4);
						over=row_match(b3);
						if(over==consider_piece) wins++;
					
						// check left-to-right diagonal
						match=1;
						for(k=1; k<BOARD_SIZE; k++)
							if(b4.b[k][k]!=b4.b[k-1][k-1] || b4.b[k][k]=='\0' ||  b4.b[k-1][k-1]=='\0')
							{
								match=0;
								break;
							}
						if(match==1) // winner
							wins++;

						// check right-to-left diagonal
						match=1;
						for(k=BOARD_SIZE-2,l=1; k>=0; k--,l++)
							if(b4.b[l][k]!=b4.b[l-1][k+1] || b4.b[l][k]=='\0' || b4.b[l-1][k+1]=='\0')
							{
								match=0;
								break;
							}
						if(match==1) // winner
							wins++;
						
						if(wins>0) // so, the first move is an option!
						{
							(*ans_row)=i;
							(*ans_col)=j;
						}
						if(wins>1) // return when the best option is a case with multiple winning possibilities
							return;
							
						b4.b[m][n]=0; // restore 2nd piece
					}
			}
			b2.b[i][j]=0; // restore 1st piece
		}
}

void block_or_win(struct Board *b1,char my_piece,char their_piece)
{
	int i,j,a=-1,b=-1;
	struct Board b2=copy_board(*b1);
	
	// is there an easy win for me?
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
		{
			if((*b1).b[i][j]==0) // empty space: add my_piece and analyze the results
			{
				b2.b[i][j]=my_piece;
				if(game_over(b2)==my_piece) // if I can easily win, choose this
				{
					(*b1).b[i][j]=my_piece;
					return;
				}else // erase the previous move and try a different scenario
					b2.b[i][j]=0;
			}
		}
	
	// if no easy win exists, "block" them from winning!!!
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
		{
			if((*b1).b[i][j]==0) // empty space: add their_piece and analyze the results
			{
				b2.b[i][j]=their_piece;
				if(game_over(b2)==their_piece) // if my counterpart wins, this is what I must "block"
				{
					(*b1).b[i][j]=my_piece;
					return;
				}else // erase the previous move and try a different scenario
					b2.b[i][j]=0;
			}
		}

	near_victory(*b1,my_piece,&a,&b);
	
	if(a==-1 || b==-1) // randomly choose a piece
	{
		srand(time(NULL));
		do
		{
			a=rand()%BOARD_SIZE;
			b=rand()%BOARD_SIZE;
		}while((*b1).b[a][b]!=0);
	}
	
	(*b1).b[a][b]=my_piece;
}

void opponent_move(struct Board *b1,char piece,int move_no)
{
	int i,j,clear=1,count_my_pieces=0,a,b;
	int optimal_move=1;
	
	// Fixed evaluation influenced by "How to Win at Tic Tac Toe"
	// article from wikihow.com -- specific for BOARD_SIZE=3
		
	// any empty board spaces
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
		{
			if((*b1).b[i][j]!=0)
				clear=0;
			if((*b1).b[i][j]==piece)
				count_my_pieces++;
		}

	if(clear==1) // I get first move!
	{
		(*b1).b[BOARD_SIZE/2][BOARD_SIZE/2]=OPPONENT_PIECE; // mark the center
		opponent_was_first=1;
	}
	
	if(rand()%10==0)
		optimal_move=0;
	
	if(optimal_move)
	{
		if(opponent_was_first && move_no>1)
		{
			switch(move_no)
			{
				case 2: // the move that helps "setup" success
					a=b=0;
					if((*b1).b[1][0]==MACHINE_PIECE) { a=1; b=0; }
					if((*b1).b[0][1]==MACHINE_PIECE) { a=0; b=1; }
					if((*b1).b[2][1]==MACHINE_PIECE) { a=2; b=1; }
					if((*b1).b[1][2]==MACHINE_PIECE) { a=1; b=2; }
					if(a!=0 && b!=0)// machine chose an "edge" piece
					{
						// choose the "corner" piece furthest away
						if(a==1 && b==0) (*b1).b[2][2]=OPPONENT_PIECE;
						else if(a==0 && b==1) (*b1).b[2][0]=OPPONENT_PIECE;
						else if(a==2 && b==1) (*b1).b[0][0]=OPPONENT_PIECE;
						else if(a==1 && b==2) (*b1).b[0][0]=OPPONENT_PIECE;
					}else // the machine chose a "corner" piece
					{
						// choose a piece to form a diagonal
						if((*b1).b[0][0]==MACHINE_PIECE) (*b1).b[2][2]=OPPONENT_PIECE;
						else if((*b1).b[0][2]==MACHINE_PIECE) (*b1).b[2][0]=OPPONENT_PIECE;
						else if((*b1).b[2][0]==MACHINE_PIECE) (*b1).b[0][2]=OPPONENT_PIECE;
						else if((*b1).b[2][2]==MACHINE_PIECE) (*b1).b[0][0]=OPPONENT_PIECE;
					}
					break;
				default:
					block_or_win(b1,OPPONENT_PIECE,MACHINE_PIECE);
					
			}
			return;
		}
		
		if(!opponent_was_first)
		{
			switch(move_no)
			{
				case 1:
					if((*b1).b[1][1]==MACHINE_PIECE) // they chose the center
						(*b1).b[0][2]=OPPONENT_PIECE; // choose a corner
					else if((*b1).b[1][0]==MACHINE_PIECE || (*b1).b[0][1]==MACHINE_PIECE || (*b1).b[2][1]==MACHINE_PIECE || (*b1).b[1][2]==MACHINE_PIECE) // they chose a corner
						(*b1).b[1][1]=OPPONENT_PIECE; // choose the center;
					else // they chose an edge
						block_or_win(b1,OPPONENT_PIECE,MACHINE_PIECE);
					break;
				default: block_or_win(b1,OPPONENT_PIECE,MACHINE_PIECE);				
			}
			return;
		}
	}else
	{
		srand(time(NULL));
		do
		{
			a=rand()%BOARD_SIZE;
			b=rand()%BOARD_SIZE;
		}while((*b1).b[a][b]!=0);
		(*b1).b[a][b]=OPPONENT_PIECE;
	}
	
	/* // Manual board selection for testing
	int i,j;
	manual_board_selection(*b1,&i,&j);
	(*b1).b[i][j]=piece;
	*/
}

void manual_board_selection(struct Board b1,int *row,int *col)
{
	do
	{
		printf("\nEnter valid integers for row and col: ");
		scanf("%d",row);
		scanf("%d",col);
	}while(*row<0 || *row>=BOARD_SIZE || *col<0 || *col>=BOARD_SIZE || b1.b[(*row)][(*col)]!='\0');
}

char row_match(struct Board b1)
{
	char winner='\0',current='\0';
	int i,j,match;
	
	for(i=0; i<BOARD_SIZE; i++)
	{
		match=0;
		for(j=0; j<BOARD_SIZE; j++)
		{
			if(j==0) current=b1.b[i][j];
			else 
			{
				if(current==b1.b[i][j])
					match=1;
				else 
				{
					match=0;
					break;
				}
			}
			if(current=='\0') break;
		}
		if(match!=0) // winner
		{
			winner=current;
			break;
		}
	}
	return winner;
}

char game_over(struct Board b1)
{
	struct Board b2;
	char over='\0',current;
	int i,j,complete=1,match;
	
	// any empty board spaces
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
			if(b1.b[i][j]==0)
				complete=0;
	
	// check horizontal
	over=row_match(b1);
	if(over=='\0')
	{
		// check vertical
		b2=transpose(b1);
		over=row_match(b2);
		
		// check left-to-right diagonal
		if(over=='\0')
		{
			match=1;
			for(i=1; i<BOARD_SIZE; i++)
				if(b1.b[i][i]!=b1.b[i-1][i-1] || b1.b[i][i]=='\0' ||  b1.b[i-1][i-1]=='\0')
				{
					match=0;
					break;
				}
			if(match==1) // winner
				over=b1.b[0][0];
			else
			// check right-to-left diagonal
			{
				match=1;
				for(i=BOARD_SIZE-2,j=1; i>=0; i--,j++)
					if(b1.b[j][i]!=b1.b[j-1][i+1] || b1.b[j][i]=='\0' || b1.b[j-1][i+1]=='\0')
					{
						match=0;
						break;
					}
				if(match==1) // winner
					over=b1.b[0][BOARD_SIZE-1];
				else if(complete)// result is a draw!!!
					over=TIE;
				else // not over yet...
					over='\0';
			}
		}
	}
	return over;
}

struct Board transpose(struct Board b1)
{
	struct Board b2;
	int i,j;
	
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
			b2.b[j][i]=b1.b[i][j];
	return b2;
}

struct Board init_board()
{
	struct Board b1;
	int i,j;
	for(i=0; i<BOARD_SIZE; i++)
		for(j=0; j<BOARD_SIZE; j++)
			b1.b[i][j]=0;
	return b1;
}

void display_board(struct Board b1)
{
	int i,j;
	
	printf("\n*** Board ***\n   ");
	for(i=0; i<BOARD_SIZE; i++)
		printf(" %d ",i);
	printf("\n");
	for(i=0; i<BOARD_SIZE; i++)
	{
		printf(" %d ",i);
		for(j=0; j<BOARD_SIZE; j++)
		{
			if(b1.b[i][j]==0) printf("   ");
			else printf(" %c ",b1.b[i][j]);
		}
		printf("\n\n");
	}
}

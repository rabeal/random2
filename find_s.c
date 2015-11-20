/*
	This program generates random days of the form <sky,air_temp,humidity,wind,water,forecast,enjoy_sport>
	where enjoy_sport==YES iff sky==Sunny and air_temp==Warm.  A total of TRAINING_NUM random days are
	generated and are classified in the aforementioned way.  The positive training examples
	(enjoy_day==YES) are observed in a modified-FIND-S algorithm.  The modified-FIND-S algorithm is
	augmented with a condition to terminate when the estimated hypothesis is equal to the true hypothesis 
	(see classification rule) and return the number of positive training examples p and total training examples
	q used.  The modified-FIND-S algorithm is executed EXPERIMENTS_NUM times and p and q are reported.	
*/

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

#define DEBUG 1 // 0 for no debugging

#define EXPERIMENTS_NUM 500 // number of experiments (running FIND-S)
#define TRAINING_NUM 100 // number of random training examples
#define MAKE_TRACE 1 // non-zero to force FIND-S to display trace of hypotheses
 
// attribute values
#define SKY_NUM 3 // {Sunny,Cloudy,Rainy}
#define AIR_TEMP_NUM 2 // {Warm,Cold}
#define HUMIDITY_NUM 2 // {Normal,High}
#define WIND_NUM 2 // {Strong,Weak}
#define WATER_NUM 2 // {Warm,Cool}
#define FORECAST_NUM 2 // {Same,Change}
#define ENJOY_SPORT_NUM 2 // {No,Yes}
#define NO 0
#define YES 1

// each attribute can take on additional values
#define ATTR_ADD_VALS 2 // {ALL,NONE}
#define ALL 0
#define NONE 1

struct Day
{ 
	short sky;
	short air_temp;
	short humidity;
	short wind;
	short water;
	short forecast;
	short enjoy_sport;
};


/*
	***********************
	****** Prototypes *****
	***********************
*/

struct Day random_day(struct Day classify_by);
short is_all_none(short attribute_num,short value,short ALL_NONE);
void display_hypothesis(struct Day h,short display_enjoy_sport);
struct Day find_s_mod(struct Day training_ex[],struct Day true_h,int *pnum,int *num);
struct Day most_specific_h();
short val_more_general(short attribute,short h_value,short t_value);
short equal_h(struct Day h1,struct Day h2);
struct Day find_s(struct Day training_ex[],int training_size);
void verify_find_s();


/*
	***********************
	******** Main *********
	***********************
*/

int main(int argc, char* argv[])
{
	struct Day training_ex[TRAINING_NUM];
	struct Day classify_by;
	struct Day h;
	int i;
	int j;
	int e;
	int num;
	int pnum;
	int sum=0;
	int psum=0;

	srand(time(NULL));

	if(DEBUG!=0)
	{
		printf("-------------------------------------------------------\n");
		printf("Number of experiments: %d\n",EXPERIMENTS_NUM);
		printf("Number of training examples: %d\n\n",TRAINING_NUM);
		printf("<sky,air_temp,humidity,wind,water,forecast,enjoy_sport>\n\n");
		printf("sky={0:Sunny,1:Cloudy,2:Rainy,3:ALL_VALUES,4:NO_VALUES}\n");
		printf("air_temp={0:Warm,1:Cold,2:ALL_VALUES,3:NO_VALUES}\n");
		printf("humidity={0:Normal,1:High,2:ALL_VALUES,3:NO_VALUES}\n");
		printf("wind={0:Strong,1:Weak,2:ALL_VALUES,3:NO_VALUES}\n");
		printf("water={0:Warm,1:Cool,2:ALL_VALUES,3:NO_VALUES}\n");
		printf("forecast={0:Same,1:Change,2:ALL_VALUES,3:NO_VALUES}\n");
		printf("enjoy_sport={0:No,1:Yes}\n");
		printf("-------------------------------------------------------\n\n\n\n");
		
		verify_find_s();
		printf("-------------------------------------------------------\n");
	}

	// make hypothesis to classify by: {Sunny,Warm,?,?,?,?}
	classify_by.sky=0; // Sunny
	classify_by.air_temp=0; // Warm
	classify_by.humidity=HUMIDITY_NUM; // ?
	classify_by.wind=WIND_NUM; // ?
	classify_by.water=WATER_NUM; // ?
	classify_by.forecast=FORECAST_NUM; // ?

	printf("\n\n\n-------------------------------------------------------");
	
	printf("\n!!!!! Exercise 2.10 !!!!!\n");

/*	if(DEBUG!=0)
	{
		printf("classify_by_hypothesis=");
		display_hypothesis(classify_by,NO);
	}*/
	
	for(e=0; e<EXPERIMENTS_NUM; e++)
	{
		if(DEBUG!=0)
			printf("\n-----------------------Run %02d--------------------------\n",e+1);
		
		// make random training examples
		for(i=0; i<TRAINING_NUM; i++)
		{
			training_ex[i]=random_day(classify_by);
		}
		
		// execute find_s_mod and specify the hypothesis according to + examples in training data
		h=find_s_mod(training_ex,classify_by,&pnum,&num);
		
		if(DEBUG!=0)
			printf("\nUsed Positive Training Examples:\n\n");
		
		for(i=0,j=0; i<TRAINING_NUM && j<pnum; i++)
		{
			if(DEBUG!=0 && training_ex[i].enjoy_sport==YES) // display positive examples
			{
				display_hypothesis(training_ex[i],YES);
				j++;
			}
		}
		
		
		printf("\nmodified-FIND-S terminates using %d training examples and %d + examples, h=",num,pnum);
		display_hypothesis(h,NO);
		
		sum+=num;
		psum+=pnum;
	}	
	
	if(DEBUG!=0)
		printf("\n-----------------------Answer--------------------------\n");

	printf("\n!!!!! Average training examples needed: %.1f !!!!!\n",(double)sum/EXPERIMENTS_NUM);
	printf("\n!!!!! Average + training examples needed: %.1f !!!!!\n",(double)psum/EXPERIMENTS_NUM);
	
	if(DEBUG!=0)
		printf("\n-------------------------------------------------------\n");

	return 0;
}


/*
	***********************
	****** Functions ******
	***********************
*/

// modified the FIND-S algorithm to terminate when the hypothesis h derived
// is equal to the true hypothesis true_h
struct Day find_s_mod(struct Day training_ex[],struct Day true_h,int *pnum,int *num)
{
	struct Day h=most_specific_h();
	int i;
	
	(*pnum)=0;
	(*num)=0;
	
	if(MAKE_TRACE!=0) 
		printf("\n...start FIND-S trace...\n");
	
	display_hypothesis(h,NO);
	
	for(i=0; !equal_h(h,true_h); i++)
	{
		if(i>=TRAINING_NUM) // not enough positive training examples for h to converge to true_h
		{
			(*pnum)=-1;
			break;
		}
		
		if(training_ex[i].enjoy_sport==YES) // if necessary, make attribute values more general
		{
			h.sky=val_more_general(SKY_NUM,h.sky,training_ex[i].sky);
			h.air_temp=val_more_general(AIR_TEMP_NUM,h.air_temp,training_ex[i].air_temp);			
			h.humidity=val_more_general(HUMIDITY_NUM,h.humidity,training_ex[i].humidity);			
			h.wind=val_more_general(WIND_NUM,h.wind,training_ex[i].wind);
			h.water=val_more_general(WATER_NUM,h.water,training_ex[i].water);			
			h.forecast=val_more_general(FORECAST_NUM,h.forecast,training_ex[i].forecast);
			(*pnum)++;

			if(MAKE_TRACE!=0) // display intermediate h
				display_hypothesis(h,NO);
		}
		(*num)++;
	}
	
	if(MAKE_TRACE!=0) 
		printf("...end FIND-S trace...\n\n");
	
	return h;
}

void verify_find_s()
{
	struct Day t[4];
	struct Day h;

	t[0].sky=t[0].air_temp=t[0].humidity=t[0].wind=t[0].water=t[0].forecast=0;
	t[0].enjoy_sport=YES;
	t[1].sky=t[1].air_temp=t[1].wind=t[1].water=t[1].forecast=0;
	t[1].humidity=1;
	t[1].enjoy_sport=YES;
	t[2].sky=2;
	t[2].air_temp=t[2].humidity=t[2].forecast=1;
	t[2].wind=t[2].water=0;
	t[2].enjoy_sport=NO;
	t[3].sky=t[3].air_temp=t[3].wind=0;
	t[3].humidity=t[3].water=t[3].forecast=1;
	t[3].enjoy_sport=YES;

	printf("-------------------------------------------------------\n");
	printf("!!!!! Verifying Example in Section 2.4 !!!!!\n\nTraining Examples:\n\n");

	display_hypothesis(t[0],YES);
	display_hypothesis(t[1],YES);	
	display_hypothesis(t[2],YES);	
	display_hypothesis(t[3],YES);	
	
	h=find_s(t,4);
	
	printf("\nResulting Hypothesis: ");
	display_hypothesis(h,NO);
}

// traditional FIND-S algorithm
struct Day find_s(struct Day training_ex[],int training_size)
{
	struct Day h=most_specific_h();
	int i;

	if(MAKE_TRACE!=0) 
		printf("\n...start FIND-S trace...\n");
	display_hypothesis(h,NO);
	
	for(i=0; i<training_size; i++)
	{
		if(training_ex[i].enjoy_sport==YES) // if necessary, make attribute values more general
		{
			h.sky=val_more_general(SKY_NUM,h.sky,training_ex[i].sky);
			h.air_temp=val_more_general(AIR_TEMP_NUM,h.air_temp,training_ex[i].air_temp);			
			h.humidity=val_more_general(HUMIDITY_NUM,h.humidity,training_ex[i].humidity);			
			h.wind=val_more_general(WIND_NUM,h.wind,training_ex[i].wind);
			h.water=val_more_general(WATER_NUM,h.water,training_ex[i].water);			
			h.forecast=val_more_general(FORECAST_NUM,h.forecast,training_ex[i].forecast);

			if(MAKE_TRACE!=0) // display intermediate h
				display_hypothesis(h,NO);
		}
	}
	
	if(MAKE_TRACE!=0) 
		printf("...end FIND-S trace...\n\n");
	
	return h;
}

// compare h1 and h2
short equal_h(struct Day h1,struct Day h2)
{
	return h1.sky==h2.sky && h1.air_temp==h2.air_temp && h1.humidity==h2.humidity && h1.wind==h2.wind && h1.water==h2.water && h1.forecast==h2.forecast;
}

// for the attribute and the current hypothesis value (h_value), determine if the current
// training value (t_value) should generalize h_value
short val_more_general(short attribute,short h_value,short t_value)
{
	short g;
	
	if(is_all_none(attribute,h_value,ALL) || h_value==t_value) g=h_value; // no change
	else if(is_all_none(attribute,h_value,NONE)) g=t_value; // one step more general
	else g=attribute+ATTR_ADD_VALS-2; // most general value
	
	return g;
}

// return the most specific hypothesis
struct Day most_specific_h()
{
	struct Day h;
	
	h.sky=SKY_NUM+ATTR_ADD_VALS-1;
	h.air_temp=AIR_TEMP_NUM+ATTR_ADD_VALS-1;
	h.humidity=HUMIDITY_NUM+ATTR_ADD_VALS-1;
	h.wind=WIND_NUM+ATTR_ADD_VALS-1;
	h.water=WATER_NUM+ATTR_ADD_VALS-1;
	h.forecast=FORECAST_NUM+ATTR_ADD_VALS-1;
	
	return h;
}

// display the hypothesis, if display_enjoy_sport==YES, also display label (positive/negative example)
void display_hypothesis(struct Day h,short display_enjoy_sport)
{
	printf("<%d,%d,%d,%d,%d,%d",h.sky,h.air_temp,h.humidity,h.wind,h.water,h.forecast);
	
	if(display_enjoy_sport==YES)
		printf(",%d",h.enjoy_sport);
	printf(">\n");
}

// generate a random training example and classify the example
struct Day random_day(struct Day classify_by)
{
	struct Day d;

	// make a random training example
	d.sky=rand()%SKY_NUM;
	d.air_temp=rand()%AIR_TEMP_NUM;
	d.humidity=rand()%HUMIDITY_NUM;
	d.wind=rand()%WIND_NUM;
	d.water=rand()%WATER_NUM;
	d.forecast=rand()%FORECAST_NUM;
	
	// classify the training example d
	if(is_all_none(SKY_NUM,classify_by.sky,NONE) || is_all_none(AIR_TEMP_NUM,classify_by.air_temp,NONE) || is_all_none(HUMIDITY_NUM,classify_by.humidity,NONE) || is_all_none(WIND_NUM,classify_by.wind,NONE) || is_all_none(WATER_NUM,classify_by.water,NONE) || is_all_none(FORECAST_NUM,classify_by.forecast,NONE))
		d.enjoy_sport=NO;
	else
	{
		if((is_all_none(SKY_NUM,classify_by.sky,ALL) || d.sky==classify_by.sky) 
		   && (is_all_none(AIR_TEMP_NUM,classify_by.air_temp,ALL) || d.air_temp==classify_by.air_temp) 
		   && (is_all_none(HUMIDITY_NUM,classify_by.humidity,ALL) || d.humidity==classify_by.humidity)
		   && (is_all_none(WIND_NUM,classify_by.wind,ALL) || d.wind==classify_by.wind)
 		   && (is_all_none(WATER_NUM,classify_by.water,ALL) || d.water==classify_by.water) 
		   && (is_all_none(FORECAST_NUM,classify_by.forecast,ALL) || d.forecast==classify_by.forecast))
			d.enjoy_sport=YES;
		else d.enjoy_sport=NO;
	}
	
	return d;
}

// determine if attribute value is ALL or NONE 
// (ex. attribute_num=AIR_TEMP_NUM: 0=Warm,1=Cold,2=ALL,3=NONE})
short is_all_none(short attribute_num,short value,short ALL_NONE)
{
	switch(ALL_NONE)
	{
		case NONE: return ((attribute_num+ATTR_ADD_VALS-1==value)?1:0);
		case ALL: return ((attribute_num+ATTR_ADD_VALS-2==value)?1:0);
		default: return 0;
	}
}

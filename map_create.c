//this file is to create map

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>

typedef struct map
{
	int scale;
	//using number 1-6
	//-1 needs to be filled but not the block (color as )
}mp;

mp map[6][6]={0};

void exchange(int *a,int *b)
{
	*a=*a^*b;
	*b=*a^*b;
	*a=*a^*b;
}
int eliminate(int x,int y,int number)
{
	//judging the eligible pixel and then eliminate it
	//every time look for the same number
	//using number 1-6,//(to be checked)0 as the block,as ascll
	//-1 needs to be filled but not the block
	if(number==-1)
	{
		return 0;
	}
	else
	{
		int x_up=x,x_down=x,y_left=y,y_right=y;
		int is_del=0;
		while((--x_up>=0) && map[x_up][y].scale==number){NULL;}
		while((++x_down<6) && map[x_down][y].scale==number){NULL;}
		while((--y_left>=0) && map[x][y_left].scale==number){NULL;}
		while((++y_right<6) && map[x][y_right].scale==number){NULL;}
		++x_up;--x_down;++y_left;--y_right;
		if(y_right-y_left>=2)
		{
			is_del|=1;
			for(int i=y_left;i<=y_right;i++)
			{
				map[x][i].scale=-1;
			}
			for(int i=y_left;i<=y_right;i++)
			{
				eliminate(x,i,number);
			}
		}
		if(x_down-x_up>=2)
		{
			is_del|=1;
			for(int i=x_up;i<=x_down;i++)
			{
				map[i][y].scale=-1;
			}
			for(int i=x_up;i<=x_down;i++)
			{
				eliminate(i,y,number);
			}
		}
		return is_del;//used as fall_down
	}
}
///create new pixel and print the effect of falling down
int fall_down(int is_reveal)//is_reveal used to if reveal the process
{
	//is_reveal 1 to reveal the process, 0 as NOT
	srand((unsigned)time(NULL));
	int is_none=1;
	while(is_none)
	{
		is_none=0;
		for(int y=0;y<6;y++)
		{
			for(int x=5;x>0;x--)
			{
				if(map[x][y].scale==-1)
				{
					is_none=1;
					exchange(&map[x][y].scale,&map[x-1][y].scale);
				}
			}
			if(map[0][y].scale==-1){map[0][y].scale=(rand()%6)+1;}
		}
	}
	//call the eliminate to examine it again
	is_none=0;
	for(int x=0;x<6;x++)
	{
		for(int y=0;y<6;y++)
		{
			is_none|=eliminate(x,y,map[x][y].scale);
		}
	}
	if(is_none)
	{
		fall_down(is_reveal);
	}
	return 0;
}

int main()
{
    FILE *level=fopen("level.txt","w");
    for(int i=1;i<=40;i++)
    {
        srand(i);
        for(int m=0;m<=5;m++)
        {
            for(int n=0;n<=5;n++)
            {
                map[m][n].scale=rand()%6+1;
            }
        }
        for(int m=0;m<=5;m++)
        {
            for(int n=0;n<=5;n++)
            {
                eliminate(m,n,map[m][n].scale);
            }
        }
        fall_down(0);
        for(int m=0;m<=5;m++)
        {
            for(int n=0;n<=5;n++)
            {
                fprintf(level,"%c",map[m][n].scale+48);
            }
        }
        fprintf(level,"\n");
    }
    fclose(level);
    return 0;
}
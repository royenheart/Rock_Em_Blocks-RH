/*
	copyright by royenheart
	e-mail: royenheart@outlook.com
	url: https://github.com/royenheart
		 http://royenheart.com
	project info: rock em blocks
	below target is still updating
	online mod: royenheart.com, memory the rank and the player name
	multiple map mod: random the type of the map, has multiple maps
					  pre_configure/creative mod/randomize map
*/

//this is the main file
/*quote the h_file*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>
#include <math.h>
#pragma warning(disable:4996)

/*macro define*/
#define DEFAULT 0 //default as 0
#define max_com_line 100 //the maximun size of com_line

/*structure define*/
///the struct of map
typedef struct map
{
	int scale;
	//using number 1-6
	//-1 needs to be filled but not the block (color as )
	//(demo version) fuction below is still tested
	//will be updated in the later version
	int color;//the color
}mp;
///the information of players
////the extented info for format users
typedef struct info_extented
{
	//used for format users,stored in internet
	int sex;
	//1 male 2 female 3 unknow
	time_t register_date;
	time_t last_play_date;
	int rank;
	int total_score;
	int experience;
	int grade;
	int achievements[30];
}ie;
typedef struct users
{
	char name[21];
	int max_level;
	int type;
	ie info;
	//1 as format users 0 as guests
	//guests's data is stored in yh pre_data.txt
}usr;
/*global var define*/
//secure
char illegal_name[100][21]={'\0'};//check some illegal word is in
int ill_num=-1;
//used for the create_map,to print the map line by line
mp map[6][6]={0};
//tools to initial the map
char level[40][37]={'\0'};///map data every level
int com_level=DEFAULT;///current level,every level has 100 scores
//used for output the intro with language_file
char pha_sta[100][max_com_line]={'\0'};
//game_related
usr users;//user_data
int *usr_da_list[]={&users.type,&users.max_level};//make a list to initial
int score=100;//reflect the score one level
int total_add_score=DEFAULT;//reflect the score once played
int pass_level=DEFAULT;//level passed one game
char command='\0';//command to control the game
time_t start,end;//one game play total time 
int play_time[3];
//system_info
int num_the[6]={1,2,3,4,5,6};
int fore_color=15;
int bac_color=DEFAULT;
int *color_da_list[]={&num_the[0],&num_the[1],&num_the[2],
					  &num_the[3],&num_the[4],&num_the[5],
					  &fore_color,&bac_color};
int themes[20][8];
int themes_amo;
int com_themes=DEFAULT;
int sys_lan=1;//system language
//config
int *config[]={&com_themes,&sys_lan};
int cfg_num=DEFAULT;//start from 0
/*function declare*/
void SetColor(UINT uFore,UINT uBack);
void print_map(void);
void com_line(int l,int r);
/*function define*/
//config read
void initial_config(void)//demo
{
	FILE *cfg;
	if(!(cfg=fopen("config.rsav","r")))
	{
		cfg=fopen("config.rsav","w");
		fprintf(cfg,"01\n");
		fclose(cfg);
	}
	else
	{
		for(int i=0;i<=1;i++)
		{
			*config[i]=fgetc(cfg)-48;
		}
		fclose(cfg);
	}
}
//themes read
void initial_themes(void)
{
	FILE *the_file;
	if(!(the_file=fopen("themes.txt","r")))
	{
		the_file=fopen("themes.txt","w");
		fprintf(the_file,"%02d%02d%02d%02d%02d%02d%02d%02d\n",
						 num_the[0],num_the[1],num_the[2],
						 num_the[3],num_the[4],num_the[5],
						 fore_color,bac_color);
		fclose(the_file);
		themes_amo=0;
	}
	else
	{
		int amoun=-1;
		while(!feof(the_file) && ++amoun<=19)
		{
			char temp[18]={"\0"};
			char num[3]={"\0"};
			fgets(temp,18,the_file);
			if(temp[0]=='\n'){amoun--;break;}
			for(int i=0;i<=7;i++)
			{
				num[0]=temp[i*2];num[1]=temp[i*2+1];
				themes[amoun][i]=atoi(num);
			}
		}
		fclose(the_file);
		themes_amo=amoun-1;
	}
}
//themes chaneg
void change_themes(int thm_num)
{
	for(int i=0;i<=7;i++)
	{
		*color_da_list[i]=themes[thm_num%(themes_amo+1)][i];
	}
	SetColor(fore_color,bac_color);
}
//time calculate
void time_cal(int total_time)
{
	for(int i=0;i<=2;i++)
	{
		int rate=3600/pow(60,i);
		play_time[i]=total_time/rate;
		total_time-=play_time[i]*rate;
	}
}
//secure-check if the name is illegal
void initial_ill(void)
{
	FILE *ill_f=fopen("illegal_name.txt","r");
	while(!feof(ill_f))
	{
		fgets(illegal_name[++ill_num],21,ill_f);
		int temp=strlen(illegal_name[ill_num])-1;
		illegal_name[ill_num][temp]='\0';
	}
	ill_num--;
	fclose(ill_f);
}
int Check_Name(char name[])
{
	for(int i=0;i<=ill_num;i++)
	{
		if(strstr(name,illegal_name[i]))
		{
			return 1;
		}
	}
	return 0;
}
//color set
void SetColor(UINT uFore,UINT uBack)
{
	HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle,uFore | uBack*0x10);
}
//title change
void SetTitle(LPCSTR lpTitle) 
{
	SetConsoleTitle(lpTitle);
}
//above are from //https://blog.csdn.net/cjz2005/article/details/104358000
void pANDs(VOID){print_map();Sleep(150);system("cls");Sleep(150);}
void exchange(int *a,int *b)
{
	*a=*a^*b;
	*b=*a^*b;
	*a=*a^*b;
}
///initial or change the user
void spilt(char nee[])
{
	int i=-1,num=-1;
	char change[21]={'\0'};
	while(nee[++i]!=':'){NULL;}
	while(nee[++i]!='\n')
	{
		change[++num]=nee[i];
	}
	strcpy(nee,change);
}
int user_change(char dat[])
{
	FILE *uss=fopen("usr_data.txt","r");
	char temp[21]={"\0"};
	fgets(temp,sizeof(char)*21,uss);
	spilt(temp);
	if(strcmp(temp,dat)==0)
	{
		com_line(52,52);
		strcpy(users.name,temp);
		for(int i=0;i<=1;i++)
		{
			fgets(temp,sizeof(char)*21,uss);
			spilt(temp);
			*usr_da_list[i]=atoi(temp);
		}
	}
	else
	{
		com_line(53,53);
		if(tolower(getchar())=='y')
		{
			getchar();
			fclose(uss);
			uss=fopen("usr_data.txt","w");
			fprintf(uss,"name:%s\n"
				   		"type:1\n"
				   		"max_level:0\n",
						dat);
			fclose(uss);
			user_change(dat);
			return 0;
		}
		else
		{
			getchar();fclose(uss);
			return -1;
		}
	}
	fclose(uss);
	return 0;
}
///every sentence has its id in arr
///introduce the language file:to change the words of UI(en-gb/zh-cn)
void intro_lan(int la)
{
	//i: 1 as Chinese; 2 as English
	FILE *p;
	int num=-1;
	if(la==1)
	{
		p=fopen("zh_lan.txt","r");
		sys_lan=1;
	}
	else if(la==2)
	{
		p=fopen("en_lan.txt","r");
		sys_lan=2;
	}
	while (!feof(p))
	{
		fgets(pha_sta[++num],max_com_line,p);
	}
	fclose(p);
}
///settings change
///print the graph of the map
void print_map(void)
{
	SetColor(fore_color,bac_color);
	com_line(5,5);printf("%s\n",users.name);
	printf("MAP:%02d|Score:%03d\n",com_level,score);
	printf("Y--------------------------->\n");
	printf("X @ | 1 | 2 | 3 | 4 | 5 | 6 |\n");
	for(int i=0;i<=5;i++)
	{
		SetColor(fore_color,bac_color);
		printf("| %d |",i+1);
		for(int j=0;j<=5;j++)
		{
			if(map[i][j].scale==-1)
			{
				SetColor(bac_color,bac_color);
				printf(" %d ",8);
				if(j<5){printf(" ");}
				continue;
			}
			SetColor(num_the[map[i][j].scale-1],bac_color);
			printf(" %d ",map[i][j].scale);
			if(j<5){printf(" ");}
		}
		SetColor(fore_color,bac_color);
		printf("|\n");
	}
	printf("+----------------------------\n");
}
///initial the map from txt (start game)
///or initial the map every new level
void initial_map(int le)
{
	score-=100;
	for(int i=0;i<=5;i++)
	{
		for(int j=0;j<=5;j++)
		{
			map[i][j].scale=level[le][i*6+j]-48;
		}
	}
}
///for cheat_com explain
int cheat_ex_com(char che_com[])
{
	if(strstr(che_com,"quit")){return 2;}
	if(strstr(che_com,"add score"))
	{
		printf("add how many?\n");
		int c_s;
		scanf("%d",&c_s);getchar();
		score+=c_s;
		return 1;
	}
	if(strstr(che_com,"pass level"))
	{
		printf("pass how many levels?\n");
		int c_l;
		scanf("%d",&c_l);getchar();
		com_level+=c_l;score+=100;
		initial_map(com_level);
		return 1;
	}
	if(strstr(che_com,"help"))
	{
		printf("add score\n"
			   "pass level\n");
		return 1;
	}
	return 0;
}
///output the command or note sentence
void com_line(int l,int r)
{
	for(int i=l;i<=r;i++){printf("%s",pha_sta[i]);}
}
///eliminate the eligible pixel
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
				if(map[x][i].scale==number)
				{
					total_add_score+=number;
					score+=number;
				}
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
				if(map[i][y].scale==number)
				{
					total_add_score+=number;
					score+=number;
				}
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
	//every time change the structure of map,call the 'print_map' function
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
		if(is_reveal){pANDs();}
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
		if(is_reveal){pANDs();}
		fall_down(is_reveal);
	}
	return 0;
}
//user data refresh
void usr_data_rew(void)//demo
{
	if(com_level>users.max_level){users.max_level=com_level;}
	FILE *update_usr=fopen("usr_data.txt","w");
	fprintf(update_usr,"name:%s\n"
					   "type:%d\n"
					   "max_level:%d\n",
					   users.name,users.type,users.max_level);
	fclose(update_usr);
}
//settings
void setting_change(char unable[])
{
	do
	{
		system("cls");
		if(command=='\0'){NULL;}
		else if(strchr(unable,command))
		{
			printf("%c:",command);com_line(12,12);
		}
		else if(command=='l')
		{
			com_line(13,15);
			int lan;scanf("%d",&lan);getchar();
			intro_lan(lan);
			sys_lan=lan;
		}
		else if(command=='u')
		{
			char id[21]={'\0'};
			com_line(50,50);
			scanf("%s",id);getchar();
			//check if is okey
			int is_legal;
			do
			{
				is_legal=0;
				//check if all is eng
				for(int i=0;i<strlen(id)-1;i++)
				{
					if(!isalpha(id[i]))
					{
						is_legal=1;
						break;
					}
				}
				if(Check_Name(id))
				{
					is_legal=1;
				}
				if(is_legal)
				{
					com_line(57,58);
					scanf("%s",id);getchar();
				}
			}while(is_legal);
			user_change(id);
			com_level=users.max_level;
			score=100;
			initial_map(com_level);
		}
		else if(command=='h')
		{
			com_line(6,10);
		}
		else if(command=='c')
		{
			com_line(89,89);
			SetColor(fore_color,bac_color);
			printf("#############################\n"
			       "#themes ls# 1 2 3 4 5 6 f b #\n");
			for(int i=0;i<=themes_amo;i++)
			{
				printf("#themes:%02d#",i);
				for(int m=0;m<=7;m++)
				{
					SetColor(themes[i][m],bac_color);
					printf(" S");
				}
				SetColor(fore_color,bac_color);
				printf(" #\n");
			}
			printf("#############################\n");
			int chose;
			scanf("%d",&chose);getchar();
			change_themes(chose);
			com_themes=chose;
			system("cls");
		}
		else if(command!='q')
		{
			printf("%c %s",command,pha_sta[11]);
		}
		com_line(16,16);
		com_line(5,5);
		printf("%s\n",users.name);
		com_line(16,24);
		scanf("%c",&command);getchar();
		if(command=='\n'){getchar();}
		command=tolower(command);
	} while (command!='q');
}
/*main function*/
int main(char argc,char *argv[])
{
	char com_unable[30]={'\0'};//unable command list
	int is_cheated=0;
	//config initial
	initial_config();
	//basic language
	intro_lan(sys_lan);
	//set title
	char game_title[]="Welcome to Rock_Em_Blocks! ";
	strcat(game_title,pha_sta[84]);
	SetTitle(game_title);
	//cols=how many words every line,lines=how many lines
	//fixed the size of the console,methods come from:
	//https://bbs.csdn.net/topics/392191971
	system("mode con cols=80 lines=40");
	HWND hWnd = GetConsoleWindow();
    RECT rc;
    GetWindowRect(hWnd, &rc);
    SetWindowLongPtr(hWnd, 
        GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & 
		~WS_THICKFRAME & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX
	);
    SetWindowPos(hWnd,
        NULL,
        rc.left, rc.top,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL
	);
	//initial the user
	FILE *ur;
	if(!(ur=fopen("usr_data.txt","r")))
	{
		ur=fopen("usr_data.txt","w");
		fprintf(ur,"name:admin\n"
				   "type:0\n"
				   "max_level:0\n");
		fclose(ur);
		user_change("admin");
	}
	else
	{
		char exer[21];
		fgets(exer,sizeof(char)*21,ur);
		spilt(exer);
		strcpy(users.name,exer);
		for(int i=0;i<2;i++)
		{
			fgets(exer,sizeof(char)*21,ur);
			spilt(exer);
			*usr_da_list[i]=atoi(exer);
		}
		fclose(ur);
	}
	//initial basic settings
	initial_ill();//include the illegal name list
	initial_themes();//include the themes config from the txt
	change_themes(com_themes);
	//read map_info from txt
	FILE *ini_m=fopen("level.txt","r");
	int i=-1;
	while (!feof(ini_m))
	{
		fgets(level[++i],38,ini_m);
	}
	fclose(ini_m);
	com_level=users.max_level;
	initial_map(com_level);
	//setting initial
	setting_change(com_unable);
	if(com_level==40)
	{
		com_line(88,88);
		scanf("%c",&command);getchar();
		command=tolower(command);
		if(command=='y')
		{
			score=100;
			com_level=0;
			initial_map(com_level);
		}
		else
		{
			score=100;
			com_level=39;
			initial_map(com_level);
		}
		command='\0';
	}
	system("cls");
	start=time(NULL);
	//start the game
	do
	{
		if(command=='p')
		{
			while(1)
			{
				//first line then column
				int posxa,posya,posxb,posyb;
				print_map();
				com_line(33,34);
				scanf("%d,%d %d,%d",&posxa,&posya,
									&posxb,&posyb);
				getchar();
				system("cls");
				if(posxa+posxb+posya+posyb==-4){break;}				
				--posxa;--posxb;--posya;--posyb;
				//check if the pos is illeagle
				if((posxa>5 || posxa<0) ||
				   (posxb>5 || posxb<0) ||
				   (posya>5 || posya<0) ||
				   (posyb>5 || posyb<0)	)
				{
					com_line(35,35);
				}
				else if(abs(posxa-posxb)+abs(posya-posyb)!=1)//not nearby
				{
					com_line(36,36);
				}
				else
				{
					exchange(&map[posxa][posya].scale,&map[posxb][posyb].scale);
					exchange(&map[posxa][posya].color,&map[posxb][posyb].color);
					int sc_a=map[posxa][posya].scale,sc_b=map[posxb][posyb].scale;
					int pd=eliminate(posxa,posya,sc_a);
					pd|=eliminate(posxb,posyb,sc_b);
                    if(pd)
					{
						pANDs();
						fall_down(1);
					}
					else
					{
						pANDs();
						exchange(&map[posxa][posya].scale,&map[posxb][posyb].scale);
						exchange(&map[posxa][posya].color,&map[posxb][posyb].color);
						pANDs();
						com_line(37,38);
						score-=5;
					}
				}
				if(score>=100)
				{
					com_level++;
					pass_level++;
					usr_data_rew();
					if(com_level<40)
					{
						initial_map(com_level);
						com_line(39,40);
						printf("%d!!!\n",com_level);
						break;
					}
				}
				else if(score<0)
				{
					com_line(41,41);
					break;
				}
			}
			if(score<0){break;}
			if(com_level==40)
			{
				com_line(54,54);
				break;
			}
		}
		else if(command=='s')
		{
			command='\0';
			memset(com_unable,0,30);
			strcat(com_unable,"u");
			setting_change(com_unable);
			system("cls");	
		}
		else if(command=='i')
		{
			com_line(66,66);
			while(1)
			{
				com_line(70,70);
				printf("%d\n",users.max_level);
				int chose_le;
				scanf("%d",&chose_le);getchar();
				if(chose_le>users.max_level)
				{
					com_line(67,67);
					com_line(69,69);
				}
				else if(chose_le>39 || chose_le<0)
				{
					com_line(68,68);
					com_line(69,69);
				}
				else
				{
					score=100;
					initial_map(chose_le);
					com_level=chose_le;
					break;
				}
			}
			system("cls");
		}
		else if(command=='t')
		{
			int abab=1;
			com_line(71,71);
			SetColor(fore_color,bac_color);
			printf("Color num 0's color is black\n");
			for(;abab<=15;abab++)
			{
				SetColor(abab,bac_color);
				printf("Color num %d's color\n",abab);
			}
			while(1)
			{
				com_line(72,72);
				for(i=0;i<=5;i++)
				{
					SetColor(num_the[i],bac_color);
					printf("%d.num %d's color\n",i,i+1);
				}
				SetColor(fore_color,bac_color);
				printf("fore color's color\n");
				printf("back color's color is %d\n",bac_color);
				com_line(73,74);
				int chan_co,what_co;
				while(1)
				{
					scanf("%d",&chan_co);getchar();
					if(chan_co<0 || chan_co>7)
					{
						com_line(76,76);
					}
					else{break;}
				}
				com_line(75,75);
				scanf("%d",&what_co);getchar();
				what_co%=16;
				*color_da_list[chan_co]=what_co;
				com_line(77,77);
				int t_com;
				scanf("%d",&t_com);getchar();
				if(t_com==9)
				{
					FILE *the_rew=fopen("themes.txt","a");
					for(int i=0;i<=7;i++)
					{
						fprintf(the_rew,"%02d",*color_da_list[i]);
					}
					fprintf(the_rew,"\n");
					fclose(the_rew);
					break;
				}
				else if(t_com==10)
				{
					com_line(71,71);
					for(abab=0;abab<=15;abab++)
					{
						SetColor(abab,bac_color);
						printf("Color num %d's color\n",abab);
					}
				}
			}
			SetColor(fore_color,bac_color);
			system("cls");
		}
		else if(command=='f')
		{
			com_line(78,80);
			scanf("%c",&command);getchar();
			command=tolower(command);
			if(command=='y')
			{
				int pre_score=score;
				int pre_tt_score=total_add_score;
				srand((unsigned)time(NULL));
				for(int i=0;i<=5;i++)
				{
					for(int j=0;j<=5;j++)
					{
						map[i][j].scale=rand()%6+1;
					}
				}
				for(int i=0;i<=5;i++)
				{
					for(int j=0;j<=5;j++)
					{
						eliminate(i,j,map[i][j].scale);
					}
				}
				fall_down(0);
				score=pre_score-10;
				total_add_score=pre_tt_score;
				if(score<0)
				{
					com_line(41,41);
					break;
				}
			}
			system("cls");
		}
		else if(command=='g')
		{
			com_line(81,86);
		}
		else if(command=='~')
		{
			is_cheated=1;
			com_line(59,63);
			char che_com[30]={"\0"};
			do
			{
				int com_fou;
				gets(che_com);
				com_fou=cheat_ex_com(che_com);
				if(!com_fou){com_line(64,64);}
				if(com_fou==1){com_line(62,63);}
			} while (!strstr(che_com,"quit"));
			system("cls");
		}
		else if(command!='q')
		{
			printf("%c %s",command,pha_sta[11]);
		}
		print_map();	
		com_line(25,32);	
		scanf("%c",&command);getchar();
		if(command=='\n'){getchar();}
		command=tolower(command);
		system("cls");
	}while(command!='q');
	if(!is_cheated)
	{
		end=time(NULL);
		usr_data_rew();
		com_line(42,42);
		printf("%d\n",total_add_score);
		switch (total_add_score/100)
		{
			case 3:
				com_line(44,44);
				break;
			case 2:
				com_line(45,45);
				break;
			case 1:
				com_line(46,46);
				break;
			case 0:
				com_line(47,47);
				break;
			default:
				com_line(43,43);
				break;
		}
		com_line(48,48);
		printf("%d\n",pass_level);
		time_cal((int)difftime(end,start));
		printf("You have played "
			   "%d hours %d mins %d secs\n",
			   play_time[0],play_time[1],play_time[2]);
		com_line(87,87);
		com_line(49,49);
	}
	else
	{
		com_line(65,65);
	}
	//save the config_change
	FILE *cfg_c=fopen("config.rsav","w");
	for(int i=0;i<=1;i++)
	{
		fprintf(cfg_c,"%d",*config[i]);
	}
	fprintf(cfg_c,"\n");
	fclose(cfg_c);
	system("pause");
	return 0;
}
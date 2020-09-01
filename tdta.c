/*
                   The Dark Tower Adventures door game
                 Copyright (C) 2018-2020  Dan Richter(RCS)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#define VERSION_MAJOR 0
#define VERSION_MINOR 12

#ifndef VERSION_TYPE
#define VERSION_TYPE "alpha"
#endif
#define VERSION_DAY 01
#define VERSION_MONTH "SEP"
#define VERSION_YEAR 2020


#ifdef WIN32
#define _MSC_VER 1
#define PATH_SEPERATOR "\\"
//#endif // WIN32
#else
#define PATH_SEPERATOR '/'
#define NEW_LINE '\n'
#endif // WIN32

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "OpenDoor.h"
#include "tdta.h"
#include "tdta2.h"

int DAILYMOVES=500;
int DAILYHUMANFIGHTS=5;

struct PlyrRec Plyr;
struct PlyrRec User;
struct wrec weapons[17];
struct arec armours[17];
struct master Mstr[13];
struct master Master;
struct ibbslinks links[10];

extern void setweapons();
extern void setarmour();
extern void setmstr();
int get_player_idx();
void add_player_idx();
int load_player();
int scan_for_player();
void SavePlyr();          //saves current player
void SaveUser();          //saves other player
void play_game();
void town(int x);
void bnb();
void dogpark();
void gamepause();
void bank();
void room();
void eat();
int gameend(int gameerrorlevel);
void tower();
void listplayers();
void playerstats();
void dailynews();
void monfight(int monstype,int usernumber);
void ranitem(int randomitem);
int findtotaldigits(unsigned long int no);
void weaponshop();
void armourshop();
void healer();
void map(int z);
char* goldconvert(long int x);
void useitems();
int checkitems();
void readconfig();
char* removefirst(char *str);
char* removelast(char *str);
char* removechar(char *str, char ch);
void newline_kill(char a[]);
char setmap(int mapnumber,int xcord,int ycord);
char *substring(char *string, int position, int length);
void insert_substring(char *a, char *b, int position);
void backstory();
void ibbs_in();
void ibbs_out();

char comma[25];
char mapchar;
char ansipath[30];
char datapath[10];
char logpath[10];
char logname[25];
char SystemName[20];
char Sysop[15];
char systemlink[4];
int totallinks;
int systemlinki;
bool IBBS=FALSE;
char league[4];
char systemaddress[20];
char linkaddress[20];
char out_directory[20];
char in_directory[20];

void logentry(int entry)  //1 log in - 2 log out - 3 fought trainer - 4 fought player - 5 new player - 6 ibbsin - 7 ibbsout
{
    struct tm* current_time;
    time_t s = time(NULL);
    current_time = localtime(&s);
    sprintf(logname,"%s%d%02d%02d.log",logpath,current_time->tm_year+1900,current_time->tm_mon+1,current_time->tm_mday);

    FILE *fptr;
    fptr=fopen(logname,"a");
    if (fptr==NULL)
    {
        fptr=fopen(logname,"w");
        fclose(fptr);
        logentry(1);
    }
    switch (entry)
    {
    case 1:
        fprintf(fptr,"\r\n----------  %02d %s %d, %s %d.%d %s (%02d %s %d)\r\n",current_time->tm_year+1900,months[current_time->tm_mon],current_time->tm_mday,od_control.od_prog_name,VERSION_MAJOR,VERSION_MINOR,VERSION_TYPE,VERSION_DAY,VERSION_MONTH,VERSION_YEAR);
        fprintf(fptr,"> %02d:%02d:%02d %s has entered the game\r\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,Plyr.Alias);
        break;
    case 2:
        fprintf(fptr,"> %02d:%02d:%02d %s has exited the game\r\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,Plyr.Alias);
        break;
    case 3:
        fprintf(fptr,"> %02d:%02d:%02d %s has fought a trainer\r\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,Plyr.Alias);
        break;
    case 4:
        fprintf(fptr,"> %02d:%02d:%02d %s has fought another player\r\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,Plyr.Alias);
        break;
    case 5:
        fprintf(fptr,"> %02d:%02d:%02d %s has created a new player\r\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,Plyr.Alias);
        break;
    case 6:
        fprintf(fptr,"> %02d:%02d:%02d Incoming IBBS information processed\r\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec);
        break;
    case 7:
        fprintf(fptr,"> %02d:%02d:%02d Outbound IBBS information processed\r\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec);
        break;
    }

    fclose(fptr);
}

#ifdef ODPLAT_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpszCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
    char ch;
    int done = 0;
    strcpy(od_control.od_prog_name, "The Dark Tower Adventures");
    sprintf(od_control.od_prog_version,"%d.%d %s",VERSION_MAJOR,VERSION_MINOR,VERSION_TYPE);
    strcpy(od_control.od_prog_copyright, "Copyright 2018-2020 by Dan Richter(RCS)");
    od_init();
    #if defined(_MSC_VER) || defined(WIN32)
    {
      od_parse_cmd_line(LPSTR pscCmdLine);
    }
    #else
    {
      od_parse_cmd_line(argc, argv);
    }
    #endif // defined
    od_clr_scr();
    readconfig();
    od_clr_scr();
    od_printf("`blue`Traveling to The Dark Tower");
    od_sleep(500);
    od_printf("`blue`.");
    od_sleep(500);
    od_printf("`blue`.");
    od_sleep(500);
    od_printf("`blue`.");
    od_sleep(500);
    char splashansi[50];
    sprintf(splashansi,"%s%s",ansipath,"splash");
    od_send_file(splashansi);
    od_clear_keybuffer();
    od_get_key(TRUE);
    while (!done) {
        od_clr_scr();
        char menuansi[50];
        sprintf(menuansi,"%s%s",ansipath,"menu");
        od_send_file(menuansi);
        od_set_cursor(5,33);
        od_printf("`bright blue`V`blue`ersion %d.%d %s",VERSION_MAJOR,VERSION_MINOR,VERSION_TYPE);
        od_set_cursor(6,32);
        od_printf("`bright blue`C`blue`ompiled %02d %s %d",VERSION_DAY,VERSION_MONTH,VERSION_YEAR);
        od_set_cursor(12,32);
        od_printf("`blue`(`bright blue`S`blue`)`bright blue`tart your quest");
        od_set_cursor(13,32);
        od_printf("`blue`(`bright blue`I`blue`)`bright blue`nstructions");
        od_set_cursor(14,32);
        od_printf("`blue`(`bright blue`L`blue`)`bright blue`ist Players");
        od_set_cursor(15,32);
        od_printf("`blue`(`bright blue`Q`blue`)`bright blue`uit back to BBS");
        od_set_cursor(24,1);
        od_printf("%s\r\n%s",logpath,logname);
        od_clear_keybuffer();
        ch = od_get_answer("LlIiVvSsQq");
        switch (ch) {
            case 'L':
                od_clr_scr();
                od_send_file("tdtaplyr.txt");
                gamepause();
                break;
            case 'I':
                od_clr_scr();
                char infoansi[50];
                sprintf(infoansi,"%s%s",ansipath,"info");
                od_send_file(infoansi);
                gamepause();
                break;
            case 'V':
                od_clr_scr();
                od_printf("                           `cyan`\n\n\nT`blue`he `cyan`D`blue`ark `cyan`T`blue`ower `cyan`A`blue`dventures\r\n\n");
                od_printf("                               `cyan`V`blue`ersion %d.%d %s\r\n\n",VERSION_MAJOR,VERSION_MINOR,VERSION_TYPE);
                od_printf("                              `cyan`C`blue`ompiled: %02d %s %d\r\n",VERSION_DAY,VERSION_MONTH,VERSION_YEAR);
                gamepause();
                break;
            case 'S':
                play_game();
                break;
            case 'Q':
                done = 1;
                break;
        }
    }
    return 0;
}

void readconfig()
{
  char s[70];
  char temp[70];
  int x=1;
  FILE *fptr;
  fptr = fopen(CFGFile,"r");
  if (!fptr) gameend(-1);
  while (fgets(s, sizeof(s),fptr) != NULL)
  {
    if (strncmp(s,"DAILYMOVES=",11)==0)
    {
      strcpy(temp,strchr(s,'='));
      removefirst(temp);
      DAILYMOVES=atoi(temp);
    }
    if (strncmp(s,"DAILYHUMANFIGHTS=",17)==0)
    {
      strcpy(temp,strchr(s,'='));
      removefirst(temp);
      DAILYHUMANFIGHTS=atoi(temp);
    }
    if (strncmp(s,"SystemName=",11)==0)
    {
      strcpy(SystemName,strchr(s,'='));
      removefirst(SystemName);
      newline_kill(SystemName);
    }
    if (strncmp(s,"Sysop=",6)==0)
    {
      strcpy(Sysop,strchr(s,'='));
      removefirst(Sysop);
      newline_kill(Sysop);
    }
    if (strncmp(s,"ANSIPath=",9)==0)
    {
      strcpy(ansipath,strchr(s,'='));
      strcpy(ansipath,removefirst(ansipath));
      newline_kill(ansipath);
    }
    if(strncmp(s,"DataPath=",9)==0)
    {
      strcpy(datapath,strchr(s,'='));
      strcpy(datapath,removefirst(datapath));
      newline_kill(datapath);
    }
    if(strncmp(s,"LogPath=",8)==0)
    {
        strcpy(logpath,strchr(s,'='));
        strcpy(logpath,removefirst(logpath));
        newline_kill(logpath);
    }
    if(strncmp(s,"System_Link_Number=",19)==0)
    {
      strcpy(systemlink,strchr(s,'='));
      strcpy(systemlink,removefirst(systemlink));
      newline_kill(systemlink);
      systemlinki=atoi(systemlink);
      if(strlen(systemlink)==1)
      {
        char zero[2]="0";
        char tmp[2];
        strcpy(tmp,systemlink);
        //strcpy(systemlink,tmp);
        sprintf(systemlink,"%s%s",zero,tmp);
      }
    }
    if(strncmp(s,"InterBBS=",8)==0)
    {
      char temp[20];
      strcpy(temp,strchr(s,'='));
      removefirst(temp);
      if((strncmp((temp),"true",4)==0)||(strncmp((temp),"TRUE",4)==0)) IBBS=TRUE;
    }
    if(strncmp(s,"League=",7)==0)
    {
      char temp[20];
      strcpy(temp,strchr(s,'='));
      removefirst(temp);
      strcpy(league,temp);
      newline_kill(league);
    }
    if(strncmp(s,"Link_Number=",12)==0)
    {
      char tmp[2];
      strcpy(links[x].linknumber,strchr(s,'='));
      //strcpy(links[x].linknumber,x);
      strcpy(links[x].linknumber,removefirst(links[x].linknumber));
      newline_kill(links[x].linknumber);
      if(strlen(links[x].linknumber)==1)
      {
        char zero[2]="0";
        //strcpy(tmp,"");
        strcpy(tmp,links[x].linknumber);
        sprintf(links[x].linknumber,"%s%s",zero,tmp);
      }
      //od_printf("%d %s %s\r\n",x,tmp,links[x].linknumber);
      //gamepause();
      totallinks=x;
      //x++;        //=atoi(links[x].linknumber);
    }
    if(strncmp(s,"Link_Address=",13)==0)
    {
      strcpy(links[x].linkaddress,strchr(s,'='));
      strcpy(links[x].linkaddress,removefirst(links[x].linkaddress));
      newline_kill(links[x].linkaddress);
      strcpy(links[x].linkfilename,removechar(links[x].linkaddress,':'));
      //strcpy(links[x].linkfilename,removefirst(links[x].linkaddress));
      //strcpy(links[x].linkfilename,removechar(links[x].linkfilename,*slash));
    }
    if(strncmp(s,"Link_Out_Directory=",19)==0)
    {
      strcpy(links[x].linkout,strchr(s,'='));
      strcpy(links[x].linkout,removefirst(links[x].linkout));
      newline_kill(links[x].linkout);
      //od_set_cursor(x,1);
      //od_printf("%s",links[x].linkout);
      //gamepause();
      x++;
    }
    if(strncmp(s,"In_Directory=",13)==0)
    {
      strcpy(in_directory,strchr(s,'='));
      strcpy(in_directory,removefirst(in_directory));
      newline_kill(in_directory);
    }
    strcpy(s,"");
   }
   if (IBBS==TRUE)
   {
     for (int y=1;y<x;y++)
     {
       sprintf(links[y].linkfilename,"%stdta%s%s.%s",links[y].linkout,links[y].linknumber,systemlink,league);
       //od_printf("%s - %s            ",links[y].linknumber,links[y].linkfilename);
       //gamepause();
       //links[y].linkfilename
     }
   }
  fclose(fptr);
}

char* removechar(char *str, char ch)
{
  int i, len, j;
  len = strlen(str);
  for (i=0;i<len;i++)
  {
    if (str[i] == ch)
    {
      for (j=i;j<len;j++)
      {
        str[j]=str[j+1];
      }
      len--;
      i--;
    }
  }
  return str;
}

char* removefirst(char *str)
{
  int i=0;
  int len=strlen(str);
  while(i<len)
  {
    str[i]=str[i+1];
    i++;
  }
  return str;
}

void newline_kill(char a[])
{
  size_t s1 = strlen(a);
  if(s1>0 && a[s1-1]=='\n') a[s1-1]='\0';
  if(s1>0 && a[s1-1]=='\r') a[s1-1]='\0';
  if(s1>0 && a[s1-2]=='\r') a[s1-2]='\0';
}

void town(int x)
{
  char ch;
  int done=0,temp;
  Plyr.map=x;
  if (Plyr.dead==true)
  {
    Plyr.dead=false;
    Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
    SavePlyr();
    map(Plyr.map);
  }
  if ((Plyr.cord_x != 0) && (Plyr.cord_y !=0))
  {
    Plyr.cord_x=0;
    Plyr.cord_y=0;
    SavePlyr();
  }
  while (!done)
  {
    od_clr_scr();
    char townansi[40];
    sprintf(townansi,"%s%s",ansipath,"town");           //need ansi screens for the different towns
    od_send_file(townansi);
    od_set_cursor(4,48);
    od_printf("`blue`(`bright blue`F`blue`)`bright blue`ollow The Beam");
    od_set_cursor(5,48);
    od_printf("`blue`(`bright blue`P`blue`)`bright blue`revious Map");
    od_set_cursor(6,48);
    od_printf("`blue`(`bright blue`T`blue`)`bright blue`rainer Battle");
    od_set_cursor(7,48);
    od_printf("`blue`(`bright blue`K`blue`)`bright blue`ick A$$ Weapons");
    od_set_cursor(8,48);
    od_printf("`blue`(`bright blue`A`blue`)`bright blue`rmour Shop");
    od_set_cursor(9,48);
    od_printf("`blue`(`bright blue`H`blue`)`bright blue`ealers");
    od_set_cursor(10,48);
    od_printf("`blue`(`bright blue`Y`blue`)`bright blue`e Olde Bank");
    od_set_cursor(11,48);
    od_printf("`blue`(`bright blue`B`blue`)`bright blue`ig Bear Bed n Breakfast");
    od_set_cursor(13,48);
    od_printf("`blue`(`bright blue`D`blue`)`bright blue`aily News");
    od_set_cursor(14,48);
    od_printf("`blue`(`bright blue`V`blue`)`bright blue`iew Your Stats");
    od_set_cursor(15,48);
    od_printf("`blue`(`bright blue`L`blue`)`bright blue`ist Other Gunslingers");
    od_set_cursor(17,48);
    od_printf("`blue`(`bright blue`Q`blue`)`bright blue`uit To Fields");

    od_set_cursor(23,1);
    od_printf("`blue`Your command, `bright blue` %s:",Plyr.Alias);
    ch = od_get_answer("SsFfBbAaKkDdHhYyLlVvTtPpQq+-");
    switch (ch)
    {
      case 'F':
        map(Plyr.map);
        SavePlyr();
        break;
      case 'P':
        temp = Plyr.map-1;
        if (temp < 1) temp=1;
        map(temp);
        SavePlyr();
        break;
      case 'B':
        bnb();
        SavePlyr();
        break;
      case 'A':
        armourshop();
        SavePlyr();
        break;
      case 'K':
        weaponshop();
        SavePlyr();
        break;
      case 'D':
        dailynews();
        SavePlyr();
        break;
      case 'H':
        healer();
        SavePlyr();
        break;
      case 'Y':
        bank();
        SavePlyr();
        break;
      case 'L':
		SavePlyr();
        listplayers();
        break;
      case 'V':
        playerstats();
        SavePlyr();
        break;
      case 'T':
        od_clr_scr();
        if ((Plyr.level<12)&&(Plyr.map>1))
        {
          if (Plyr.level>Plyr.map)
          {
            od_printf("You cannot fight this master again.");
            gamepause();
            break;
          }
          if (Plyr.seen_master==FALSE)
          {
              logentry(3);
              monfight(2,x-1);
          }
          else
          {
            od_printf("You have already fought a master today.");
            gamepause();
          }
        }
        if (Plyr.level==12) tower();
        SavePlyr();
        break;
      case 'S':
        Plyr.exp+=10;
        Plyr.gold+=1000000000;
        Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
        Plyr.moves_left=DAILYMOVES;
        Plyr.human_left=DAILYHUMANFIGHTS;
        //Plyr.def_multi=1;
        //Plyr.str_multi=1;
        Plyr.dead=false;
        Plyr.seen_master=FALSE;
        break;
      case '+':
        Plyr.level++;
        Plyr.hit_max+=(5*Plyr.level);
        Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
        if (Plyr.level>12) Plyr.level=12;
        SavePlyr();
        break;
      case '-':
        Plyr.level--;
        if (Plyr.level<1) Plyr.level=1;
        Plyr.map--;
        if (Plyr.map<1) Plyr.map=1;
        Plyr.gold+=100000;
        SavePlyr();
        break;
      case 'Q':
        SavePlyr();
        done=1;
        gameend(0);
        break;
    }
  }
}

void map(int z)
{
  int x,y;
  x=Plyr.cord_x;
  y=Plyr.cord_y;
  int randmap;
  int randitem;
  int done=0;
  bool userbattle=false;
  char ch;
  char mapansi[40];
  int v=1;
  FILE *fptr;
  struct PlyrRec PlyrInfo;
  struct PlyrLocation PlyrLoc[20];
  fptr = fopen(PlyrFile,"rb");
  if (!fptr) gameend(-1);
  while (fread(&PlyrInfo, sizeof(struct PlyrRec), 1, fptr) == 1)
  {
    PlyrLoc[v].Index=PlyrInfo.Index;
    strcpy(PlyrLoc[v].Alias,PlyrInfo.Alias);
    PlyrLoc[v].map=PlyrInfo.map;
    PlyrLoc[v].cord_x=PlyrInfo.cord_x;
    PlyrLoc[v].cord_y=PlyrInfo.cord_y;
    PlyrLoc[v].dead=PlyrInfo.dead;
    v++;
  }
  fclose(fptr);
  while (!done && Plyr.hit_points>0 && Plyr.moves_left>0)
  {
    od_clr_scr();
    switch (z)
    {
      case 1:
        if (Plyr.cord_x==0 && Plyr.cord_y==0)
        {
          x=6;
          y=19;
        }
        SavePlyr();
        sprintf(mapansi,"%s%s",ansipath,"map1");
        od_send_file(mapansi);
        break;
      case 2:
        if (Plyr.cord_x==0 && Plyr.cord_y==0)
        {
          x=5;
          y=10;
        }
        SavePlyr();
        sprintf(mapansi,"%s%s",ansipath,"map2");
        od_send_file(mapansi);
        break;
      case 3:
        if (Plyr.cord_x==0 && Plyr.cord_y==0)
        {
          x=5;
          y=2;
        }
        SavePlyr();
        sprintf(mapansi,"%s%s",ansipath,"map3");
        od_send_file(mapansi);
        break;
      case 4:
        if (Plyr.cord_x==0 && Plyr.cord_y==0)
        {
            x=5;
            y=10;
        }
        SavePlyr();
        sprintf(mapansi,"%s%s",ansipath,"map4");
        od_send_file(mapansi);
        break;
    }
    if (x<1) x=1;
    if (y>20) y=20;
    if (y<1) y=1;
    if (x>80) x=80;
    od_set_cursor(y,x);
    od_printf("`bright blue`*");
    od_set_cursor(21,1);
    od_printf("     `blue`(`bright blue`8`blue`)Up");
    od_set_cursor(22,1);
    od_printf("`blue`(`bright blue`4`blue`)Left (`bright blue`6`blue`)Right    (`bright blue`V`blue`)iew Stats                      `bright blue`%d `blue`moves left",Plyr.moves_left);
    od_set_cursor(23,1);
    od_printf("    `blue`(`bright blue`2`blue`)Down");
    od_set_cursor(24,1);
    od_sleep(100);
    od_clear_keybuffer();
    ch=od_get_answer("2468VvQq");
    switch (ch)
    {
      case '2':
        setmap(z,x,y+1);
        if(mapchar!='X')
        {
          y++;
          Plyr.moves_left--;
        }
        else userbattle=TRUE;
        break;
      case '8':
        setmap(z,x,y-1);
        if(mapchar!='X')
        {
          y--;
          Plyr.moves_left--;
        }
        else userbattle=TRUE;
        break;
      case '4':
        setmap(z,x-1,y);
        if(mapchar!='X')
        {
          x--;
          Plyr.moves_left--;
        }
        else userbattle=TRUE;
        break;
      case '6':
        setmap(z,x+1,y);
        if(mapchar!='X')
        {
          x++;
          Plyr.moves_left--;
        }
        else userbattle=TRUE;
        break;
      case 'V':
        playerstats();
        break;
      case 'Q':
        done=1;
        break;
    }
    Plyr.map=z;
    Plyr.cord_x=x;
    Plyr.cord_y=y;
    SavePlyr();
    for (int s=1;s<=v;s++)
    {
      if ((Plyr.map==PlyrLoc[s].map)&&(Plyr.cord_x==PlyrLoc[s].cord_x)&&(Plyr.cord_y==PlyrLoc[s].cord_y)&&(PlyrLoc[s].dead==false)&&(PlyrLoc[s].Alias!=Plyr.Alias))
      {
        od_printf("`blue`You have encountered `bright blue`%i %s! `blue`Do you wish to battle them? (Y/N):",PlyrLoc[s].Index,PlyrLoc[s].Alias);
        ch=od_get_answer("YyNn");
        switch (ch)
        {
          case 'Y':
            logentry(4);
            monfight(3,s);
            userbattle=TRUE;
            break;
        }
      }
    }
    srand(time(0));
    randmap=(rand() % 100)+1;
    if ((randmap>=50)&&(userbattle==FALSE))
    {
      srand(time(0));
      randitem=(rand() % 100)+1;
      //randitem=50;          //used for testing specific event
      switch (randitem)
      {
        case 5:
        case 6:
        case 7:
          ranitem(2);         //Found gold
          break;
        case 97:
          ranitem(3);         //Lose gold
          break;
        case 88:
        case 89:
        case 91:
        case 92:
        case 94:
          ranitem(1);         //Refill HP
          break;
        case 12:
          ranitem(4);         //lose HP
          break;
        case 65:
          ranitem(5);         //Upgrade weapon
          break;
        case 78:
          ranitem(6);         //Upgrade armour
          break;
        case 23:
          ranitem(7);         //Downgrade weapon
          break;
        case 37:
          ranitem(8);         //Downgrade armour
          break;
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
          ranitem(9);         //Find random item
          gamepause();
          break;
        default :
          monfight(1,Plyr.map);        //Fight enemy
      }
    }
    if ((Plyr.moves_left<=0)||(Plyr.dead==true)) done=1;
    int tmp=atoi(&mapchar);
    if (tmp!=0)
    {
      switch (tmp)
      {
        case 1:
          //if ((y<=20)&&(y>=18)&&(x<=5)) town(1);
          //if ((y>=10)&&(y<=13)&&(x>=76)) town(2);
          town(1);
          break;
        case 2:
          //if ((y<=11)&&(y>=9)&&(x<=4)) town(2);
          //if ((y<=3)&&(x>=77)) town(3);
          town(2);
          break;
        case 3:
          //if ((y<=4)&&(x<=4)) town(3);
          //if ((y>=12)&&(y<=14)&&(x>=77)) town(4);
          town(3);
          break;
        case 4:
          town(4);
          break;
        case 5:
          town(5);
          break;
        case 6:
          town(6);
          break;
        case 7:
          town(7);
          break;
        case 8:
          town(8);
          break;
        case 9:
          town(9);
          break;
        case 10:
          town(10);
          break;
        case 11:
          town(11);
          break;
        case 12:
          town(12);
          break;
      }
    }
  }
}

char setmap(int mapnumber,int xcord,int ycord)
{
  char mapasc[40];
  char temp[85];
  int b;
  FILE *fptr;
  mapchar=' ';
  int a=((mapnumber-1)*20)+ycord;
  sprintf(mapasc,"%s%s",ansipath,"maps.asc");
  fptr = fopen(mapasc,"r");
  if (fptr)
  {
    for(b=1;b<a;b++)
    {
      fgets(temp,sizeof(temp),fptr);
      //od_printf("%s",temp);
    }
    //od_set_cursor(23,1);
    //od_printf("%d %d\n\r%s\n\r",b,a,temp);
    //od_printf("%d %d %c\n\r",xcord,ycord,temp[xcord]);
    mapchar=temp[xcord];
    return mapchar;
  }
  else
  {
    od_printf("Unable to open %s",mapasc);
    gamepause();
  }
  fclose(fptr);
  return 0;
}

void healer()
{
  char ch;
  int temp;
  int temp1;
  int tmp;
  char healansi[40];
  od_clr_scr();
  sprintf(healansi,"%s%s",ansipath,"healers");
  od_send_file(healansi);
  od_set_cursor(6,52);
  od_printf("`blue`(`bright blue`H`blue`)`bright blue`eal All Possible");
  od_set_cursor(7,52);
  od_printf("`blue`(`bright blue`C`blue`)`bright blue`ertain Amount Healed");
  od_set_cursor(9,52);
  od_printf("`blue`(`bright blue`R`blue`)`bright blue`eturn");
  od_set_cursor(18,1);
  od_printf("  `blue`HitPoints: `bright blue`%d `blue`of `bright blue`%d  `blue`Gold: `bright blue`%d \r\n",Plyr.hit_points,Plyr.hit_max+Plyr.hit_multi,Plyr.gold);
  od_printf("  `blue`(it costs 5 gold to heal 1 hitpoint) \r\n\n");
  od_printf("  `blue`The Healers   (H,C,R)\r\n");
  od_printf("  `blue`Your Command, `bright blue`%s? : ",Plyr.Alias);
  ch=toupper(od_get_answer("HhCcRr"));
  switch (ch)
  {
    case 'H':
    {
      tmp=Plyr.hit_max+Plyr.hit_multi;
      if (tmp>Plyr.hit_points) temp=(tmp-Plyr.hit_points)*5;
      else
      {
        od_printf("   You look fine to me...");
        gamepause();
        break;
      }
      if (Plyr.gold<temp)
      {
        temp1=Plyr.gold/5;
        temp=temp1*5;
        Plyr.gold-=temp;
        Plyr.hit_points+=temp1;
        od_printf("   Restored %d hit points.",temp1);
        gamepause();
        break;
      }
      if (Plyr.gold>=temp)
      {
        temp1=tmp-Plyr.hit_points;
        Plyr.gold-=temp;
        Plyr.hit_points+=tmp;
        od_printf("   Restored %d hit points. ",temp1);
        gamepause();
        break;
      }
    }
    case 'C':
      {
        od_printf("  \r\nHow many hit points would you like healed? ");
        scanf("%d",&temp);
        if ((Plyr.hit_points+temp)<=tmp)
        {
          if ((temp*5)<=Plyr.gold)
          {
            Plyr.gold-=temp*5;
            Plyr.hit_points+=temp;
            od_printf("   Done!\r\n");
            gamepause();
            break;
          }
        }
      }
    case 'R': break;
  }
}

void weaponshop()
{
  char ch[3];
  //char x[2];
  char yn;
  bool done=false;
  bool buy=false;
  char weapansi[40];
  //bool sell=false;
  od_clr_scr();
  sprintf(weapansi,"%s%s",ansipath,"weapon");
  od_send_file(weapansi);
  od_set_cursor(4,46);
  od_printf("`blue`(`bright blue`A`blue`) `bright blue`Stick                  200");
  od_set_cursor(5,46);
  od_printf("`blue`(`bright blue`B`blue`) `bright blue`Dagger               1,000");
  od_set_cursor(6,46);
  od_printf("`blue`(`bright blue`C`blue`) `bright blue`Short Sword          3,000");
  od_set_cursor(7,46);
  od_printf("`blue`(`bright blue`D`blue`) `bright blue`Long Sword          10,000");
  od_set_cursor(8,46);
  od_printf("`blue`(`bright blue`E`blue`) `bright blue`Huge Axe            30,000");
  od_set_cursor(9,46);
  od_printf("`blue`(`bright blue`F`blue`) `bright blue`Bone Cruncher      100,000");
  od_set_cursor(10,46);
  od_printf("`blue`(`bright blue`G`blue`) `bright blue`Twin Swords        150,000");
  od_set_cursor(11,46);
  od_printf("`blue`(`bright blue`H`blue`) `bright blue`Power Axe          200,000");
  od_set_cursor(12,46);
  od_printf("`blue`(`bright blue`I`blue`) `bright blue`Able's Sword       400,000");
  od_set_cursor(13,46);
  od_printf("`blue`(`bright blue`J`blue`) `bright blue`Wan's Weapon     1,000,000");
  od_set_cursor(14,46);
  od_printf("`blue`(`bright blue`K`blue`) `bright blue`Spear Of Gold    4,000,000");
  od_set_cursor(15,46);
  od_printf("`blue`(`bright blue`L`blue`) `bright blue`Crystal Shard   10,000,000");
  od_set_cursor(16,46);
  od_printf("`blue`(`bright blue`M`blue`) `bright blue`Nira's Teeth    40,000,000");
  od_set_cursor(17,46);
  od_printf("`blue`(`bright blue`N`blue`) `bright blue`Blood Sword    100,000,000");
  od_set_cursor(18,46);
  od_printf("`blue`(`bright blue`O`blue`) `bright blue`Death Sword    400,000,000");
  od_set_cursor(18,11);
  od_printf("`blue`(`bright blue`S`blue`) `bright blue`ell your current Weapon");
  od_set_cursor(19,11);
  od_printf("`blue`(`bright blue`R`blue`) `bright blue`eturn to Town");
  od_set_cursor(21,1);
  od_printf("\r\n  %lu gold in hand - %s ",Plyr.gold,Plyr.weapon);
  od_printf("\r\n  Your command : ");
  od_input_str(ch,2,32,122);
  //od_printf("\r\n%s\r\n\n",ch);
  if (strcasecmp(ch,"r")==0 && !done)
  {
    od_printf("\r\n     Well, maybe next time...\n");
    gamepause();
    done=true;
  }
  if (strcasecmp(ch,"A")==0 && !buy)
  {
    strcpy(ch,"2");
    buy=true;
  }
  if (strcasecmp(ch,"B")==0 && !buy)
  {
    strcpy(ch,"3");
    od_printf("%s\r\n",ch);
    gamepause();
    buy=true;
  }
  if (strcasecmp(ch,"C")==0 && !buy)
  {
    strcpy(ch,"4");
    buy=true;
  }
  if (strcasecmp(ch,"D")==0 && !buy)
  {
    strcpy(ch,"5");
    buy=true;
  }
  if (strcasecmp(ch,"E")==0 && !buy)
  {
    strcpy(ch,"6");
    buy=true;
  }
  if (strcasecmp(ch,"F")==0 && !buy)
  {
    strcpy(ch,"7");
    buy=true;
  }
  if (strcasecmp(ch,"G")==0 && !buy)
  {
    strcpy(ch,"8");
    buy=true;
  }
  if (strcasecmp(ch,"H")==0 && !buy)
  {
    strcpy(ch,"9");
    buy=true;
  }
  if (strcasecmp(ch,"I")==0 && !buy)
  {
    strcpy(ch,"10");
    buy=true;
  }
  if (strcasecmp(ch,"J")==0 && !buy)
  {
    strcpy(ch,"11");
    buy=true;
  }
  if (strcasecmp(ch,"K")==0 && !buy)
  {
    strcpy(ch,"12");
    buy=true;
  }
  if (strcasecmp(ch,"L")==0 && !buy)
  {
    strcpy(ch,"13");
    buy=true;
  }
  if (strcasecmp(ch,"M")==0 && !buy)
  {
    strcpy(ch,"14");
    buy=true;
  }
  if (strcasecmp(ch,"N")==0 && !buy)
  {
    strcpy(ch,"15");
    buy=true;
  }
  if (strcasecmp(ch,"O")==0 && !buy)
  {
    strcpy(ch,"16");
    buy=true;
  }
  if (strcasecmp(ch,"S")==0 && !buy)
  {
    od_printf("\r\n     Are you sure you want to sell your %s for %d? (Y/N) ",Plyr.weapon,weapons[Plyr.weapon_num].price/2);
    yn=toupper(od_get_answer("YyNn"));
    switch (yn)
    {
      case 'Y':
        Plyr.gold+=weapons[Plyr.weapon_num].price/2;
        Plyr.weapon_num=1;
        strcpy(Plyr.weapon,weapons[Plyr.weapon_num].name);
        Plyr.strength=weapons[Plyr.weapon_num].strength;
        od_printf("\r\n     Sold! ");
        done=true;
        gamepause();
        break;
      case 'N':
        od_printf("\r\n     Maybe next time...");
        done=true;
        gamepause();
        break;
    }
  }
  if (atoi(ch)<17 && atoi(ch)>1 && buy)
  {
    if (Plyr.gold >= weapons[atoi(ch)].price)
    {
      Plyr.gold-=weapons[atoi(ch)].price;
      Plyr.weapon_num=weapons[atoi(ch)].index;
      strcpy(Plyr.weapon,weapons[atoi(ch)].name);
      Plyr.strength=weapons[atoi(ch)].strength;
      od_printf("\r\n     You have purchased the %s for %d gold!\r\n",Plyr.weapon,weapons[atoi(ch)].price);
      done=true;
      gamepause();
    }
    else
    {
      od_printf("     You can't afford the %s for %d gold...\r\n",weapons[atoi(ch)].name,weapons[atoi(ch)].price);
      done=true;
      gamepause();
    }
  }
}

void armourshop()
{
  char ch[3];
  char x;
  bool done=false;
  bool buy=false;
  char armansi[40];
  //bool sell=false;
  od_clr_scr();
  sprintf(armansi,"%s%s",ansipath,"armour");
  od_send_file(armansi);
  od_set_cursor(4,46);
  od_printf("`blue`(`bright blue`A`blue`) `bright blue`Coat                     200");
  od_set_cursor(5,46);
  od_printf("`blue`(`bright blue`B`blue`) `bright blue`Heavy Coat             1,000");
  od_set_cursor(6,46);
  od_printf("`blue`(`bright blue`C`blue`) `bright blue`Leather Vest           3,000");
  od_set_cursor(7,46);
  od_printf("`blue`(`bright blue`D`blue`) `bright blue`Bronze Armour         10,000");
  od_set_cursor(8,46);
  od_printf("`blue`(`bright blue`E`blue`) `bright blue`Iron Armour           30,000");
  od_set_cursor(9,46);
  od_printf("`blue`(`bright blue`F`blue`) `bright blue`Graphite Armour      100,000");
  od_set_cursor(10,46);
  od_printf("`blue`(`bright blue`G`blue`) `bright blue`Edrick's Armour      150,000");
  od_set_cursor(11,46);
  od_printf("`blue`(`bright blue`H`blue`) `bright blue`Armour Of Death      200,000");
  od_set_cursor(12,46);
  od_printf("`blue`(`bright blue`I`blue`) `bright blue`Able's Armour        400,000");
  od_set_cursor(13,46);
  od_printf("`blue`(`bright blue`J`blue`) `bright blue`Full Body Armour   1,000,000");
  od_set_cursor(14,46);
  od_printf("`blue`(`bright blue`K`blue`) `bright blue`Blood Armour       4,000,000");
  od_set_cursor(15,46);
  od_printf("`blue`(`bright blue`L`blue`) `bright blue`Magic Protection  10,000,000");
  od_set_cursor(16,46);
  od_printf("`blue`(`bright blue`M`blue`) `bright blue`Belars' Mail      40,000,000");
  od_set_cursor(17,46);
  od_printf("`blue`(`bright blue`N`blue`) `bright blue`Golden Armour    100,000,000");
  od_set_cursor(18,46);
  od_printf("`blue`(`bright blue`O`blue`) `bright blue`Armour Of Lore   400,000,000");
  od_set_cursor(18,11);
  od_printf("`blue`(`bright blue`S`blue`) `bright blue`ell your current Armour");
  od_set_cursor(19,11);
  od_printf("`blue`(`bright blue`R`blue`) `bright blue`eturn to Town");
  od_set_cursor(21,1);

  od_printf("\r\n  %lu gold in hand - %s ",Plyr.gold,Plyr.arm);
  od_printf("\r\n  Your command : ");
  od_input_str(ch,2,32,122);
  //od_printf("\r\n%s\r\n\n",ch);
  if (strcasecmp(ch,"r")==0 && !done)
  {
    od_printf("\r\n     Well, maybe next time...\n");
    gamepause();
    done=true;
  }
  if (strcasecmp(ch,"A")==0 && !buy)
  {
    strcpy(ch,"2");
    buy=true;
  }
  if (strcasecmp(ch,"B")==0 && !buy)
  {
    strcpy(ch,"3");
    buy=true;
  }
  if (strcasecmp(ch,"C")==0 && !buy)
  {
    strcpy(ch,"4");
    buy=true;
  }
  if (strcasecmp(ch,"D")==0 && !buy)
  {
    strcpy(ch,"5");
    buy=true;
  }
  if (strcasecmp(ch,"E")==0 && !buy)
  {
    strcpy(ch,"6");
    buy=true;
  }
  if (strcasecmp(ch,"F")==0 && !buy)
  {
    strcpy(ch,"7");
    buy=true;
  }
  if (strcasecmp(ch,"G")==0 && !buy)
  {
    strcpy(ch,"8");
    buy=true;
  }
  if (strcasecmp(ch,"H")==0 && !buy)
  {
    strcpy(ch,"9");
    buy=true;
  }
  if (strcasecmp(ch,"I")==0 && !buy)
  {
    strcpy(ch,"10");
    buy=true;
  }
  if (strcasecmp(ch,"J")==0 && !buy)
  {
    strcpy(ch,"11");
    buy=true;
  }
  if (strcasecmp(ch,"K")==0 && !buy)
  {
    strcpy(ch,"12");
    buy=true;
  }
  if (strcasecmp(ch,"L")==0 && !buy)
  {
    strcpy(ch,"13");
    buy=true;
  }
  if (strcasecmp(ch,"M")==0 && !buy)
  {
    strcpy(ch,"14");
    buy=true;
  }
  if (strcasecmp(ch,"N")==0 && !buy)
  {
    strcpy(ch,"15");
    buy=true;
  }
  if (strcasecmp(ch,"O")==0 && !buy)
  {
    strcpy(ch,"16");
    buy=true;
  }
  if (strcasecmp(ch,"S")==0 && !buy)
  {
    od_printf("\r\n     Are you sure you want to sell your %s for %d? (Y/N) ",Plyr.arm,armours[Plyr.arm_num].price/2);
    x=od_get_answer("YyNn");
    switch (x)
    {
      case 'Y':
        Plyr.gold+=armours[Plyr.arm_num].price/2;
        Plyr.arm_num=1;
        strcpy(Plyr.arm,armours[Plyr.arm_num].name);
        Plyr.def=armours[Plyr.arm_num].strength;
        od_printf("\r\n     Sold! ");
        done=true;
        gamepause();
        break;
      case 'N':
        od_printf("\r\n     Maybe next time...");
        done=true;
        gamepause();
        break;
    }
  }
  if (atoi(ch)<16 && atoi(ch)>1 && buy)
  {
    if (Plyr.gold >= armours[atoi(ch)].price)
    {
      Plyr.gold-=armours[atoi(ch)].price;
      Plyr.arm_num=armours[atoi(ch)].index;
      strcpy(Plyr.arm,armours[atoi(ch)].name);
      Plyr.def=armours[atoi(ch)].strength;
      od_printf("\r\n     You have purchased the %s for %d gold!\r\n",Plyr.arm,armours[atoi(ch)].price);
      done=true;
      gamepause();
    }
    else
    {
      od_printf("     You can't afford the %s for %d gold...\r\n",armours[atoi(ch)].name,armours[atoi(ch)].price);
      done=true;
      gamepause();
    }
  }
}

void addnews(char name[32], char enemy[32], int happening)
{
  int day,month,year;
  char wordmonth[12][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  time_t now;
  time(&now);
  struct tm *local = localtime(&now);
  day = local->tm_mday;
  month = local->tm_mon+1;
  year = local->tm_year+1900;

  FILE *fptr;
  fptr = fopen(NewsFile,"a");
  if (fptr)
  {
    switch (happening)
    {
      case 1:
        fprintf(fptr,"%02d %s %d: %s just woke up on the beach\r\n\n",day,wordmonth[month-1],year,name);
        break;
      case 2:
        fprintf(fptr,"%02d %s %d: %s has just beaten %s in battle!\r\n\n",day,wordmonth[month-1],year,name,enemy);
        break;
      case 3:
        fprintf(fptr,"%02d %s %d: %s has been defeated by %s!\r\n\n",day,wordmonth[month-1],year,name,enemy);
        break;
      case 4:
        fprintf(fptr,"%02d %s %d: %s has been seen wandering around!\r\n\n",day,wordmonth[month-1],year,name);
        break;
    }
    fclose(fptr);
  }
  else
  {
    od_printf("Error opening file %s ...",NewsFile);
    fclose(fptr);
    gameend(-1);
  }
}

void dailynews()
{
  char s[70];
  int x=0;
  int y=0;
  char newsansi[40];
  od_clr_scr();
  FILE *fptr;
  FILE *fptr2;
  fptr = fopen(NewsFile,"r");
  fptr2 = fopen("rcstdta.tmp","w");
  if (fptr)
  {
    if (fptr2)
    {
      while (fgets(s, sizeof(s),fptr) != NULL)
      {
        x++;
        fprintf(fptr2,"%s",s);
      }
    }
    else
    {
      od_printf("Error opening file rcstdta.tmp ...");
      fclose(fptr2);
      fclose(fptr);
      gameend(-1);
    }
  }
  else
  {
    od_printf("Error opening file %s ...",NewsFile);
    fclose(fptr2);
    fclose(fptr);
    gameend(-1);
  }
  fclose(fptr2);
  fclose(fptr);
  if (x>=20)
  {
    fptr = fopen(NewsFile,"w+");
    fptr2 = fopen("rcstdta.tmp","r");
    if (fptr)
    {
      if (fptr2)
      {
        y=x;
        while (y>19)
        {
          fgets(s,sizeof(s),fptr2);
          y--;
        }
        while (y>0)
        {
          fgets(s,sizeof(s),fptr2);
          fprintf(fptr,"%s",s);
          y--;
        }
      }
      else
      {
        od_printf("Error opening file...");
        fclose(fptr2);
        fclose(fptr);
        gameend(-1);
      }
    }
    else
    {
      od_printf("Error opening file...");
      fclose(fptr2);
      fclose(fptr);
      gameend(-1);
    }
    fclose(fptr2);
    fclose(fptr);
  }
  od_control_get()->od_page_pausing = FALSE;
  sprintf(newsansi,"%s%s",ansipath,"news");
  od_send_file(newsansi);
  od_control_get()->od_page_pausing = TRUE;
  fptr = fopen(NewsFile,"r");
  od_set_cursor(4,1);
  while (fgets(s,sizeof(s),fptr) != NULL)
  {
    od_printf("%s\r",s);
  }
  fclose(fptr);
  remove("rcstdta.tmp");
  gamepause();
}

char* goldconvert(long int x)          //turns int into string containing commas for better readablity
{
  sprintf(comma,"%ld",x);
  switch (findtotaldigits(x))
  {
    case 0:
    case 1:
    case 2:
    case 3:
    {
      return comma;
      break;
    }
    case 4:
    {
      insert_substring(comma,",",2);
      break;
    }
    case 5:
    {
      insert_substring(comma,",",3);
      break;
    }
    case 6:
    {
      insert_substring(comma,",",4);
      break;
    }
    case 7:
    {
      insert_substring(comma,",",2);
      insert_substring(comma,",",6);
      break;
    }
    case 8:
    {
      insert_substring(comma,",",3);
      insert_substring(comma,",",7);
      break;
    }
    case 9:
    {
      insert_substring(comma,",",4);
      insert_substring(comma,",",8);
      break;
    }
    case 10:
    {
      insert_substring(comma,",",2);
      insert_substring(comma,",",6);
      insert_substring(comma,",",10);
      break;
    }
    case 11:
    {
      insert_substring(comma,",",3);
      insert_substring(comma,",",7);
      insert_substring(comma,",",11);
      break;
    }
    case 12:
    {
      insert_substring(comma,",",4);
      insert_substring(comma,",",8);
      insert_substring(comma,",",12);
      break;
    }
    case 13:
    {
      insert_substring(comma,",",2);
      insert_substring(comma,",",6);
      insert_substring(comma,",",10);
      insert_substring(comma,",",14);
      break;
    }
    case 14:
    {
      insert_substring(comma,",",3);
      insert_substring(comma,",",7);
      insert_substring(comma,",",11);
      insert_substring(comma,",",15);
      break;
    }
    case 15:
    {
      insert_substring(comma,",",4);
      insert_substring(comma,",",8);
      insert_substring(comma,",",12);
      insert_substring(comma,",",16);
      break;
    }
    default:
    {
      return comma;
    }
  }
  return comma;
}

void insert_substring(char *a, char *b, int position)
{
  char *f, *e;
  int length;
  length=strlen(a);
  f=substring(a,1,position -1);
  e=substring(a,position,length-position+1);

  strcpy(a,"");
  strcat(a,f);
  free(f);
  strcat(a,b);
  strcat(a,e);
  free(e);
}

char *substring(char *string, int position, int length)
{
  char *pointer;
  int c;
  pointer=malloc(length+1);
  if(pointer == NULL)
    exit(EXIT_FAILURE);
  for(c=0;c<length;c++)
    *(pointer+c) = *((string+position-1)+c);
  *(pointer+c)='\0';
  return pointer;
}

void bank()
{
  char ch;
  int done=0;
  unsigned long int dep;
  //char buffer[12]="";
  char bankansi[40];
  while (!done)
  {
    od_clr_scr();
    sprintf(bankansi,"%s%s",ansipath,"bank");
    od_send_file(bankansi);
    od_set_cursor(7,52);
    od_printf("`blue`(`bright blue`D`blue`) `bright blue`Deposit Gold");
    od_set_cursor(8,52);
    od_printf("`blue`(`bright blue`W`blue`) `bright blue`Withdraw Gold");
    od_set_cursor(10,52);
    od_printf("`blue`(`bright blue`R`blue`) `bright blue`Return to Town");
    od_set_cursor(16,5);
    od_printf("`blue`Gold In Hand: `bright blue`%s",goldconvert(Plyr.gold));
    od_set_cursor(16,40);
    goldconvert(Plyr.bank);
    od_printf("`blue`Gold In Bank: `bright blue`%s",comma);
    od_set_cursor(18,5);
    od_printf("`blue`Your command, `bright blue`%s: ",Plyr.Alias);
    ch=od_get_answer("DdWwRr");
    switch (tolower(ch))
    {
      case 'd':
        od_clr_scr();
        od_send_file(bankansi);
        od_set_cursor(8,48);
        od_printf("`blue`H`bright blue`ow much gold would you");
        od_set_cursor(10,48);
        od_printf("`bright blue`like to deposit?");
        od_set_cursor(16,5);
        goldconvert(Plyr.gold);
        od_printf("`blue`Gold In Hand: `bright blue`%s",comma);
        od_set_cursor(16,35);
        goldconvert(Plyr.bank);
        od_printf("`blue`Gold In Bank: `bright blue`%s",comma);
        od_set_cursor(18,5);
        od_printf("`blue`H`bright blue`ow much gold to deposit? `blue`(1 for all)");
        od_set_cursor(19,5);
        od_printf("`blue`Amount: `bright blue`");
        //od_input_str(buffer,12,'0','9');
        //dep = atoi(buffer);
        scanf("%ld",&dep);
        if (dep > 0)
        {
          if (dep == 1) dep = Plyr.gold;
          if (dep <= Plyr.gold)
          {
            Plyr.bank += dep;
            Plyr.gold -= dep;
            goldconvert(dep);
            od_printf("\r\n      `blue`Done! `bright blue`%s `blue`deposited.",comma);
          }
          else od_printf("\r\n     `blue`Y`bright blue`ou don't have that much gold...");
        }
        else od_printf("\r\n     `blue`D`bright blue`eposit needs to be greater than `blue`0 (zero)");
        gamepause();
        SavePlyr();
        break;
      case 'w':
        od_clr_scr();
        od_send_file(bankansi);
        od_set_cursor(8,48);
        od_printf("`blue`H`bright blue`ow much gold would you");
        od_set_cursor(10,48);
        od_printf("`bright blue`like to withdraw?");
        od_set_cursor(16,5);
        goldconvert(Plyr.gold);
        od_printf("`blue`Gold In Hand: `bright blue`%s",comma);
        od_set_cursor(16,35);
        goldconvert(Plyr.bank);
        od_printf("`blue`Gold In Bank: `bright blue`%s",comma);
        od_set_cursor(18,5);
        od_printf("`blue`H`bright blue`ow much gold would you like to withdraw? `blue`(1 for all)");
        od_set_cursor(19,5);
        od_printf("`blue`Amount: `bright blue`");
        //od_input_str(buffer,20,'0','9');
        //dep = atoi(buffer);
        scanf("%ld",&dep);
        if (dep > 0)
        {
          if (dep == 1) dep = Plyr.bank;
          if (dep <= Plyr.bank)
          {
            Plyr.gold = Plyr.gold + dep;
            Plyr.bank = Plyr.bank - dep;
            goldconvert(dep);
            od_printf("\r\n     `blue`Done! `bright blue`%s `blue`withdrawn.",comma);
          }
          else od_printf("\r\n     `blue`Y`bright blue`ou don't have that much gold...");
        }
        else od_printf("\r\n     `blue`W`bright blue`ithdrawl must be greater than `blue`0 (zero)");
        gamepause();
        SavePlyr();
        break;
      case 'r':
        done = 1;
        break;
    }
  }
}

void listplayers()          //Creates list to show of other players - Also creates ascii file with same info
{
  FILE *fptr;
  FILE *fptr2;
  char s[70];
  struct PlyrRec PlyrInfo;
  fptr = fopen(PlyrFile,"rb");
  fptr2 = fopen("tdtaplyr.txt","w");
  if (!fptr || !fptr2) return;
  od_clr_scr();
  strcpy(s,"  List of Players in The Dark Tower Adventures");
  od_printf("`blue`%s",s);
  od_printf("  Ver %d.%d %s\r\n\n",VERSION_MAJOR,VERSION_MINOR,VERSION_TYPE);
  fprintf(fptr2,"%s",s);
  fprintf(fptr2,"  Ver %d.%d %s\r\n\n",VERSION_MAJOR,VERSION_MINOR,VERSION_TYPE);
  od_printf(" -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
  fprintf(fptr2," -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
  od_printf("  Name                   Level  Exp           BBS\r\n");
  fprintf(fptr2,"  Name                   Level  Exp           BBS\r\n");
  od_printf(" -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
  fprintf(fptr2," -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
  while (fread(&PlyrInfo, sizeof(struct PlyrRec), 1, fptr) == 1)
  {
    od_printf("`blue`  %-22s %-2d     %-10lu    %-20s",PlyrInfo.Alias,PlyrInfo.level,PlyrInfo.exp,PlyrInfo.BBS);
    fprintf(fptr2,"  %-22s %-2d     %-10lu    %-20s",PlyrInfo.Alias,PlyrInfo.level,PlyrInfo.exp,PlyrInfo.BBS);
    if (PlyrInfo.dead==true)
    {
      od_printf(" `red`-=>DEAD<=-\r\n");
      fprintf(fptr2," -=>DEAD<=-\r\n");
    }
    else
    {
      od_printf("\r\n");
      fprintf(fptr2,"\r\n");
    }
  }
  fclose(fptr);
  fclose(fptr2);
  gamepause();
}

int findtotaldigits(unsigned long int no)
{
  if (no==0) return 0;
  return 1+findtotaldigits(no/10);
}

void playerstats()
{
  char ch;
  bool done=false;
  char statansi[40];
  while (!done)
  {
    od_clr_scr();
    sprintf(statansi,"%s%s",ansipath,"stats");
    od_send_file(statansi);
    od_set_cursor(2,2);
    od_printf("  `bright blue`%s`blue`'s Stats...",Plyr.Alias);
    od_set_cursor(3,2);
    od_printf("`blue`-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
    od_set_cursor(4,2);
    od_printf("  `blue`Experience           : `bright blue`%d",Plyr.exp);
    od_set_cursor(5,2);
    od_printf("  `blue`Level                : `bright blue`%d",Plyr.level);
    od_set_cursor(6,2);
    if (Plyr.hit_points<0) Plyr.hit_points=0;
    od_printf("  `blue`HitPoints            : `bright blue`%d `blue`of `bright blue`%d",Plyr.hit_points,(Plyr.hit_max+Plyr.hit_multi));
    od_set_cursor(7,2);
    od_printf("  `blue`Moves Left           : `bright blue`%d",Plyr.moves_left);
    od_set_cursor(8,2);
    od_printf("  `blue`Player Fights Left   : `bright blue`%d",Plyr.human_left);
    od_set_cursor(9,2);
    goldconvert(Plyr.gold);
    od_printf("  `blue`Gold In Hand         : `bright blue`%s",comma);
    od_set_cursor(10,2);
    goldconvert(Plyr.bank);
    od_printf("  `blue`Gold In Bank         : `bright blue`%s",comma);
    od_set_cursor(11,2);
    od_printf("  `blue`Weapon               : `bright blue`%s",Plyr.weapon);
    od_set_cursor(12,2);
    od_printf("  `blue`Attack Strength      : `bright blue`%d",Plyr.strength+Plyr.str_multi);
    od_set_cursor(13,2);
    od_printf("  `blue`Armour               : `bright blue`%s",Plyr.arm);
    od_set_cursor(14,2);
    od_printf("  `blue`Defensive Strength   : `bright blue`%d",Plyr.def+Plyr.def_multi);
    od_set_cursor(15,2);
    od_printf("  `blue`Seen Master          : ");
    if (Plyr.seen_master==true) od_printf("`bright blue`Yes");
    else od_printf("`bright blue`No");
    od_set_cursor(16,2);
    od_printf("  `blue`Player Floor         : `bright blue`%d",Plyr.floor);
    od_set_cursor(18,2);
    od_printf("  `blue`Day Last Played      : `bright blue`%s",ctime(&Plyr.time));
    od_set_cursor(17,2);
    od_printf("  `blue`Times Won            : `bright blue`%d",Plyr.king);
    od_set_cursor(19,2);
    od_printf("  `blue`Time Last Played     : `bright blue`%s",ctime(&Plyr.last_play));
    od_set_cursor(20,2);
    od_printf("  `blue`Map                  : `bright blue`%i   `blue`Coordinates: `bright blue`%i %i",Plyr.map,Plyr.cord_x,Plyr.cord_y);
    od_set_cursor(22,2);
    od_printf("`blue`(`bright blue`R`blue`)eturn (`bright blue`I`blue`)tems\n\r ");
    od_set_cursor(24,1);
    ch=od_get_answer("RrIi");
    switch(tolower(ch))
    {
    case 'r':
      done=true;
      od_clr_scr();
      break;
    case 'i':
      useitems();
      break;
    }
  }
}

void useitems()
{
  int x,itemuse;
  char ch,tf;
  char buffer[3];
  bool done=false;
  while (!done)
  {
    od_clr_scr();
    for (x=1;x<16;x++)
    {
      od_set_cursor(4+x,4);
      if (Plyr.items[x].i_index!=0) od_printf("`blue`(`bright blue`%i`blue`) `bright blue`%s",x,Plyr.items[x].i_name);
    }
    for (x=16;x<31;x++)
    {
      od_set_cursor(4+(x-15),40);
      if (Plyr.items[x].i_index!=0) od_printf("`blue`(`bright blue`%i`blue`) `bright blue%s",x,Plyr.items[x].i_name);
    }
    od_set_cursor(22,2);
    od_printf("`blue`(`bright blue`R`blue`)eturn (`bright blue`U`blue`)se");
    od_set_cursor(24,1);
    ch=od_get_answer("RrUu");
    switch (ch)
    {
      case 'R':
        done=true;
        break;
      case 'U':
        od_printf("Use which item? ");
        od_input_str(buffer,2,'0','9');
        itemuse = atoi(buffer);
        if (Plyr.items[itemuse].i_index!=0)
        {
          od_printf("\r\n `blue`Your `bright blue`%s `blue`will add the following stats:\r\n\n",Plyr.items[itemuse].i_name);
          od_printf(" `bright blue`%i `blue`to hit points\r\n",Plyr.items[itemuse].i_hit_points);
          od_printf(" `bright blue`%i `blue`to hit point modifier\r\n",Plyr.items[itemuse].i_hit_multi);
          od_printf(" `bright blue`%i `blue`to moves left\r\n",Plyr.items[itemuse].i_fights_left);
          od_printf(" `bright blue`%i `blue`to player fights left\r\n",Plyr.items[itemuse].i_human_left);
          od_printf(" `bright blue`%i `blue`to interest modifier\r\n",Plyr.items[itemuse].i_int_multi);
          od_printf(" `bright blue`%i `blue`to defense modifier\r\n",Plyr.items[itemuse].i_def_multi);
          od_printf(" `bright blue`%i `blue`to strength modifier\r\n\n",Plyr.items[itemuse].i_str_multi);
          od_printf("`blue`Are you sure you want to do this? `bright blue`(Y/N)");
          tf = od_get_key(TRUE);
          switch (toupper(tf))
          {
            case 'Y':
              Plyr.hit_points+=Plyr.items[itemuse].i_hit_points;
              if (Plyr.hit_points>Plyr.hit_max+Plyr.hit_multi) Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
              Plyr.hit_multi+=Plyr.items[itemuse].i_hit_multi;
              Plyr.moves_left+=Plyr.items[itemuse].i_fights_left;
              Plyr.human_left+=Plyr.items[itemuse].i_human_left;
              Plyr.int_multi+=Plyr.items[itemuse].i_int_multi;
              Plyr.def_multi+=Plyr.items[itemuse].i_def_multi;
              Plyr.str_multi+=Plyr.items[itemuse].i_str_multi;
              Plyr.items[itemuse].i_index=0;
              strcpy(Plyr.items[itemuse].i_name,"\0");
              Plyr.items[itemuse].i_hit_points=0;
              Plyr.items[itemuse].i_hit_multi=0;
              Plyr.items[itemuse].i_fights_left=0;
              Plyr.items[itemuse].i_human_left=0;
              Plyr.items[itemuse].i_int_multi=0;
              Plyr.items[itemuse].i_def_multi=0;
              Plyr.items[itemuse].i_str_multi=0;
              SavePlyr();
              break;
            case 'N':
              od_printf("`blue`Maybe next time...");
              break;
          }
        }
        break;
    }
  }
}

void tower()          //will be used at end of game...
{
  char ch;
  int done=0;
  bool ran=false;
  int tmp;
  char toweransi[40];
  while (!done)
  {
    if (Plyr.dead==true) done=1;
    od_clr_scr();
    sprintf(toweransi,"%s%s",ansipath,"tower");
    od_send_file(toweransi);
    od_set_cursor(7,3);
    od_printf("`blue`(`bright blue`L`blue`)`bright blue`ook around the room");
    od_set_cursor(9,3);
    od_printf("`blue`(`bright blue`H`blue`)`bright blue`ealer's");
    od_set_cursor(10,3);
    od_printf("`blue`(`bright blue`V`blue`)`bright blue`iew Stats");
    od_set_cursor(11,3);
    od_printf("`blue`(`bright blue`U`blue`)`bright blue`p a level");
    od_set_cursor(12,3);
    //od_printf("`blue`(`bright blue`D`blue`)`bright blue`own a level");
    od_set_cursor(14,3);
    od_printf("`blue`(`bright blue`R`blue`)`bright blue`eturn to town");
    od_set_cursor(4,59);
    od_printf("`bright blue`%d",Plyr.floor);
    if (Plyr.floor==12)
    {
      od_set_cursor(8,3);
      od_printf("`blue`(`bright blue`O`blue`)`bright blue`pen the final door");
    }
    od_set_cursor(20,5);
    od_printf("`blue`HitPoints: `bright blue`%d `blue`of `bright blue`%d  `blue`Moves: `bright blue`%d  `blue`Gold: `bright blue`%d",Plyr.hit_points,Plyr.hit_max+Plyr.hit_multi,Plyr.moves_left,Plyr.gold);
    od_set_cursor(21,5);
    od_printf("`blue`The Tower - Floor `bright blue`%d   `blue`(L,H,U,D,R)",Plyr.floor);
    od_set_cursor(23,5);
    od_printf("`blue`Your Command, `bright blue`%s`blue`? :",Plyr.Alias);
    ch = od_get_answer("LlHhUuRrVvOo");
    switch (ch)
    {
      case 'R':
        done=1;
        break;
      case 'O':
        if (Plyr.floor==12)
        {
          //endbattle();
          monfight(5,0);
          break;
        }
        break;
      case 'V':
        playerstats();
        break;
      case 'U':
        monfight(4,Plyr.floor);
        if (Plyr.floor>12) Plyr.floor=12;
        break;
      case 'H':
        healer();
        break;
      case 'L':
        if (Plyr.moves_left>0 && Plyr.dead==false)
        {
          srand(time(0));
          tmp=(rand() % 100)+1;
          //tmp=65;                             //used to test random events
          switch (tmp)
          {
            case 5:
            case 6:
            case 7:
              ranitem(2);   //found gold
              ran=true;
              break;
            case 97:
              ranitem(3);   //lose gold
              ran=true;
              break;
            case 89:
            case 91:
            case 92:
            case 94:
              ranitem(1);   //refill HP
              ran=true;
              break;
            case 12:
              ranitem(4);   //lose HP
              ran=true;
              break;
            /*case 65:
              ranitem(5);   //upgrade weapon
              ran=true;
              break;
            case 78:
              ranitem(6);   //upgrade armour
              ran=true;
              break;
            case 23:
              ranitem(7);   //downgrade weapon
              ran=true;
              break;
            case 37:
              ranitem(8);   //downgrade armour
              ran=true;
            break;*/
            case 45:
            case 46:
            case 47:
            case 48:
            case 49:
            case 50:
              ranitem(9);  //find random item
              ran=true;
              break;
          }
          if (!ran) monfight(1,Plyr.floor);
        }
        else
        {
          if (Plyr.moves_left<=0)
          {
            od_printf("\r\n\n     `bright blue`You are out of moves for today.");
            done=1;
            gamepause();
          }
          if (Plyr.dead==true)
          {
            od_printf("\r\n\n    `bright blue`You cannot fight when you're dead...");
            done=1;
            gamepause();
          }
        }
        ran=false;
        break;
    }
  }
}

void ranitem(int randomitem)
{
  int temp;
  int temp1;
  char eventansi[40];
  od_clr_scr();
  switch (randomitem)
  {
  case 1:
    {
      sprintf(eventansi,"%s%s",ansipath,"evnthlth");
      od_send_file(eventansi);
      od_set_cursor(9,5);
      od_printf("`blue`A vial containing a `green`green `blue`liquid in it...");
      od_set_cursor(11,5);
      od_printf("`blue`You feel brave and drink it...");
      od_set_cursor(13,5);
      od_printf("`blue`After drinking it...");
      od_set_cursor(15,5);
      od_printf("`blue`You feel refreshed!");
      Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
      //Plyr.moves_left-=1;
      SavePlyr();
      gamepause();
      break;
    }
    case 2:
    {
      sprintf(eventansi,"%s%s",ansipath,"evntgold");
      od_send_file(eventansi);
      od_set_cursor(8,5);
      od_printf("`blue`A bag of `bright blue`GOLD!");
      srand(time(0));
      temp=(rand() % 10)+1;
      temp1=Plyr.gold/temp;
      od_set_cursor(10,5);
      goldconvert(temp1);
      od_printf("`blue`The bag contains `bright blue`%s `blue`gold!",comma);
      Plyr.gold+=temp1;
      //Plyr.moves_left-=1;
      SavePlyr();
      gamepause();
      break;
    }
  case 3:
    {
      sprintf(eventansi,"%s%s",ansipath,"evntgold");
      od_send_file(eventansi);
      od_set_cursor(8,5);
      od_printf("`blue`A Gold Eating Reptile!");
      srand(time(0));
      temp=(rand() % 10)+5;
      temp1=Plyr.gold/temp;
      od_set_cursor(10,5);
      goldconvert(temp1);
      od_printf("`blue`It eats `bright blue`%s `blue`of your gold!",comma);
      Plyr.gold-=temp1;
      //Plyr.moves_left-=1;
      SavePlyr();
      gamepause();
      break;
    }
  case 4:
    {
      sprintf(eventansi,"%s%s",ansipath,"evnthlth");
      od_send_file(eventansi);
      od_set_cursor(9,5);
      od_printf("`blue`A vial containing a `green`green `blue`liquid in it...");
      od_set_cursor(11,5);
      od_printf("`blue`You feel brave and drink it...");
      od_set_cursor(13,5);
      od_printf("`blue`After drinking it...");
      od_set_cursor(15,5);
      od_printf("`blue`You feel weaker...");
      Plyr.hit_points=Plyr.hit_points/2;
      //Plyr.moves_left-=1;
      SavePlyr();
      gamepause();
      break;
    }
  case 5:
    {
      if (Plyr.weapon_num<16)
      {
        sprintf(eventansi,"%s%s",ansipath,"evntwepn");
        od_send_file(eventansi);
        od_set_cursor(8,5);
        od_printf("`blue`An `bright blue`upgraded weapon!");
        od_set_cursor(10,5);
        od_printf("`blue`Your `bright blue`%s `blue`is now a `bright blue`%s!",weapons[Plyr.weapon_num].name,weapons[Plyr.weapon_num+1].name);
        Plyr.weapon_num+=1;
        strcpy(Plyr.weapon,weapons[Plyr.weapon_num].name);
        Plyr.strength=weapons[Plyr.weapon_num].strength;
        //Plyr.moves_left-=1;
        SavePlyr();
        gamepause();
      }
      else
      {
        od_printf("\r\n   `blue`You cannot upgrade your weapon anymore.");
        od_printf("\r\n   `blue`You already have the highest level weapon!");
        gamepause();
      }
      break;
    }
  case 6:
    {
      if (Plyr.arm_num<16)
      {
        sprintf(eventansi,"%s%s",ansipath,"evntarmr");
        od_send_file(eventansi);
        od_set_cursor(8,5);
        od_printf("`blue`New `bright blue`upgraded armour!");
        od_set_cursor(10,5);
        od_printf("`blue`Your `bright blue`%s `blue`is now a `bright blue`%s!",armours[Plyr.arm_num].name,armours[Plyr.arm_num+1].name);
        Plyr.arm_num+=1;
        strcpy(Plyr.arm,armours[Plyr.arm_num].name);
        Plyr.def=armours[Plyr.arm_num].strength;
        //Plyr.moves_left-=1;
        SavePlyr();
        gamepause();
      }
      else
      {
        od_printf("\r\n   `blue`You cannot upgrade your armour anymore.");
        od_printf("\r\n   `blue`You are at the highest level armour already!");
        gamepause();
      }
      break;
    }
  case 7:
    {
      sprintf(eventansi,"%s%s",ansipath,"evntwepn");
      od_send_file(eventansi);
      if (Plyr.weapon_num>1)
      {
        od_set_cursor(8,5);
        od_printf("Your %s got",weapons[Plyr.weapon_num].name);
        od_set_cursor(10,5);
        od_printf("downgraded to %s...",weapons[Plyr.weapon_num-1].name);
        Plyr.weapon_num-=1;
        strcpy(Plyr.weapon,weapons[Plyr.weapon_num].name);
        Plyr.strength=weapons[Plyr.weapon_num].strength;
      }
      else
      {
        od_set_cursor(8,5);
        od_printf("Your %s got",weapons[Plyr.weapon_num].name);
        od_set_cursor(10,5);
        od_printf("downgraded, but we can't leave you without fists...");
        od_set_cursor(12,5);
        od_printf("There is no weapon lower than what you have...");
      }
      //Plyr.moves_left-=1;
      SavePlyr();
      gamepause();
      break;
    }
  case 8:
    {
      sprintf(eventansi,"%s%s",ansipath,"evntarmr");
      od_send_file(eventansi);
      if (Plyr.arm_num>1)
      {
        od_set_cursor(8,5);
        od_printf("Your %s got",armours[Plyr.arm_num].name);
        od_set_cursor(10,5);
        od_printf("downgraded to %s...",armours[Plyr.arm_num-1].name);
        Plyr.arm_num-=1;
        strcpy(Plyr.arm,armours[Plyr.arm_num].name);
        Plyr.def=armours[Plyr.arm_num].strength;
      }
      else
      {
        od_set_cursor(8,5);
        od_printf("Your %s got",armours[Plyr.arm_num].name);
        od_set_cursor(10,5);
        od_printf("downgraded, but we can't leave you without a shirt...");
        od_set_cursor(12,5);
        od_printf("There is no armour lower than what you have...");
      }
      //Plyr.moves_left-=1;
      SavePlyr();
      gamepause();
      break;
    }
  case 9:
    {
      FILE *fptr;
      srand(time(0));
      int rnum=(rand() % 30)+1;
      fptr = fopen(ItemFile,"rb");
      if (!fptr)
      {
        fprintf(stderr, "%s missing! Please Reset. \n",ItemFile);
        od_exit(0, FALSE);
      }
      fseek(fptr,sizeof(struct ritems) * (rnum-1),SEEK_SET);
      fread(&Items,sizeof(struct ritems),1,fptr);
      fclose(fptr);
      int x = checkitems();
      //od_printf("%i\r\n%i\r\n%i\r\n",x,rnum,Items->index);
      if (x!=0)
      {
        Plyr.items[x].i_index=Items->index;
        strcpy(Plyr.items[x].i_name,Items->name);
        Plyr.items[x].i_value=Items->value;
        Plyr.items[x].i_hit_points=Items->hit_points;
        Plyr.items[x].i_hit_multi=Items->hit_multi;
        Plyr.items[x].i_fights_left=Items->moves_left;
        Plyr.items[x].i_human_left=Items->human_left;
        Plyr.items[x].i_int_multi=Items->int_multi;
        Plyr.items[x].i_def_multi=Items->def_multi;
        Plyr.items[x].i_str_multi=Items->str_multi;
        SavePlyr();
        //od_printf("You found %i %s!!\r\n",Items->index,Items->name);
        od_printf("`blue`You found `bright blue`%i %s!!\r\n",Plyr.items[x].i_index,Plyr.items[x].i_name);
      }else
      {
        od_printf("`blue`You found `bright blue`%s...\r\n",Items->name);
        od_printf("`blue`...but your inventory is full...\r\n");
      }
      //gamepause();
    }
  }
}

int checkitems()          //Find next available slot for items
{
  int y,ret=0;
  for (y=1;y<=30;y++)
  {
    if (Plyr.items[y].i_index==0)
    {
      ret=y;
      break;
    }
  }
  return ret;
}

void monfight(int monstype,int usernumber)         //1-normal enemy 2-master 3-user 4-tower master 5-end battle
{
  int x=0;
  int MHPoint;
  int PHPoint;
  char ch;
  int monran=0;
  int battleend=0;
  int plyrran=0;
  int runaway=0;
  FILE *fptr;
  struct monst mon1;          //mon1 is struct for any enemy type
  if (Plyr.moves_left<=0)
  {
    od_printf("   You have no moves left for today");
    gamepause();
  }
  if (Plyr.hit_points>0 && Plyr.moves_left>0)
  {
    if (monstype==5)
    {
      mon1.index=Plyr.Index;
      //strcpy(mon1.name,Plyr.Alias);
      sprintf(mon1.name,"%s%s","Shadow of ",Plyr.Alias);
      mon1.strength=Plyr.strength;
      strcpy(mon1.weapon,Plyr.weapon);
      mon1.exp_points=Plyr.exp;
      mon1.gold=Plyr.gold;
      mon1.hit_points=Plyr.hit_max+Plyr.hit_multi;
      mon1.def=Plyr.def+Plyr.def_multi;
    }
    if (monstype==1)
    {
      fptr = fopen(MonFile,"rb");
      if (!fptr)
      {
        fprintf(stderr, "%s missing! please reset.\n",MonFile);
        od_exit(0, FALSE);
      }
      srand(time(0));
      int y=(rand() % 11);
      x=((usernumber*11)-y);
      fseek(fptr,sizeof(mon1) * (x-1),SEEK_SET);
      fread(&mon1,sizeof (struct monst),1,fptr);
      //if (!mon1.def) mon1.def=0;
      fclose(fptr);
    }
    if ((monstype==2)||(monstype==4))
    {
      fptr = fopen(MstrFile,"rb");
      if (!fptr)
      {
        fprintf(stderr,"%s missing! Please reset.\n",MstrFile);
        od_exit(0, FALSE);
      }
      fseek(fptr,sizeof(Master) * (usernumber-1),SEEK_SET);
      fread(&Master,sizeof(struct master),1,fptr);
      mon1.index=Master.index;
      strcpy(mon1.name,Master.name);
      mon1.strength=Master.strength;
      mon1.gold=Master.gold;
      strcpy(mon1.weapon,Master.weapon);
      mon1.exp_points=Master.exp_points;
      mon1.hit_points=Master.hit_points;
      mon1.def=Master.def;
      strcpy(mon1.death,Master.death);
      fclose(fptr);
    }
    if ((monstype==3)&&(User.dead==false))
    {
      fptr = fopen(PlyrFile,"rb");
      if (!fptr)
      {
        fprintf(stderr, "%s missing! Please reset. \n",PlyrFile);
        od_exit(0, FALSE);
      }
      fseek(fptr,sizeof(User) * (usernumber-1), SEEK_SET);
      fread(&User,sizeof(struct PlyrRec),1,fptr);
      mon1.index=User.Index;
      strcpy(mon1.name,User.Alias);
      strcpy(mon1.weapon,User.weapon);
      mon1.hit_points=User.hit_multi+User.hit_max;
      mon1.exp_points=User.exp;
      mon1.strength=User.strength+User.str_multi;
      mon1.def=User.def+User.def_multi;
      mon1.gold=User.gold;
      strcpy(mon1.death,"`bright blue`You have defeated your opponent!");
      fclose(fptr);
    }
    if ((monstype==3)&&(User.dead==true)) battleend=1;
    od_clr_scr();
    if (battleend==0) od_printf("`blue`   You have encountered `bright blue`%d %s",x,mon1.name);
    while (Plyr.hit_points>0 || mon1.hit_points>0 || runaway==0 || battleend==0)
    {
      if (runaway==1 || battleend==1) break;
      od_set_cursor(5,5);
      od_printf("`blue`Your Hitpoints: `bright blue`%d `blue`of `bright blue`%d ",Plyr.hit_points,(Plyr.hit_max+Plyr.hit_multi));
      od_set_cursor(6,5);
      od_printf("`bright blue`%s's `blue`Hitpoints: `bright blue`%d ",mon1.name,mon1.hit_points);
      od_set_cursor(8,5);
      od_printf("`blue`(`bright blue`A`blue`)`bright blue`ttack");
      od_set_cursor(9,5);
      od_printf("`blue`(`bright blue`U`blue`)`bright blue`se Item");
      od_set_cursor(10,5);
      od_printf("`blue`(`bright blue`S`blue`)`bright blue`tats");
      od_set_cursor(11,5);
      od_printf("`blue`(`bright blue`R`blue`)`bright blue`un");
      od_set_cursor(13,5);
      od_printf("`blue`Your command, `bright blue`%s:",Plyr.Alias);
      ch=od_get_answer("AaUuSsRr");
      switch (ch)
      {
        case 'A':
          monran=mon1.strength/2;
          plyrran=Plyr.strength/2;
          PHPoint=(((Plyr.strength/2)+(rand() % plyrran))-(mon1.def));
          MHPoint=(((mon1.strength/2)+(rand() % monran))-(Plyr.def+Plyr.def_multi));
          if (PHPoint>0)
          {
            od_printf("\r\n\n`blue`You hit `bright blue`%s `blue`for `bright blue`%d `blue`damage!                        ",mon1.name,PHPoint);
            mon1.hit_points-=PHPoint;
          }
          else od_printf("\r\n\n`blue`You attack `bright blue`%s`blue` with your `bright blue`%s `blue` and miss              ",mon1.name,Plyr.weapon);
          if (mon1.hit_points>0)
          {
            if (MHPoint>0)
            {
              od_printf("\r\n\n`bright blue`%s `blue`hits you with its `bright blue`%s `blue`for `bright blue`%d `blue`damage!            ",mon1.name,mon1.weapon,MHPoint);
              Plyr.hit_points-=MHPoint;
            }
            else od_printf("\r\n\n`bright blue`%s `blue`attacks with its `bright blue`%s `blue`and misses                          ",mon1.name,mon1.weapon);
          }
          if (Plyr.hit_points<=0 || mon1.hit_points<=0) battleend=1;
          break;
        case 'U':
          useitems();
          break;
        case 'S':
          playerstats();
          break;
        case 'R':
          srand(time(NULL));
          int runrandom = (rand() % 100)+1;
          if (runrandom<=30)
          {
            runaway=1;
            //Plyr.moves_left-=1;
            break;
          }
          else
          {
            monran=mon1.strength/2;
            MHPoint=(((mon1.strength/2)+(rand() % monran))-(Plyr.def+Plyr.def_multi));
            if (MHPoint>0)
            {
              od_printf("\r\n\n`bright blue`%s `blue`hits you with its `bright blue`%s `blue`for `bright blue`%d `blue`damage!               ",mon1.name,mon1.weapon,MHPoint);
              Plyr.hit_points-=MHPoint;
              gamepause();
            }
            else
            {
              od_printf("\r\n\n`bright blue`%s `blue`attacks with its `bright blue`%s `blue`and misses...                               ",mon1.name,mon1.weapon);
              gamepause();
            }
          }
        od_clr_scr();
      }
      if (Plyr.hit_points<=0)
      {
        Plyr.dead=true;
        addnews(Plyr.Alias,mon1.name,3);
        if (monstype==3)
        {
          //Plyr.human_left--;
          User.gold+=Plyr.gold;
          User.exp+=Plyr.exp;
          SaveUser();
        }
        if (monstype==2)
        {
          //addnews(Plyr.Alias,mon1.name,3);
          SavePlyr();
        }
        SavePlyr();
        od_printf("\r\n\n`bright blue`Sorry, but you're dead. Come back tomorrow...`blue`");
        gamepause();
        break;
      }
      if (mon1.hit_points<=0)
      {
        od_set_cursor(16,5);
        od_printf("\r\n\n\n\n`bright blue`%s`blue`",mon1.death);
        if (monstype==1)
        {
          Plyr.gold+=mon1.gold;
          Plyr.exp+=mon1.exp_points;
          //Plyr.moves_left--;
          SavePlyr();
        }
        if (monstype==3)
        {
          addnews(Plyr.Alias,mon1.name,2);
          Plyr.human_left--;
          User.dead=true;
          User.gold=0;
          User.hit_points=0;
          SaveUser();
        }
        if (monstype==2)
        {
          addnews(Plyr.Alias,mon1.name,2);
          Plyr.level++;
          Plyr.exp+=mon1.exp_points;
          Plyr.gold+=mon1.gold;
          Plyr.seen_master=TRUE;
          Plyr.hit_max+=(5*Plyr.level);
          Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
          Plyr.def+=(3*Plyr.level)+Plyr.def_multi;
          Plyr.strength+=(3*Plyr.level)+Plyr.str_multi;
          SavePlyr();
        }
        if (monstype==4)
        {
          Plyr.gold+=mon1.gold;
          Plyr.exp+=mon1.exp_points;
          //Plyr.moves_left--;
          Plyr.floor++;
          SavePlyr();
          od_clr_scr();
          od_set_cursor(10,3);
          od_printf("`blue`I`bright blue` just wanted to make sure you didn't forget me.");
          od_set_cursor(12,3);
          od_printf("`blue`H`bright blue`ere is a gift so you will remember me...");
          gamepause();
          ranitem(9);
        }
        SavePlyr();
        gamepause();
      }
    }
  }
}

void bnb()
{
  char ch;
  int done=0;
  char bnbansi[40];
  while (!done)
  {
    od_clr_scr();
    sprintf(bnbansi,"%s%s",ansipath,"bnb");
    od_send_file(bnbansi);
    od_set_cursor(23,1);
    od_printf("`blue`Your Command: `bright blue`%s`blue`:",od_control.user_name);
    ch = od_get_answer("RrCcBbEeSsGgDdXxTt");
    switch(ch)
    {
      case 'R':
        done=1;
        break;
      case 'C':
        if (Plyr.level>=8) dogpark();
        break;
      case 'B':
        room();
        break;
      case 'E':
        eat();
        break;
      case 'S':
        //shop();
        break;
      case 'G':
        //gamble();
        break;
      case 'D':
        //saloon();
        break;
      case 'X':
        //southoftown();
        break;
      case 'T':
        od_clr_scr();
        od_set_cursor(7,2);
        od_printf("`blue`Frannie: `white`Welcome to the Big Bear Bed n Breakfast!\r\n");
        od_sleep(500);
        od_set_cursor(9,2);
        od_printf("My husband is looking for the doors out of here.\r\n");
        od_sleep(500);
        od_set_cursor(11,2);
        od_printf("I've lost my gold wedding ring.\r\n");
        od_sleep(500);
        od_set_cursor(13,2);
        od_printf("My husband will be upset if he finds out it's missing.\r\n");
        od_sleep(500);
        od_set_cursor(15,2);
        od_printf("Is there any chance you have found it? (Y/N)\r\n");
        od_sleep(500);
        od_set_cursor(17,2);
        od_printf("If you bring me my gold wedding ring,\r\n");
        od_set_cursor(18,2);
        od_printf("I'll give you something to help you on your quest.\r\n");
        //od_get_key(TRUE);
        gamepause();
    }
  }
}

void dogpark()
{
  od_clr_scr();
  od_set_cursor(5,5);
  od_printf("`blue`You finally find the dog park.");
  od_sleep(500);
  od_set_cursor(7,5);
  od_printf("It should be a good place to find women...");
  od_sleep(500);
  od_set_cursor(9,5);
  od_printf("There's a beautiful woman over there...");
  od_sleep(500);
  od_set_cursor(11,5);
  od_printf("What is that terrible sound?");
  od_sleep(500);
  od_set_cursor(13,5);
  od_printf("It's coming from the same direction...");
  od_sleep(500);
  od_set_cursor(15,5);
  od_printf("You have to make sure she's alright...");
  od_sleep(500);
  od_set_cursor(17,5);
  od_printf("Something jumps out from the bushes...");
  gamepause();
}

void room()
{
  char ch;
  char roomansi[40];
  int roomprice;
  od_clr_scr();
  sprintf(roomansi,"%s%s",ansipath,"room");
  od_send_file(roomansi);
  roomprice=Plyr.level*500;
  od_set_cursor(10,54);
  goldconvert(roomprice);
  od_printf("`blue`%s `white`gold per night",comma);
  od_set_cursor(21,3);
  od_printf("`blue`Would you like the room? (Y/N): ");
  ch=od_get_answer("YyNn");
  switch (tolower(ch))
  {
    case 'y':
      break;
    case 'n':
      gamepause();
      break;
  }
}

void eat()
{
  char ch;
  char eatansi[40];
  int foodprice;
  od_clr_scr();
  sprintf(eatansi,"%s%s",ansipath,"eat");
  od_send_file(eatansi);
  foodprice=Plyr.level*500;
  od_set_cursor(5,46);
  goldconvert(foodprice);
  od_printf("`bright blue`D`blue`inner will cost you `bright blue`%s `blue`gold",comma);
  od_set_cursor(8,46);
  od_printf("`bright blue`I`blue`t will replenish your hitpoints");
  od_set_cursor(21,3);
  od_printf("`bright blue`W`blue`ould you like dinner? (Y/N): ");
  ch=od_get_answer("YyNn");
  switch (ch)
  {
    case 'Y':
      if (Plyr.gold>=foodprice && Plyr.hit_points<Plyr.hit_max)
      {
        Plyr.gold=Plyr.gold-foodprice;
        Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
        od_printf("`bright blue`Y`blue`our Hit points have been restored!");
        gamepause();
      }
      else
      {
        od_printf("`bright blue`Y`blue`ou don't really want to do that...");
        gamepause();
      }
      break;
    case 'N':
      gamepause();
      break;
  }
}

void backstory()
{
  char threedoors[40];
  od_clr_scr();
  od_printf("`blue`You wake up and quickly look around you.\n\n\r");
  od_printf("`blue`As you look behind you, where there are waves crashing on the beach.\n\n\r");
  od_printf("`blue`Looking in front of you, there are three doors.\n\n\r");
  od_printf("`blue`The only things you can remember, is that you are the last gunslinger,\n\n\r");
  od_printf("`blue`and that you are following the 'Beam' to the Dark Tower. Once there, you\n\n\r");
  od_printf("`blue`know that you must defeat the Man in Black.");
  gamepause();
  sprintf(threedoors,"%s%s",ansipath,"3doors");
  od_send_file(threedoors);
  gamepause();
}

void play_game()
{
  char ch;
  int name_taken = 1;
  struct PlyrRec otherplayer;
  //int done = 0;
  time_t timenow;
  struct tm today_tm;
  struct tm last_tm;
  struct tm *ptr;
  setweapons();
  setarmour();
  setitems();
  //setenemys();
  if (!load_player())
  {
    od_clr_scr();
    while (name_taken)
    {
      od_printf("\r\n`blue`You look new. What's your name? ");
      od_input_str(Plyr.Alias,32,32,126);
      if (strlen(Plyr.Alias)==0)
      {
        od_printf("\r\n\r\nOk, see you later.\r\n");
        gamepause();
        gameend(0);
      }
      if (strlen(Plyr.Alias) < 3)
      {
        od_printf("\r\n\r\nThat's a short name. Try again! ");
      }
      else
      {
        if (scan_for_player(Plyr.Alias, &otherplayer))
        {
          od_printf("\r\n\r\nSorry, that name is already taken.");
          od_printf("\r\nTry that again...");
        }
        else name_taken = 0;
      }
    }
    od_printf("\r\nWelcome %s!",Plyr.Alias);
    //gamepause();
    od_printf("\r\nAnd what is your gender? \r\n");
    ch = od_get_answer("MmFf\r");
    if (ch=='M' || ch=='m')
    {
      Plyr.sex=true;
      od_printf("Alright, you're a male...\r\n");
    }
    else if (ch=='F' || ch=='f')
    {
      Plyr.sex=false;
      od_printf("Alright, you're a female...\r\n");
    }
    gamepause();
    strcpy(Plyr.BBS,SystemName);
    //od_printf("%s %s",SystemName,Plyr.BBS);
    //gamepause();
    Plyr.hit_points=20;
    Plyr.hit_max=20;
    Plyr.hit_multi=0;
    Plyr.king=0;
    Plyr.weapon_num=Plyr.king+1;
    strcpy(Plyr.weapon,weapons[Plyr.weapon_num].name);
    Plyr.seen_master=false;
    Plyr.moves_left=DAILYMOVES;
    Plyr.human_left=DAILYHUMANFIGHTS;
    Plyr.gold=500;
    Plyr.bank=0;
    Plyr.int_multi=1;
    Plyr.def=1;
    Plyr.def_multi=1;
    Plyr.strength=5;
    Plyr.str_multi=0;
    Plyr.level=1;
    Plyr.arm_num=Plyr.king+1;
    strcpy(Plyr.arm,armours[Plyr.arm_num].name);
    Plyr.dead=false;
    Plyr.exp=1;
    Plyr.floor=1;
    Plyr.time=time(NULL);
    od_printf("Setting items...\r\n");
    for (int x=1;x<=30;x++) Plyr.items[x].i_index=0;
    add_player_idx();
    Plyr.Index=get_player_idx();
    od_printf("Saving Player info...\r\n");
    SavePlyr();
    addnews(Plyr.Alias,NULL,1);
    logentry(5);
    backstory();
  }
  else
  {
    ptr = localtime(&Plyr.time);
    memcpy(&last_tm, ptr, sizeof(struct tm));
    timenow=time(NULL);
    ptr = localtime(&timenow);
    memcpy(&today_tm, ptr, sizeof(struct tm));
    if (today_tm.tm_yday != last_tm.tm_yday || today_tm.tm_year != last_tm.tm_year)
    {
      Plyr.hit_points=Plyr.hit_max+Plyr.hit_multi;
      Plyr.moves_left=DAILYMOVES;
      Plyr.human_left=DAILYHUMANFIGHTS;
      strcpy(Plyr.BBS,SystemName);
      Plyr.gold=Plyr.gold*Plyr.int_multi;
      Plyr.bank=Plyr.bank*Plyr.int_multi;
      Plyr.seen_master=FALSE;
      Plyr.time=timenow;
      SavePlyr();
    }
    addnews(Plyr.Alias,NULL,4);
  }
  Plyr.last_play=time(NULL);
  logentry(1);
    if (IBBS)
    {
      ibbs_in();
    }

  town(Plyr.map);
}

void add_player_idx()
{
  FILE *fptr;

  fptr = fopen("tdtaplyr.idx", "a");
  if (!fptr)
    {
      fprintf(stderr, "ERROR OPENING players.idx!\n");
      gameend(-1);
    }
  fprintf(fptr, "%s+%s\n", od_control_get()->user_name, od_control_get()->user_handle);
  fclose(fptr);
}

int get_player_idx()
{
    FILE *fptr;

  char buffer[256];
  char savefile[256];
  int idx = 0;
  fptr = fopen("tdtaplyr.idx", "r");

  snprintf(savefile, 255, "%s+%s", od_control_get()->user_name, od_control_get()->user_handle);

  if (fptr != NULL)
    {
    fgets(buffer, 256, fptr);
    while (!feof(fptr))
    {
      if (strncmp(buffer, savefile, strlen(savefile)) == 0)
      {
        fclose(fptr);
        return idx;
      }
      idx++;
      fgets(buffer, 256, fptr);
    }
    fclose(fptr);
  }
  return -1;
}

int load_player()
{
  FILE *fptr;
  Plyr.Index = get_player_idx();
  if (Plyr.Index == -1)
    {
    return 0;
    }
  fptr = fopen(PlyrFile, "rb");
  if (!fptr)
  {
    fprintf(stderr, "rcstdta.ply missing! please reset.\n");
    od_exit(0, FALSE);
  }
  fseek(fptr, sizeof(struct PlyrRec) * Plyr.Index, SEEK_SET);
  if (fread(&Plyr, sizeof(struct PlyrRec), 1, fptr) < 1)
  {
    fclose(fptr);
    return 0;
  }
  fclose(fptr);
  return 1;
}

int scan_for_player(char *username, struct PlyrRec *Plyr)
{
  FILE *fptr;
  fptr = fopen(PlyrFile, "rb");
  if (!fptr)
  {
    return 0;
  }
  while (fread(Plyr, sizeof(struct PlyrRec), 1, fptr) == 1)
  {
    if (strcasecmp(Plyr->Name, username) == 0)
    {
      fclose(fptr);
      return 1;
    }
  }
  fclose(fptr);
  return 0;
}

void SavePlyr()
{
  #if defined(_MSC_VER) || defined(WIN32)
    int fno = open("PlyrFile", O_WRONLY | O_CREAT | O_BINARY, 0644);
  #else
    int fno = open(PlyrFile, O_WRONLY | O_CREAT, 0644);
  #endif // defined
  if (fno < 0)
  {
    gameend(-1);
  }
  lseek(fno, sizeof(struct PlyrRec) * Plyr.Index, SEEK_SET);
  write(fno, &Plyr, sizeof(struct PlyrRec));
  close(fno);
}

void SaveUser()
{
  #if defined(_MSC_VER) || defined(WIN32)
    int fno = open("PlyrFile", O_WRONLY | O_CREAT | O_BINARY, 0644);
  #else
    int fno = open(PlyrFile, O_WRONLY | O_CREAT, 0644);
  #endif // defined
  if (fno < 0)
  {
    gameend(-1);
  }
  lseek(fno, sizeof(struct PlyrRec) * User.Index, SEEK_SET);
  write(fno, &User, sizeof(struct PlyrRec));
  close(fno);
}

void gamepause()
{
  char done=0;
  od_printf("\r\n");
  while (!done)
  {
    od_set_cursor(23,1);
    od_printf("`blue`-=>Pause  <=-");
    od_sleep(250);
    od_set_cursor(23,1);
    if (od_get_key(FALSE)) done=1;
    od_printf("`blue`-=> Pause <=-");
    od_sleep(250);
    od_set_cursor(23,1);
    if (od_get_key(FALSE)) done=1;
    od_printf("`blue`-=>  Pause<=-");
    od_sleep(250);
    od_set_cursor(23,1);
    if (od_get_key(FALSE)) done=1;
    od_printf("`blue`-=> Pause <=-");
    od_sleep(250);
    if (od_get_key(FALSE)) done=1;
  }
}

int gameend(int gameerrorlevel)
{
  //fprintf(stderr,"\r\nExit with errorlevel %d\r\n\n",gameerrorlevel);
  //gamepause();
  listplayers();
  if (IBBS) ibbs_out();
  logentry(2);
  od_exit(gameerrorlevel,FALSE);
  return gameerrorlevel;
}

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef _MSC_VER
#include <Windows.h>
#include <io.h>
#define strcasecmp stricmp
#else
#include <unistd.h>
#endif
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include "tdtaibbs.h"
#include "OpenDoor.h"

struct PlyrRec Plyr1;
struct ibbsPlyrRec IBBSPlyr;
struct ibbslinks links1[20];
void gameend(int gameerrorlevel);
int IBBSload_player();
void IBBSadd_player_idx();
void IBBSSavePlyr(int x);
int loc_load_player(int x);
int copy_file(char *src_file, char *dest_file);
void gamepause();
void logentry(int entry);

//char systemlink[4];
int systemlinkint;
int totallinksibbs;

void ibbs_in()
{
    logentry(6);
}

void ibbs_out()
{
  logentry(7);
  od_clr_scr();
  remove("tdtaibbs.idx");
  int x=1;
  int y=1;
  while (loc_load_player(x))
  {
    IBBSPlyr.Index=Plyr1.Index;
    strcpy(IBBSPlyr.Name,od_control_get()->user_name);
    strcpy(IBBSPlyr.Alias,Plyr1.Alias);
    strcpy(IBBSPlyr.BBS,Plyr1.BBS);
    IBBSPlyr.hit_points=Plyr1.hit_points;
    IBBSPlyr.hit_max=Plyr1.hit_max;
    IBBSPlyr.hit_multi=Plyr1.hit_multi;
    IBBSPlyr.weapon_num=Plyr1.weapon_num;
    strcpy(IBBSPlyr.weapon,Plyr1.weapon);
    IBBSPlyr.gold=Plyr1.gold;
    IBBSPlyr.def=Plyr1.def;
    IBBSPlyr.def_multi=Plyr1.def_multi;
    IBBSPlyr.strength=Plyr1.strength;
    IBBSPlyr.str_multi=Plyr1.str_multi;
    IBBSPlyr.level=Plyr1.level;
    strcpy(IBBSPlyr.arm,Plyr1.arm);
    IBBSPlyr.arm_num=Plyr1.arm_num;
    IBBSPlyr.dead=Plyr1.dead;
    IBBSPlyr.exp=Plyr1.exp;
    IBBSPlyr.king=Plyr1.king;
    IBBSPlyr.map=Plyr1.map;
    IBBSPlyr.cord_x=Plyr1.cord_x;
    IBBSPlyr.cord_y=Plyr1.cord_y;
    IBBSSavePlyr(x);
    IBBSadd_player_idx();
    x++;
  }
  char link[60];
  //char link1[]=links[y].linkout"/test.dat");
  od_set_cursor(1,1);
  y=1;
  //od_printf("%d - %d\n\r",totallinks,systemlinki);
  if (systemlinkint==1)
  {
    //sprintf(char link1,"%s",links[y].linkfilename);
    do
    {
      if (systemlinkint!=y)
      {
        //od_printf("%s - %s\n\r",IBBSPlyrFile,links1[y].linkfilename);
        copy_file(IBBSPlyrFile,links1[y].linkfilename);
        //od_printf("%d - %s\r\n",y,links1[y].linkfilename);
        //gamepause();
      }
      y++;
      od_set_cursor(y,1);
    } while (y<=totallinksibbs);
  }
  else
  {
    sprintf(link,"%s",links1[y].linkfilename);
    //od_printf("%s\r\n%s\r\n",links1[y].linkfilename,link);
    copy_file(IBBSPlyrFile, link);
  }
  //gamepause();
}

int copy_file(char *src_file, char *dest_file)
{
  FILE *ptr_src, *ptr_dest;
  int a;
  ptr_src = fopen(src_file,"rb");
  ptr_dest = fopen(dest_file,"wb");
  if (ptr_src == NULL) return -1;
  if (ptr_dest == NULL)
  {
    fclose(ptr_src);
    return -1;
  }
  while(1)
  {
    a = fgetc(ptr_src);
    if(!feof(ptr_src)) fputc(a, ptr_dest);
    else break;
  }
  fclose(ptr_src);
  fclose(ptr_dest);
  return 0;
}

void IBBSadd_player_idx()
{
  FILE *fptr;

  fptr = fopen("tdtaibbs.idx", "a");
  if (!fptr)
    {
      fprintf(stderr, "ERROR OPENING tdtaibbs.idx!\n");
      gameend(-1);
  }
  else {
      fprintf(fptr, "%s+%s\n", od_control_get()->user_name, Plyr1.Alias);
      fclose(fptr);
  }
}

int IBBSget_player_idx()
{
    FILE *fptr;

  char buffer[256];
  char savefile[256];
  int idx = 0;
  fptr = fopen("tdtaibbs.idx", "r");

  snprintf(savefile, 255, "%s+%s", od_control_get()->user_name, Plyr1.Alias);

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

int IBBSload_player()
{
  FILE *fptr;
  Plyr1.Index = IBBSget_player_idx();
  if (Plyr1.Index == -1)
    {
    return 0;
    }
  fptr = fopen(IBBSPlyrFile, "rb");
  if (!fptr)
  {
    fprintf(stderr, "tdtaibbs.ply missing! please reset.\n");
    od_exit(0, FALSE);
  }
  else {
      fseek(fptr, sizeof(struct PlyrRec) * Plyr1.Index, SEEK_SET);
      if (fread(&Plyr1, sizeof(struct PlyrRec), 1, fptr) < 1)
      {
          fclose(fptr);
          return 0;
      }
      fclose(fptr);
  }
  return 1;
}

int loc_load_player(int x)
{
  FILE *fptr;
  //Plyr1.Index = get_player_idx();
  if (Plyr1.Index == -1)
    {
    return 0;
    }
  fptr = fopen(LocPlyrFile, "rb");
  if (!fptr)
  {
    fprintf(stderr, "rcstdta.ply missing! please reset.\n");
    od_exit(0, FALSE);
  }
  fseek(fptr, sizeof(struct PlyrRec) * (x-1), SEEK_SET);
  if (fread(&Plyr1, sizeof(struct PlyrRec), 1, fptr) < 1)
  {
    fclose(fptr);
    return 0;
  }
  fclose(fptr);
  return 1;
}

int IBBSscan_for_player(char *username, struct PlyrRec *Plyr1)
{
  FILE *fptr;
  fptr = fopen(IBBSPlyrFile, "rb");
  if (!fptr)
  {
    return 0;
  }
  while (fread(Plyr1, sizeof(struct PlyrRec), 1, fptr) == 1)
  {
    if (strcasecmp(Plyr1->Name, username) == 0)
    {
      fclose(fptr);
      return 1;
    }
  }
  fclose(fptr);
  return 0;
}

void IBBSSavePlyr(int x)
{
  #if defined(_MSC_VER) || defined(WIN32)
    int fno = open("PlyrFile", O_WRONLY | O_CREAT | O_BINARY, 0644);
  #else
    int fno = open(IBBSPlyrFile, O_WRONLY | O_CREAT, 0644);
  #endif // defined
  if (fno < 0)
  {
    gameend(-1);
  }
  lseek(fno, sizeof(struct ibbsPlyrRec) * x, SEEK_SET);
  write(fno, &Plyr1, sizeof(struct ibbsPlyrRec));
  close(fno);
}

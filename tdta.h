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

#ifndef __TDTA_H__
#define __TDTA_H__

#if defined(_MSC_VER) || defined(WIN32)
#include <direct.h>
#define GetCurrentDir _getcwd

#else
#include <unistd.h>
//#define GetCurrentDir getcwd

#endif // defined

struct PlyrRec {
int Index;
char Name[32];
char Alias[32];
char BBS[20];
int hit_points;
int hit_max;
int hit_multi;
int weapon_num;
char weapon[20];
bool seen_master;
int moves_left;     //change to moves_left
int human_left;
unsigned long int gold;
unsigned long int bank;
int int_multi;
int def;
int def_multi;
int strength;
int str_multi;
int level;
int floor;
time_t time;
time_t last_play;
char arm[20];
int arm_num;
bool dead;
unsigned long exp;
bool sex;
int king;
bool room;
int map;
int cord_x;
int cord_y;
struct ritems
  {
    int i_index;
    int i_value;
    char i_name[25];
    int i_hit_points;
    int i_hit_multi;
    int i_fights_left;
    int i_human_left;
    int i_int_multi;
    int i_def_multi;
    int i_str_multi;
  } items[31];
};

struct fitems
{
  int index;
  int value;
  char name[25];
  int hit_points;
  int hit_multi;
  int moves_left;
  int human_left;
  int int_multi;
  int def_multi;
  int str_multi;
} Items[31];

struct Plyrlist {
int Index;
char Alias[32];
unsigned long exp;
int level;
int map;
int cord_x;
int cord_y;
bool dead;
};

struct PlyrLocation {
	int Index;
	char Alias[32];
	int map;
	int cord_x;
	int cord_y;
	bool dead;
};

struct monst {
  int index;
  char name[32];
  int strength;
  int def;
  int gold;
  char weapon[60];
  int exp_points;
  int hit_points;
  char death[100];
};

struct master {                  //master enemy record
int index;
char name[60];
int strength;
int def;               //added defense
int gold;
char weapon[60];
int exp_points;
int hit_points;
char death[100];
};

struct wrec {                 //weapon record
int index;
char name[20];
int price;
int strength;
};

struct arec {                 //armour record
int index;
char name[20];
int price;
int strength;
};

struct ibbslinks {
char linknumber[4];
char linkaddress[20];
char linkfilename[35];
char linkout[20];
};

char rcspath[40];
char PlyrFile[15]="tdtaplyr.dat";
char MonFile[15]="tdtaenem.dat";
char MstrFile[15]="tdtamstr.dat";
char NewsFile[15]="rcstdta.dly";
char WeaponFile[15]="tdtaweap.dat";
char ArmourFile[15]="tdtaarmo.dat";
char ItemFile[15]="tdtaitem.dat";
char CFGFile[15]="tdta.cfg";
char months[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

#endif // __TDTA_H__

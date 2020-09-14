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

#ifndef TDTAIBBS_H_INCLUDED
#define TDTAIBBS_H_INCLUDED

struct ibbsPlyrRec
{
  int Index;
  char Name[32];
  char Alias[32];
  char BBS[20];
  int hit_points;
  int hit_max;
  int hit_multi;
  int weapon_num;
  char weapon[20];
  unsigned long gold;
  int def;
  int def_multi;
  int strength;
  int str_multi;
  int level;
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
};

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
unsigned long gold;
unsigned long bank;
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

struct ibbslinks {
char linknumber[4];
char linkaddress[20];
char linkfilename[60];
char linkout[20];
};

char IBBSPlyrFile[15]="tdtaibbs.ply";
char LocPlyrFile[15]="tdtaplyr.dat";

#endif // TDTAIBBS_H_INCLUDED

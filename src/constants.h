#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>

struct Address
{
  void *blockAddress;
  short int offset;
};

struct Record
{
  char game_date_est[11];
  long team_id_home;
  int pts_home;
  float fg_pct_home;
  float ft_pct_home;
  float fg3_pct_home;
  int ast_home;
  int reb_home;
  int home_team_wins;

};

#endif
///////////////////////////////////////////////////////////////////////////////
// GameSpy ATLAS Competition System Source File
//
// NOTE: This is an auto-generated file, do not edit this file directly.
//
// Atlasサーバから吐き出されるhとcです。
// 変更項目は以下
// ・DWC_GdbGetMyRecordsAsyncで使うため、使用キー文字列をテーブル化
// ・GFL_STD_StrCmpを使用していたのをGFL_STD_StrCmpへ変更
//
///////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "atlas_syachi2ds_v1.h"

int atlas_rule_set_version = 1;

static const char* ATLAS_Rnd_Field_Name[] = 
{
		"ARENA_ELO_RATING_1V1_DOUBLE",
		"ARENA_ELO_RATING_1V1_ROTATE",
		"ARENA_ELO_RATING_1V1_SHOOTER",
		"ARENA_ELO_RATING_1V1_SINGLE",
		"ARENA_ELO_RATING_1V1_TRIPLE",
		"ARENA_ELO_RATING_1V1_WIFICUP",
		"CHEATS_COUNTER",
		"COMPLETE_MATCHES_COUNTER",
		"DISCONNECTS_COUNTER",
		"NUM_DOUBLE_LOSE_COUNTER",
		"NUM_DOUBLE_WIN_COUNTER",
		"NUM_ROTATE_LOSE_COUNTER",
		"NUM_ROTATE_WIN_COUNTER",
		"NUM_SHOOTER_LOSE_COUNTER",
		"NUM_SHOOTER_WIN_COUNTER",
		"NUM_SINGLE_LOSE_COUNTER",
		"NUM_SINGLE_WIN_COUNTER",
		"NUM_TRIPLE_LOSE_COUNTER",
		"NUM_TRIPLE_WIN_COUNTER",
};

static const char* ATLAS_Wifi_Field_Name[] = 
{
		"ARENA_ELO_RATING_1V1_WIFICUP",
		"CHEATS_WIFICUP_COUNTER",
		"DISCONNECTS_WIFICUP_COUNTER",
		"NUM_WIFICUP_LOSE_COUNTER",
		"NUM_WIFICUP_WIN_COUNTER",
    "WIFICUP_POKEMON_PARTY",
};

const char** ATLAS_RND_GetFieldName( void )
{ 
  return ATLAS_Rnd_Field_Name;
}

u32 ATLAS_RND_GetFieldNameNum( void )
{ 
  return NELEMS( ATLAS_Rnd_Field_Name );
}
const char** ATLAS_WIFI_GetFieldName( void )
{ 
  return ATLAS_Wifi_Field_Name;
}

u32 ATLAS_WIFI_GetFieldNameNum( void )
{ 
  return NELEMS( ATLAS_Wifi_Field_Name );
}

int ATLAS_GET_KEY(char* keyName)
{
   if(!keyName)
      return 0;
   
   if(!GFL_STD_StrCmp("KEY_NUM_SINGLE_WIN_COUNTER", keyName))
      return KEY_NUM_SINGLE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_SINGLE_LOSE_COUNTER", keyName))
      return KEY_NUM_SINGLE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_DOUBLE_LOSE_COUNTER", keyName))
      return KEY_NUM_DOUBLE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_DOUBLE_WIN_COUNTER", keyName))
      return KEY_NUM_DOUBLE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_ROTATE_WIN_COUNTER", keyName))
      return KEY_NUM_ROTATE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_ROTATE_LOSE_COUNTER", keyName))
      return KEY_NUM_ROTATE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_TRIPLE_WIN_COUNTER", keyName))
      return KEY_NUM_TRIPLE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_TRIPLE_LOSE_COUNTER", keyName))
      return KEY_NUM_TRIPLE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_SHOOTER_WIN_COUNTER", keyName))
      return KEY_NUM_SHOOTER_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_SHOOTER_LOSE_COUNTER", keyName))
      return KEY_NUM_SHOOTER_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_ARENA_GAMETYPE_1V1_SINGLE", keyName))
      return KEY_ARENA_GAMETYPE_1V1_SINGLE;   
   if(!GFL_STD_StrCmp("KEY_ARENA_GAMETYPE_1V1_DOUBLE", keyName))
      return KEY_ARENA_GAMETYPE_1V1_DOUBLE;   
   if(!GFL_STD_StrCmp("KEY_ARENA_GAMETYPE_1V1_ROTATE", keyName))
      return KEY_ARENA_GAMETYPE_1V1_ROTATE;   
   if(!GFL_STD_StrCmp("KEY_ARENA_GAMETYPE_1V1_TRIPLE", keyName))
      return KEY_ARENA_GAMETYPE_1V1_TRIPLE;   
   if(!GFL_STD_StrCmp("KEY_ARENA_GAMETYPE_1V1_SHOOTER", keyName))
      return KEY_ARENA_GAMETYPE_1V1_SHOOTER;   
   if(!GFL_STD_StrCmp("KEY_CHEATS_COUNTER", keyName))
      return KEY_CHEATS_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_ARENA_GAMETYPE_1V1_WIFICUP", keyName))
      return KEY_ARENA_GAMETYPE_1V1_WIFICUP;   
   if(!GFL_STD_StrCmp("KEY_NUM_WIFICUP_WIN_COUNTER", keyName))
      return KEY_NUM_WIFICUP_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_NUM_WIFICUP_LOSE_COUNTER", keyName))
      return KEY_NUM_WIFICUP_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_CHEATS_WIFICUP_COUNTER", keyName))
      return KEY_CHEATS_WIFICUP_COUNTER;   
   if(!GFL_STD_StrCmp("KEY_DISCONNECTS_WIFICUP_COUNTER", keyName))
      return KEY_DISCONNECTS_WIFICUP_COUNTER;   
   
   return 0;
}

char* ATLAS_GET_KEY_NAME(int keyId)
{
   if(keyId <= 0)
      return "";
   
   if(keyId == KEY_NUM_SINGLE_WIN_COUNTER)
      return "KEY_NUM_SINGLE_WIN_COUNTER";   
   if(keyId == KEY_NUM_SINGLE_LOSE_COUNTER)
      return "KEY_NUM_SINGLE_LOSE_COUNTER";   
   if(keyId == KEY_NUM_DOUBLE_LOSE_COUNTER)
      return "KEY_NUM_DOUBLE_LOSE_COUNTER";   
   if(keyId == KEY_NUM_DOUBLE_WIN_COUNTER)
      return "KEY_NUM_DOUBLE_WIN_COUNTER";   
   if(keyId == KEY_NUM_ROTATE_WIN_COUNTER)
      return "KEY_NUM_ROTATE_WIN_COUNTER";   
   if(keyId == KEY_NUM_ROTATE_LOSE_COUNTER)
      return "KEY_NUM_ROTATE_LOSE_COUNTER";   
   if(keyId == KEY_NUM_TRIPLE_WIN_COUNTER)
      return "KEY_NUM_TRIPLE_WIN_COUNTER";   
   if(keyId == KEY_NUM_TRIPLE_LOSE_COUNTER)
      return "KEY_NUM_TRIPLE_LOSE_COUNTER";   
   if(keyId == KEY_NUM_SHOOTER_WIN_COUNTER)
      return "KEY_NUM_SHOOTER_WIN_COUNTER";   
   if(keyId == KEY_NUM_SHOOTER_LOSE_COUNTER)
      return "KEY_NUM_SHOOTER_LOSE_COUNTER";   
   if(keyId == KEY_ARENA_GAMETYPE_1V1_SINGLE)
      return "KEY_ARENA_GAMETYPE_1V1_SINGLE";   
   if(keyId == KEY_ARENA_GAMETYPE_1V1_DOUBLE)
      return "KEY_ARENA_GAMETYPE_1V1_DOUBLE";   
   if(keyId == KEY_ARENA_GAMETYPE_1V1_ROTATE)
      return "KEY_ARENA_GAMETYPE_1V1_ROTATE";   
   if(keyId == KEY_ARENA_GAMETYPE_1V1_TRIPLE)
      return "KEY_ARENA_GAMETYPE_1V1_TRIPLE";   
   if(keyId == KEY_ARENA_GAMETYPE_1V1_SHOOTER)
      return "KEY_ARENA_GAMETYPE_1V1_SHOOTER";   
   if(keyId == KEY_CHEATS_COUNTER)
      return "KEY_CHEATS_COUNTER";   
   if(keyId == KEY_ARENA_GAMETYPE_1V1_WIFICUP)
      return "KEY_ARENA_GAMETYPE_1V1_WIFICUP";   
   if(keyId == KEY_NUM_WIFICUP_WIN_COUNTER)
      return "KEY_NUM_WIFICUP_WIN_COUNTER";   
   if(keyId == KEY_NUM_WIFICUP_LOSE_COUNTER)
      return "KEY_NUM_WIFICUP_LOSE_COUNTER";   
   if(keyId == KEY_CHEATS_WIFICUP_COUNTER)
      return "KEY_CHEATS_WIFICUP_COUNTER";   
   if(keyId == KEY_DISCONNECTS_WIFICUP_COUNTER)
      return "KEY_DISCONNECTS_WIFICUP_COUNTER";   
   
   return "";
}

int ATLAS_GET_STAT(char* statName)
{
   if(!statName)
      return 0;
   
   if(!GFL_STD_StrCmp("ARENA_ADVANCED_MODIFIER", statName))
      return ARENA_ADVANCED_MODIFIER;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_DOUBLE", statName))
      return ARENA_ELO_RATING_1V1_DOUBLE;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_ROTATE", statName))
      return ARENA_ELO_RATING_1V1_ROTATE;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_SHOOTER", statName))
      return ARENA_ELO_RATING_1V1_SHOOTER;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_SINGLE", statName))
      return ARENA_ELO_RATING_1V1_SINGLE;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_TRIPLE", statName))
      return ARENA_ELO_RATING_1V1_TRIPLE;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_WIFICUP", statName))
      return ARENA_ELO_RATING_1V1_WIFICUP;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_MAXIMUM_STAT", statName))
      return ARENA_ELO_RATING_MAXIMUM_STAT;   
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_MINIMUM_STAT", statName))
      return ARENA_ELO_RATING_MINIMUM_STAT;   
   if(!GFL_STD_StrCmp("ARENA_FLOOR_MODIFIER", statName))
      return ARENA_FLOOR_MODIFIER;   
   if(!GFL_STD_StrCmp("ARENA_INITIAL_RATING", statName))
      return ARENA_INITIAL_RATING;   
   if(!GFL_STD_StrCmp("ARENA_INTERMED_MODIFIER", statName))
      return ARENA_INTERMED_MODIFIER;   
   if(!GFL_STD_StrCmp("ARENA_MASTER_MODIFIER", statName))
      return ARENA_MASTER_MODIFIER;   
   if(!GFL_STD_StrCmp("ARENA_RATING_DIFF_HIGH", statName))
      return ARENA_RATING_DIFF_HIGH;   
   if(!GFL_STD_StrCmp("ARENA_RATING_DIFF_LOW", statName))
      return ARENA_RATING_DIFF_LOW;   
   if(!GFL_STD_StrCmp("ARENA_SCORE_WEIGHT_NORMAL", statName))
      return ARENA_SCORE_WEIGHT_NORMAL;   
   if(!GFL_STD_StrCmp("CHEATS_COUNTER", statName))
      return CHEATS_COUNTER;   
   if(!GFL_STD_StrCmp("CHEATS_WIFICUP_COUNTER", statName))
      return CHEATS_WIFICUP_COUNTER;   
   if(!GFL_STD_StrCmp("COMPLETE_MATCHES_COUNTER", statName))
      return COMPLETE_MATCHES_COUNTER;   
   if(!GFL_STD_StrCmp("DISCONNECTS_COUNTER", statName))
      return DISCONNECTS_COUNTER;   
   if(!GFL_STD_StrCmp("DISCONNECTS_WIFICUP_COUNTER", statName))
      return DISCONNECTS_WIFICUP_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_DOUBLE_LOSE_COUNTER", statName))
      return NUM_DOUBLE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_DOUBLE_WIN_COUNTER", statName))
      return NUM_DOUBLE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_ROTATE_LOSE_COUNTER", statName))
      return NUM_ROTATE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_ROTATE_WIN_COUNTER", statName))
      return NUM_ROTATE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_SHOOTER_LOSE_COUNTER", statName))
      return NUM_SHOOTER_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_SHOOTER_WIN_COUNTER", statName))
      return NUM_SHOOTER_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_SINGLE_LOSE_COUNTER", statName))
      return NUM_SINGLE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_SINGLE_WIN_COUNTER", statName))
      return NUM_SINGLE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_TRIPLE_LOSE_COUNTER", statName))
      return NUM_TRIPLE_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_TRIPLE_WIN_COUNTER", statName))
      return NUM_TRIPLE_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_WIFICUP_LOSE_COUNTER", statName))
      return NUM_WIFICUP_LOSE_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_WIFICUP_WIN_COUNTER", statName))
      return NUM_WIFICUP_WIN_COUNTER;   
   if(!GFL_STD_StrCmp("NUM_WINLOSE_COUNTER_MINIMUM_STAT", statName))
      return NUM_WINLOSE_COUNTER_MINIMUM_STAT;   
   
   return 0;
}
char* ATLAS_GET_STAT_NAME(int statId)
{
   if(statId <= 0)
      return "";
   
   if(statId == ARENA_ADVANCED_MODIFIER)
      return "ARENA_ADVANCED_MODIFIER";   
   if(statId == ARENA_ELO_RATING_1V1_DOUBLE)
      return "ARENA_ELO_RATING_1V1_DOUBLE";   
   if(statId == ARENA_ELO_RATING_1V1_ROTATE)
      return "ARENA_ELO_RATING_1V1_ROTATE";   
   if(statId == ARENA_ELO_RATING_1V1_SHOOTER)
      return "ARENA_ELO_RATING_1V1_SHOOTER";   
   if(statId == ARENA_ELO_RATING_1V1_SINGLE)
      return "ARENA_ELO_RATING_1V1_SINGLE";   
   if(statId == ARENA_ELO_RATING_1V1_TRIPLE)
      return "ARENA_ELO_RATING_1V1_TRIPLE";   
   if(statId == ARENA_ELO_RATING_1V1_WIFICUP)
      return "ARENA_ELO_RATING_1V1_WIFICUP";   
   if(statId == ARENA_ELO_RATING_MAXIMUM_STAT)
      return "ARENA_ELO_RATING_MAXIMUM_STAT";   
   if(statId == ARENA_ELO_RATING_MINIMUM_STAT)
      return "ARENA_ELO_RATING_MINIMUM_STAT";   
   if(statId == ARENA_FLOOR_MODIFIER)
      return "ARENA_FLOOR_MODIFIER";   
   if(statId == ARENA_INITIAL_RATING)
      return "ARENA_INITIAL_RATING";   
   if(statId == ARENA_INTERMED_MODIFIER)
      return "ARENA_INTERMED_MODIFIER";   
   if(statId == ARENA_MASTER_MODIFIER)
      return "ARENA_MASTER_MODIFIER";   
   if(statId == ARENA_RATING_DIFF_HIGH)
      return "ARENA_RATING_DIFF_HIGH";   
   if(statId == ARENA_RATING_DIFF_LOW)
      return "ARENA_RATING_DIFF_LOW";   
   if(statId == ARENA_SCORE_WEIGHT_NORMAL)
      return "ARENA_SCORE_WEIGHT_NORMAL";   
   if(statId == CHEATS_COUNTER)
      return "CHEATS_COUNTER";   
   if(statId == CHEATS_WIFICUP_COUNTER)
      return "CHEATS_WIFICUP_COUNTER";   
   if(statId == COMPLETE_MATCHES_COUNTER)
      return "COMPLETE_MATCHES_COUNTER";   
   if(statId == DISCONNECTS_COUNTER)
      return "DISCONNECTS_COUNTER";   
   if(statId == DISCONNECTS_WIFICUP_COUNTER)
      return "DISCONNECTS_WIFICUP_COUNTER";   
   if(statId == NUM_DOUBLE_LOSE_COUNTER)
      return "NUM_DOUBLE_LOSE_COUNTER";   
   if(statId == NUM_DOUBLE_WIN_COUNTER)
      return "NUM_DOUBLE_WIN_COUNTER";   
   if(statId == NUM_ROTATE_LOSE_COUNTER)
      return "NUM_ROTATE_LOSE_COUNTER";   
   if(statId == NUM_ROTATE_WIN_COUNTER)
      return "NUM_ROTATE_WIN_COUNTER";   
   if(statId == NUM_SHOOTER_LOSE_COUNTER)
      return "NUM_SHOOTER_LOSE_COUNTER";   
   if(statId == NUM_SHOOTER_WIN_COUNTER)
      return "NUM_SHOOTER_WIN_COUNTER";   
   if(statId == NUM_SINGLE_LOSE_COUNTER)
      return "NUM_SINGLE_LOSE_COUNTER";   
   if(statId == NUM_SINGLE_WIN_COUNTER)
      return "NUM_SINGLE_WIN_COUNTER";   
   if(statId == NUM_TRIPLE_LOSE_COUNTER)
      return "NUM_TRIPLE_LOSE_COUNTER";   
   if(statId == NUM_TRIPLE_WIN_COUNTER)
      return "NUM_TRIPLE_WIN_COUNTER";   
   if(statId == NUM_WIFICUP_LOSE_COUNTER)
      return "NUM_WIFICUP_LOSE_COUNTER";   
   if(statId == NUM_WIFICUP_WIN_COUNTER)
      return "NUM_WIFICUP_WIN_COUNTER";   
   if(statId == NUM_WINLOSE_COUNTER_MINIMUM_STAT)
      return "NUM_WINLOSE_COUNTER_MINIMUM_STAT";   
   
   return "";
}

int ATLAS_GET_STAT_PAGE_BY_ID(int statId)
{
   if(statId <= 0)
      return 0;
   

   //PlayerStats
   if(statId == 3 || statId == 4 || statId == 19 || statId == 20 || statId == 21 || statId == 22 || statId == 23 ||
      statId == 24 || statId == 15 || statId == 16 || statId == 9 || statId == 10 || statId == 17 || statId == 18 ||
      statId == 13 || statId == 14 || statId == 11 || statId == 12 || statId == 36 || statId == 37 || statId == 38 ||
      statId == 39 || statId == 40)
      return 1;   

   
   //StaticStats
   if(statId == 25 || statId == 26 || statId == 27 || statId == 28 || statId == 29 || statId == 30 || statId == 31 ||
      statId == 32 || statId == 33 || statId == 34 || statId == 35)
      return 1;   
      
   
   return 0;
}

int ATLAS_GET_STAT_PAGE_BY_NAME(char* statName)
{
   if(!statName)
      return 0;
   

   //PlayerStats
   if(!GFL_STD_StrCmp("DISCONNECTS_COUNTER", statName) || !GFL_STD_StrCmp("COMPLETE_MATCHES_COUNTER", statName) ||
      !GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_SINGLE", statName) || !GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_DOUBLE", statName) ||
      !GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_ROTATE", statName) || !GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_TRIPLE", statName) ||
      !GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_SHOOTER", statName) || !GFL_STD_StrCmp("CHEATS_COUNTER", statName) ||
      !GFL_STD_StrCmp("NUM_TRIPLE_WIN_COUNTER", statName) || !GFL_STD_StrCmp("NUM_TRIPLE_LOSE_COUNTER", statName) ||
      !GFL_STD_StrCmp("NUM_SINGLE_WIN_COUNTER", statName) || !GFL_STD_StrCmp("NUM_SINGLE_LOSE_COUNTER", statName) ||
      !GFL_STD_StrCmp("NUM_SHOOTER_WIN_COUNTER", statName) || !GFL_STD_StrCmp("NUM_SHOOTER_LOSE_COUNTER", statName) ||
      !GFL_STD_StrCmp("NUM_ROTATE_WIN_COUNTER", statName) || !GFL_STD_StrCmp("NUM_ROTATE_LOSE_COUNTER", statName) ||
      !GFL_STD_StrCmp("NUM_DOUBLE_LOSE_COUNTER", statName) || !GFL_STD_StrCmp("NUM_DOUBLE_WIN_COUNTER", statName) ||
      !GFL_STD_StrCmp("ARENA_ELO_RATING_1V1_WIFICUP", statName) || !GFL_STD_StrCmp("NUM_WIFICUP_WIN_COUNTER", statName) ||
      !GFL_STD_StrCmp("NUM_WIFICUP_LOSE_COUNTER", statName) || !GFL_STD_StrCmp("CHEATS_WIFICUP_COUNTER", statName) ||
      !GFL_STD_StrCmp("DISCONNECTS_WIFICUP_COUNTER", statName))
      return 1;   

   
   //StaticStats
   if(!GFL_STD_StrCmp("ARENA_ELO_RATING_MAXIMUM_STAT", statName) || !GFL_STD_StrCmp("ARENA_ELO_RATING_MINIMUM_STAT", statName) ||
      !GFL_STD_StrCmp("ARENA_SCORE_WEIGHT_NORMAL", statName) || !GFL_STD_StrCmp("ARENA_FLOOR_MODIFIER", statName) ||
      !GFL_STD_StrCmp("ARENA_INTERMED_MODIFIER", statName) || !GFL_STD_StrCmp("ARENA_ADVANCED_MODIFIER", statName) ||
      !GFL_STD_StrCmp("ARENA_MASTER_MODIFIER", statName) || !GFL_STD_StrCmp("ARENA_RATING_DIFF_HIGH", statName) ||
      !GFL_STD_StrCmp("ARENA_RATING_DIFF_LOW", statName) || !GFL_STD_StrCmp("NUM_WINLOSE_COUNTER_MINIMUM_STAT", statName) ||
      !GFL_STD_StrCmp("ARENA_INITIAL_RATING", statName))
      return 1;   
      
   
   return 0;
}


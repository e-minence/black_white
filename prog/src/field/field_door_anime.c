//============================================================================================
/**
 * @file field_door_anime.c
 * @brief
 * @date  2009.06.13
 * @author  tamada GAME FREAK inc.
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "fieldmap.h"
#include "field_player.h"


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  FIELDMAP_WORK * fieldmap;
  HEAPID heapID;
  const GFL_G3D_MAP_GLOBALOBJ_ST * st;
}FIELD_DOOR_ANIME_WORK;


static void makeRect(FLDHIT_RECT * rect, VecFx32 * pos);
static void getPlayerFrontPos(FIELDMAP_WORK * fieldmap, VecFx32 * pos);
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT_RESULT FieldDoorInAnime(GMEVENT * event, int *seq, void * work)
{
  FIELD_DOOR_ANIME_WORK * fdaw = work;
  VecFx32 pos;
  FLDHIT_RECT rect;
  u32 num;
  FLDMAPPER * fldmapper = FIELDMAP_GetFieldG3Dmapper(fdaw->fieldmap);
  int i;

  switch (*seq) {
  case 0:
    getPlayerFrontPos(fdaw->fieldmap, &pos);
    makeRect(&rect, &pos);
    fdaw->st = FLDMAPPER_CreateObjStatusList( fldmapper, &rect, fdaw->heapID, &num);
    if (fdaw->st == NULL) return GMEVENT_RES_FINISH;
    for (i = 0; i < num; i++)
    {
      TAMADA_Printf("id:%2d rotate:%04x\n", fdaw->st[i].id, fdaw->st[i].rotate);
      TAMADA_Printf("x,y,z (%d,%d,%d)\n",
          FX_Whole(fdaw->st[i].trans.x),
          FX_Whole(fdaw->st[i].trans.y),
          FX_Whole(fdaw->st[i].trans.z) );
    }
    

    ++ *seq;
    break;
  case 1:
    GFL_HEAP_FreeMemory((void*)fdaw->st);
    return GMEVENT_RES_FINISH;

    ++ *seq;
    break;
  }
    //���@�̖ڂ̑O�Ƀh�A��T��
    //  ���@�̖ڂ̑O�̗̈���擾
    //  �z�u���f��������
    //  �q�b�g�����z�u���f���̒��Ƀh�A�����邩�ǂ������`�F�b�N
    //
    //�h�A���J���A�j�������s
    //  ���̃h�A�ɓK�������A�j�����擾
    //���@���h�A�ɓ���A�j�������s
    //���@������
  return GMEVENT_RES_CONTINUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldDoorInAnime(GAMESYS_WORK * gsys, FIELDMAP_WORK *fieldmap)
{
  GMEVENT * event;
  FIELD_DOOR_ANIME_WORK * fdaw;
  event = GMEVENT_Create( gsys, NULL, FieldDoorInAnime, sizeof(FIELD_DOOR_ANIME_WORK) );
  fdaw = GMEVENT_GetEventWork(event);
  fdaw->gsys = gsys;
  fdaw->gamedata = GAMESYSTEM_GetGameData(gsys);
  fdaw->fieldmap = fieldmap;
  fdaw->heapID = FIELDMAP_GetHeapID(fieldmap);
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeRect(FLDHIT_RECT * rect, VecFx32 * pos)
{
  rect->top = pos->z - 16 * FX32_ONE;
  rect->bottom = pos->z + 16 * FX32_ONE;
  rect->left = pos->x - 16 * FX32_ONE;
  rect->right = pos->x + 16 * FX32_ONE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void getPlayerFrontPos(FIELDMAP_WORK * fieldmap, VecFx32 * pos)
{
  enum { GRIDSIZE = 16 * FX32_ONE };
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldmap);
  u16 dir;

  FIELD_PLAYER_GetPos(player, pos);
  {
    FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( player );
    dir = FLDMMDL_GetDirDisp( fmmdl );
  }
	switch( dir ) {
	case DIR_UP:		pos->z -= GRIDSIZE; break;
	case DIR_DOWN:	pos->z += GRIDSIZE; break;
	case DIR_LEFT:	pos->x -= GRIDSIZE; break;
	case DIR_RIGHT:	pos->x += GRIDSIZE; break;
	default:
                  GF_ASSERT(0);
  }
}




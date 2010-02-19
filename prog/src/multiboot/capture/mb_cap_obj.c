//======================================================================
/**
 * @file	  mb_cap_obj.c
 * @brief	  �ߊl�Q�[���E��Q��
 * @author	ariizumi
 * @data	  09/11/24
 *
 * ���W���[�����FMB_CAP_OBJ
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "mb_capture_gra.naix"

#include "./mb_cap_obj.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_CAP_OBJ_ANM_SPD (4)
#define MB_CAP_OBJ_ANM_FRAME (4)
#define MB_CAP_OBJ_STAR_ANM_SPD (16)
#define MB_CAP_OBJ_STAR_ANM_FRAME (3)

#define MB_CAP_OBJ_STAR_SPD (FX32_CONST(0.33))
#define MB_CAP_OBJ_STAR_ROT (0x400)
#define MB_CAP_OBJ_STAR_HEIGHT (16)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _MB_CAP_OBJ
{
  BOOL isPlayAnime;
  BOOL isEnable;
  u16 anmCnt;
  int objIdx;
  
  VecFx32 pos;
  
  MB_CAP_OBJ_TYPE objType;
  
  //���p
  s8  moveDir;
  u16 rad[3];
  fx32 height;
  fx32 baseY;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	�I�u�W�F�N�g�쐬
//--------------------------------------------------------------
MB_CAP_OBJ* MB_CAP_OBJ_CreateObject( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ_INIT_WORK *initWork )
{
  const HEAPID heapId = MB_CAPTURE_GetHeapId( capWork );
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  ARCHANDLE *arcHandle = MB_CAPTURE_GetArcHandle( capWork );
  MB_CAP_OBJ *objWork = GFL_HEAP_AllocClearMemory( heapId , sizeof( MB_CAP_OBJ ) );
  const int resIdx = MB_CAPTURE_GetBbdResIdx( capWork , MCBR_OBJ_GRASS + initWork->type );
  const BOOL flg = TRUE;
  
  objWork->pos = initWork->pos;
  //�������o�̂��߃T�C�Y��2�{�ɂ��Ă�̂Ńf�t�H�͔���
  objWork->objIdx = GFL_BBD_AddObject( bbdSys , 
                                     resIdx ,
                                     FX32_HALF , 
                                     FX32_HALF , 
                                     &objWork->pos ,
                                     31 ,
                                     GFL_BBD_LIGHT_NONE );
  GFL_BBD_SetObjectDrawEnable( bbdSys , objWork->objIdx , &flg );
  //3D�ݒ�̊֌W�Ŕ��]������E�E�E
  GFL_BBD_SetObjectFlipT( bbdSys , objWork->objIdx , &flg );
  
  objWork->isPlayAnime = FALSE;
  objWork->anmCnt = 0;
  objWork->objType = initWork->type;
  objWork->isEnable = TRUE;
  objWork->height = 0;
  
  if( objWork->objType == MCOT_STAR )
  {
    //-1 or +1
    objWork->moveDir = GFUser_GetPublicRand0(2)*2-1;
    if( objWork->moveDir == -1 )
    {
      objWork->pos.x = FX32_CONST(256+32);
    }
    else
    {
      objWork->pos.x = FX32_CONST(-32);
    }
    objWork->pos.y = FX32_CONST( 64+GFUser_GetPublicRand0(64) );
    objWork->pos.z = FX32_CONST( MB_CAP_OBJ_BASE_Z ) + objWork->pos.y;
    objWork->rad[0] = GFUser_GetPublicRand0(0x10000);
    objWork->rad[1] = GFUser_GetPublicRand0(0x10000);
    objWork->rad[2] = GFUser_GetPublicRand0(0x10000);
    objWork->baseY = FX32_CONST( GFUser_GetPublicRand0(40)-20.0f );
  }
  
  return objWork;
}

//--------------------------------------------------------------
//	�I�u�W�F�N�g�J��
//--------------------------------------------------------------
void MB_CAP_OBJ_DeleteObject( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );

  GFL_BBD_RemoveObject( bbdSys , objWork->objIdx );
  
  GFL_HEAP_FreeMemory( objWork );
}

//--------------------------------------------------------------
//	�I�u�W�F�N�g�X�V
//--------------------------------------------------------------
void MB_CAP_OBJ_UpdateObject( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  if( objWork->isPlayAnime == TRUE )
  {
    u16 frame = objWork->anmCnt/MB_CAP_OBJ_ANM_SPD;
    objWork->anmCnt++;
    if( objWork->anmCnt >= MB_CAP_OBJ_ANM_SPD*MB_CAP_OBJ_ANM_FRAME )
    {
      frame = 0;
      objWork->isPlayAnime = FALSE;
    }
    GFL_BBD_SetObjectCelIdx( bbdSys , objWork->objIdx , &frame );
  }
}

//--------------------------------------------------------------
//	�I�u�W�F�N�g�X�V(���p
//--------------------------------------------------------------
void MB_CAP_OBJ_UpdateObject_Star( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  VecFx32 pos;
  u8 i;
  fx32 posY = 0;
  /*
  //�W�����v�ړ�(���������f
  objWork->rad += MB_CAP_OBJ_STAR_ROT;
  if( objWork->rad >= 0x8000 )
  {
    objWork->rad -= 0x8000;
  }

  objWork->pos.x += MB_CAP_OBJ_STAR_SPD*objWork->moveDir;
  
  pos = objWork->pos;

  //�����v�Z
  pos.y -= FX_SinIdx( objWork->rad ) * MB_CAP_OBJ_STAR_HEIGHT;
  */

  static const u16 addRad[3] = {2*0x10000/360,1*0x10000/360,1*0x10000/360};
  static const u16 lenRate[3] = {12,24,0};
  objWork->height = FX32_CONST(8.0f);
  objWork->pos.x += MB_CAP_OBJ_STAR_SPD*objWork->moveDir;
  pos = objWork->pos;
  
  for( i=0;i<2;i++ )
  {
    if( objWork->rad[i] + addRad[i] < 0x10000 )
    {
      objWork->rad[i] += addRad[i];
    }
    else
    {
      objWork->rad[i] = objWork->rad[i] + addRad[i] - 0x10000;
    }
    posY += FX_SinIdx(objWork->rad[i])*lenRate[i];
  }
  objWork->pos.y = FX32_CONST(96)+(posY)+objWork->baseY;
  pos.y = objWork->pos.y+objWork->height;
  
  GFL_BBD_SetObjectTrans( bbdSys , objWork->objIdx , &pos );
  
  {
    u16 frame = objWork->anmCnt/MB_CAP_OBJ_STAR_ANM_SPD;
    objWork->anmCnt++;
    if( objWork->anmCnt >= MB_CAP_OBJ_STAR_ANM_SPD*MB_CAP_OBJ_STAR_ANM_FRAME )
    {
      objWork->anmCnt = 0;
      frame = 0;
    }
    GFL_BBD_SetObjectCelIdx( bbdSys , objWork->objIdx , &frame );
  }

}


//--------------------------------------------------------------
//  �����蔻��쐬
//--------------------------------------------------------------
void MB_CAP_OBJ_GetHitWork( MB_CAP_OBJ *objWork , MB_CAP_HIT_WORK *hitWork )
{
  hitWork->pos = &objWork->pos;
  hitWork->height = objWork->height;
  hitWork->size.x = FX32_CONST(MB_CAP_OBJ_HITSIZE_XY);
  hitWork->size.y = FX32_CONST(MB_CAP_OBJ_HITSIZE_XY);
  hitWork->size.z = FX32_CONST(MB_CAP_OBJ_HITSIZE_Z);
}

//--------------------------------------------------------------
//  �A�j���[�V���������N�G�X�g
//--------------------------------------------------------------
void MB_CAP_OBJ_StartAnime( MB_CAP_OBJ *objWork )
{
  objWork->isPlayAnime = TRUE;
  objWork->anmCnt = 0;
}

//--------------------------------------------------------------
//  �L���E�����ݒ�
//--------------------------------------------------------------
void MB_CAP_OBJ_SetEnable( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork , const BOOL flg )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );

  objWork->isEnable = flg;
  GFL_BBD_SetObjectDrawEnable( bbdSys , objWork->objIdx , &flg );
}
const BOOL MB_CAP_OBJ_GetEnable( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork )
{
  return objWork->isEnable;
}

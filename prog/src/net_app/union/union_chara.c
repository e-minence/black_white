//==============================================================================
/**
 * @file    union_char.c
 * @brief   ���j�I�����[���ł̐l���֘A
 * @author  matsuda
 * @date    2009.07.06(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "union_types.h"
#include "net_app/union/union_main.h"
#include "net_app/union/union_beacon_tool.h"


//==============================================================================
//  �f�[�^
//==============================================================================
///�L�����N�^�̔z�u���W
static const struct{
  u16 x;    //�O���b�h���W�FX
  u16 z;    //�O���b�h���W�FY
}UnionCharPosTbl[] = {
  {10, 20},   //index 0
  {10, 20},   //index 1
  {10, 20},   //index 2
  {10, 20},   //index 3
  {10, 20},   //index 4
  {10, 20},   //index 5
  {10, 20},   //index 6
  {10, 20},   //index 7
  {10, 20},   //index 8
  {10, 20},   //index 9
  {10, 20},   //index 10
  {10, 20},   //index 11
  {10, 20},   //index 12
  {10, 20},   //index 13
  {10, 20},   //index 14
  {10, 20},   //index 15
  {10, 20},   //index 16
  {10, 20},   //index 17
  {10, 20},   //index 18
  {10, 20},   //index 19
  {10, 20},   //index 20
};

///���j�I���L�����N�^�[��MMDL�w�b�_�[�f�[�^
static const MMDL_HEADER UnionChar_MMdlHeader = 
{
  0,
  0,
  MV_DIR_RND,
  0,	///<�C�x���g�^�C�v
  0,	///<�C�x���g�t���O
  0,	///<�C�x���gID
  DIR_DOWN,	///<�w�����
  0,	///<�w��p�����^ 0
  0,	///<�w��p�����^ 1
  0,	///<�w��p�����^ 2
  0,	///<X�����ړ�����
  0,	///<Z�����ړ�����
  0,	///<�O���b�hX
  0,	///<�O���b�hZ
  0,	///<Y�l fx32�^
};



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ���j�I���L�����N�^�[�F�o�^
 *
 * @param   unisys		      
 * @param   gdata		        �Q�[���f�[�^�ւ̃|�C���^
 * @param   trainer_view		������
 * @param   sex		          ����
 * @param   char_index		  �L�����N�^�[Index
 *
 * @retval  MMDL *		      
 */
//==================================================================
MMDL * UNION_CHAR_AddOBJ(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata, u8 trainer_view, u8 sex, u16 char_index)
{
  MMDL *addmdl;
  MMDLSYS *mdlsys;
  MMDL_HEADER head;
  
  mdlsys = GAMEDATA_GetMMdlSys( gdata );
  
  head = UnionChar_MMdlHeader;
  head.id = char_index;
  head.obj_code = UnionView_GetCharaNo(sex, trainer_view);
  head.gx = UnionCharPosTbl[char_index].x;
  head.gz = UnionCharPosTbl[char_index].z;
  
  addmdl = MMDLSYS_AddMMdl(mdlsys, &head, ZONE_ID_PALACETEST);
  return addmdl;
}


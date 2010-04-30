//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_item_judge.c
 *	@brief  �Ӓ�m�@�X�N���v�g�R�}���h
 *	@author	tomoya takahashi
 *	@date		2010.03.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc/arc_def.h"
#include "arc/fieldmap/item_judge.naix"

#include "item/itemsym.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"


#include "fieldmap.h"

#include "scrcmd_item_judge.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
static const u8 sc_ARC_DATA[ SCR_ITEM_JUDGE_OBJTYPE_KOUKO+1 ] = {
  NARC_item_judge_gourmet_dat,
  NARC_item_judge_stone_mania_dat,
  NARC_item_judge_rich_dat,
  NARC_item_judge_kouko_dat,
};


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ITEM�Ӓ���
//=====================================
typedef struct {
  u32 item_no;
  u32 money;
} ITEM_JUDGE_DATA;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  �Ӓ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdItemJudgeCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  HEAPID heapID = FIELDMAP_GetHeapID( fparam->fieldMap );
  u16 item_no = SCRCMD_GetVMWorkValue( core, work );
  u16 obj_type = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_money = SCRCMD_GetVMWork( core, work );
  ITEM_JUDGE_DATA* p_data;
  u32 size;
  int  i;

  GF_ASSERT( obj_type <= SCR_ITEM_JUDGE_OBJTYPE_KOUKO );
  GF_ASSERT( item_no < ITEM_DATA_MAX );

  //TOMOYA_Printf( "item_no %d  obj_type %d\n", item_no, obj_type );

  // �e�[�u���̓ǂݍ���
  p_data = GFL_ARC_UTIL_LoadEx( ARCID_ITEM_JUDGE, sc_ARC_DATA[obj_type], FALSE, GFL_HEAP_LOWID(heapID), &size );
  size /= sizeof(ITEM_JUDGE_DATA);

  // SEARCH
  *ret_money = 0;
  for( i=0; i<size; i++ ){
    //TOMOYA_Printf( "item_no %d == %d\n", p_data[i].item_no, item_no );
    if( p_data[i].item_no == item_no ){
      //TOMOYA_Printf( "money %d\n", p_data[i].money );
      *ret_money = p_data[i].money;
      break;
    }
  }
  GFL_HEAP_FreeMemory( p_data );

	return VMCMD_RESULT_CONTINUE;

}



//----------------------------------------------------------------------------
/**
 *	@brief  �Ӓ肵�Ă��炦��A�C�e���������Ă��邩�`�F�b�N
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdItemJudgeHaveCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA* gdata = SCRCMD_WORK_GetGameData( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  HEAPID heapID = FIELDMAP_GetHeapID( fparam->fieldMap );
  u16 obj_type = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gdata );
  ITEM_JUDGE_DATA* p_data;
  u32 size;
  int  i;

  GF_ASSERT( obj_type <= SCR_ITEM_JUDGE_OBJTYPE_KOUKO );

  //TOMOYA_Printf( "item_no %d  obj_type %d\n", item_no, obj_type );

  // �e�[�u���̓ǂݍ���
  p_data = GFL_ARC_UTIL_LoadEx( ARCID_ITEM_JUDGE, sc_ARC_DATA[obj_type], FALSE, GFL_HEAP_LOWID(heapID), &size );
  size /= sizeof(ITEM_JUDGE_DATA);

  // SEARCH
  *ret_wk = FALSE;
  for( i=0; i<size; i++ ){
    if( MYITEM_CheckItem( myitem, p_data[i].item_no, 1, heapID ) ){
      *ret_wk = TRUE;
      break;
    }
  }
  GFL_HEAP_FreeMemory( p_data );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���z�����[�h�Z�b�g�ɐݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdItemJudgeSetWordSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  HEAPID heapID = FIELDMAP_GetHeapID( fparam->fieldMap );
  u16 item_no = SCRCMD_GetVMWorkValue( core, work );
  u16 obj_type = SCRCMD_GetVMWorkValue( core, work );
  u16 buf_id = SCRCMD_GetVMWorkValue( core, work );
  u16 keta = SCRCMD_GetVMWorkValue( core, work );
  WORDSET*     wordset = SCRIPT_GetWordSet( sc );
  ITEM_JUDGE_DATA* p_data;
  u32 size;
  int  i;

  GF_ASSERT( obj_type <= SCR_ITEM_JUDGE_OBJTYPE_KOUKO );
  GF_ASSERT( item_no < ITEM_DATA_MAX );

  //TOMOYA_Printf( "item_no %d  obj_type %d\n", item_no, obj_type );

  // �e�[�u���̓ǂݍ���
  p_data = GFL_ARC_UTIL_LoadEx( ARCID_ITEM_JUDGE, sc_ARC_DATA[obj_type], FALSE, GFL_HEAP_LOWID(heapID), &size );
  size /= sizeof(ITEM_JUDGE_DATA);

  // SEARCH
  for( i=0; i<size; i++ ){
    //TOMOYA_Printf( "item_no %d == %d\n", p_data[i].item_no, item_no );
    if( p_data[i].item_no == item_no ){
      //TOMOYA_Printf( "money %d\n", p_data[i].money );
      WORDSET_RegisterNumber( wordset, buf_id, p_data[i].money, keta, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      break;
    }
  }
  GFL_HEAP_FreeMemory( p_data );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���z�𑫂�����
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdItemJudgeAddMoney( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  HEAPID heapID = FIELDMAP_GetHeapID( fparam->fieldMap );
  u16 item_no = SCRCMD_GetVMWorkValue( core, work );
  u16 obj_type = SCRCMD_GetVMWorkValue( core, work );
  GAMEDATA*     gdata = SCRCMD_WORK_GetGameData( work );
  PLAYER_WORK* player = GAMEDATA_GetMyPlayerWork( gdata );
  MYSTATUS*  mystatus = &player->mystatus;
  ITEM_JUDGE_DATA* p_data;
  u32 size;
  int  i;

  GF_ASSERT( obj_type <= SCR_ITEM_JUDGE_OBJTYPE_KOUKO );
  GF_ASSERT( item_no < ITEM_DATA_MAX );

  //TOMOYA_Printf( "item_no %d  obj_type %d\n", item_no, obj_type );

  // �e�[�u���̓ǂݍ���
  p_data = GFL_ARC_UTIL_LoadEx( ARCID_ITEM_JUDGE, sc_ARC_DATA[obj_type], FALSE, GFL_HEAP_LOWID(heapID), &size );
  size /= sizeof(ITEM_JUDGE_DATA);

  // SEARCH
  for( i=0; i<size; i++ ){
    //TOMOYA_Printf( "item_no %d == %d\n", p_data[i].item_no, item_no );
    if( p_data[i].item_no == item_no ){
      //TOMOYA_Printf( "money %d\n", p_data[i].money );
      MISC_AddGold( GAMEDATA_GetMiscWork(gdata), p_data[i].money );
      break;
    }
  }
  GFL_HEAP_FreeMemory( p_data );

	return VMCMD_RESULT_CONTINUE;
}



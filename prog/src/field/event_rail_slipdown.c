//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_rail_slipdown.c
 *	@brief  �`�����s�I�����[�h��p�@���[���}�b�v�@���藎���C�x���g
 *	@author	tomoya takahashi
 *	@date		2009.10.16
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "arc/fieldmap/zone_id.h"

#include "field/field_const.h"

#include "event_rail_slipdown.h"

#include "field_nogrid_mapper.h"

#include "fldeff_kemuri.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
enum
{
  EV_RAILSLIPDOWN_SARCH_LOCATION, // ���P�[�V�����r�d�`�q�b�g
  EV_RAILSLIPDOWN_JUMP_START,     // �W�����v
  EV_RAILSLIPDOWN_JUMP_WAIT,
  EV_RAILSLIPDOWN_SLIPDOWN_START, // ���藎��
  EV_RAILSLIPDOWN_SLIPDOWN_WAIT,
  EV_RAILSLIPDOWN_LANDING_START,  // ���n
  EV_RAILSLIPDOWN_LANDING_WAIT,
  EV_RAILSLIPDOWN_END,            // �I��
};


//-------------------------------------
///	���藎������
//  Y��1����邲�Ƃ̂w�y�����̈ړ�����
//=====================================
#define RAILSLIPDOWN_MOVE_XZ  ( FX32_CONST(1.1250f) )

//-------------------------------------
///	1�t���[���ł̂��藎������
//=====================================
#define RAILSLIPDOWN_ONE_DIST ( FX32_CONST(8) )


//-------------------------------------
///	���藎��SE�Đ�
//=====================================
#define RAILSLIPDOWN_SE ( SEQ_SE_FLD_92 )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���藎���@�C�x���g���[�N
//=====================================
typedef struct 
{
  FLDNOGRID_MAPPER* p_mapper;
  FIELD_RAIL_WORK* p_player_rail;
  FIELD_PLAYER* p_player;
  FLDEFF_CTRL*  p_effctrl;

  VecFx32 start_pos;
  VecFx32 end_pos;
  VecFx16 way;
  s32 count;
  u32 count_max;

  RAIL_LOCATION target_location;
  
} EVENT_RAIL_SLIPDOWN_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// ���藎��
static GMEVENT_RESULT EVENT_RailSlipDownMain(GMEVENT * p_event, int *  p_seq, void * p_work);



//----------------------------------------------------------------------------
/**
 *	@brief  �`�����s�I���}�b�v�@���[���}�b�v�@���藎������
 *
 *	@param	gsys        �Q�[���V�X�e��
 *	@param	fieldmap    �t�B�[���h�}�b�v
 *
 *	@return �ݒ肵���C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_RailSlipDown(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
{
  GMEVENT *event;
  EVENT_RAIL_SLIPDOWN_WORK* p_slipdown;

  event = GMEVENT_Create(
    gsys, NULL, EVENT_RailSlipDownMain, sizeof(EVENT_RAIL_SLIPDOWN_WORK));

  // �`�����s�I�����[�h���`�F�b�N
  GF_ASSERT( FIELDMAP_GetZoneID( fieldmap ) == ZONE_ID_D09 );

  // ���[���}�b�v���`�F�b�N
  GF_ASSERT( FIELDMAP_GetMapControlType( fieldmap ) == FLDMAP_CTRLTYPE_NOGRID );

  // ���[�N�̏�����
  p_slipdown = GMEVENT_GetEventWork(event);

  p_slipdown->p_mapper          = FIELDMAP_GetFldNoGridMapper( fieldmap );
  p_slipdown->p_player          = FIELDMAP_GetFieldPlayer( fieldmap );
  p_slipdown->p_player_rail     = FIELD_PLAYER_GetNoGridRailWork( p_slipdown->p_player );
  p_slipdown->p_effctrl         = FIELDMAP_GetFldEffCtrl( fieldmap );

  // ���@�̋�����~
  FIELD_PLAYER_ForceStop( p_slipdown->p_player );
  
  return event;
}









//-----------------------------------------------------------------------------
/**
 *      ���������������֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�v�C�x���g
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_RailSlipDownMain(GMEVENT * p_event, int *  p_seq, void * p_work)
{
  EVENT_RAIL_SLIPDOWN_WORK* p_slipdown;

  //���[�N�̏����� 
  p_slipdown = GMEVENT_GetEventWork(p_event);
  
  switch( *p_seq )
  {
  case EV_RAILSLIPDOWN_SARCH_LOCATION: // ���P�[�V�����r�d�`�q�b�g
    
    // �T�[�`
    // ���̈ʒu����A�p�x�H�H�ŁA�ړ�����
    {
      VecFx16 front_way;
      VecFx32 now_pos;
      fx32 dist_y;
      const FIELD_RAIL_MAN* cp_railMan;
      BOOL result;

      MMDL_Rail_GetFrontWay( FIELD_PLAYER_GetMMdl(p_slipdown->p_player), &front_way );

      TOMOYA_Printf( "front_way x[0x%x] y[0x%x] z[0x%x]\n", front_way.x, front_way.y, front_way.z );
      // ���ʂ̕����ɕύX
      front_way.y = 0;
      VEC_Fx16Normalize( &front_way, &front_way );


      FIELD_RAIL_WORK_GetPos( p_slipdown->p_player_rail, &now_pos );

//      TOMOYA_Printf( "nowpos x[%d] y[%d] z[%d]\n", FX_Whole(now_pos.x), FX_Whole(now_pos.y), FX_Whole(now_pos.z) );

      // ���̕�����,�P�O���b�h�T�C�Y�ړ����A
      // ����
      p_slipdown->start_pos.x =  now_pos.x + FX_Mul( front_way.x, FIELD_CONST_GRID_FX32_SIZE );
      p_slipdown->start_pos.y =  now_pos.y + FX_Mul( front_way.y, FIELD_CONST_GRID_FX32_SIZE );
      p_slipdown->start_pos.z =  now_pos.z + FX_Mul( front_way.z, FIELD_CONST_GRID_FX32_SIZE );

/*      TOMOYA_Printf( "startpos x[%d] y[%d] z[%d]\n", FX_Whole(p_slipdown->start_pos.x), 
        FX_Whole(p_slipdown->start_pos.y), FX_Whole(p_slipdown->start_pos.z) );
//*/

      // �����
      dist_y = p_slipdown->start_pos.y + FX32_CONST(10.0f);
      p_slipdown->end_pos.y = -FX32_CONST(10.0f);
      p_slipdown->end_pos.x = p_slipdown->start_pos.x + FX_Mul( front_way.x, FX_Mul( RAILSLIPDOWN_MOVE_XZ, dist_y ) );
      p_slipdown->end_pos.z = p_slipdown->start_pos.z + FX_Mul( front_way.z, FX_Mul( RAILSLIPDOWN_MOVE_XZ, dist_y ) );

      /*
      TOMOYA_Printf( "endpos x[%d] y[%d] z[%d]\n", FX_Whole(p_slipdown->end_pos.x), 
          FX_Whole(p_slipdown->end_pos.y), FX_Whole(p_slipdown->end_pos.z) );
          //*/

      // ���������r�d�`�q�b�g
      cp_railMan = FLDNOGRID_MAPPER_GetRailMan( p_slipdown->p_mapper );
      result = FIELD_RAIL_MAN_Calc3DVecRailLocation( cp_railMan, 
          &p_slipdown->start_pos, &p_slipdown->end_pos,
          &p_slipdown->target_location, &p_slipdown->end_pos );

      // �����Ȃ��̂ł���΁A�������Ȃ�
      if( !result )
      {
        OS_TPrintf( "slipdown ���̃��[����������܂���B\n" );
        //GF_ASSERT( result );
        return GMEVENT_RES_FINISH;
      }

     
      // �ړ��J�E���g�����߂�
      // �����Ȃǂ����߂�
      {
        VecFx32 way;
        fx32 slipdown_dist;
          
        VEC_Subtract( &p_slipdown->end_pos, &p_slipdown->start_pos, &way );
        slipdown_dist = VEC_Mag( &way );
        VEC_Normalize( &way, &way );
        VEC_Fx16Set( &p_slipdown->way, way.x, way.y, way.z ); 

        slipdown_dist = FX_Div( slipdown_dist, RAILSLIPDOWN_ONE_DIST ); 
        if( FX_Modf( slipdown_dist, &slipdown_dist ) == 0 )
        {
          p_slipdown->count_max = slipdown_dist>>FX32_SHIFT;
        }
        else
        {
          // �����_�؂�グ
          p_slipdown->count_max = slipdown_dist>>FX32_SHIFT;
          p_slipdown->count_max ++;
        }
      }

      p_slipdown->count = 0;

      PMSND_PlaySE( RAILSLIPDOWN_SE );

      (*p_seq) = EV_RAILSLIPDOWN_SLIPDOWN_START;
    }
    
    break;

  case EV_RAILSLIPDOWN_JUMP_START:     // �W�����v
  case EV_RAILSLIPDOWN_JUMP_WAIT:
    break;

  case EV_RAILSLIPDOWN_SLIPDOWN_START: // ���藎��
      (*p_seq) = EV_RAILSLIPDOWN_SLIPDOWN_WAIT;
  case EV_RAILSLIPDOWN_SLIPDOWN_WAIT:
    {
      VecFx32 pos;
      fx32 dist;

      p_slipdown->count ++;
      if( p_slipdown->count >= p_slipdown->count_max )
      {
        VEC_Set( &pos, p_slipdown->end_pos.x, p_slipdown->end_pos.y, p_slipdown->end_pos.z );

        // ����
        (*p_seq) = EV_RAILSLIPDOWN_END;
      }
      else
      {
        dist = FX_Mul( FX32_CONST(p_slipdown->count), RAILSLIPDOWN_ONE_DIST );

        VEC_Set( &pos, 
            p_slipdown->start_pos.x + FX_Mul( p_slipdown->way.x, dist ),
            p_slipdown->start_pos.y + FX_Mul( p_slipdown->way.y, dist ),
            p_slipdown->start_pos.z + FX_Mul( p_slipdown->way.z, dist ) );


        // �y�����o��
        if( (p_slipdown->count % 2) == 0 )
        {
          FLDEFF_KEMURI_SetMMdl( FIELD_PLAYER_GetMMdl( p_slipdown->p_player ), p_slipdown->p_effctrl );
        }
      }

      // ���W�ݒ�
      FIELD_PLAYER_SetPos( p_slipdown->p_player, &pos );    

    }
    break;
    
  case EV_RAILSLIPDOWN_LANDING_START:  // ���n
  case EV_RAILSLIPDOWN_LANDING_WAIT:
    break;
    
  case EV_RAILSLIPDOWN_END:            // �I��

    // �v���C���[ID
    PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID( RAILSLIPDOWN_SE ) );
    
    {
      VecFx32 pos;

      TOMOYA_Printf( "rail index %d\n", p_slipdown->target_location.rail_index );
      TOMOYA_Printf( "line_grid %d\n", p_slipdown->target_location.line_grid );
      TOMOYA_Printf( "width_grid %d\n", p_slipdown->target_location.width_grid );
      
      // ���[�����P�[�V������ݒ�
      FIELD_PLAYER_SetNoGridLocation( p_slipdown->p_player, &p_slipdown->target_location );
      FIELD_PLAYER_GetNoGridPos( p_slipdown->p_player, &pos );
      // ���W���v���C���[�ɐݒ�
      FIELD_PLAYER_SetPos( p_slipdown->p_player, &pos );    
    }
    return GMEVENT_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }

  return GMEVENT_RES_CONTINUE;
}


//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		rail_slipdown.c
 *	@brief  ���[���@����~��
 *	@author	tomoya takahashi
 *	@date		2010.04.12
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "arc/fieldmap/zone_id.h"

#include "field/field_const.h"

#include "rail_slipdown.h"

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
  EV_RAILSLIPDOWN_WAIT_CAMERA_TRACE, // ���P�[�V�����r�d�`�q�b�g
  EV_RAILSLIPDOWN_JUMP_START,     // �W�����v
  EV_RAILSLIPDOWN_JUMP_WAIT,
  EV_RAILSLIPDOWN_SLIPDOWN_START, // ���藎��
  EV_RAILSLIPDOWN_SLIPDOWN_WAIT,
  EV_RAILSLIPDOWN_LANDING_START,  // ���n
  EV_RAILSLIPDOWN_LANDING_WAIT,
  EV_RAILSLIPDOWN_END,            // �I��

  EV_RAILSLIPDOWN_END_LOOP,            // �I��
};


//-------------------------------------
///	���藎������
//  Y��1����邲�Ƃ̂w�y�����̈ړ�����
//=====================================
#define RAILSLIPDOWN_MOVE_XZ  ( FX32_CONST(1.1550f) )
#define RAILSLIPDOWN_MOVE_SIDE_HOSEI  (2)

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
struct _RAIL_SLIPDOWN_WORK
{
  u32 seq;
  FLDNOGRID_MAPPER* p_mapper;
  FIELD_RAIL_WORK* p_mmdl_rail;
  FIELD_PLAYER* p_player;
  MMDL*         p_mmdl;
  FLDEFF_CTRL*  p_effctrl;
  FIELD_CAMERA* p_camera;

  FIELD_RAIL_MAN* p_railman;

  VecFx32 start_pos;
  VecFx32 end_pos;
  VecFx16 way;
  s32 count;
  u32 count_max;

  VecFx32 camera_start;
  VecFx32 target_start;
  VecFx32 camera_move;
  VecFx32 target_move;

  RAIL_LOCATION target_location;

  GFL_TCB* p_tcb;
  
} ;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// ���藎��TCB
static void RailSlipDown_Update( GFL_TCB* p_tcb, void* p_work );



// TCB�œ������B
//----------------------------------------------------------------------------
/**
 *	@brief  ���藎������  TCB�쐬
 */
//-----------------------------------------------------------------------------
RAIL_SLIPDOWN_WORK* RailSlipDown_Create(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, MMDL* mmdl, BOOL jiki)
{
  RAIL_SLIPDOWN_WORK* p_slipdown;
  HEAPID heapID = FIELDMAP_GetHeapID( fieldmap );

  p_slipdown = GFL_HEAP_AllocClearMemory( heapID, sizeof(RAIL_SLIPDOWN_WORK) );

  p_slipdown->p_mapper          = FIELDMAP_GetFldNoGridMapper( fieldmap );
  p_slipdown->p_mmdl            = mmdl;
  p_slipdown->p_mmdl_rail       = MMDL_GetRailWork( mmdl );
  p_slipdown->p_effctrl         = FIELDMAP_GetFldEffCtrl( fieldmap );
  p_slipdown->p_camera          = FIELDMAP_GetFieldCamera( fieldmap );

  if( jiki ){
    p_slipdown->p_player          = FIELDMAP_GetFieldPlayer( fieldmap );

    // ���@�̋�����~
    FIELD_PLAYER_ForceStop( p_slipdown->p_player );
  }else{
    p_slipdown->p_player          = NULL; // p_player == NULL ��NPC
  }

  // ���f���̓��쎩�̂��ꎞ�I�ɒ�~
  MMDL_SetStatusBitHeightGetOFF( p_slipdown->p_mmdl, TRUE );  // �����擾OFF
  MMDL_SetStatusBitAttrGetOFF( p_slipdown->p_mmdl, TRUE );    // �A�g���r���[�g�擾OFF
  MMDL_SetStatusBitFellowHit( p_slipdown->p_mmdl, FALSE );    // ���̃I�u�W�F�Ƃ̔��� OFF
  MMDL_OnMoveBitMoveProcPause( p_slipdown->p_mmdl );          // ����Pause

  // TCB�o�^
  p_slipdown->p_tcb = GFL_TCB_AddTask( FIELDMAP_GetFieldmapTCBSys( fieldmap ), 
      RailSlipDown_Update, p_slipdown, 0 );

  return p_slipdown;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���藎������  �j��
 */
//-----------------------------------------------------------------------------
void RailSlipDown_Delete( RAIL_SLIPDOWN_WORK* p_wk )
{
  GFL_TCB_DeleteTask( p_wk->p_tcb );
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���藎������  �I���܂�
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    �I��
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
BOOL RailSlipDown_IsEnd( const RAIL_SLIPDOWN_WORK* cp_wk )
{
  if( cp_wk->seq == EV_RAILSLIPDOWN_END_LOOP ){
    return TRUE;
  }
  return FALSE;
}










//-----------------------------------------------------------------------------
/**
 *      ���������������֐�
 */
//-----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/**
 *	@brief  ���藎�����C���@�A�b�v�f�[�g
 *
 *	@param	p_tcb
 *	@param	p_work 
 */
//-----------------------------------------------------------------------------
static void RailSlipDown_Update( GFL_TCB* p_tcb, void* p_work )
{
  RAIL_SLIPDOWN_WORK* p_slipdown = p_work;

  switch( p_slipdown->seq )
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

      MMDL_Rail_GetFrontWay( p_slipdown->p_mmdl, &front_way );

      TOMOYA_Printf( "front_way x[0x%x] y[0x%x] z[0x%x]\n", front_way.x, front_way.y, front_way.z );
      // ���ʂ̕����ɕύX
      front_way.y = 0;
      VEC_Fx16Normalize( &front_way, &front_way );


      FIELD_RAIL_WORK_GetPos( p_slipdown->p_mmdl_rail, &now_pos );

      //TOMOYA_Printf( "nowpos x[%d] y[%d] z[%d]\n", (now_pos.x), (now_pos.y), (now_pos.z) );

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
        p_slipdown->seq = EV_RAILSLIPDOWN_END_LOOP;
        return ;
      }

      // �ꏊ�́A�^�[�Q�b�g���P�[�V������width�ň��UP�̂ق��ɂ���
      {
        int i;
        u16 roop_num;
        RAIL_LOCATION location = p_slipdown->target_location;
        int width = FIELD_RAIL_MAN_GetLocationWidthGrid( cp_railMan, &p_slipdown->target_location );
        MAPATTR attr;
        MAPATTR_FLAG attr_flag;

#ifdef BUGFIX_AF_GFBTS2021_20100923
        BOOL location_seting = FALSE;

        //���̈ʒu�̃A�g���r���[�g�`�F�b�N
        attr = FLDNOGRID_MAPPER_GetAttr( p_slipdown->p_mapper, &location );
        attr_flag = MAPATTR_GetAttrFlag( attr );

        if( (attr_flag & MAPATTR_FLAGBIT_HITCH) == 0 ){
          location_seting = TRUE;
        }
#endif //BUGFIX_AF_GFBTS2021_20100923
        
        // �␳
        location.width_grid += RAILSLIPDOWN_MOVE_SIDE_HOSEI;
        if( location.width_grid > width ){
           location.width_grid = width;
        }

        // �����}�C�i�X����
        roop_num = width + location.width_grid;
        
        // �A�g���r���[�g�����邩�`�F�b�N����
        for( i=0; i<roop_num; i++ ){
          
          location.width_grid --;

          attr = FLDNOGRID_MAPPER_GetAttr( p_slipdown->p_mapper, &location );
          attr_flag = MAPATTR_GetAttrFlag( attr );

          if( attr_flag & MAPATTR_FLAGBIT_HITCH ){
            break;
          }

          // 
          p_slipdown->target_location = location;

#ifdef BUGFIX_AF_GFBTS2021_20100923
          location_seting = TRUE;
#endif
        }

#ifdef BUGFIX_AF_GFBTS2021_20100923
        if( location_seting == FALSE ){
          p_slipdown->seq = EV_RAILSLIPDOWN_END_LOOP;
          TOMOYA_Printf( "�������ړ��s�\�Ȃ̂ŁA����~��Ȃ��B\n" );
          return ;
        }
#endif

        // ���߂����P�[�V�����̍��W��ݒ�
        FIELD_RAIL_MAN_GetLocationPosition( cp_railMan, &p_slipdown->target_location, &p_slipdown->end_pos );
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


      if( p_slipdown->p_player ){
        // �J�����g���[�XOFF
        FIELD_CAMERA_StopTraceRequest( p_slipdown->p_camera );

        p_slipdown->seq = EV_RAILSLIPDOWN_WAIT_CAMERA_TRACE;
      }else{

        p_slipdown->seq = EV_RAILSLIPDOWN_SLIPDOWN_START;
      }
    }
    
    break;

  case EV_RAILSLIPDOWN_WAIT_CAMERA_TRACE:// ���P�[�V�����r�d�`�q�b�g
    if( FIELD_CAMERA_CheckTrace( p_slipdown->p_camera ) == FALSE ){

      // �J�����Ǐ]OFF
      // �������Ȃ��Ƃ����Ƃ����^�[�Q�b�g���W���v�Z�ł��Ȃ��B
      FIELD_CAMERA_FreeTarget( p_slipdown->p_camera );

      // ���̃J�������ƁA�ړ���̃J������񂩂�A
      // �J�����̈ړ��l���v�Z�B
      {
        // ���̃J�����A���O���擾
        FIELD_CAMERA_ChangeMode( p_slipdown->p_camera, FIELD_CAMERA_MODE_DIRECT_POS );
        FIELD_CAMERA_GetCameraPos( p_slipdown->p_camera, &p_slipdown->camera_start );
        FIELD_CAMERA_GetTargetPos( p_slipdown->p_camera, &p_slipdown->target_start );
        
        // �ړ���ł̃A���O�����擾
        FLDNOGRID_MAPPER_SetUpLocationCamera( p_slipdown->p_mapper, &p_slipdown->target_location );
        if( FIELD_CAMERA_GetWatchTarget( p_slipdown->p_camera ) != NULL ){
          FIELD_CAMERA_SetTargetPos( p_slipdown->p_camera, &p_slipdown->end_pos );
          FIELD_CAMERA_FreeTarget( p_slipdown->p_camera );
        }
        FIELD_CAMERA_ChangeMode( p_slipdown->p_camera, FIELD_CAMERA_MODE_DIRECT_POS );
        FIELD_CAMERA_GetCameraPos( p_slipdown->p_camera, &p_slipdown->camera_move );
        FIELD_CAMERA_GetTargetPos( p_slipdown->p_camera, &p_slipdown->target_move );

        // �ړ��l�����߂�
        VEC_Subtract( &p_slipdown->camera_move, &p_slipdown->camera_start, &p_slipdown->camera_move );
        VEC_Subtract( &p_slipdown->target_move, &p_slipdown->target_start, &p_slipdown->target_move );
      }

      // Nogrid���̃J���������OFF�ɂ���B
      FLDNOGRID_MAPPER_SetRailCameraActive( p_slipdown->p_mapper, FALSE );

      // �J�������Βl���[�h�ɂ���B
      FIELD_CAMERA_SetMode( p_slipdown->p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

      // �J�����Ǐ]OFF
      FIELD_CAMERA_FreeTarget( p_slipdown->p_camera );

      // �J�������W��ݒ�
      FIELD_CAMERA_SetTargetPos( p_slipdown->p_camera, &p_slipdown->target_start );
      FIELD_CAMERA_SetCameraPos( p_slipdown->p_camera, &p_slipdown->camera_start );

      
      p_slipdown->seq = EV_RAILSLIPDOWN_SLIPDOWN_START;
    }
    break;

  case EV_RAILSLIPDOWN_JUMP_START:     // �W�����v
  case EV_RAILSLIPDOWN_JUMP_WAIT:
    break;

  case EV_RAILSLIPDOWN_SLIPDOWN_START: // ���藎��
      PMSND_PlaySE( RAILSLIPDOWN_SE );
      p_slipdown->seq = EV_RAILSLIPDOWN_SLIPDOWN_WAIT;
      // �e�������B
      MMDL_OnMoveBit( p_slipdown->p_mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
      
  case EV_RAILSLIPDOWN_SLIPDOWN_WAIT:
    {
      VecFx32 pos;
      fx32 dist;

      p_slipdown->count ++;
      if( p_slipdown->count >= p_slipdown->count_max )
      {
        VEC_Set( &pos, p_slipdown->end_pos.x, p_slipdown->end_pos.y, p_slipdown->end_pos.z );

        // ����
        p_slipdown->seq = EV_RAILSLIPDOWN_END;
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
          FLDEFF_KEMURI_SetMMdl( p_slipdown->p_mmdl, p_slipdown->p_effctrl );
        }
      }

      // ���W�ݒ�

      if( p_slipdown->p_player ){
        FIELD_PLAYER_SetPos( p_slipdown->p_player, &pos );    
      }else{
        MMDL_SetVectorPos( p_slipdown->p_mmdl, &pos );
      }

      // �J�����ݒ�
      if( p_slipdown->p_player ){
        VecFx32 camera;
        VecFx32 target;

        VEC_Set( &camera, 
            p_slipdown->camera_start.x + FX_Div( FX_Mul( p_slipdown->camera_move.x, (p_slipdown->count<<FX32_SHIFT) ), p_slipdown->count_max<<FX32_SHIFT ),
            p_slipdown->camera_start.y + FX_Div( FX_Mul( p_slipdown->camera_move.y, (p_slipdown->count<<FX32_SHIFT) ), p_slipdown->count_max<<FX32_SHIFT ),
            p_slipdown->camera_start.z + FX_Div( FX_Mul( p_slipdown->camera_move.z, (p_slipdown->count<<FX32_SHIFT) ), p_slipdown->count_max<<FX32_SHIFT ) );

        VEC_Set( &target, 
            p_slipdown->target_start.x + FX_Div( FX_Mul( p_slipdown->target_move.x, (p_slipdown->count<<FX32_SHIFT) ), p_slipdown->count_max<<FX32_SHIFT ),
            p_slipdown->target_start.y + FX_Div( FX_Mul( p_slipdown->target_move.y, (p_slipdown->count<<FX32_SHIFT) ), p_slipdown->count_max<<FX32_SHIFT ),
            p_slipdown->target_start.z + FX_Div( FX_Mul( p_slipdown->target_move.z, (p_slipdown->count<<FX32_SHIFT) ), p_slipdown->count_max<<FX32_SHIFT ) );


        // �J�������W��ݒ�
        FIELD_CAMERA_SetTargetPos( p_slipdown->p_camera, &target );
        FIELD_CAMERA_SetCameraPos( p_slipdown->p_camera, &camera );
      }

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
      if( p_slipdown->p_player ){
        FIELD_PLAYER_SetNoGridLocation( p_slipdown->p_player, &p_slipdown->target_location );
        FIELD_PLAYER_GetNoGridPos( p_slipdown->p_player, &pos );
        // ���W���v���C���[�ɐݒ�
        FIELD_PLAYER_SetPos( p_slipdown->p_player, &pos );    

        // �J�����A�b�v�f�[�g�J�n
        FLDNOGRID_MAPPER_SetRailCameraActive( p_slipdown->p_mapper, TRUE );
        FLDNOGRID_MAPPER_UpdateCamera( p_slipdown->p_mapper );
      }
      else
      {
        MMDL_SetRailLocation( p_slipdown->p_mmdl, &p_slipdown->target_location );
      }
    }

    // ���앜�A
    MMDL_OffMoveBit( p_slipdown->p_mmdl, MMDL_MOVEBIT_SHADOW_VANISH ); // �e���A
    MMDL_SetStatusBitHeightGetOFF( p_slipdown->p_mmdl, FALSE );  // �����擾OFF
    MMDL_SetStatusBitAttrGetOFF( p_slipdown->p_mmdl, FALSE );    // �A�g���r���[�g�擾OFF
    MMDL_SetStatusBitFellowHit( p_slipdown->p_mmdl, TRUE );    // ���̃I�u�W�F�Ƃ̔��� ON
    MMDL_OffMoveBitMoveProcPause( p_slipdown->p_mmdl );          // ����Pause



    p_slipdown->seq = EV_RAILSLIPDOWN_END_LOOP;
    break;

  case EV_RAILSLIPDOWN_END_LOOP:            // �I��
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}


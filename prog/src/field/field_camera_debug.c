//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		prog\src\field\field_camera_debug.c
 *	@brief
 *	@author	 
 *	@date		2010.04.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include <calctool.h>

#include "field/field_const.h"

#include "field_g3d_mapper.h"
#include "field_common.h"
#include "field_camera_debug.h"
	
#include "arc_def.h"


// �f�o�b�N�@�\���Ⴄ�\�[�X�ɂ��邽�߂ɁA��`��ʃw�b�_�[�Ɉڂ��܂����B
#include "field_camera_local.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


#ifdef  PM_DEBUG
#include "test/camera_adjust_view.h"
static fx32	fldWipeScale = 0;
//------------------------------------------------------------------
//  �f�o�b�O�p�F����ʑ���Ƃ̃o�C���h
//------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_BindSubScreen(FIELD_CAMERA * camera, void * param, FIELD_CAMERA_DEBUG_BIND_TYPE type, HEAPID heapID)
{ 
  GFL_CAMADJUST * gflCamAdjust = param;

  // ���C�v�\��������
  camera->p_debug_wipe = FLD_WIPEOBJ_Create( heapID );

  // �t�@�[���W���擾
  GFL_G3D_CAMERA_GetFar( camera->g3Dcamera, &camera->debug_far );

  camera->debug_subscreen_type = type;
  camera->debug_trace_off = TRUE;
  if( type == FIELD_CAMERA_DEBUG_BIND_CAMERA_POS )
  {
    camera->debug_save_camera_mode = camera->mode;
	  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    GFL_CAMADJUST_SetCameraParam(	gflCamAdjust,
																	&camera->angle_yaw, 
																	&camera->angle_pitch, 
																	&camera->angle_len, 
																	&camera->fovy, 
																	&camera->debug_far);  
    GFL_CAMADJUST_SetWipeParam(		gflCamAdjust, &fldWipeScale );
  }
  else if( type == FIELD_CAMERA_DEBUG_BIND_TARGET_POS )
  {
    camera->debug_save_camera_mode = camera->mode;
	  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_TARGET_POS );
    camera->debug_target_yaw    = camera->angle_yaw;
    camera->debug_target_pitch  = camera->angle_pitch;
    camera->debug_target_len    = camera->angle_len;
    GFL_CAMADJUST_SetCameraParam(	gflCamAdjust,
																	&camera->debug_target_yaw, 
																	&camera->debug_target_pitch, 
																	&camera->debug_target_len, 
																	&camera->fovy, 
																	&camera->debug_far);  
    GFL_CAMADJUST_SetWipeParam(		gflCamAdjust, &fldWipeScale );
  }
}

void FIELD_CAMERA_DEBUG_ReleaseSubScreen(FIELD_CAMERA * camera)
{ 

  if( camera->debug_subscreen_type == FIELD_CAMERA_DEBUG_BIND_TARGET_POS ){

    OS_TPrintf( "DEBUG Camera Target x[0x%x] y[0x%x] z[0x%x]\n", camera->debug_target.x, camera->debug_target.y, camera->debug_target.z  );
    OS_TPrintf( "DEBUG Camera Pos x[0x%x] y[0x%x] z[0x%x]\n", camera->camPos.x, camera->camPos.y, camera->camPos.z  );

    VEC_Subtract( &camera->debug_target, &camera->target, &camera->target_offset );
    OS_TPrintf( "DEBUG Scroll Camera Target Offset x[0x%x] y[0x%x] z[0x%x]\n", camera->target_offset.x, camera->target_offset.y, camera->target_offset.z  );
  }

  camera->debug_subscreen_type = FIELD_CAMERA_DEBUG_BIND_NONE;
  camera->debug_trace_off = FALSE;

  FIELD_CAMERA_ChangeMode( camera, camera->debug_save_camera_mode );
  if(camera->debug_save_camera_mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS)
  {
    FIELD_CAMERA_BindDefaultTarget( camera );
  }

  // ���C�v�\���j��
  FLD_WIPEOBJ_Delete( camera->p_debug_wipe );
  camera->p_debug_wipe = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���삵�Ă���^�[�Q�b�g���擾
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_GetBindSubScreenTarget( const FIELD_CAMERA * camera, VecFx32* p_target )
{
	*p_target = camera->debug_target;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �J�����f�o�b�N�RD�`��
 *
 *	@param	camera
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_Draw( const FIELD_CAMERA* camera)
{ 
  if(camera->p_debug_wipe)
  {
    FLD_WIPEOBJ_Main( camera->p_debug_wipe, fldWipeScale, 20 );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�N�R���g���[���@���Ǘ�
 */
//-----------------------------------------------------------------------------
static void DEBUG_ControlWork( FIELD_CAMERA* camera )
{
  // �����field_camera���ێ������܂܁A�J�����̕��s�ړ��Ȃǂ��s�����߂ɁA
  // ���ۂɁAgflib�̃J�����ɓn�������J�������W����A
  // FIELD_CAMERA_MODE_CALC_CAMERA_POS�p�̃A���O����A
  // �^�[�Q�b�g�I�t�Z�b�g������o���B
  
  // �A���O������f�o�b�N�J�������W���v�Z
  FIELD_CAMERA_LOCAL_calcAnglePos( &camera->debug_target, &camera->debug_camera, camera->debug_target_yaw, camera->debug_target_pitch, camera->debug_target_len );
  
  // �f�o�b�N�^�[�Q�b�g
  // �f�o�b�N�J�����̈ʒu����A
  // �^�[�Q�b�g�I�t�Z�b�g��
  // �A���O�������߂�
  {
    VecFx32 move_camera;
    
    // �^�[�Q�b�g�I�t�Z�b�g
    if( camera->watch_target != NULL )
    {
      VEC_Subtract( &camera->debug_target, &camera->target_write, &camera->target_offset );
    }
    else
    {
      
#ifdef DEBUG_CAMERA_ALLCONTROL_MODE_2
      VEC_Subtract( &camera->debug_target, &camera->target_offset, &camera->target );
#else
      VEC_Set( &camera->target, camera->debug_target.x, camera->debug_target.y, camera->debug_target.z );
      VEC_Set( &camera->target_offset, 0,0,0 );
#endif
    }

    // �J�����A���O��
    VEC_Subtract( &camera->debug_camera, &camera->target_write, &move_camera ); 
    FIELD_CAMERA_LOCAL_modeChange_CalcVecAngel( &move_camera, &camera->angle_pitch, &camera->angle_yaw, &camera->angle_len );
  }
  
  // �J��������ݒ�
  FIELD_CAMERA_LOCAL_ControlParameter( camera, 0 );
}




//-----------------------------------------------------------------------------
/**
 *
 */
//-----------------------------------------------------------------------------
#define CAMERA_DEBUG_ANGLE_MOVE  (64)
#define CAMERA_DEBUG_PAERS_MOVE  (64)
#define CAMERA_DEBUG_NEARFAR_MOVE  (FX32_CONST(2))
#define CAMERA_DEBUG_POS_MOVE  (FX32_ONE)
#define CAMERA_DEBUG_LEN_MOVE  (FX32_ONE)
enum
{
  CAMERA_DEBUG_BUFFER_MODE_ZBUFF_MANUAL,
  CAMERA_DEBUG_BUFFER_MODE_WBUFF_MANUAL,
  CAMERA_DEBUG_BUFFER_MODE_ZBUFF_AUTO,
  CAMERA_DEBUG_BUFFER_MODE_WBUFF_AUTO,

  CAMERA_DEBUG_BUFFER_MODE_MAX,
};
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�N�R���g���[��������
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_InitControl( FIELD_CAMERA* camera, HEAPID heapID )
{
  GF_ASSERT( !camera->p_debug_wordset );
  GF_ASSERT( !camera->p_debug_msgdata );

  // trace OFF
  camera->debug_trace_off = TRUE;

  // ���[�h�Z�b�g�쐬
  camera->p_debug_wordset = WORDSET_Create( heapID );
  camera->p_debug_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_tomoya_dat, heapID );

  camera->p_debug_strbuff   = GFL_STR_CreateBuffer( 512, heapID );
  camera->p_debug_strbuff_tmp = GFL_STR_CreateBuffer( 512, heapID );

  // �t�H���g�f�[�^
  camera->p_debug_font = GFL_FONT_Create(
    ARCID_FONT, NARC_font_large_gftr,
    GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

  GFL_UI_KEY_SetRepeatSpeed( 1,8 );

  // ���̐ݒ�J�����p�����[�^���擾
  {
    VecFx32 move_camera;
    GFL_G3D_CAMERA_GetTarget( camera->g3Dcamera, &camera->debug_target );
    GFL_G3D_CAMERA_GetPos( camera->g3Dcamera, &camera->debug_camera );

    // �A���O��������
    VEC_Subtract( &camera->debug_camera, &camera->debug_target, &move_camera );
    FIELD_CAMERA_LOCAL_modeChange_CalcVecAngel( &move_camera, &camera->debug_target_pitch, &camera->debug_target_yaw, &camera->debug_target_len );
  }
  
  DEBUG_ControlWork( camera );

}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�N�R���g���[���j��
 *
 *	@param	camera 
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_ExitControl( FIELD_CAMERA* camera )
{
  // �t�H���g�f�[�^
  GFL_FONT_Delete( camera->p_debug_font );
  camera->p_debug_font = NULL;


  GFL_MSG_Delete( camera->p_debug_msgdata );
  camera->p_debug_msgdata = NULL;

  WORDSET_Delete( camera->p_debug_wordset );
  camera->p_debug_wordset = NULL;

  GFL_STR_DeleteBuffer( camera->p_debug_strbuff );
  camera->p_debug_strbuff = NULL;
  GFL_STR_DeleteBuffer( camera->p_debug_strbuff_tmp );
  camera->p_debug_strbuff_tmp = NULL;

  GFL_UI_KEY_SetRepeatSpeed( 8,15 );

  // trace ON
  camera->debug_trace_off = FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �A���O������̗L���`�F�b�N
 *
 *	@param	pitch 
 *
 *	@retval TRUE    �n�j
 *	@retval FALSE   �m�f
 */
//-----------------------------------------------------------------------------
static BOOL DEBUG_is_PitchAngleMove( u16 pitch )
{
  if( (((pitch - CAMERA_DEBUG_ANGLE_MOVE) > GFL_CALC_GET_ROTA_NUM(270)) && ((pitch - CAMERA_DEBUG_ANGLE_MOVE) < 0x10000)) ||
      (((pitch - CAMERA_DEBUG_ANGLE_MOVE) >= GFL_CALC_GET_ROTA_NUM(0)) && ((pitch - CAMERA_DEBUG_ANGLE_MOVE) < GFL_CALC_GET_ROTA_NUM(90))) )
  {
    return TRUE;
  }
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�J����  �R���g���[���ڍ�
 *
 *	@param	camera      �J����  
 *	@param	trg         �L�[���
 *	@param	cont        
 *	@param	repeat      
 *
 *  NONE�F�^�[�Q�b�g�A�J�������s�ړ�
 *	B�F�J�������]�i�^�[�Q�b�g���W���ς��j
 *	Y�F�J�������]�i�J�������W���ς��j
 *	A�F�p�[�X����
 *	X�F�^�[�Q�b�g�J�����o�C���h��ON�EOFF
 *	
 *	START�F�o�b�t�@�����O���[�h�ύX
 */
//-----------------------------------------------------------------------------
BOOL FIELD_CAMERA_DEBUG_Control( FIELD_CAMERA* camera, int trg, int cont, int repeat )
{
  BOOL ret = FALSE;
  
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  // �J�������]�i�^�[�Q�b�g���W�ύX�@�J�����I�t�Z�b�g�𓮂����j
  if( cont & PAD_BUTTON_B )
  {
    u16 pitch, yaw;

    // �^�[�Q�b�g�𓮂������߂̃A���O���ɕϊ�
    pitch = camera->debug_target_pitch;
    yaw   = camera->debug_target_yaw;
     
    if( repeat & PAD_KEY_UP )
    {
      if( DEBUG_is_PitchAngleMove(pitch - CAMERA_DEBUG_ANGLE_MOVE) )
      {
        pitch -= CAMERA_DEBUG_ANGLE_MOVE;
        ret = TRUE;
      }
    }
    else if( repeat & PAD_KEY_DOWN )
    {
      if( DEBUG_is_PitchAngleMove(pitch + CAMERA_DEBUG_ANGLE_MOVE) )
      {
        pitch += CAMERA_DEBUG_ANGLE_MOVE;
        ret = TRUE;
      }
    }

    if( repeat & PAD_KEY_LEFT )
    {
      yaw += CAMERA_DEBUG_ANGLE_MOVE;
      ret = TRUE;
    }
    else if( repeat & PAD_KEY_RIGHT )
    {
      yaw -= CAMERA_DEBUG_ANGLE_MOVE;
      ret = TRUE;
    }

    if( repeat & PAD_BUTTON_L )
    {
      camera->debug_target_len -= CAMERA_DEBUG_LEN_MOVE;
      ret = TRUE;
    }
    else if( repeat & PAD_BUTTON_R )
    {
      camera->debug_target_len += CAMERA_DEBUG_LEN_MOVE;
      ret = TRUE;
    }


    // �ύX�n��ۑ�
    camera->debug_target_pitch  = pitch;
    camera->debug_target_yaw    = yaw;

    // �ύX���W�����߂�
    if( ret )
    {
      pitch += 0x8000;  // �����𔽓]
      yaw   += 0x8000;
      
      // �^�[�Q�b�g�̍��W�����߂�
      FIELD_CAMERA_LOCAL_calcAnglePos( &camera->debug_camera, &camera->debug_target, yaw, pitch, camera->debug_target_len );
    }
  }
  // �J�������]�i�J�������W�ύX�j
  else if( cont & PAD_BUTTON_Y )
  {

    if( repeat & PAD_KEY_UP )
    {
      if( DEBUG_is_PitchAngleMove(camera->debug_target_pitch + CAMERA_DEBUG_ANGLE_MOVE) )
      {
        camera->debug_target_pitch += CAMERA_DEBUG_ANGLE_MOVE;
        ret = TRUE;
      }
    }
    else if( repeat & PAD_KEY_DOWN )
    {
      if( DEBUG_is_PitchAngleMove(camera->angle_pitch - CAMERA_DEBUG_ANGLE_MOVE) )
      {
        camera->debug_target_pitch -= CAMERA_DEBUG_ANGLE_MOVE;
        ret = TRUE;
      }
    }

    if( repeat & PAD_KEY_LEFT )
    {
      camera->debug_target_yaw -= CAMERA_DEBUG_ANGLE_MOVE;
      ret = TRUE;
    }
    else if( repeat & PAD_KEY_RIGHT )
    {
      camera->debug_target_yaw += CAMERA_DEBUG_ANGLE_MOVE;
      ret = TRUE;
    }

  }
  // �p�[�X����
  else if( cont & PAD_BUTTON_A )
  {
    if( repeat & PAD_KEY_UP )
    {
      if( (camera->fovy + CAMERA_DEBUG_PAERS_MOVE) < GFL_CALC_GET_ROTA_NUM(90) )
      {
        camera->fovy += CAMERA_DEBUG_PAERS_MOVE;
        ret = TRUE;
      }
    }
    else if( repeat & PAD_KEY_DOWN )
    {
      if( (camera->fovy - CAMERA_DEBUG_PAERS_MOVE) > 0 )
      {
        camera->fovy -= CAMERA_DEBUG_PAERS_MOVE;
        ret = TRUE;
      }
    }
    // ���E�ł́A�h�b�g����̂Ȃ����W�����߂� 
    else if( (repeat & PAD_KEY_LEFT) || (repeat & PAD_KEY_RIGHT) )
    {
      fx32 dist;

      if( repeat & PAD_KEY_LEFT )
      {
        if( (camera->fovy + CAMERA_DEBUG_PAERS_MOVE) < GFL_CALC_GET_ROTA_NUM(90) )
        {
          camera->fovy += CAMERA_DEBUG_PAERS_MOVE;
          ret = TRUE;
        }
      }
      else if( repeat & PAD_KEY_RIGHT )
      {  
        if( (camera->fovy - CAMERA_DEBUG_PAERS_MOVE) > 0 )
        {
          camera->fovy -= CAMERA_DEBUG_PAERS_MOVE;
          ret = TRUE;
        }
      }

      //�J�����̒��S�ɂ���2D�摜���A���ꂸ�ɕ\���ł��鋗�������߂�
      //�Ԃꂸ�ɕ\�����邽�߂ɂ́A1Dot�@���@1Unit�Ōv�Z����΂悢
      //�J��������ʒ��S�����߂�Ƃ��A��ʔ����̃T�C�Y�́A96Dot
      //�\�����������A�J�����̃p�[�X���l���A96Dot�@���@96Unit�ɂȂ鋗���ɂ���΂悢
      dist = FX_Div( FX_Mul( FX_CosIdx( camera->fovy ), FX_F32_TO_FX32(96) ),	
                 FX_SinIdx( camera->fovy ));					

      camera->debug_target_len = dist; 
    }

    if( ret )
    {
      GFL_G3D_CAMERA_SetfovySin( camera->g3Dcamera, FX_SinIdx( camera->fovy ) );
      GFL_G3D_CAMERA_SetfovyCos( camera->g3Dcamera, FX_CosIdx( camera->fovy ) );
    }
  }
  // Near Far����
  // �J�����G���A�@�n�m�E�n�e�e
  else if( cont & PAD_BUTTON_DEBUG )
  {
    fx32 near, far;

    GFL_G3D_CAMERA_GetNear(camera->g3Dcamera, &near);
    GFL_G3D_CAMERA_GetFar(camera->g3Dcamera, &far);

    if( repeat & PAD_KEY_UP )
    {
      if( (near + CAMERA_DEBUG_NEARFAR_MOVE) < far )
      {
        near += CAMERA_DEBUG_NEARFAR_MOVE;
        ret = TRUE;
      }
    }
    else if( repeat & PAD_KEY_DOWN )
    {
      if( (near - CAMERA_DEBUG_NEARFAR_MOVE) > 0 )
      {
        near -= CAMERA_DEBUG_NEARFAR_MOVE;
        ret = TRUE;
      }
    }

    if( repeat & PAD_KEY_LEFT )
    {
      if( (far - CAMERA_DEBUG_NEARFAR_MOVE) > near )
      {
        far -= CAMERA_DEBUG_NEARFAR_MOVE;
        ret = TRUE;
      }
    }
    else if( repeat & PAD_KEY_RIGHT )
    {
      far += CAMERA_DEBUG_NEARFAR_MOVE;
      ret = TRUE;
    }

    if( ret )
    {
      GFL_G3D_CAMERA_SetNear(camera->g3Dcamera, &near);
      GFL_G3D_CAMERA_SetFar(camera->g3Dcamera, &far);
    }

    if( trg & PAD_BUTTON_X ){
      if( camera->camera_area_active ){
        camera->camera_area_active = FALSE;
      }else{
        camera->camera_area_active = TRUE;
      }
      ret = TRUE;
    }
  }
  // ���s�ړ�
  else 
  {
    // �^�[�Q�b�g�I�t�Z�b�g��ύX
    if( repeat & PAD_KEY_UP )
    {
      camera->debug_target.z -= CAMERA_DEBUG_POS_MOVE;
      ret = TRUE;
    }
    else if( repeat & PAD_KEY_DOWN )
    {
      camera->debug_target.z += CAMERA_DEBUG_POS_MOVE;
      ret = TRUE;
    }
    if( repeat & PAD_KEY_LEFT )
    {
      camera->debug_target.x -= CAMERA_DEBUG_POS_MOVE;
      ret = TRUE;
    }
    else if( repeat & PAD_KEY_RIGHT )
    {
      camera->debug_target.x += CAMERA_DEBUG_POS_MOVE;
      ret = TRUE;
    }
    if( repeat & PAD_BUTTON_L )
    {
      camera->debug_target.y -= CAMERA_DEBUG_POS_MOVE;
      ret = TRUE;
    }
    else if( repeat & PAD_BUTTON_R )
    {
      camera->debug_target.y += CAMERA_DEBUG_POS_MOVE;
      ret = TRUE;
    }

    //  �^�[�Q�b�g�̃o�C���hOn�EOff
    if( trg & PAD_BUTTON_X )
    {
      if( camera->watch_target )
      {
        FIELD_CAMERA_FreeTarget( camera );
      }
      else
      {
        FIELD_CAMERA_BindDefaultTarget( camera );
      }
      ret = TRUE;
    }

  }
   

  // �o�b�t�@�����O���[�h�ύX
  if( trg & PAD_BUTTON_START )
  {
    camera->debug_save_buffer_mode = (camera->debug_save_buffer_mode + 1) % CAMERA_DEBUG_BUFFER_MODE_MAX;
    
    switch( camera->debug_save_buffer_mode )
    {
    case CAMERA_DEBUG_BUFFER_MODE_ZBUFF_MANUAL:
      GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
      ret = TRUE;
      break;
    case CAMERA_DEBUG_BUFFER_MODE_WBUFF_MANUAL:
      GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
      ret = TRUE;
      break;
    case CAMERA_DEBUG_BUFFER_MODE_ZBUFF_AUTO:
      GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
      ret = TRUE;
      break;
    case CAMERA_DEBUG_BUFFER_MODE_WBUFF_AUTO:
      GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
      ret = TRUE;
      break;
    }
  }

  // �ĕ\��
  if( trg & PAD_BUTTON_DEBUG )
  {
    ret = TRUE;
  }

  DEBUG_ControlWork( camera );

  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J����������������
 *
 *	@param	camera    �J����
 *	@param	p_win     �E�B���h�E
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_DrawInfo( FIELD_CAMERA* camera, GFL_BMPWIN* p_win, fx32 map_size_x, fx32 map_size_z )
{
  //�N���A
  
  // �J�����A���O��
  {
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 0, camera->angle_pitch, 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 1, camera->angle_yaw, 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 2, FX_Whole(camera->angle_len), 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

    GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_ANGLE, camera->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 0, camera->p_debug_strbuff, camera->p_debug_font );
  }

  // ���W
  {
    VecFx32 camerapos, target;

    GFL_G3D_CAMERA_GetTarget( camera->g3Dcamera, &target );
    GFL_G3D_CAMERA_GetPos( camera->g3Dcamera, &camerapos );
    
    // �J����
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 0, FX_Whole(camerapos.x), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 1, FX_Whole(camerapos.y), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 2, FX_Whole(camerapos.z), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 3, FX_Whole(target.x), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 4, FX_Whole(target.y), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 5, FX_Whole(target.z), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

    GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_, camera->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 32, camera->p_debug_strbuff, camera->p_debug_font );
  }

  // �^�[�Q�b�g�I�t�Z�b�g
  {
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 0, FX_Whole(camera->target_offset.x), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 1, FX_Whole(camera->target_offset.y), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 2, FX_Whole(camera->target_offset.z), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

    GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_OFFSET, camera->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 64, camera->p_debug_strbuff, camera->p_debug_font );
  }

  // �p�[�X
  // Near Far
  {
    fx32 near, far;
    
    GFL_G3D_CAMERA_GetNear(camera->g3Dcamera, &near);
    GFL_G3D_CAMERA_GetFar(camera->g3Dcamera, &far);

    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 0, camera->fovy, 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 1, FX_Whole(near), 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterHexNumber( camera->p_debug_wordset, 2, FX_Whole(far), 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

    GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_PARS, camera->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 96, camera->p_debug_strbuff, camera->p_debug_font );
  }
  

  // �o�b�t�@�����O
  {
    GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_BUFF + camera->debug_save_buffer_mode, camera->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 112, camera->p_debug_strbuff, camera->p_debug_font );
  }

  // ��l���ǔ�
  {
    if( camera->watch_target )
    {
      GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_BIND, camera->p_debug_strbuff_tmp );
    }
    else
    {
      GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_BIND0002, camera->p_debug_strbuff_tmp );
    }

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 128, camera->p_debug_strbuff, camera->p_debug_font );
  }

  // �J�����G���A�̂n�m�E�n�e�e
  {
    if( camera->camera_area_active )
    {
      GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_BIND0003, camera->p_debug_strbuff_tmp );
    }
    else
    {
      GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_BIND0004, camera->p_debug_strbuff_tmp );
    }

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 128, 128, camera->p_debug_strbuff, camera->p_debug_font );
  }

  // Maya Camera Anime Trans
  // �����_�@��2�ʂ܂ŕ\��
  {
    f32 f_x, f_y, f_z;
    s32 x, y, z;
    u16 rot_x, rot_y, rot_z;
    fx32 xz_dist;
    VecFx32 camerapos, target, xz_vec;

    GFL_G3D_CAMERA_GetTarget( camera->g3Dcamera, &target );
    GFL_G3D_CAMERA_GetPos( camera->g3Dcamera, &camerapos );

    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG )
    {
      fx32 half_x, half_z;
      half_x = FX_Div( map_size_x, 2<<FX32_SHIFT );
      half_z = FX_Div( map_size_z, 2<<FX32_SHIFT );
      OS_Printf( "\n--------------Maya Param-------------\nMaya Trans x[%f] y[%f] z[%f]\n", FX_FX32_TO_F32(camerapos.x - half_x), FX_FX32_TO_F32(camerapos.y), FX_FX32_TO_F32(camerapos.z - half_z) );
    }
    
    // �J�������W��\��
    // �����_�ȉ�����
    x = FX_Whole( camerapos.x ) - (FX_Whole(map_size_x)/2);
    y = FX_Whole( camerapos.y );
    z = FX_Whole( camerapos.z ) - (FX_Whole(map_size_z)/2);

    f_x = FX_FX32_TO_F32(camerapos.x - FX_Floor(camerapos.x));
    f_y = FX_FX32_TO_F32(camerapos.y - FX_Floor(camerapos.y));
    f_z = FX_FX32_TO_F32(camerapos.z - FX_Floor(camerapos.z));

    /// ������
    WORDSET_RegisterNumber( camera->p_debug_wordset, 0, x, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 1, y, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 2, z, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

    /// ������
    x = (s32)(f_x * 100.0f);
    y = (s32)(f_y * 100.0f);
    z = (s32)(f_z * 100.0f);
    WORDSET_RegisterNumber( camera->p_debug_wordset, 3, x, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 4, y, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 5, z, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

    GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_MAYA_TRANS, camera->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 144, camera->p_debug_strbuff, camera->p_debug_font );

    // �J��������^�[�Q�b�g�ւ̃x�N�g���𐶐�
    VEC_Subtract( &target, &camerapos, &target );

    // XZ���ʂ̋��������߂�
    VEC_Set( &xz_vec, target.x, 0, target.z );
    xz_dist = VEC_Mag( &xz_vec );

    // �p�x�����߂�
    // XZ���ʂ̊p�x
    // ���ʂ�Y�����̊p�x
    rot_y = FX_Atan2Idx( -target.x, -target.z );
    rot_x = FX_Atan2Idx( target.y, xz_dist );
    rot_z = 0;

    f_x = (f32)(rot_x / (f32)0x10000) * 360.0f;
    f_y = (f32)(rot_y / (f32)0x10000) * 360.0f;

    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG )
    {
      OS_Printf( "Maya Rotate x[%f] y[%f]\n", f_x, f_y );
    }
 
    x = (s32)f_x;
    y = (s32)f_y;
    z = 0;
    
    /// ������
    WORDSET_RegisterNumber( camera->p_debug_wordset, 0, x, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 1, y, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 2, z, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

    /// ������
    f_x -= x;
    f_y -= y;
    x = (s32)(f_x * 100.0f);
    y = (s32)(f_y * 100.0f);
    WORDSET_RegisterNumber( camera->p_debug_wordset, 3, x, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 4, y, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( camera->p_debug_wordset, 5, z, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

    GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_MAYA_ROTATE, camera->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 160, camera->p_debug_strbuff, camera->p_debug_font );
  }

  GFL_BMPWIN_TransVramCharacter( p_win );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J����������@����������
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_DrawControlHelp( FIELD_CAMERA* camera, GFL_BMPWIN* p_win )
{
  GFL_MSG_GetString( camera->p_debug_msgdata, CAMERA_DELICATE_HELP, camera->p_debug_strbuff_tmp );

  WORDSET_ExpandStr( camera->p_debug_wordset, camera->p_debug_strbuff, camera->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 0, camera->p_debug_strbuff, camera->p_debug_font );

  GFL_BMPWIN_TransVramCharacter( p_win );
}



#endif  //PM_DEBUG

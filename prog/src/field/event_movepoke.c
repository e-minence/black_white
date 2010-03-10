//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_movepoke.c
 *	@brief  �ړ��|�P�����@�C�x���g
 *	@author	tomoya takahashi
 *	@date		2010.03.09
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "system/ica_anime.h"

#include "field/field_const.h"

#include "arc/arc_def.h"
#include "arc/event_movepoke.naix"


#include "fieldmap.h"
#include "fldmmdl.h"
#include "fieldmap_func.h"

#include "event_movepoke.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�A�j���[�V�����f�[�^�o�b�t�@
//=====================================
enum
{
  ANIME_STATUS_NONE,
  ANIME_STATUS_DOING,
};


//-------------------------------------
///	�A�j���[�V�����f�[�^�o�b�t�@
//=====================================
#define EV_MOVEPOKE_ANIME_INTERVAL  (10)
#define EV_MOVEPOKE_ANIME_BUF ( 0x180 )




//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�ړ��|�P�����@���샏�[�N
//=====================================
struct _EV_MOVEPOKE_WORK 
{
  MMDL* p_poke;
  u8 animebuf[ EV_MOVEPOKE_ANIME_BUF ];
  ICA_ANIME* p_anime;
  FLDMAPFUNC_WORK * p_task;

  VecFx32 offset;

  u8 anime_req;
  u8 anime_no;
  u8 anime_status;
  u8 pad;

  HEAPID heapID;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �ʒu�Ɖ�]��ݒ�
static void MOVEPOKE_SetUpMMdl( const ICA_ANIME* cp_anime, MMDL* p_mmdl, const VecFx32* cp_pos );



// �^�X�N�֐�
static void MOVEPOKE_Update( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void MOVEPOKE_Draw( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work );

//-------------------------------------
///	�f�[�^
//=====================================
static const FLDMAPFUNC_DATA sc_MAPFUNC_DATA = {
  0,
  sizeof(EV_MOVEPOKE_WORK),
  NULL,
  NULL,
  MOVEPOKE_Update,
  MOVEPOKE_Draw,
};



//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P����  ���[�N�N���G�C�g
 *
 *	@param	p_fieldmap    �t�B�[���h�}�b�v
 *	@param  id            ����ID
 *	@param  cp_pos        ��l���ʒu
 *	@param	heapID        �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
EV_MOVEPOKE_WORK* EVENT_MOVEPOKE_Create( FIELDMAP_WORK* p_fieldmap, u16 id, const VecFx32* cp_pos, HEAPID heapID )
{
  EV_MOVEPOKE_WORK* p_wk;
  MMDLSYS* p_fos = FIELDMAP_GetMMdlSys( p_fieldmap );
  FLDMAPFUNC_SYS * p_funcsys = FIELDMAP_GetFldmapFuncSys( p_fieldmap );
  FLDMAPFUNC_WORK* p_funcwk;

  
  p_funcwk = FLDMAPFUNC_Create( GFL_OVERLAY_BLANK_ID, p_funcsys, &sc_MAPFUNC_DATA );
  p_wk     = FLDMAPFUNC_GetFreeWork( p_funcwk );
  p_wk->p_task    = p_funcwk;
  p_wk->heapID    = heapID;

  p_wk->offset    = *cp_pos;

  // �I�u�W�F��SEARCH
  p_wk->p_poke = MMDLSYS_SearchOBJID(
	  p_fos, id );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����@���[�N�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void EVENT_MOVEPOKE_Delete( EV_MOVEPOKE_WORK* p_wk )
{
  // �^�X�N�j��
  FLDMAPFUNC_Delete( p_wk->p_task );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����@�A�j���[�V�����J�n
 *
 *	@param	p_wk      ���[�N
 *	@param	anime_no  �A�j���[�V�����i���o�[ 
 */
//-----------------------------------------------------------------------------
void EVENT_MOVEPOKE_StartAnime( EV_MOVEPOKE_WORK* p_wk, EV_MOVEPOKE_ANIME_TYPE anime_no )
{
  GF_ASSERT( p_wk->anime_status == ANIME_STATUS_NONE );

  // �A�j���[�V��������
  p_wk->p_anime = ICA_ANIME_CreateStreaming( p_wk->heapID, ARCID_EV_MOVEPOKE, 
      NARC_event_movepoke_rai_kaza_01_bin + anime_no, 
      EV_MOVEPOKE_ANIME_INTERVAL, p_wk->animebuf, EV_MOVEPOKE_ANIME_BUF );

  // �A�j���[�V�����J�n
  p_wk->anime_status = ANIME_STATUS_DOING;

  MOVEPOKE_SetUpMMdl( p_wk->p_anime, p_wk->p_poke, &p_wk->offset );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����@�A�j���[�V�����I���`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    �I��
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
BOOL EVENT_MOVEPOKE_IsAnimeEnd( const EV_MOVEPOKE_WORK* cp_wk )
{
  if( cp_wk->anime_status != ANIME_STATUS_NONE ){
    return FALSE;
  }
  return TRUE;
}







//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
  
//----------------------------------------------------------------------------
/**
 *	@brief  �A�b�v�f�[�g
 *
 *	@param	p_taskwk        �^�X�N���[�N
 *	@param	p_fieldmap      �t�B�[���h�}�b�v
 *	@param	p_work          ���[�N
 */
//-----------------------------------------------------------------------------
static void MOVEPOKE_Update( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  EV_MOVEPOKE_WORK* p_wk = p_work;

  switch( p_wk->anime_status ){
  case ANIME_STATUS_NONE:
    break;

  case ANIME_STATUS_DOING:
    {
      BOOL result;

      // �A�j���[�V������i�߂Ĕ��f
      result = ICA_ANIME_IncAnimeFrame( p_wk->p_anime, FX32_ONE );
      
      // ���f
      MOVEPOKE_SetUpMMdl( p_wk->p_anime, p_wk->p_poke, &p_wk->offset );
      
      if( result == TRUE ){

        // �I��
        ICA_ANIME_Delete( p_wk->p_anime );
        p_wk->p_anime = NULL;
        p_wk->anime_status = ANIME_STATUS_NONE;
      }
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`��
 */
//-----------------------------------------------------------------------------
static void MOVEPOKE_Draw( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���f���̃Z�b�g�A�b�v
 *
 *	@param	cp_anime  �A�j���[�V�����f�[�^
 *	@param	p_mmdl    ���샂�f��
 */
//-----------------------------------------------------------------------------
static void MOVEPOKE_SetUpMMdl( const ICA_ANIME* cp_anime, MMDL* p_mmdl, const VecFx32* cp_pos )
{
  VecFx32 trans;
  VecFx32 rotate;

  ICA_ANIME_GetTranslate( cp_anime, &trans );
  ICA_ANIME_GetRotate( cp_anime, &rotate );

  VEC_Add( &trans, cp_pos, &trans );
  
  // �ʒu���f
  MMDL_SetVectorPos( p_mmdl, &trans );

  // �p�x����A�������f

}


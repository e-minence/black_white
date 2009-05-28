
//============================================================================================
/**
 * @file  btlv_effect.c
 * @brief �퓬�G�t�F�N�g����
 * @author  soga
 * @date  2008.11.21
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "btlv_effect.h"
#include "btlv_effect_def.h"

//�b��Ŗ߂�
#include "arc_def.h"
#include "particle/wazaeffect/spa.naix"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================

#define BTLV_EFFECT_TCB_MAX ( 10 )    //�g�p����TCB��MAX

#define BTLV_EFFECT_BLINK_TIME  ( 3 )
#define BTLV_EFFECT_BLINK_WAIT  ( 4 )

#define BTLV_EFFECT_TRAINER_INDEX_NONE  ( 0xffffffff )

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

struct _BTLV_EFFECT_WORK
{
  //�{���̓X�N���v�g�G���W�����ڂ��āA���삳���邪�A�b���TCB�𗘗p����
  //�ŏI�I�ɂ̓G�t�F�N�g�Ŏg�p����TCB��BTLV_MCSS�ABTLV_CAMERA�ABTLV_EFFECT�ŃV�F�A����`�ɂ���
  GFL_TCBSYS        *tcb_sys;
  void              *tcb_work;
  VMHANDLE          *vm_core;
  PALETTE_FADE_PTR  pfd;
  BTLV_MCSS_WORK    *bmw;
  BTLV_STAGE_WORK   *bsw;
  BTLV_FIELD_WORK   *bfw;
  BTLV_CAMERA_WORK  *bcw;
  BTLV_CLACT_WORK   *bclw;
  GFL_TCB           *v_tcb;
  int               execute_flag;
  HEAPID            heapID;

  int               trainer_index[ BTLV_MCSS_POS_MAX ];

  //�b��Ŗ߂�
  GFL_PTC_PTR       ptc;
  u8                spa_work[ PARTICLE_LIB_HEAP_SIZE ];
};

typedef struct
{
  void        *resource;
  BtlvMcssPos target;
  int         seq_no;
  int         work;
  int         wait;
}BTLV_EFFECT_TCB;

static  BTLV_EFFECT_WORK  *bew = NULL;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================

static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work );

//�b��ō�����G�t�F�N�g�V�[�P���X
static  void  BTLV_EFFECT_TCB_Damage( GFL_TCB *tcb, void *work );

#ifdef PM_DEBUG
void  BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

//============================================================================================
/**
 *  �V�X�e��������
 *
 * @param[in] index     �w�i�����肷��C���f�b�N�X�i���o�[
 * @param[in] vramBank  VRAM�\���e�[�u���iCLACT�̏������ɕK�v�j
 * @param[in] heapID    �q�[�vID
 */
//============================================================================================
void  BTLV_EFFECT_Init( int index, const GFL_DISP_VRAM *vramBank, HEAPID heapID )
{
  GF_ASSERT( bew == NULL );
  bew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_EFFECT_WORK ) );

  bew->heapID = heapID;

  //�{���̓X�N���v�g�G���W�����ڂ��āA���삳���邪�A�b���TCB�𗘗p����
  //�ŏI�I�ɂ̓G�t�F�N�g�Ŏg�p����TCB��BTLV_MCSS�ABTLV_CAMERA�ABTLV_EFFECT�ŃV�F�A����`�ɂ���
  bew->tcb_work = GFL_HEAP_AllocClearMemory( heapID, GFL_TCB_CalcSystemWorkSize( BTLV_EFFECT_TCB_MAX ) );
  bew->tcb_sys = GFL_TCB_Init( BTLV_EFFECT_TCB_MAX, bew->tcb_work );

  bew->vm_core = BTLV_EFFVM_Init( bew->tcb_sys, heapID );

  //�p���b�g�t�F�[�h������
  bew->pfd = PaletteFadeInit( heapID );
  PaletteTrans_AutoSet( bew->pfd, TRUE );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_BG, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_BG, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_OBJ, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_OBJ, 0x200, heapID );

  bew->bmw  = BTLV_MCSS_Init( bew->tcb_sys, heapID );
  bew->bsw  = BTLV_STAGE_Init( index, heapID );
  bew->bfw  = BTLV_FIELD_Init( bew->tcb_sys, index, heapID );
  bew->bcw  = BTLV_CAMERA_Init( bew->tcb_sys, heapID );
  bew->bclw = BTLV_CLACT_Init( bew->tcb_sys, vramBank, heapID );

  BTLV_MCSS_SetOrthoMode( bew->bmw );

  //�p�[�e�B�N��������
  GFL_PTC_Init( heapID );

  //�g���[�i�[�C���f�b�N�X�Ǘ��z�񏉊���
  {
    int index;

    for( index = 0 ; index < BTLV_MCSS_POS_MAX ; index++ )
    {
      bew->trainer_index[ index ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
    }
  }

  //VBlank�֐�
  bew->v_tcb = GFUser_VIntr_CreateTCB( BTLV_EFFECT_VBlank, NULL, 0 );
}

//============================================================================================
/**
 *  �V�X�e���I��
 */
//============================================================================================
void  BTLV_EFFECT_Exit( void )
{
  GF_ASSERT( bew != NULL );

  PaletteFadeWorkAllocFree( bew->pfd, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_SUB_BG );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_MAIN_OBJ );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_SUB_OBJ );
  PaletteFadeFree( bew->pfd );
  BTLV_MCSS_Exit( bew->bmw );
  BTLV_STAGE_Exit( bew->bsw );
  BTLV_FIELD_Exit( bew->bfw );
  BTLV_CAMERA_Exit( bew->bcw );
  BTLV_CLACT_Exit( bew->bclw );
  GFL_PTC_Exit();
  BTLV_EFFVM_Exit( bew->vm_core );
  GFL_TCB_DeleteTask( bew->v_tcb );
  GFL_TCB_Exit( bew->tcb_sys );
  GFL_HEAP_FreeMemory( bew->tcb_work );
  GFL_HEAP_FreeMemory( bew );

  bew = NULL;

}

//============================================================================================
/**
 *  �V�X�e�����C��
 */
//============================================================================================
void  BTLV_EFFECT_Main( void )
{
  if( bew == NULL ) return;

  bew->execute_flag = BTLV_EFFVM_Main( bew->vm_core );

  GFL_TCB_Main( bew->tcb_sys );

  BTLV_MCSS_Main( bew->bmw );
  BTLV_STAGE_Main( bew->bsw );
  BTLV_FIELD_Main( bew->bfw );
  BTLV_CAMERA_Main( bew->bcw );
  BTLV_CLACT_Main( bew->bclw );

  //3D�`��
  {
    GFL_G3D_DRAW_Start();
    GFL_G3D_DRAW_SetLookAt();
    {
      //�����̕`�揇�Ԃ́ANitroSDK�ŕ`�悳��Ă�����̂̂��Ƃ�NitroSystem�ŕ`�悳��Ă�����̂�u��
      //���Ԃ����݂�����Ɛ������\�����ꂸ�ň��t���[�Y����
      BTLV_MCSS_Draw( bew->bmw );
      {
        s32 particle_count;

        particle_count = G3X_GetPolygonListRamCount();

        GFL_PTC_Main();

        particle_count = G3X_GetPolygonListRamCount() - particle_count;
      }
      BTLV_STAGE_Draw( bew->bsw );
      BTLV_FIELD_Draw( bew->bfw );
    }

    GFL_G3D_DRAW_End();
  }
}

//============================================================================================
/**
 *  �G�t�F�N�g�N��
 *
 * @param[in] eff_no  �N������G�t�F�N�g�i���o�[
 */
//============================================================================================
void  BTLV_EFFECT_Add( int eff_no )
{
  BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_ERROR, BTLV_MCSS_POS_ERROR, eff_no );
}
//=============================================================================================
/**
 * �G�t�F�N�g�N���i�����ʒu�̎w�肪�K�v�Ȃ��� ... �\�̓A�b�v�E�_�E���C����E�ޏꓙ�j
 *
 * @param   pos       �����ʒu
 * @param   eff_no    �G�t�F�N�g�i���o�[
 *
 */
//=============================================================================================
void BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no )
{
  BTLV_EFFVM_Start( bew->vm_core, pos, BTLV_MCSS_POS_ERROR, eff_no );
}
//=============================================================================================
/**
 * �G�t�F�N�g�N���i���U�G�t�F�N�g��p�j
 *
 * @param   param   [in] �G�t�F�N�g�p�����[�^
 */
//=============================================================================================
void BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param )
{
  // @@@ ���͊�{��񂵂������n���Ă��Ȃ�
  BTLV_EFFVM_Start( bew->vm_core, param->from, param->to, param->waza );
}

//=============================================================================================
/**
 * �G�t�F�N�g������~
 */
//=============================================================================================
void BTLV_EFFECT_Stop( void )
{
  BTLV_EFFVM_Stop( bew->vm_core );
}

//=============================================================================================
/**
 * �_���[�W�G�t�F�N�g�N��
 *
 * @param   target    �����ʒu
 *
 */
//=============================================================================================
void BTLV_EFFECT_Damage( BtlvMcssPos target )
{
  BTLV_EFFECT_TCB *bet = GFL_HEAP_AllocMemory( bew->heapID, sizeof( BTLV_EFFECT_TCB ) );

  bet->seq_no = 0;
  bet->target = target;
  bet->work = BTLV_EFFECT_BLINK_TIME;
  bet->wait = 0;

  bew->execute_flag = 1;

  GFL_TCB_AddTask( bew->tcb_sys, BTLV_EFFECT_TCB_Damage, bet, 0 );
}

//============================================================================================
/**
 *  �G�t�F�N�g�N�������`�F�b�N
 *
 * @retval FALSE:�N�����Ă��Ȃ��@TRUE:�N����
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecute( void )
{
  return ( bew->execute_flag != 0 );
}

//============================================================================================
/**
 *  �w�肳�ꂽ�����ʒu�Ƀ|�P�������Z�b�g
 *
 * @param[in] pp      �Z�b�g����|�P������POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in] position  �Z�b�g����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position )
{
  BTLV_MCSS_Add( bew->bmw, pp, position );
}

//============================================================================================
/**
 *  �w�肳�ꂽ�����ʒu�̃|�P�������폜
 *
 * @param[in] position  �폜����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_DelPokemon( int position )
{
  BTLV_MCSS_Del( bew->bmw, position );
}

//============================================================================================
/**
 *  �w�肳�ꂽ�����ʒu�̃|�P���������݂��邩�`�F�b�N
 *
 * @param[in] position  �`�F�b�N����|�P�����̗����ʒu
 *
 * @retval  TRUE:���݂���@FALSE:���݂��Ȃ�
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExistPokemon( int position )
{
  return BTLV_MCSS_CheckExistPokemon( bew->bmw, position );
}

//============================================================================================
/**
 *  �w�肳�ꂽ�ʒu�Ƀg���[�i�[���Z�b�g
 *
 * @param[in] trtype    �Z�b�g����g���[�i�[�^�C�v
 * @param[in] position  �Z�b�g����g���[�i�[�̗����ʒu
 * @param[in] pos_x     �Z�b�g����g���[�i�[��X���W�i�O�ŗ����ʒu��X���W�����j
 * @param[in] pos_y     �Z�b�g����g���[�i�[��Y���W�i�O�ŗ����ʒu��Y���W�����j
 */
//============================================================================================
void  BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y )
{
  GF_ASSERT( position < BTLV_MCSS_POS_MAX );
  GF_ASSERT( bew->trainer_index[ position ] == BTLV_EFFECT_TRAINER_INDEX_NONE );

#if 0
  if( pos_x == 0 )
  {
  }
  if( pos_y == 0 )
  {
  }
#endif

  bew->trainer_index[ position ] = BTLV_CLACT_Add( bew->bclw, ARCID_TRGRA, trtype * 4, pos_x, pos_y );
}

//============================================================================================
/**
 *  �w�肳�ꂽ�����ʒu�̃g���[�i�[���폜
 *
 * @param[in] position  �폜����g���[�i�[�̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_DelTrainer( int position )
{
  GF_ASSERT( position < BTLV_MCSS_POS_MAX );
  GF_ASSERT( bew->trainer_index[ position ] != BTLV_EFFECT_TRAINER_INDEX_NONE );

  BTLV_CLACT_Delete( bew->bclw, bew->trainer_index[ position ] );
  bew->trainer_index[ position ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
}

//============================================================================================
/**
 *  �w�肳�ꂽ�����ʒu�̃g���[�i�[�C���f�b�N�X���擾
 *
 * @param[in] position  �擾����g���[�i�[�̗����ʒu
 */
//============================================================================================
int BTLV_EFFECT_GetTrainerIndex( int position )
{
  GF_ASSERT( position < BTLV_MCSS_POS_MAX );

  return bew->trainer_index[ position ];
}

//============================================================================================
/**
 *  �G�t�F�N�g�Ŏg�p����Ă���J�����Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bcw �J�����Ǘ��\����
 */
//============================================================================================
BTLV_CAMERA_WORK  *BTLV_EFFECT_GetCameraWork( void )
{
  return bew->bcw;
}

//============================================================================================
/**
 *  �G�t�F�N�g�Ŏg�p����Ă���MCSS�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bmw MCSS�Ǘ��\����
 */
//============================================================================================
BTLV_MCSS_WORK  *BTLV_EFFECT_GetMcssWork( void )
{
  return bew->bmw;
}

//============================================================================================
/**
 *  �G�t�F�N�g�Ŏg�p����Ă���VMHANDLE�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval vm_core VMHANDLE�Ǘ��\����
 */
//============================================================================================
VMHANDLE  *BTLV_EFFECT_GetVMHandle( void )
{
  return bew->vm_core;
}

//============================================================================================
/**
 *  �G�t�F�N�g�Ŏg�p����Ă���GFL_TCBSYS�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval vm_core VMHANDLE�Ǘ��\����
 */
//============================================================================================
GFL_TCBSYS  *BTLV_EFFECT_GetTCBSYS( void )
{
  return bew->tcb_sys;
}

//============================================================================================
/**
 *  �G�t�F�N�g�Ŏg�p����Ă���PALETTE_FADE_PTR�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval vm_core VMHANDLE�Ǘ��\����
 */
//============================================================================================
PALETTE_FADE_PTR  BTLV_EFFECT_GetPfd( void )
{
  return bew->pfd;
}

//============================================================================================
/**
 *  �G�t�F�N�g�Ŏg�p����Ă���CLWK�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bclw CLWK�Ǘ��\����
 */
//============================================================================================
BTLV_CLACT_WORK *BTLV_EFFECT_GetCLWK( void )
{
  return bew->bclw;
}

//============================================================================================
/**
 *  VBlank�֐�
 */
//============================================================================================
static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work )
{
  GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( bew->pfd );
}

//============================================================================================
/**
 *  �_���[�W�G�t�F�N�g�V�[�P���X�i����TCB�ō쐬�j
 */
//============================================================================================
static  void  BTLV_EFFECT_TCB_Damage( GFL_TCB *tcb, void *work )
{
  BTLV_EFFECT_TCB *bet = (BTLV_EFFECT_TCB*)work;

  if( bet->wait ){
    bet->wait--;
    return;
  }
  switch( bet->seq_no ){
  case 0:
    bet->seq_no ^= 1;
    BTLV_MCSS_SetVanishFlag( bew->bmw, bet->target, BTLV_MCSS_VANISH_ON );
    bet->wait = BTLV_EFFECT_BLINK_WAIT;
    break;
  case 1:
    bet->seq_no ^= 1;
    BTLV_MCSS_SetVanishFlag( bew->bmw, bet->target, BTLV_MCSS_VANISH_OFF );
    bet->wait = BTLV_EFFECT_BLINK_WAIT;
    if( --bet->work == 0 ){
      bew->execute_flag = 0;
      GFL_HEAP_FreeMemory( bet );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

#ifdef PM_DEBUG
//============================================================================================
/**
 *  �w�肳�ꂽ�����ʒu�Ƀ|�P�������Z�b�g
 *
 * @param[in] pp      �Z�b�g����|�P������POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in] position  �Z�b�g����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position )
{
  BTLV_MCSS_AddDebug( bew->bmw, madw, position );
}
#endif

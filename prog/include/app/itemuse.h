//============================================================================================
/**
 * @file  itemuse.h
 * @brief �t�B�[���h�A�C�e���g�p����
 * @author  Hiroyuki Nakamura
 * @date  05.12.12
 */
//============================================================================================

#pragma once


#include "gamesystem/gamesystem.h"



//============================================================================================
//  �萔��`
//============================================================================================

/// �o�b�O���甲����Ƃ��̓���
enum BAG_NEXTPROC_ENUM
{
  BAG_NEXTPROC_EXIT,          // �I��
  BAG_NEXTPROC_RETURN,        // ��O�ɖ߂�
  BAG_NEXTPROC_HAVE,          // �|�P�����ɃA�C�e������������
  BAG_NEXTPROC_ITEMEQUIP,     // �|�P�������X�g�̃A�C�e�������ɖ߂�
  BAG_NEXTPROC_ITEMHAVE_RET,  // �|�P�������X�g�̃A�C�e�������ɖ߂邪�A�L�����Z���̂Ƃ�
  BAG_NEXTPROC_ITEMUSE,       // �g��
  BAG_NEXTPROC_WAZASET,       // �Z�Z�b�g
  BAG_NEXTPROC_TOWNMAP,       // �^�E���}�b�v
  BAG_NEXTPROC_FRIENDNOTE,    // �Ƃ������蒠
  BAG_NEXTPROC_EXITEM,        
  BAG_NEXTPROC_RIDECYCLE,     // �̂�
  BAG_NEXTPROC_DROPCYCLE,     // �����
  BAG_NEXTPROC_EVOLUTION,     // ����
  BAG_NEXTPROC_PALACEJUMP,    // �p���X�ړ�
  BAG_NEXTPROC_ANANUKENOHIMO, // ���Ȃʂ��̂Ђ�
  BAG_NEXTPROC_AMAIMITU,      // ���܂��~�c
  BAG_NEXTPROC_TURIZAO,       // �肴��
  BAG_NEXTPROC_BATTLERECORDER,// �o�g�����R�[�_�[
  BAG_NEXTPROC_MAILVIEW,      // ���[���{��
  BAG_NEXTPROC_DOWSINGMACHINE,      // �_�E�W���O�}�V��
};


// �t�B�[���h�Ŏg�p�E�s�g�p�`�F�b�N�̎��W�Ɏg�p����ID
// ���V�����g�p���铹��������ꍇ�͂���enum��`�ɐV�������O��ǉ�����
//   ���Abag_param.h��BAG_SetItemUseCheck�֐��œ���g�p�E�s�g�p�̏�Ԃ��i�[����K�v����
// ���o�^�ł���ID��32�܂�(ITEMCHECK_WORK)
typedef enum {
  ITEMCHECK_CYCLE=0, // ���]��(����Ă��Ȃ�����TRUE�ŏ���A����Ă��鎞��TRUE�ō~����)
  ITEMCHECK_TOWNMAP,      // �^�E���}�b�v
  ITEMCHECK_WIFINOTE,     // �Ƃ������蒠
  ITEMCHECK_ANANUKENOHIMO,// ���Ȃʂ��̃q��
  ITEMCHECK_AMAIMITU,     // ���܂��~�c
  ITEMCHECK_TURIZAO,      // �肴��
  ITEMCHECK_BATTLE_RECORDER,  // �o�g�����R�[�_�[
  ITEMCHECK_MAIL,         // ���[��
  ITEMCHECK_SPLAY,        // �X�v���[
  ITEMCHECK_MAX,

} ITEMCHECK_ENABLE ;




//============================================================================================
//  �萔��`
//============================================================================================
// �A�C�e���g�p���[�N
typedef struct {
  GMEVENT * event;  // �C�x���g
  u16 item;         // �A�C�e���ԍ�
  u8  use_poke;       // �g�p����|�P����
} ITEMUSE_WORK;

// �A�C�e���g�p�`�F�b�N���[�N
typedef struct {
  int zone_id;  // �]�[��ID
  int PlayerForm; //�@���@�̌`��i���]�Ԃɏ���Ă��邩�Ƃ��j
  
  u16 Companion;  //�@�A�������Ԃ��H
  u16 FrontAttr;  //�@���@�O���A�g���r���[�g
  u16 NowAttr;  //�@���@���݃A�g���r���[�g
  u16 SeedInfo; //�g�p�\�ȃA�C�e�����i���̂݊֘A�j
   GAMESYS_WORK * gsys;
  u32 item_enable;   // �w��̓���̎g�p���E�s�����w�肷��bit�t�B�[���h
                     // (�Q�Ƃ�enum ITEMCHECK_ENABLE����j
}ITEMCHECK_WORK;



// �A�C�e���g�p�`�F�b�N�֐�
typedef u32 (*ITEMCHECK_FUNC)(const ITEMCHECK_WORK*);

// �A�C�e���g�p�֐�
typedef void (*ITEMUSE_FUNC)(ITEMUSE_WORK*, const ITEMCHECK_WORK *);

// �A�C�e���g�p�֐��擾�p�����[�^
enum {
  ITEMUSE_PRM_USEFUNC = 0,  // �o�b�O����̎g�p�֐�
  ITEMUSE_PRM_CNVFUNC,    // �֗��{�^������̎g�p�֐�
  ITEMUSE_PRM_CHECKFUNC   // �`�F�b�N�֐�
};

// �A�C�e���g�p�`�F�b�N�̖߂�l
enum {
  ITEMCHECK_TRUE = 0,       // �g�p�\

  ITEMCHECK_ERR_CYCLE_OFF,    // ���]�Ԃ��~���Ȃ�
  ITEMCHECK_ERR_COMPANION,    // �g�p�s�E�A�����
  ITEMCHECK_ERR_DISGUISE,     // �g�p�s�E���P�b�g�c�ϑ���

  ITEMCHECK_FALSE = 0xffffffff  // �g�p�s�E���m�̌��t
};



//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �g�p�֐��擾
 *
 * @param prm   �擾�p�����[�^
 * @param id    �֐�ID
 *
 * @return  �g�p�֐�
 */
//--------------------------------------------------------------------------------------------
extern u32 ITEMUSE_GetFunc( u16 prm, u16 id );

//--------------------------------------------------------------------------------------------
/**
 * �g�p�`�F�b�N���[�N�쐬
 *
 * @param fsys  �t�B�[���h���[�N
 * @param id    �`�F�b�N���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void ItemUse_CheckWorkMake( GAMESYS_WORK * fsys, ITEMCHECK_WORK * icwk );

//--------------------------------------------------------------------------------------------
/**
 * ���̂ݎg�p�^�C�v�擾
 *
 * @param iuwk  �A�C�e���g�p���[�N
 *
 * @retval  "TRUE = ���߂�"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ItemUse_KinomiTypeCheck( const ITEMCHECK_WORK * icwk );

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���N������
 *
 * @param repw  �t�B�[���h���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
//extern int FieldConvButtonEventInit( GAMESYS_WORK * repw, int no );


extern u32 ITEMUSE_GetUseFunc( u16 prm, u16 id );



//=============================================================================================
/**
 * @brief �w��̓���g���邩���E�s�������i�[����(itemmenu.h����enum��`ITEMCHECK_ENABLE)
 *
 * @param   param   BAG_PARAM�̃|�C���^
 * @param   type    �g������̎�ށiitemmenu.h����enum��`ITEMCHECK_ENABLE����w��j
 * @param   check   0:�g���Ȃ�  1:�g����
 *
 * @retval  none
 */
//=============================================================================================
extern void ITEMUSE_SetItemUseCheck( ITEMCHECK_WORK * icwk, ITEMCHECK_ENABLE type, BOOL check );
extern BOOL ITEMUSE_GetItemUseCheck( ITEMCHECK_WORK * icwk, ITEMCHECK_ENABLE type );

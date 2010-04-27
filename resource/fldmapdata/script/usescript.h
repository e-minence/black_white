//======================================================================
/**
 * @file  usescript.h
 * @brief �X�N���v�g�Ŏg�p����t�@�C���̃C���N���[�h�Ȃ�
 * @author  Satoshi Nohara
 * @date  2005.10.04
 */
//======================================================================
#ifndef __USESCRIPT_H__
#define __USESCRIPT_H__

#define __ASM_NO_DEF_
#define ASM_CPP             //pm_version.h

//======================================================================
// include
//======================================================================
//���ʃX�N���v�g�f�[�^ID
#include "common_scr_def.h"
#include "shop_scr_def.h"
#include "item_get_scr_def.h"
#include "btl_util_scr_def.h"

//���샂�f���\���R�[�h
#include "../../../prog/src/field/fldmmdl_code.h"
#include "../../../prog/arc/fieldmap/fldmmdl_objcode.h"

//���@�֘A
#include "../../../prog/src/field/field_player_code.h"
//����
#include "../../../prog/include/field/field_dir.h"

//�X�N���v�g�R�}���h��ʂ��ăX�N���v�g�ƃv���O�����̊Ԃ�
//�����g�p����l�̒�`
#include "../../../prog/src/field/script_def.h"

//�C�x���g���[�N�A�t���O
#include "../flagwork/flag_define.h"  //�C�x���g�t���O
#include "../flagwork/work_define.h"  //�C�x���g���[�N

//�T�E���h���x��
#include "../../../prog/include/sound/wb_sound_data.sadl"

#include "../../personal/monsno_def.h"    //�|�P�����i���o�[
#include "../../personal/monsnum_def.h"    //�|�P�����i���o�[
#include "../../itemconv/itempocket_def.h"  //�ǂ����|�P�b�g�i���o�[�w��
#include "../../../prog/include/item/itemsym.h"   //�A�C�e���i���o�[��`

#include "../zonetable/zone_id.h"     //�]�[��ID�Q��
#include "../warpdata/warpdata.h"     //���[�vID�Q��

//�g���[�i�[ID
#include "../../../prog/include/tr_tool/trno_def.h"

//�|�P�����̃^�C�v�w��ID
#include "../../../prog/include/poke_tool/poketype_def.h"
//���M�����[�V�����w��
#include "../../../prog/include/poke_tool/regulation_def.h"
//�����w��
#include "../../../prog/include/poke_tool/tokusyu_def.h"

//�t�B�[���h�ʐM�̎�ޒ�`
#include "../../../prog/include/gamesystem/game_comm.h"

//�ړ��|�P������`
#include "../../../prog/src/field/move_pokemon_def.h"

//���ʁA�����o�[�W�����Ȃǂ̒�`
#include "../../../prog/include/pm_version.h"

//�G�ߎw��ID�̒�`
#include "../../../prog/include/gamesystem/pm_season.h"

//���UNo
#include "../../../prog/include/waza_tool/wazano_def.h"

//�t�B�[���h�J�b�g�C���i���o�[��`
#include "../../fld3d_ci/fldci_id_def.h"

//�V���D�g���[�i�[�������^�C�v��`
#include "../../../prog/src/field/pl_boat_def.h"

//�{�b�N�X�Ăяo�����[�h
#include "../../../prog/include/app/box_mode.h"

//3D�f���w��ID�̒�`
#include "../../../prog/include/demo/demo3d_demoid.h"

//�J�����p�����[�^ID
#include "../scr_cam_prm/scr_cam_prm_id.h"

//�Q�[��������ID
#include "../../fld_trade/fld_trade_list.h"

//�}�b�v�u�������C�x���g�w��ID
#include "../../../prog/src/field/map_replace.h"

//�Œ�A�C�e���V���b�v�w��p��`
#include "../../shopdata/shop_data_index.h"

//�t�B�[���h����R�[�������ȈՉ�b
#include "../../../prog/src/field/easytalk_mode_def.h"

//�V��i���o�[
#include "../../../prog/include/field/weather_no.h"

//���A�������^�C�v��`
#include "../../../prog/src/field/un_roominfo_def.h"

// �ID
#include "../../research_radar/data/hobby_id.h"

//�g���C�A���n�E�X
#include "../../../prog/src/field/trial_house_scr_def.h"
#include "../../../prog/include/savedata/th_rank_def.h"

//�X�N���v�g�����ʒ�`�w�b�_�t�@�C��
//�ŏ���3�̃^�C�v�킯�Ȃ�
#include "user_define.h"

#include  "../../../prog/include/system/timezone.h" //���ԑђ�`

//��A�b�v��`
#include "../../../prog/src/field/fld_faceup_def.h"

//�����̐��i
#include "../../../prog/include/savedata/nature_def.h"

// ����Ⴂ�������֘A
#include "../../../prog/include/field/research_team_def.h"
#include "../../../resource/research_radar/data/question_id.h"
#include "../../../resource/research_radar/data/answer_id.h"

//�����f�B�A
#include "../../../resource/fldmapdata/gimmick/merodhia/scr_gmk_timing_def.h"

//�A�C�e���^�C�v
#include "../../../prog/include/item/itemtype_def.h"

//�z�z�n���b�N
#include "../../../prog/src/field/evt_lock_def.h"

//���R�[�h
#include "../../../prog/include/savedata/record.h"

//======================================================================
//  ���ʃ��b�Z�[�W�֘A
//======================================================================
#include "../../message/dst/script/msg_common_scr.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------
//#ifdef PM_DEBUG
#define SCRIPT_PM_DEBUG         //*.ev����PM_DEBUG�������Ȃ̂�
#define SCRIPT_PL_NULL          //�@�\������
//#endif

//--------------------------------------------------------------
//  ���Z�A����n
//--------------------------------------------------------------
#define LT        0       /* <  */
#define EQ        1       /* == */
#define GT        2       /* >  */
#define LE        3       /* <= */
#define GE        4       /* >= */
#define NE        5       /* != */
#define EQUAL     EQ
#define LITTLER     LT
#define GREATER     GT
#define LT_EQ     LE
#define GT_EQ     GE

#define FLGON     1
#define FLGOFF      0

#define CMPID_LT    0
#define CMPID_EQ    1
#define CMPID_GT    2
#define CMPID_LT_EQ 3
#define CMPID_GT_EQ 4
#define CMPID_NE    5
#define CMPID_OR    6
#define CMPID_AND   7
#define CMPID_TRUE  8
#define CMPID_FALSE 9
#define CMPID_GET   0xff

//nitro/type.h�Œ�`����Ă��邪�C���N���[�h�ł��Ȃ��̂Ŏ��͂Œ�`
#define TRUE      1
#define FALSE     0

//--------------------------------------------------------------
//  �t�F�[�h�֘A
//--------------------------------------------------------------
//�t�F�[�h���ʑ��x
#define DISP_FADE_SPEED (2)
//�X���[�t�F�[�h���ʑ��x
#define DISP_SLOW_FADE_SPEED (4)

//--------------------------------------------------------------
//  ���j���[�p�߂�l
//--------------------------------------------------------------
#define EV_WIN_B_CANCEL (0xfffe)
#define EV_WIN_X_BREAK  (0xfffd)

//--------------------------------------------------------------
//  ���b�Z�[�W�E�B���h�E�֘A
//--------------------------------------------------------------
//�V�X�e���A�����o���E�B���h�E�p�ʒu��`
#define POS_DEFAULT SCRCMD_MSGWIN_DEFAULT   //�����U�蕪�� ���p �ߋ��ʒu�Q�l
#define POS_UP      SCRCMD_MSGWIN_UP        //�� ���p �K������
#define POS_DOWN    SCRCMD_MSGWIN_DOWN      //�� ���p �K������
#define POS_UL      SCRCMD_MSGWIN_UPLEFT    //���� BALLOONWIN��p
#define POS_DL      SCRCMD_MSGWIN_DOWNLEFT  //���� BALLOONWIN��p
#define POS_UR      SCRCMD_MSGWIN_UPRIGHT   //�E�� BALLOONWIN��p
#define POS_DR      SCRCMD_MSGWIN_DOWNRIGHT //���� BALLOONWIN��p

//BG�E�B���h�E�^�C�v
#define TYPE_INFO (SCRCMD_BGWIN_TYPE_INFO) //�f����
#define TYPE_TOWN (SCRCMD_BGWIN_TYPE_TOWN) //�X
#define TYPE_POST (SCRCMD_BGWIN_TYPE_POST) //�{��
#define TYPE_ROAD (SCRCMD_BGWIN_TYPE_ROAD) //��

//����E�B���h�E�^�C�v
#define SPWIN_LETTER  (SCRCMD_SPWIN_TYPE_LETTER)
#define SPWIN_BOOK    (SCRCMD_SPWIN_TYPE_BOOK)

//�����o���E�B���h�E�@���
#define BALLOONWIN_TYPE_NORMAL (0)
#define BALLOONWIN_TYPE_GIZA   (1)

//--------------------------------------------------------------
// �p�\�R���̓a�������ʌĂяo��
//--------------------------------------------------------------
#define SCRCMD_DENDOU_PC_FIRST  (0) //�u�͂��߂ăN���A�v
#define SCRCMD_DENDOU_PC_DENDOU (1) //�u�a������v

//--------------------------------------------------------------
//  ���̑�
//--------------------------------------------------------------
//����������G�t�F�N�g�\���J�n����, OBJ�������܂ł̑҂�����
#define EV_HIDEN_IAIGIRI_OBJDEL_WAIT (3)

//T01-R01�ԂŔz�u�����]�[�����ׂ�OBJ�pID
#define T01_R01_RIVAL (MMDL_ID_T01_R01_RIVAL)
#define T01_R01_SUPPORT (MMDL_ID_T01_R01_SUPPORT)

//======================================================================
// �V���b�v�̈��A�R�[�h
//======================================================================
#define SHOP_GREETING_LESS    (0) //���A����
#define SHOP_GREETING_NORMAL  (1) //��������Ⴂ�܂�
#define SHOP_GREETING_WAZA    (2) //�Z�}�V���V���b�v�ł�

#endif //__USESCRIPT_H__

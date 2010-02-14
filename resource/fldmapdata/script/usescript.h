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
#include "../../itemconv/itempocket_def.h"  //�ǂ����|�P�b�g�i���o�[�w��
#include "../../../prog/include/item/itemsym.h"

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

//�X�N���v�g�����ʒ�`�w�b�_�t�@�C��
//�ŏ���3�̃^�C�v�킯�Ȃ�
#include "user_define.h"

#include  "../../../prog/include/system/timezone.h" //���ԑђ�`

//--------------------------------------------------------------
//  wb_null
//--------------------------------------------------------------
#if 0
//include/field

#include "../../../include/system/brightness.h"   //�P�x
#include "../../../include/system/window.h"     //�E�B���h�E
#include "../../../include/system/wipe.h"     //���C�v

#include "../../../include/battle/battle_common.h"  //�퓬�֘A
#include "../../../include/battle/trno_def.h"   //�g���[�i�[�i���o�[��`

#include "../../../include/application/br_sys_def.h"//GDS��`
#include "../../../include/application/wifi_p2pmatch_def.h" //P2P�ΐ�}�b�`���O�{�[�h
#include "../../../include/application/imageClip/imc_itemid_define.h" //�C���[�W�N���b�v��`
#include "../../../include/application/box_mode.h"  //�{�b�N�X���[�h��`
#include "../../../include/application/bag_def.h" //�|�P�b�g��`
#include "../../../include/application/seal_id.h" //�V�[����`
#include "../../../include/contest/contest_def.h" //�R���e�X�g
#include "../../../include/itemtool/itemsym.h"    //�A�C�e���i���o�[��`
#include "../../../include/communication/comm_def.h"//�ʐM

#include "../../../include/battle/wazano_def.h"   //���UNo
#include "../../../include/battle/battle_server.h"  //�t�H����No

//include/savedata
#include "../../../include/savedata/fnote_mem.h"  //�`���m�[�g
#include "../../../include/savedata/score_def.h"  //�X�R�A
#include "../../../include/savedata/record.h" //�X�R�A

//src/field
#include "../../field/ev_win.h"           //�C�x���g�E�B���h�E��`
#include "../../field/comm_union_def.h"       //���j�I����`
#include "../../field/comm_union_beacon.h"      //���j�I�����[���p�r�[�R����`
#include "../../field/comm_direct_counter_def.h"  //�ʐM�_�C���N�g�R�[�i�[
#include "../../field/fieldobj_code.h"        //�t�B�[���hOBJ�Ŏg�p����V���{���A�R�[�h�錾
#include "../../field/player_code.h"        //REQBIT
#include "../../field/board.h"            //�Ŕ�
#include "../../field/honey_tree_def.h"       //���؃X�e�[�g��`
#include "../../field/sysflag_def.h"        //�V�X�e���t���O��`
#include "../../field/syswork_def.h"        //�V�X�e�����[�N��`
#include "../../field/gym_def.h"          //�W���֘A��`
#include "../../field/safari_train_def.h"
#include "../../field/field_trade.h"        //�Q�[��������
#include "../../field/scr_cycling.h"        //�T�C�N�����O���[�h���W

#include "../../../include/system/snd_def.h"    //�T�E���h��`�̒u��������`
#include "../maptable/zone_id.h"          //�]�[��ID��`
#include "../shopdata/fs_goods_def.h"       //�Œ�O�b�Y�V���b�v��`
#include "../shopdata/fs_seal_def.h"        //�Œ�V�[���V���b�v��`

#include "../../field/move_pokemon_def.h"     //�ړ��|�P������`
#include "../../field/elevator_anm_def.h"     //�G���x�[�^�֘A��`
#include "../../field/ship_demo_def.h"        //�D�f���֘A��`

#include "../../frontier/factory_def.h"       //�t�@�N�g���[��`
#include "../../frontier/stage_def.h"       //�X�e�[�W��`
#include "../../frontier/castle_def.h"        //�L���b�X����`
#include "../../frontier/roulette_def.h"      //���[���b�g��`
#include "../../frontier/fss_scene.h"       //�t�����e�B�A�V�[����`
#include "../../frontier/frontier_def.h"      //�t�����e�B�A��`
#endif //wb null

//======================================================================
//  ���ʃ��b�Z�[�W�֘A
//======================================================================
#include "../../message/dst/script/msg_common_scr.h"

//======================================================================
//  define
//======================================================================
//#ifdef PM_DEBUG
#define SCRIPT_PM_DEBUG         //*.ev����PM_DEBUG�������Ȃ̂�
#define SCRIPT_PL_NULL          //�@�\������
//#endif

//--------------------------------------------------------------
//(�G�������h�ł�vmstat.h�ɒ�`���Ă�����)
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

//�X�N���v�g�ʐM��t�̔��ʒ�`
#define SCR_COMM_ID_NOTHING (0) //�����Ȃ�
#define SCR_COMM_ID_BATTLE  (1) //�o�g��
#define SCR_COMM_ID_UNION (2) //���j�I��
#define SCR_COMM_ID_WIFI  (3) //WiFi
#define SCR_COMM_ID_RECORD  (4) //���R�[�h(sp_***.ev�ɖ��ߍ���ł��܂����̂Ŏc���Ă���)
#define SCR_COMM_ID_CONTEST (5)     //�R���e�X�g
#define SCR_COMM_ID_GTC   (6)     //GTC

//--------------------------------------------------------------
//*.ev�̒��Ŏg�p�����`
//--------------------------------------------------------------
/*
  // �b������������ێ����郏�[�N
  u16 TalkSiteWork = 0;

  // �莝���������ς��ło�b�ɓ]�����ꂽ��
  u16 BoxNoWork = 0;      //���Ԃ�BOX��
  u16 PasoNoWork = 0;     //����BOX�̉��Ԗڂ�

  //���j�I�����[���ł̃C�x���g��Ԑ���p���[�N
  u16 UnionRoomWork = 0;
*/

/*�A���m�[��������*/
#define ANOON_SEE_NUM_1 (10)
#define ANOON_SEE_NUM_2 (26)

//���j���[�p�߂�l
#define EV_WIN_B_CANCEL (0xfffe)
#define EV_WIN_X_BREAK  (0xfffd)

//�t�F�[�h���ʑ��x
#define DISP_FADE_SPEED (2)

#define BAG_POCKET_NORMAL   0
#define BAG_POCKET_DRUG     1
#define BAG_POCKET_WAZA     2
#define BAG_POCKET_NUTS     3
#define BAG_POCKET_EVENT    4

//�E�B���h�E�ʒu
#define WIN_UP (SCRCMD_MSGWIN_UP)
#define WIN_DOWN (SCRCMD_MSGWIN_DOWN)
#define WIN_NONE (SCRCMD_MSGWIN_NON)

//���C�v�t�F�[�h�̊�{�̒l
#define SCR_WIPE_DIV        (WIPE_DEF_DIV)
#define SCR_WIPE_SYNC       (WIPE_DEF_SYNC)

//BG�E�B���h�E�^�C�v
#define TYPE_INFO (SCRCMD_BGWIN_TYPE_INFO) //�f����
#define TYPE_TOWN (SCRCMD_BGWIN_TYPE_TOWN) //�X
#define TYPE_POST (SCRCMD_BGWIN_TYPE_POST) //�{��
#define TYPE_ROAD (SCRCMD_BGWIN_TYPE_ROAD) //��

//����E�B���h�E�^�C�v
#define SPWIN_LETTER (SCRCMD_SPWIN_TYPE_LETTER)
#define SPWIN_BOOK (SCRCMD_SPWIN_TYPE_BOOK)

//����������G�t�F�N�g�\���J�n����, OBJ�������܂ł̑҂�����
#define EV_HIDEN_IAIGIRI_OBJDEL_WAIT (3)

//�����o���E�B���h�E�@���
#define BALLOONWIN_TYPE_NORMAL (0)
#define BALLOONWIN_TYPE_GIZA (1)

#endif //__USESCRIPT_H__

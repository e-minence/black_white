//======================================================================
/**
 *  @file   event_debug_numinput.c
 *  @brief  �f�o�b�O���l����
 *  @author Miyuki Iwasawa
 *  @date   09.11.25
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"
#include "font/font.naix"

#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"
#include "field/eventdata_sxy.h"

#include "sound/pm_sndsys.h"

#include "field_sound.h"
#include "eventwork.h"

#include "event_debug_local.h"
#include "event_debug_numinput.h"

#include "message.naix"
#include "msg/msg_d_numinput.h"

#include "debug/debug_str_conv.h" // for DEB_STR_CONV_SjisToStrcode
#include "print/wordset.h"  //WORDSET
#include "msg/msg_d_field.h"

#include "arc/fieldmap/debug_symbol.naix"  //DEBUG_SYM_

#include "arc/fieldmap/debug_list.h"  //DEBUG_SCR_

#include "effect_encount.h"

#include "united_nations.h" 

//======================================================================
//======================================================================
///���ڑI��p�f�[�^�̒�`
typedef enum {
  D_NINPUT_DATA_LIST,    ///<�Œ�f�[�^
  D_NINPUT_DATA_BIN,    ///<���������f�[�^
}D_NINPUT_DATA_TYPE;

//���l���̓��C���C�x���g���䃏�[�N
typedef struct _DNINPUT_EV_WORK{
  HEAPID  heapID;

  GAMESYS_WORK* gsys;
  GAMEDATA* gdata;
  FIELDMAP_WORK* fieldWork;

  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DNINPUT_EV_WORK;

///���l���̓��X�g�f�[�^
typedef struct {
  u16 gmm_id;
  u32 param;
  const DEBUG_NUMINPUT_PARAM * dni_param;
}DEBUG_NUMINPUT_LIST;

///���l���̓��X�g�������p�����[�^��`
typedef struct{ 
  D_NINPUT_DATA_TYPE data_type;       ///<�f�[�^�̎��

  /** D_NINPUT_DATA_LIST�̏ꍇ�F���X�g�̍ő吔
   *  D_NINPUT_DATA_BIN�̏ꍇ�F�������Ɏg�p����Q�ƃt�@�C����ID
   */
  const u32 count_or_id;               ///<���X�g�̑傫��
  
  /** D_NINPUT_DATA_LIST�̏ꍇ�F�e���ڂ̐��l���͎������w��f�[�^
   *  D_NINPUT_DATA_BIN�̏ꍇ�F�������Ɏg�p����f�t�H���g�f�[�^
   */
  const DEBUG_NUMINPUT_LIST * pList;

}DEBUG_NUMINPUT_INITIALIZER;



//======================================================================
//�v���g�^�C�v
//======================================================================
static GMEVENT_RESULT dninput_MainEvent( GMEVENT * event, int *seq, void * work);

//--------------------------------------------------------------
///�T�u���j���[�C�x���g�F�f�o�b�O�t���O����
//--------------------------------------------------------------
static GMEVENT * DEBUG_EVENT_FLDMENU_FlagWork(
    GAMESYS_WORK * gsys, const DEBUG_NUMINPUT_INITIALIZER * init );

//======================================================================
//�f�[�^��`
//======================================================================
static u32 DebugGetGold(GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param);
static void DebugSetGold(GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param, u32 value);

#include "debug_numinput.cdat"

static const DEBUG_NUMINPUT_PARAM DNUM_DATA_EventFlag;
static const DEBUG_NUMINPUT_PARAM DNUM_DATA_EventWork;

static const DEBUG_NUMINPUT_LIST listPrototype_flag = {
  //gmm_id, param, dni_param
  0, 0, &DNUM_DATA_EventFlag,
};
static const DEBUG_NUMINPUT_LIST listPrototype_work = {
  //gmm_id, param, dni_param
  0, 0, &DNUM_DATA_EventWork,
};

static const DEBUG_NUMINPUT_INITIALIZER DATA_event_flag = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_event_flag_bin, &listPrototype_flag, };
static const DEBUG_NUMINPUT_INITIALIZER DATA_sys_flag = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_sys_flag_bin, &listPrototype_flag, };
static const DEBUG_NUMINPUT_INITIALIZER DATA_vanish_flag = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_vanish_flag_bin, &listPrototype_flag, };
static const DEBUG_NUMINPUT_INITIALIZER DATA_item_flag = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_item_flag_bin, &listPrototype_flag, };
static const DEBUG_NUMINPUT_INITIALIZER DATA_etc_flag = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_etc_flag_bin, &listPrototype_flag, };

static const DEBUG_NUMINPUT_INITIALIZER DATA_sys_work = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_sys_work_bin, &listPrototype_work, };
static const DEBUG_NUMINPUT_INITIALIZER DATA_scene_work = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_scene_work_bin, &listPrototype_work, };
static const DEBUG_NUMINPUT_INITIALIZER DATA_other_work = {
  D_NINPUT_DATA_BIN, NARC_debug_symbol_other_work_bin, &listPrototype_work, };

static const DEBUG_NUMINPUT_INITIALIZER DATA_eff_enc = { 
  D_NINPUT_DATA_LIST,   NELEMS( DNI_EffectEncountList ), DNI_EffectEncountList, };

static const DEBUG_NUMINPUT_INITIALIZER DATA_Initializer[D_NUMINPUT_MODE_MAX] = { 
  { D_NINPUT_DATA_LIST,   NELEMS( DNI_EffectEncountList ), DNI_EffectEncountList, },
  { D_NINPUT_DATA_LIST,   NELEMS( DNI_EffectEncountList ), DNI_EffectEncountList, },
  { D_NINPUT_DATA_LIST,   NELEMS( DNI_EffectEncountList ), DNI_EffectEncountList, },
};

static  const DEBUG_NUMINPUT_INITIALIZER DATA_united = { 
  D_NINPUT_DATA_LIST,   NELEMS( DNI_UnitedNationsList ), DNI_UnitedNationsList, };

static  const DEBUG_NUMINPUT_INITIALIZER DATA_Fortune = { 
  D_NINPUT_DATA_LIST,   NELEMS( DNI_FortuneList ), DNI_FortuneList, };

static  const DEBUG_NUMINPUT_INITIALIZER DATA_researchTeam = { 
  D_NINPUT_DATA_LIST,   NELEMS(NumInputList_researchTeam), NumInputList_researchTeam, };

static const DEBUG_NUMINPUT_INITIALIZER DATA_Q_today = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfQ), NumInputList_todayCountOfQ };
static const DEBUG_NUMINPUT_INITIALIZER DATA_Q_total = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfQ), NumInputList_totalCountOfQ };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q01 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q01), NumInputList_todayCountOfA_Q01 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q02 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q02), NumInputList_todayCountOfA_Q02 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q03 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q03), NumInputList_todayCountOfA_Q03 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q04 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q04), NumInputList_todayCountOfA_Q04 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q05 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q05), NumInputList_todayCountOfA_Q05 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q06 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q06), NumInputList_todayCountOfA_Q06 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q07 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q07), NumInputList_todayCountOfA_Q07 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q08 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q08), NumInputList_todayCountOfA_Q08 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q09 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q09), NumInputList_todayCountOfA_Q09 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q10 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q10), NumInputList_todayCountOfA_Q10 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q11 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q11), NumInputList_todayCountOfA_Q11 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q12 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q12), NumInputList_todayCountOfA_Q12 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q13 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q13), NumInputList_todayCountOfA_Q13 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q14 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q14), NumInputList_todayCountOfA_Q14 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q15 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q15), NumInputList_todayCountOfA_Q15 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q16 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q16), NumInputList_todayCountOfA_Q16 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q17 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q17), NumInputList_todayCountOfA_Q17 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q18 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q18), NumInputList_todayCountOfA_Q18 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q19 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q19), NumInputList_todayCountOfA_Q19 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q20 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q20), NumInputList_todayCountOfA_Q20 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q21 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q21), NumInputList_todayCountOfA_Q21 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q22 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q22), NumInputList_todayCountOfA_Q22 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q23 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q23), NumInputList_todayCountOfA_Q23 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q24 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q24), NumInputList_todayCountOfA_Q24 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q25 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q25), NumInputList_todayCountOfA_Q25 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q26 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q26), NumInputList_todayCountOfA_Q26 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q27 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q27), NumInputList_todayCountOfA_Q27 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q28 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q28), NumInputList_todayCountOfA_Q28 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q29 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q29), NumInputList_todayCountOfA_Q29 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_today_Q30 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_todayCountOfA_Q30), NumInputList_todayCountOfA_Q30 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q01 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q01), NumInputList_totalCountOfA_Q01 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q02 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q02), NumInputList_totalCountOfA_Q02 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q03 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q03), NumInputList_totalCountOfA_Q03 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q04 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q04), NumInputList_totalCountOfA_Q04 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q05 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q05), NumInputList_totalCountOfA_Q05 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q06 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q06), NumInputList_totalCountOfA_Q06 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q07 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q07), NumInputList_totalCountOfA_Q07 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q08 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q08), NumInputList_totalCountOfA_Q08 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q09 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q09), NumInputList_totalCountOfA_Q09 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q10 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q10), NumInputList_totalCountOfA_Q10 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q11 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q11), NumInputList_totalCountOfA_Q11 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q12 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q12), NumInputList_totalCountOfA_Q12 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q13 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q13), NumInputList_totalCountOfA_Q13 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q14 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q14), NumInputList_totalCountOfA_Q14 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q15 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q15), NumInputList_totalCountOfA_Q15 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q16 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q16), NumInputList_totalCountOfA_Q16 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q17 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q17), NumInputList_totalCountOfA_Q17 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q18 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q18), NumInputList_totalCountOfA_Q18 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q19 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q19), NumInputList_totalCountOfA_Q19 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q20 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q20), NumInputList_totalCountOfA_Q20 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q21 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q21), NumInputList_totalCountOfA_Q21 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q22 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q22), NumInputList_totalCountOfA_Q22 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q23 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q23), NumInputList_totalCountOfA_Q23 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q24 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q24), NumInputList_totalCountOfA_Q24 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q25 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q25), NumInputList_totalCountOfA_Q25 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q26 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q26), NumInputList_totalCountOfA_Q26 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q27 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q27), NumInputList_totalCountOfA_Q27 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q28 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q28), NumInputList_totalCountOfA_Q28 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q29 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q29), NumInputList_totalCountOfA_Q29 };
static const DEBUG_NUMINPUT_INITIALIZER DATA_A_total_Q30 = { D_NINPUT_DATA_LIST, NELEMS(NumInputList_totalCountOfA_Q30), NumInputList_totalCountOfA_Q30 };

/// ���l���́@���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DNumInput_MenuFuncHeader =
{
  1,    //���X�g���ڐ�
  9,    //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
  0,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  1,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

static const FLDMENUFUNC_LIST DATA_DNumInputMenu[] =
{
  { dni_event_flag, (void*)&DATA_event_flag },
  { dni_sys_flag, (void*)&DATA_sys_flag },
  { dni_item_flag, (void*)&DATA_item_flag },
  { dni_vanish_flag, (void*)&DATA_vanish_flag },
  { dni_etc_flag, (void*)&DATA_etc_flag },
  { dni_sys_work, (void*)&DATA_sys_work },
  { dni_scene_work, (void*)&DATA_scene_work },
  { dni_other_work, (void*)&DATA_other_work },
  { dni_top_effect_encount, (void*)&DATA_eff_enc },
  { dni_top_scenario, (void*)NULL },
  { dni_top_united_nations, (void*)&DATA_united },
  { dni_top_fortune, (void*)&DATA_Fortune },
  { dni_research_team, (void*)&DATA_researchTeam },
  { dni_q_today, (void*)&DATA_Q_today },
  { dni_q_total, (void*)&DATA_Q_total },
  { dni_a_today_q01, (void*)&DATA_A_today_Q01 },
  { dni_a_today_q02, (void*)&DATA_A_today_Q02 },
  { dni_a_today_q03, (void*)&DATA_A_today_Q03 },
  { dni_a_today_q04, (void*)&DATA_A_today_Q04 },
  { dni_a_today_q05, (void*)&DATA_A_today_Q05 },
  { dni_a_today_q06, (void*)&DATA_A_today_Q06 },
  { dni_a_today_q07, (void*)&DATA_A_today_Q07 },
  { dni_a_today_q08, (void*)&DATA_A_today_Q08 },
  { dni_a_today_q09, (void*)&DATA_A_today_Q09 },
  { dni_a_today_q10, (void*)&DATA_A_today_Q10 },
  { dni_a_today_q11, (void*)&DATA_A_today_Q11 },
  { dni_a_today_q12, (void*)&DATA_A_today_Q12 },
  { dni_a_today_q13, (void*)&DATA_A_today_Q13 },
  { dni_a_today_q14, (void*)&DATA_A_today_Q14 },
  { dni_a_today_q15, (void*)&DATA_A_today_Q15 },
  { dni_a_today_q16, (void*)&DATA_A_today_Q16 },
  { dni_a_today_q17, (void*)&DATA_A_today_Q17 },
  { dni_a_today_q18, (void*)&DATA_A_today_Q18 },
  { dni_a_today_q19, (void*)&DATA_A_today_Q19 },
  { dni_a_today_q20, (void*)&DATA_A_today_Q20 },
  { dni_a_today_q21, (void*)&DATA_A_today_Q21 },
  { dni_a_today_q22, (void*)&DATA_A_today_Q22 },
  { dni_a_today_q23, (void*)&DATA_A_today_Q23 },
  { dni_a_today_q24, (void*)&DATA_A_today_Q24 },
  { dni_a_today_q25, (void*)&DATA_A_today_Q25 },
  { dni_a_today_q26, (void*)&DATA_A_today_Q26 },
  { dni_a_today_q27, (void*)&DATA_A_today_Q27 },
  { dni_a_today_q28, (void*)&DATA_A_today_Q28 },
  { dni_a_today_q29, (void*)&DATA_A_today_Q29 },
  { dni_a_today_q30, (void*)&DATA_A_today_Q30 },
  { dni_a_total_q01, (void*)&DATA_A_total_Q01 },
  { dni_a_total_q02, (void*)&DATA_A_total_Q02 },
  { dni_a_total_q03, (void*)&DATA_A_total_Q03 },
  { dni_a_total_q04, (void*)&DATA_A_total_Q04 },
  { dni_a_total_q05, (void*)&DATA_A_total_Q05 },
  { dni_a_total_q06, (void*)&DATA_A_total_Q06 },
  { dni_a_total_q07, (void*)&DATA_A_total_Q07 },
  { dni_a_total_q08, (void*)&DATA_A_total_Q08 },
  { dni_a_total_q09, (void*)&DATA_A_total_Q09 },
  { dni_a_total_q10, (void*)&DATA_A_total_Q10 },
  { dni_a_total_q11, (void*)&DATA_A_total_Q11 },
  { dni_a_total_q12, (void*)&DATA_A_total_Q12 },
  { dni_a_total_q13, (void*)&DATA_A_total_Q13 },
  { dni_a_total_q14, (void*)&DATA_A_total_Q14 },
  { dni_a_total_q15, (void*)&DATA_A_total_Q15 },
  { dni_a_total_q16, (void*)&DATA_A_total_Q16 },
  { dni_a_total_q17, (void*)&DATA_A_total_Q17 },
  { dni_a_total_q18, (void*)&DATA_A_total_Q18 },
  { dni_a_total_q19, (void*)&DATA_A_total_Q19 },
  { dni_a_total_q20, (void*)&DATA_A_total_Q20 },
  { dni_a_total_q21, (void*)&DATA_A_total_Q21 },
  { dni_a_total_q22, (void*)&DATA_A_total_Q22 },
  { dni_a_total_q23, (void*)&DATA_A_total_Q23 },
  { dni_a_total_q24, (void*)&DATA_A_total_Q24 },
  { dni_a_total_q25, (void*)&DATA_A_total_Q25 },
  { dni_a_total_q26, (void*)&DATA_A_total_Q26 },
  { dni_a_total_q27, (void*)&DATA_A_total_Q27 },
  { dni_a_total_q28, (void*)&DATA_A_total_Q28 },
  { dni_a_total_q29, (void*)&DATA_A_total_Q29 },
  { dni_a_total_q30, (void*)&DATA_A_total_Q30 },
};

static const DEBUG_MENU_INITIALIZER DATA_DNumInput_MenuInitializer = {
  NARC_message_d_numinput_dat,  //���b�Z�[�W�A�[�J�C�u
  NELEMS(DATA_DNumInputMenu),  //���ڐ�max
  DATA_DNumInputMenu, //���j���[���ڃ��X�g
  &DATA_DNumInput_MenuFuncHeader, //���j���w�b�_
  1, 4, 30, 16,
  NULL/*DEBUG_SetMenuWorkZoneID_SelectZone*/,
  NULL,
};

static const DEBUG_MENU_INITIALIZER DATA_DNumInput_ListMenuInitializer = {
  NARC_message_d_numinput_dat,  //���b�Z�[�W�A�[�J�C�u
  0,  //���ڐ�max
  NULL, //���j���[���ڃ��X�g
  &DATA_DNumInput_MenuFuncHeader, //���j���w�b�_
  1, 4, 30, 16,
  NULL/*DEBUG_SetMenuWorkZoneID_SelectZone*/,
  NULL,
};

//======================================================================
//
//�֐���`
//
//======================================================================
//--------------------------------------------------------------
/**
 * ���l���͂̃W�������I�����j���[�C�x���g����
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_NumInput( GAMESYS_WORK* gsys, void* work )
{
  DEBUG_MENU_EVENT_WORK* dme_wk = (DEBUG_MENU_EVENT_WORK*)work;
  DNINPUT_EV_WORK* wk;
  GMEVENT* event;

  event = GMEVENT_Create( gsys, NULL, dninput_MainEvent, sizeof(DNINPUT_EV_WORK) );

  wk = GMEVENT_GetEventWork( event );
  wk->gsys = gsys; 
  wk->gdata = dme_wk->gdata; 
  wk->fieldWork = dme_wk->fieldWork;
  wk->heapID = dme_wk->heapID;

  return event;
}

//--------------------------------------------------------------
/*
 *  @brief  ���l���̓��C�����j���[�C�x���g
 */
//--------------------------------------------------------------
static GMEVENT_RESULT dninput_MainEvent( GMEVENT * event, int *seq, void * work)
{
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork(event);
  DNINPUT_EV_WORK* wk = (DNINPUT_EV_WORK*)work;

  switch(*seq)
  {
  case 0:
    wk->menuFunc = DEBUGFLDMENU_Init( wk->fieldWork, wk->heapID, &DATA_DNumInput_MenuInitializer );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( wk->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){
        break;
      }
      FLDMENUFUNC_DeleteMenu( wk->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){
        return GMEVENT_RES_FINISH;
      }
      if ( ret != NULL ) {
        const DEBUG_NUMINPUT_INITIALIZER *init;
        init = (const DEBUG_NUMINPUT_INITIALIZER *)ret;
        GMEVENT_CallEvent( event, DEBUG_EVENT_FLDMENU_FlagWork( gsys, init ) );
        (*seq)++;
        break;
      }
      return GMEVENT_RES_FINISH;
    }
    break;
  case 2:
    *seq = 0;
    break;

  default:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//
//
//    �f�o�b�O���l����  �{��
//
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  _DISP_INITX = 1,
  _DISP_INITY = 18,
  _DISP_SIZEX = 30,
  _DISP_SIZEY = 4,

  PANO_FONT = 14,
  FBMP_COL_WHITE = 15,
};

#define WINCLR_COL(col) (((col)<<4)|(col))
//--------------------------------------------------------------
/**
 * @brief �f�o�b�O���l���͐��䃏�[�N
 */
//--------------------------------------------------------------
typedef struct {
  /// ����Ώێw��ID
  u32 select_id;
  /// ���쐧��p�����[�^
  const DEBUG_NUMINPUT_PARAM * dni_param;

  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  FIELDMAP_WORK * fieldmap;
  HEAPID heapID;
  GFL_FONT * fontHandle;

	GFL_MSGDATA* msgman;						//���b�Z�[�W�}�l�[�W���[
  WORDSET * wordset;
  GFL_BMPWIN * bmpwin;

  u32 value;
}DEBUG_NUMINPUT_WORK;

//--------------------------------------------------------------
/// ���l���̓E�B���h�E�F����
//--------------------------------------------------------------
static void createNumWin( DEBUG_NUMINPUT_WORK * wk )
{
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
  
  wk->wordset = WORDSET_Create( wk->heapID );
  wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
      ARCID_MESSAGE, NARC_message_d_numinput_dat, wk->heapID);

	bmpwin = GFL_BMPWIN_Create( FLDBG_MFRM_MSG,
		_DISP_INITX, _DISP_INITY, _DISP_SIZEX, _DISP_SIZEY,
		PANO_FONT, GFL_BMP_CHRAREA_GET_B );
  wk->bmpwin = bmpwin;
  
	bmp = GFL_BMPWIN_GetBmp( bmpwin );
	
	GFL_BMP_Clear( bmp, WINCLR_COL(FBMP_COL_WHITE) );
	GFL_BMPWIN_MakeScreen( bmpwin );
	GFL_BMPWIN_TransVramCharacter( bmpwin );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( bmpwin ) );
}

//--------------------------------------------------------------
/// ���l���̓E�B���h�E�F�폜
//--------------------------------------------------------------
static void deleteNumWin( DEBUG_NUMINPUT_WORK * wk )
{
  GFL_BMPWIN * bmpwin = wk->bmpwin;

	GFL_BMPWIN_ClearScreen( bmpwin );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( bmpwin ) );

  GFL_BMPWIN_Delete( bmpwin );

  GFL_MSG_Delete( wk->msgman );
  WORDSET_Delete( wk->wordset );
}

//--------------------------------------------------------------
/// ���l���̓E�B���h�E�F�\���X�V
//--------------------------------------------------------------
static void printNumWin( DEBUG_NUMINPUT_WORK * wk, u32 num )
{
  GFL_BMPWIN * bmpwin = wk->bmpwin;

  STRBUF * strbuf = GFL_STR_CreateBuffer( 64, wk->heapID );
  STRBUF * expandBuf = GFL_STR_CreateBuffer( 64, wk->heapID );

  GFL_MSG_GetString( wk->msgman, dni_number_string, strbuf );
  WORDSET_RegisterNumber(wk->wordset, 0, num,
                         10, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( wk->wordset, expandBuf, strbuf );

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp( bmpwin ), WINCLR_COL(FBMP_COL_WHITE) );

  GFL_FONTSYS_SetColor( 1, 2, 15 );
	PRINTSYS_Print( GFL_BMPWIN_GetBmp( bmpwin ), 1, 10, expandBuf, wk->fontHandle );		

  GFL_BMPWIN_TransVramCharacter( bmpwin );  //transfer characters
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( bmpwin ) );  //transfer screen

  GFL_STR_DeleteBuffer( strbuf );
  GFL_STR_DeleteBuffer( expandBuf );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void dniSetValue( DEBUG_NUMINPUT_WORK * dni_wk , u32 value )
{
  const DEBUG_NUMINPUT_PARAM * def = dni_wk->dni_param;
  if (def->set_func == NULL) return;
  def->set_func( dni_wk->gsys, dni_wk->gamedata, dni_wk->select_id, value );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u32 dniGetValue( DEBUG_NUMINPUT_WORK * dni_wk )
{
  const DEBUG_NUMINPUT_PARAM * def = dni_wk->dni_param;
  if (def->get_func == NULL) return 0;
  return def->get_func( dni_wk->gsys, dni_wk->gamedata, dni_wk->select_id );
}
//--------------------------------------------------------------
/**
 * @brief �f�o�b�O���l���͐���C�x���g
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugNumInputEvent( GMEVENT * event , int *seq, void * work )
{
  DEBUG_NUMINPUT_WORK * dni_wk = work;
  EVENTWORK *evwork = GAMEDATA_GetEventWork( dni_wk->gamedata );
  const DEBUG_NUMINPUT_PARAM * def = dni_wk->dni_param;

  switch (*seq)
  {
  case 0:
    {
      dni_wk->value = dniGetValue( dni_wk );
      createNumWin( dni_wk );
      printNumWin( dni_wk, dni_wk->value );
      (*seq) ++;
      break;
    }
  case 1:
    {
      int trg = GFL_UI_KEY_GetRepeat();
      int cont = GFL_UI_KEY_GetCont();
      int diff;
      u32 before, after;
      if ( trg & PAD_BUTTON_B ) { //�L�����Z��
        (*seq) ++;
        break;
      }
      if ( trg & PAD_BUTTON_A ) { //����
        dniSetValue( dni_wk, dni_wk->value );
        (*seq) ++;
        break;
      }
      after = before = dni_wk->value;
      diff = 0;
      if (trg & PAD_KEY_UP){
        diff = 1;
      } else if (trg & PAD_KEY_DOWN) {
        diff = -1;
      } else if (trg & PAD_KEY_LEFT){
        diff = -10;
      } else if (trg & PAD_KEY_RIGHT){
        diff = 10;
      }
      if( cont & PAD_BUTTON_R ){
        diff *= 10;
      }else if( cont & PAD_BUTTON_L ){
        diff *= 100;
      }
      if(diff == 0){
        break;
      }
      if( diff < 0 && ( (after-def->min) < (diff*-1))){
        after = def->max;
      }else if( diff > 0 && ((def->max-after) < diff)){
        after = def->min;
      }else{
        after += diff;
      }

      if (after != before ) {
        printNumWin( dni_wk, after );
        dni_wk->value = after;
      }
    }
    break;
  case 2:
    deleteNumWin( dni_wk );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �f�o�b�O���l���͐���C�x���g����
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_NumInput(
    GAMESYS_WORK * gsys, const DEBUG_NUMINPUT_PARAM * dni_param, u32 id )
{
  GMEVENT * event;
  DEBUG_NUMINPUT_WORK * wk;
  event = GMEVENT_Create( gsys, NULL, DebugNumInputEvent, sizeof(DEBUG_NUMINPUT_WORK) );
  wk = GMEVENT_GetEventWork( event );

  wk->select_id = id;
  wk->dni_param = dni_param;
  wk->gsys = gsys;
  wk->gamedata = GAMESYSTEM_GetGameData( gsys );
  wk->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  wk->fontHandle = FLDMSGBG_GetFontHandle( FIELDMAP_GetFldMsgBG( wk->fieldmap ) );
  wk->heapID = FIELDMAP_GetHeapID( wk->fieldmap );

  return event;
}

//======================================================================
//
//
//  �f�o�b�O���l���͌Ăяo���F�t���O����
//
//
//======================================================================
//--------------------------------------------------------------
//  �t���O����pGetter/Setter
//--------------------------------------------------------------
static u32 DNUM_Getter_EventFlag( GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param )
{
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gamedata );
  return EVENTWORK_CheckEventFlag( evwork , param );
}

static void DNUM_Setter_EventFlag( GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param, u32 value )
{
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gamedata );
  if ( value == 0 ) {
    EVENTWORK_ResetEventFlag( evwork, param );
  } else {
    EVENTWORK_SetEventFlag( evwork, param );
  }
}
static const DEBUG_NUMINPUT_PARAM DNUM_DATA_EventFlag = {
  0,1,
  DNUM_Getter_EventFlag,
  DNUM_Setter_EventFlag,
};

//--------------------------------------------------------------
//  ���[�N����pGetter/Setter
//--------------------------------------------------------------
static u32 DNUM_Getter_EventWork( GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param )
{
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gamedata );
  u16 * work = EVENTWORK_GetEventWorkAdrs( evwork, param );
  return *work;
}

static void DNUM_Setter_EventWork( GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param, u32 value )
{
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gamedata );
  u16 * work = EVENTWORK_GetEventWorkAdrs( evwork, param );
  *work = value;
}

static const DEBUG_NUMINPUT_PARAM DNUM_DATA_EventWork = {
  0, 0x3fff,
  DNUM_Getter_EventWork,
  DNUM_Setter_EventWork,
};

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GAMEDATA * gamedata;
  FIELDMAP_WORK * fieldWork;
  
  DEBUG_MENU_CALLPROC call_proc;
  FLDMENUFUNC *menuFunc;

  const DEBUG_NUMINPUT_INITIALIZER * init;
}DEBUG_FLAGMENU_EVENT_WORK;

//--------------------------------------------------------------
/// ���j���[���ڍő吔�擾�F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static u16 DEBUG_GetDebugListMax( GAMESYS_WORK* gsys, void* cb_work )
{
  DEBUG_FLAGMENU_EVENT_WORK * df_work = cb_work;
  return df_work->init->count_or_id;
}

//--------------------------------------------------------------
/// ���j���[���ڐ����F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static void DEBUG_SetMenuWork_DebugList(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  DEBUG_FLAGMENU_EVENT_WORK * df_work = cb_work;
  const DEBUG_NUMINPUT_INITIALIZER * init = df_work->init;
  int id,max = DEBUG_GetDebugListMax(gsys, cb_work);
  GFL_MSGDATA * msgman = GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_numinput_dat, df_work->heapID);
  STRBUF *str = GFL_STR_CreateBuffer( 64, heapID );

  for( id = 0; id < max; id++ ){
    u16 real_id;
    GFL_STR_ClearBuffer( str );
    GFL_MSG_GetString( msgman, init->pList[id].gmm_id, str );
    FLDMENUFUNC_AddStringListData( list, str, id, heapID );
  }
  GFL_HEAP_FreeMemory( str );
  GFL_MSG_Delete( msgman );
}
//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�f�o�b�O���j���[�������f�[�^�F�f�o�b�O�t���O�E���[�N����
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugListSelectData = {
  0,
  0,                                ///<���ڍő吔�i�Œ胊�X�g�łȂ��ꍇ�A�O�j
  NULL,                             ///<�Q�Ƃ��郁�j���[�f�[�^�i��������ꍇ��NULL)
  &DATA_DebugMenuList_ZoneSel,      ///<���j���[�\���w��f�[�^�i���p�j
  1, 1, 20, 16,
  DEBUG_SetMenuWork_DebugList,    ///<���j���[�����֐��ւ̃|�C���^
  DEBUG_GetDebugListMax,          ///<���ڍő吔�擾�֐��ւ̃|�C���^
};

//--------------------------------------------------------------
/// ���j���[���ڍő吔�擾�F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static u16 DEBUG_GetDebugFlagMax( GAMESYS_WORK* gsys, void* cb_work )
{
  DEBUG_FLAGMENU_EVENT_WORK * df_work = cb_work;
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_DEBUG_SYM, df_work->heapID );
  u16 size = GFL_ARC_GetDataSizeByHandle(p_handle, df_work->init->count_or_id );
  GFL_ARC_CloseDataHandle( p_handle );
  GF_ASSERT( size % DEBUG_SCR_EACH_SIZE == 0 );
  return size / DEBUG_SCR_EACH_SIZE;
}

//--------------------------------------------------------------
/// ���j���[���ڐ����F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static void DEBUG_SetMenuWork_DebugFlag(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  DEBUG_FLAGMENU_EVENT_WORK * df_work = cb_work;
  int id,max = DEBUG_GetDebugFlagMax(gsys, cb_work);
  
  u8 buffer[DEBUG_SCR_EACH_SIZE];
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_DEBUG_SYM, heapID );
  u16 * utfStr = GFL_HEAP_AllocClearMemory( heapID, DEBUG_SCR_NAME_LEN * sizeof(u16) );
  STRBUF *strBuf = GFL_STR_CreateBuffer( DEBUG_SCR_NAME_LEN + 1, heapID );

  for( id = 0; id < max; id++ ){
    u16 real_id;
    GFL_STR_ClearBuffer( strBuf );
    {
      GFL_ARC_LoadDataOfsByHandle(p_handle, df_work->init->count_or_id,
          id * DEBUG_SCR_EACH_SIZE, DEBUG_SCR_EACH_SIZE, buffer);
      real_id = *((u16*)&buffer[DEBUG_SCR_OFS_ID]);
      DEB_STR_CONV_SjisToStrcode(
          (const char*)(buffer + DEBUG_SCR_OFS_NAME), utfStr, DEBUG_SCR_NAME_LEN );
      //OS_Printf("DEBUG Flag %5d:%s\n", real_id, buffer + DEBUG_SCR_OFS_NAME);
      GFL_STR_SetStringCode( strBuf, utfStr );
    }
    FLDMENUFUNC_AddStringListData( list, strBuf, real_id, heapID );
  }
  GFL_HEAP_FreeMemory( strBuf );
  GFL_HEAP_FreeMemory( utfStr );
  GFL_ARC_CloseDataHandle( p_handle );
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�f�o�b�O���j���[�������f�[�^�F�f�o�b�O�t���O�E���[�N����
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugFlagSelectData = {
  0,
  0,                                ///<���ڍő吔�i�Œ胊�X�g�łȂ��ꍇ�A�O�j
  NULL,                             ///<�Q�Ƃ��郁�j���[�f�[�^�i��������ꍇ��NULL)
  &DATA_DebugMenuList_ZoneSel,      ///<���j���[�\���w��f�[�^�i���p�j
  1, 1, 20, 16,
  DEBUG_SetMenuWork_DebugFlag,    ///<���j���[�����֐��ւ̃|�C���^
  DEBUG_GetDebugFlagMax,          ///<���ڍő吔�擾�֐��ւ̃|�C���^
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�f�o�b�O�X�N���v�g�I��
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugFlagWorkSelectMenuEvent(
    GMEVENT *event, int *seq, void *wk )
{
	DEBUG_FLAGMENU_EVENT_WORK * new_wk = wk;
  EVENTWORK *evwork = GAMEDATA_GetEventWork( new_wk->gamedata );
  const DEBUG_NUMINPUT_INITIALIZER * init = new_wk->init;
  GMEVENT * input_ev;
  
  switch( (*seq) ){
  case 0:
    {
      static const DEBUG_MENU_INITIALIZER * menu_init;
      switch ( init->data_type )
      {
      case D_NINPUT_DATA_LIST: 
        menu_init = &DebugListSelectData;
        break;
      case D_NINPUT_DATA_BIN:
        menu_init = &DebugFlagSelectData;
        break;
      default:
        GF_ASSERT(0);
      }
      new_wk->menuFunc = DEBUGFLDMENU_InitEx(
          new_wk->fieldWork, new_wk->heapID,  menu_init, new_wk );
    }
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( new_wk->menuFunc );
      
      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        FLDMENUFUNC_DeleteMenu( new_wk->menuFunc );
        return( GMEVENT_RES_FINISH );
      }
      switch ( init->data_type )
      {
      case D_NINPUT_DATA_LIST: 
        input_ev = DEBUG_EVENT_NumInput(
            new_wk->gmSys, init->pList[ret].dni_param, init->pList[ret].param );
        break;
      case D_NINPUT_DATA_BIN:
        input_ev = DEBUG_EVENT_NumInput( new_wk->gmSys, init->pList->dni_param, ret );
        break;
      default:
        GF_ASSERT(0);
      }
      GMEVENT_CallEvent( event, input_ev );
      break;
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * DEBUG_EVENT_FLDMENU_FlagWork(
    GAMESYS_WORK * gsys, const DEBUG_NUMINPUT_INITIALIZER * init )
{
	GMEVENT * new_event = GMEVENT_Create( gsys, NULL,
      debugFlagWorkSelectMenuEvent, sizeof(DEBUG_FLAGMENU_EVENT_WORK) );
	DEBUG_FLAGMENU_EVENT_WORK * new_wk = GMEVENT_GetEventWork( new_event );

	GFL_STD_MemClear( new_wk, sizeof(DEBUG_FLAGMENU_EVENT_WORK) );
  new_wk->heapID = HEAPID_PROC;
  new_wk->gmSys = gsys;
  new_wk->gamedata = GAMESYSTEM_GetGameData( gsys );
  new_wk->fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  new_wk->call_proc = NULL;
  new_wk->menuFunc = NULL;

  new_wk->init = init;

  return new_event;
}

//--------------------------------------------------------------
/**
 * @brief ���������l�Q�b�g
 */
//--------------------------------------------------------------
static u32 DebugGetGold(GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param)
{
  MISC * misc;
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  misc = SaveData_GetMisc( sv );
  return MISC_GetGold(misc);  
}

//--------------------------------------------------------------
/**
 * @brief ���������l�Z�b�g
 */
//--------------------------------------------------------------
static void DebugSetGold(GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param, u32 value)
{
  MISC * misc;
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  misc = SaveData_GetMisc( sv );
  MISC_SetGold(misc, value);
}



//======================================================================
/**
 * @file  scrcmd_tv.c
 * @brief  �X�N���v�g�R�}���h�F�e���r
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "event_fieldmap_control.h"
#include "scrcmd_tv.h"

#include "msg/script/msg_tv_01_scr.h"

#include "savedata/misc.h"               // for MISC

#define RECORD_TV_SEX_MAX (11)
#define RECORD_TV_COMMON_MAX (18)

#define RECORD_TV_MAX (29)    //�j����11�@����18
#define LECTURE_TV_MAX  (50)
#define VARIETY_TV_MAX (80)
#define NOTBADGE_RETRY_TV_MAX   (12)    //�o�b�W�����Ă��Ȃ������Ƃ��̍Ē��I�ԑg��
#define NOTCLEAR_RETRY_TV_MAX   (20)    //�N���A���ĂȂ������Ƃ��̍Ē��I�ԑg��

#define REC_TAG_MAX (2)

#define NO_REC_ID (0xffffffff)

#define BADGE_MAX (8)
#define AFTER_CL_MAX (15)

#ifdef PM_DEBUG
//�f�o�b�O�e���r�p�ԑg�ԍ� ���̂�misc.c
extern u32 DebugTvNo;
#define TV_MAX ( (RECORD_TV_SEX_MAX*2)+RECORD_TV_COMMON_MAX+LECTURE_TV_MAX+VARIETY_TV_MAX )  //11*2+18+50+80=170

#endif //PM_DEBUG

static const u32 RecordTbl[RECORD_TV_MAX][REC_TAG_MAX+2] = {    //{ ���R�[�h�h�c1�A���R�[�h�h�c2�A�h�c1�̕\�����A�h�c2�̕\���� }
  { NO_REC_ID, NO_REC_ID, 0, 0 },                               // 0  �Ȃ��A�Ȃ� 0,0
  { RECID_NICKNAME, NO_REC_ID, 6, 0 },                          // 1  �j�b�N�l�[���A�Ȃ� 6,0
  { RECID_REPORT_COUNT,  NO_REC_ID, 9, 0 },                     // 2  ���|�[�g�A�Ȃ� 9,0
  { RECID_RECOVERY_PC,  NO_REC_ID, 9, 0 },                      // 3  �|�P�Z���ł₷�񂾁A�Ȃ� 9,0
  { RECID_SHOPPING_CNT,  RECID_SHOPPING_MONEY, 9, 9 },          // 4  �������������A�g�p�������v���z  9,9
  { RECID_WATCH_TV,  NO_REC_ID, 9, 0 },                         // 5  �e���r�����A�Ȃ�  9,0
  { RECID_CAPTURE_POKE,  NO_REC_ID, 9, 0 },                     // 6  �ߊl�����|�P�����A�Ȃ�  9,0
  { RECID_BTL_ENCOUNT,  RECID_BTL_TRAINER, 9, 9 },              // 7  �G���J�E���g�A�g���[�i�[��  9,9
  { RECID_POKE_EVOLUTION,  NO_REC_ID, 9, 0 },                   // 8  �i���������|�P�����A�Ȃ�  9,0
  { RECID_FISHING_SUCCESS,  RECID_FISHING_FAILURE, 6, 4 },      // 9  �ނ�グ�A�΂炵  6,4
  { RECID_BATTLE_COUNT,  RECID_DENDOU_CNT, 9, 4 },              // 10  �S�퓬�񐔁A�a������ 9,4

  { RECID_RIDE_CYCLE,  NO_REC_ID, 9, 0 },                   // 11 ���]�ԁA�Ȃ�  9,0
  { RECID_TAMAGO_HATCHING,  NO_REC_ID, 6, 0 },              // 12 �^�}�S�z���A�Ȃ� 6,0
  { RECID_SODATEYA_CNT,  NO_REC_ID, 9, 0 },                 // 13 ��ĉ��A�Ȃ�  9,0
  { RECID_GTS_PUT,  NO_REC_ID, 6, 0 },                      // 14 �f�s�r�A�Ȃ�  6,0
  { RECID_MAIL_WRITE,  NO_REC_ID, 6, 0 },                   // 15 ���[���A�Ȃ�  6,0
  { RECID_PREMIUM_BALL,  NO_REC_ID, 6, 0 },                 // 16 �v���~�A�{�[���A�Ȃ�  6,0
  { RECID_RECOVERY_HOME,  NO_REC_ID, 4, 0 },                // 17 �Ƃŋx�񂾁A�Ȃ� 4, 0
  { RECID_WAZA_HANERU,  NO_REC_ID, 4, 0 },                  // 18 �͂˂�A�Ȃ� 4, 0
  { RECID_WAZA_WARUAGAKI,  NO_REC_ID, 4, 0 },               // 19 ��邠�����A�Ȃ� 4, 0
  { RECID_WAZA_MUKOU,  NO_REC_ID, 4, 0 },                   // 20 �������ʁA�Ȃ� 4, 0
  { RECID_NIGERU_SIPPAI,  NO_REC_ID, 4, 0 },                // 21 ���������Ȃ����A�Ȃ� 4, 0
  { RECID_NIGERARETA,  NO_REC_ID, 4, 0 },                   // 22 ������ꂽ�A�Ȃ� 4, 0
  { RECID_FOSSIL_RESTORE,  NO_REC_ID, 4, 0 },               // 23 ���Ε����A�Ȃ� 4, 0
  { RECID_GURUGURU_COUNT,  NO_REC_ID, 4, 0 },               // 24 ���邮������A�Ȃ� 4, 0
  { RECID_DAYMAX_KILL,  NO_REC_ID, 4, 0 },                  // 25 1���œ|�����|�P�����ō��A�Ȃ� 4, 0
  { RECID_DAYMAX_CAPTURE,  NO_REC_ID, 4, 0 },               // 26 1���ŕߊl�����|�P�����ō��A�Ȃ� 4, 0
  { RECID_DAYMAX_TRAINER_BATTLE,  NO_REC_ID, 4, 0 },        // 27 1���œ|�����g���[�i�[�ō��A�Ȃ� 4, 0
  { RECID_DAYMAX_EVOLUTION,  NO_REC_ID, 4, 0 },             // 28 1���Ői���������|�P�����ō��A�Ȃ� 4, 0
};

//�W���o�b�W�ɉe�����郁�b�Z�[�W
static const int BadgeMsg[BADGE_MAX] = {
  msg_tv_03_13,
  msg_tv_03_14,
  msg_tv_03_15,
  msg_tv_03_16,
  msg_tv_03_17,
  msg_tv_03_18,
  msg_tv_03_19,
  msg_tv_03_20
};

//�N���A�㒊�I���b�Z�[�W
static const int AfterClearMsg[AFTER_CL_MAX] = {
  msg_tv_03_46,
  msg_tv_03_47,
  msg_tv_03_48,
  msg_tv_03_49,
  msg_tv_03_50,
  msg_tv_03_51,
  msg_tv_03_52,
  msg_tv_03_53,
  msg_tv_03_54,
  msg_tv_03_55,
  msg_tv_03_56,
  msg_tv_03_57,
  msg_tv_03_58,
  msg_tv_03_59,
  msg_tv_03_60,
};

static int CheckBadge(GAMEDATA *gdata, const int inMsg);
static int CheckClear(GAMEDATA *gdata, const int inMsg, const BOOL inClear);

//--------------------------------------------------------------
/**
 * �e���r�ԑg���I
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTV_GetMsg( VMHANDLE *core, void *wk )
{
  u32 minute;
  int msg_base;
  int msg;
  int rnd;
  RTCTime time;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData(gsys);
  MYSTATUS *my = GAMEDATA_GetMyStatus(gdata);
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  u16 clear = SCRCMD_GetVMWorkValue( core, work );
  u16 *ret = SCRCMD_GetVMWork( core, work );

  //�Œ�^�O�W�J
  {
    //0�ԂɃv���[���[��
    WORDSET_RegisterPlayerName( wordset, 0, my );
    //3�ԂɎ莝���̐擪�|�P�����̎푰��(�^�}�S������)
    {
      int i;
      int num;
      POKEPARTY*     party = GAMEDATA_GetMyPokemon( gdata );
      num = PokeParty_GetPokeCount( party );
      for (i=0;i<num;i++)
      {
        BOOL tamago_flg;
        POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, i );
        //�^�}�S�ł͂Ȃ��|�P����
        tamago_flg = PP_Get( pp, ID_PARA_tamago_flag, NULL );
        if ( !tamago_flg )
        {
          u32 monsno = PP_Get( pp, ID_PARA_monsno, NULL );
          WORDSET_RegisterPokeMonsNameNo( wordset, 3, monsno );
          break;
        }
      }
      //�^�}�S�ł͂Ȃ��|�P�������������Ȃ�����
      if (i == num) GF_ASSERT(0);
    }
  }

  //���݂̎��ԁi�����擾�j
  GFL_RTC_GetTime(&time);
  minute = time.minute;
  if (minute < 10)        //���R�[�h�n
  {
    OS_Printf("���R�[�h�ԑg\n");
    msg_base = msg_tv_01_01;
    rnd = GFUser_GetPublicRand(RECORD_TV_MAX);
    OS_Printf("���R�[�h���I�i���o�[%d\n",rnd);
    //���R�[�h���ׂ�
    {
      int rec1 = 0;
      int rec2 = 0;
      RECORD * rec = GAMEDATA_GetRecordPtr(gdata);
      if ( RecordTbl[rnd][0] != NO_REC_ID )
      {
        rec1 = RECORD_Get(rec, RecordTbl[rnd][0]);
        //�^�O�W�J
        WORDSET_RegisterNumber( wordset, 1, rec1, RecordTbl[rnd][2], STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

      }
      if ( RecordTbl[rnd][1] != NO_REC_ID )
      {
        rec2 = RECORD_Get(rec, RecordTbl[rnd][1]);
        //�^�O�W�J
        WORDSET_RegisterNumber( wordset, 2, rec2, RecordTbl[rnd][3], STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      }

      OS_Printf("���R�[�h1 %d\n",rec1);
      OS_Printf("���R�[�h2 %d\n",rec2);

      //�j���ʃ��b�Z�[�W�̏ꍇ
      if (rnd < RECORD_TV_SEX_MAX)
      {
        //2�̃��R�[�h���K�v�ȏꍇ
        if ( (RecordTbl[rnd][0] != NO_REC_ID)&&(RecordTbl[rnd][1] != NO_REC_ID) )
        {
          //�Q�̃��R�[�h�����ꂩ���O�̂Ƃ��́A�f�t�H���g���b�Z�[�W�ɕς���
          if ( (rec1==0) || (rec2 == 0))
          {
            OS_Printf("�����R�[�h��0�Ȃ̂ŁA�f�t�H���g�ԑg�ɕύX\n");
            rnd = 0;
          }
        }
        //1�ڂ̃��R�[�h�����K�v�ȏꍇ(2�ڂ�NO_REC_ID�̏ꍇ)
        else if( RecordTbl[rnd][1] == NO_REC_ID )
        {
          //1�ڂ̃��R�[�h���O�̂Ƃ��́A�f�t�H���g���b�Z�[�W�ɕς���
          if ( rec1==0 )
          {
            OS_Printf("���R�[�h��0�Ȃ̂ŁA�f�t�H���g�ԑg�ɕύX\n");
            rnd = 0;
          }
        }
      }
    }

    if ( rnd < RECORD_TV_SEX_MAX )    //�j���ʃ��b�Z�[�W�̏ꍇ
    {
      u8 sex;
      //���ʂŕ���
      sex = MyStatus_GetMySex(my);
      if (sex == PTL_SEX_FEMALE)  //��
      {
        rnd += RECORD_TV_SEX_MAX;
      }
    }else                             //���ʃ��b�Z�[�W�̏ꍇ
    {
      rnd += RECORD_TV_SEX_MAX;
    }
  }
  else if(minute < 35)    //���N�`���[�n
  {
    OS_Printf("���N�`���[�ԑg\n");
    msg_base = msg_tv_02_01;
    rnd = GFUser_GetPublicRand(LECTURE_TV_MAX);
  }
  else                    //�o���G�e�B�n
  {
    OS_Printf("�o���G�e�B�ԑg\n");
    msg_base = msg_tv_03_01;
    rnd = GFUser_GetPublicRand(VARIETY_TV_MAX);
  }

  msg = msg_base+rnd;
  
  OS_Printf("�e���r�ԑg���I���ʁ@min = %d, rnd = %d, msg = %d\n",minute, rnd, msg);

#ifdef PM_DEBUG
  if (DebugTvNo)
  {
    OS_Printf("�f�o�b�O�ݒ肳��Ă���̂ŏ��������@%d ==> %d\n",msg, DebugTvNo-1);
    if (DebugTvNo > TV_MAX)
    {
      GF_ASSERT_MSG(0,"debug tv_no error %d",DebugTvNo);
      msg = TV_MAX-1;
    }
    else{
      msg = DebugTvNo-1;
    }

    if ( msg < RECORD_TV_SEX_MAX*2)
    {
      u8 idx = msg % RECORD_TV_SEX_MAX;
      //�^�O�W�J
      int rec1 = 0;
      int rec2 = 0;
      RECORD * rec = GAMEDATA_GetRecordPtr(gdata);
      if ( RecordTbl[idx][0] != NO_REC_ID )
      {
        rec1 = RECORD_Get(rec, RecordTbl[idx][0]);
        //�^�O�W�J
        WORDSET_RegisterNumber( wordset, 1, rec1, RecordTbl[idx][2], STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      }
      if ( RecordTbl[idx][1] != NO_REC_ID )
      {
        rec2 = RECORD_Get(rec, RecordTbl[idx][1]);
        //�^�O�W�J
        WORDSET_RegisterNumber( wordset, 2, rec2, RecordTbl[idx][3], STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      }

      OS_Printf("���R�[�h1 %d\n",rec1);
      OS_Printf("���R�[�h2 %d\n",rec2);

      //�j���ʃ��b�Z�[�W�̏ꍇ
      if (idx < RECORD_TV_SEX_MAX)
      {
        //2�̃��R�[�h���K�v�ȏꍇ
        if ( (RecordTbl[idx][0] != NO_REC_ID)&&(RecordTbl[idx][1] != NO_REC_ID) )
        {
          //�Q�̃��R�[�h�����ꂩ���O�̂Ƃ��́A�f�t�H���g���b�Z�[�W�ɕς���
          if ( (rec1==0) || (rec2 == 0))
          {
            OS_Printf("�����R�[�h��0�Ȃ̂ŁA�f�t�H���g�ԑg�ɕύX\n");
            if (msg < RECORD_TV_SEX_MAX ) msg = msg_tv_01_01;
            else msg = msg_tv_01_12;
          }
        }
        //1�ڂ̃��R�[�h�����K�v�ȏꍇ(2�ڂ�NO_REC_ID�̏ꍇ)
        else if( RecordTbl[idx][1] == NO_REC_ID )
        {
          //1�ڂ̃��R�[�h���O�̂Ƃ��́A�f�t�H���g���b�Z�[�W�ɕς���
          if ( rec1==0 )
          {
            OS_Printf("���R�[�h��0�Ȃ̂ŁA�f�t�H���g�ԑg�ɕύX\n");
            if (msg < RECORD_TV_SEX_MAX ) msg = msg_tv_01_01;
            else msg = msg_tv_01_12;
          }
        }
      }
    }
    else if ((RECORD_TV_SEX_MAX*2 <= msg)&& (msg<RECORD_TV_SEX_MAX*2+18)){
      u8 idx = msg - RECORD_TV_SEX_MAX;
      //�^�O�W�J
      int rec1 = 0;
      int rec2 = 0;
      RECORD * rec = GAMEDATA_GetRecordPtr(gdata);
      if ( RecordTbl[idx][0] != NO_REC_ID )
      {
        rec1 = RECORD_Get(rec, RecordTbl[idx][0]);
        //�^�O�W�J
        WORDSET_RegisterNumber( wordset, 1, rec1, RecordTbl[idx][2], STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      }
      if ( RecordTbl[idx][1] != NO_REC_ID )
      {
        rec2 = RECORD_Get(rec, RecordTbl[idx][1]);
        //�^�O�W�J
        WORDSET_RegisterNumber( wordset, 2, rec2, RecordTbl[idx][3], STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      }

      OS_Printf("���R�[�h1 %d\n",rec1);
      OS_Printf("���R�[�h2 %d\n",rec2);
    }
  }
#endif  //PM_DEBUG

  msg = CheckBadge(gdata, msg);
  msg = CheckClear(gdata, msg, clear);

  *ret = msg;

  return VMCMD_RESULT_CONTINUE;

}

//--------------------------------------------------------------
/**
 * �W���o�b�W�̏�����Ԃɂ�郁�b�Z�[�W���I
 * @param   gdata    �Q�[���f�[�^�|�C���^
 * @param   inMag     ���I���ꂽ���b�Z�[�W
 * @retval int      �Ē��I���ꂽ���b�Z�[�W
 */
//--------------------------------------------------------------
static int CheckBadge(GAMEDATA *gdata, const int inMsg)
{
  int i;
  int msg;
  MISC* misc;
  // �Z�[�u�f�[�^�擾
  {
    SAVE_CONTROL_WORK* save;
    save = GAMEDATA_GetSaveControlWork( gdata );
    misc = SaveData_GetMisc( save );
  }

  msg = inMsg;
  for (i=0;i<BADGE_MAX;i++)
  {
    if ( BadgeMsg[i] == msg )
    {
      //�o�b�W�`�F�b�N
      if ( MISC_GetBadgeFlag( misc, BADGE_ID_01+i ) )
      {
        //�����Ă���Ȃ�����n�j
        return msg;
      }
      else
      {
        //�����Ė����ꍇ�͕ʂ̃��b�Z�[�W
        msg = msg_tv_03_01 + GFUser_GetPublicRand(NOTBADGE_RETRY_TV_MAX);
        OS_Printf("%d�o�b�W�ԑg���I�������ǁA�����Ė�������ʂ̂ɕύX >> %d\n",i, msg);
        return msg;
      }
    }
  }
  //�o�b�W�֌W�Ȃ����b�Z�[�W
  return msg;
}

//--------------------------------------------------------------
/**
 * �N���A��Ԃɂ�郁�b�Z�[�W���I
 * @param   gdata    �Q�[���f�[�^�|�C���^
 * @param   inMag     ���I���ꂽ���b�Z�[�W
 * @param   inClear   �N���A���Ă邩�H
 * @retval int      �Ē��I���ꂽ���b�Z�[�W
 */
//--------------------------------------------------------------
static int CheckClear(GAMEDATA *gdata, const int inMsg, const BOOL inClear)
{
  int i;
  int msg;
  msg = inMsg;

  //�N���A���Ă��Ȃ��Ȃ�Ē��I����������
  if (!inClear)
  {
    for (i=0;i<AFTER_CL_MAX;i++)
    {
      if ( AfterClearMsg[i] == msg )
      {
        //�N���A���Ă��Ȃ��ꍇ�͕ʂ̃��b�Z�[�W
        msg = msg_tv_03_21 + GFUser_GetPublicRand(NOTCLEAR_RETRY_TV_MAX);
        OS_Printf("%d�N���A���ĂȂ�����ʂ̂ɕύX >> %d\n",i, msg);
        return msg;
      }
    }
  }
  //�N���A�֌W�Ȃ����b�Z�[�W
  return msg;
}

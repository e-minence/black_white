/////////////////////////////////////////////////////////////////////////////
/**
 * @breif ����Ⴂ������ �֘A�C�x���g
 * @file  event_research_team.c
 * @auhor obata
 * @date  2010.03.21
 */
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "event_research_team.h"

#include "gamesystem/gamesystem.h"   // for GAMESYSTEM
#include "gamesystem/game_data.h"    // for GAMEDATA
#include "savedata/save_control.h"   // for SAVE_CONTROL_WORK
#include "savedata/misc.h"           // for MISC
#include "field/field_msgbg.h"       // for FLDMSGBG, FLDMSGWIN
#include "field/research_team_def.h" // for RESEARCH_TEAM_RANK_xxxx
#include "print/wordset.h"           // for WORDSET

#include "scrcmd.h"   // for EVENT_WAIT_LAST_KEY
#include "fieldmap.h" // for FIELDMAP_xxxx

#include "arc/arc_def.h"             // for ARCID_xxxx
#include "arc/script_message.naix"   // for NARC_script_message_xxxx
#include "msg/script/msg_c03r0101.h" // for msg_c03r0101_xxxx


//===========================================================================
// ���萔
//===========================================================================
#define WINPOS_X   (1) // �E�B���h�E�̕\��x���W ( �L�����P�� )
#define WINPOS_Y   (1) // �E�B���h�E�̕\��y���W ( �L�����P�� )
#define WINSIZE_X (30) // �E�B���h�E�̕\��x�T�C�Y ( �L�����P�� )
#define WINSIZE_Y (10) // �E�B���h�E�̕\��y�T�C�Y ( �L�����P�� )


//===========================================================================
// ���C�x���g���[�N
//===========================================================================
typedef struct
{
  HEAPID         heapID;
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;
  FLDMSGBG*      fieldMsgBG;
  FLDMSGWIN*     fieldMsgWin;
  GFL_MSGDATA*   msgData;
  WORDSET*       wordset;

} EVENT_WORK;


//===========================================================================
// ���֐��C���f�b�N�X
//===========================================================================
static void InitEventWork( EVENT_WORK* work, GAMESYS_WORK* gameSystem ); // �C�x���g���[�N������������
static void CreateMsgData( EVENT_WORK* work ); // ���b�Z�[�W�f�[�^�𐶐�����
static void DeleteMsgData( EVENT_WORK* work ); // ���b�Z�[�W�f�[�^��j������
static void CreateFldMsgWin( EVENT_WORK* work ); // ���b�Z�[�W�E�B���h�E�𐶐�����
static void DeleteFldMsgWin( EVENT_WORK* work ); // ���b�Z�[�W�E�B���h�E��j������
static void CreateWordset( EVENT_WORK* work ); // ���[�h�Z�b�g�𐶐�����
static void DeleteWordset( EVENT_WORK* work ); // ���[�h�Z�b�g��j������
static void SetupWinMsg( EVENT_WORK* work ); // �E�B���h�E�ɕ\�����镶������Z�b�g�A�b�v����
static void ClearWinMsg( EVENT_WORK* work ); // �E�B���h�E�̕\�����N���A����
static int GetResearchTeamRank( const EVENT_WORK* work ); // ���������N���擾����
static int GetCrossCount( const EVENT_WORK* work ); // ����Ⴂ�l�����擾����
static int GetThanksCount( const EVENT_WORK* work ); // ����񐔂��擾����
static u32 GetStringID( int rank ); // �\�����郁�b�Z�[�WID���擾����


//---------------------------------------------------------------------------
/**
 * @brief �v���C���[�̑������\���C�x���g
 */
//---------------------------------------------------------------------------
static GMEVENT_RESULT DispResearchTeamInfoEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq ) {
  // ���b�Z�[�W��\��
  case 0:
    CreateMsgData( work );
    CreateFldMsgWin( work );
    CreateWordset( work );
    SetupWinMsg( work );
    (*seq)++;
    break;

  // �E�B���h�E�̕`��҂�
  case 1:
    if( FLDMSGWIN_CheckPrintTrans( work->fieldMsgWin ) == TRUE ) { (*seq)++; }
    break;

  // �L�[���͑҂�
  case 2:
    if( GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    ClearWinMsg( work );
    DeleteWordset( work );
    DeleteFldMsgWin( work );
    DeleteMsgData( work );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//---------------------------------------------------------------------------
/**
 * @brief �v���C���[�̑������\���C�x���g�𐶐�����
 *
 * @param gameSystem
 *
 * @return ���������C�x���g
 */
//---------------------------------------------------------------------------
GMEVENT* EVENT_DispResearchTeamInfo( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // �C�x���g�𐶐�
  event = GMEVENT_Create( 
      gameSystem, NULL, DispResearchTeamInfoEvent, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  InitEventWork( work, gameSystem );

  return event;
} 


//---------------------------------------------------------------------------
/**
 * @brief �C�x���g���[�N������������
 *
 * @param work �C�x���g���[�N
 * @param gameSystem 
 */
//---------------------------------------------------------------------------
static void InitEventWork( EVENT_WORK* work, GAMESYS_WORK* gameSystem )
{
  FIELDMAP_WORK* fieldmap;

  fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem );

  // �[���N���A
  GFL_STD_MemClear( work, sizeof(EVENT_WORK));

  // ������
  work->heapID      = FIELDMAP_GetHeapID( fieldmap );
  work->gameSystem  = gameSystem;
  work->gameData    = GAMESYSTEM_GetGameData( gameSystem );
  work->fieldmap    = fieldmap;
  work->fieldMsgBG  = FIELDMAP_GetFldMsgBG( fieldmap );
}

//---------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^�𐶐�����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static void CreateMsgData( EVENT_WORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->msgData == NULL );

  work->msgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                  ARCID_SCRIPT_MESSAGE, 
                                  NARC_script_message_c03r0101_dat, 
                                  work->heapID );
}

//---------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static void DeleteMsgData( EVENT_WORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->msgData );

  GFL_MSG_Delete( work->msgData );
  work->msgData = NULL;
}

//---------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�E�B���h�E�𐶐�����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static void CreateFldMsgWin( EVENT_WORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->msgData );
  GF_ASSERT( work->fieldMsgBG );
  GF_ASSERT( work->fieldMsgWin == NULL );

  work->fieldMsgWin = FLDMSGWIN_Add( work->fieldMsgBG, work->msgData, 
                                     WINPOS_X, WINPOS_Y, WINSIZE_X, WINSIZE_Y );
}

//---------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�E�B���h�E��j������
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static void DeleteFldMsgWin( EVENT_WORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->fieldMsgWin );

  FLDMSGWIN_Delete( work->fieldMsgWin );
  work->fieldMsgWin = NULL;
} 

//---------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g�𐶐�����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static void CreateWordset( EVENT_WORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->wordset == NULL );

  work->wordset = WORDSET_Create( work->heapID );
}

//---------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g��j������
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static void DeleteWordset( EVENT_WORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->wordset );

  WORDSET_Delete( work->wordset );
  work->wordset = NULL;
}

//---------------------------------------------------------------------------
/**
 * @brief �E�B���h�E�ɕ\�����镶������Z�b�g�A�b�v����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static void SetupWinMsg( EVENT_WORK* work )
{
  int rank;
  int crossCount;
  int thanksCount;
  u32 strID;
  STRBUF* strbuf;
  STRBUF* strbuf_ex;

  GF_ASSERT( work );
  GF_ASSERT( work->msgData );
  GF_ASSERT( work->fieldMsgWin );

  // �o�b�t�@���m��
  strbuf    = GFL_STR_CreateBuffer( 256, work->heapID );
  strbuf_ex = GFL_STR_CreateBuffer( 256, work->heapID );

  // �e��f�[�^���擾
  rank        = GetResearchTeamRank( work ); // ���������N
  crossCount  = GetCrossCount( work ); // ����Ⴂ�l��
  thanksCount = GetThanksCount( work ); // �����
  strID       = GetStringID( rank ); // ���b�Z�[�WID

  // �\�����镶������쐬
  { 
    // ���[�h�Z�b�g�o�^
    WORDSET_RegisterPlayerName( 
        work->wordset, 0, GAMEDATA_GetMyStatus( work->gameData ) );
    WORDSET_RegisterNumber( 
        work->wordset, 1, crossCount, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( 
        work->wordset, 2, thanksCount, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );

    // ���[�h�Z�b�g�W�J
    GFL_MSG_GetString( work->msgData, strID, strbuf );
    WORDSET_ExpandStr( work->wordset, strbuf_ex, strbuf ); 
    WORDSET_ClearAllBuffer( work->wordset );
  }

  // �쐬�����������\��
  FLDMSGWIN_PrintStrBuf( work->fieldMsgWin, 1, 0, strbuf_ex );
  GFL_BG_LoadScreenReq( 
      GFL_BMPWIN_GetFrame( FLDMSGWIN_GetBmpWin( work->fieldMsgWin ) ) );

  // �o�b�t�@�����
  GFL_STR_DeleteBuffer( strbuf );
  GFL_STR_DeleteBuffer( strbuf_ex );
}

//---------------------------------------------------------------------------
/**
 * @brief �E�B���h�E�̕\�����N���A����
 *
 * @param work
 */
//---------------------------------------------------------------------------
static void ClearWinMsg( EVENT_WORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->fieldMsgWin );

  FLDMSGWIN_ClearWindow( work->fieldMsgWin );
  GFL_BG_LoadScreenReq( 
      GFL_BMPWIN_GetFrame( FLDMSGWIN_GetBmpWin( work->fieldMsgWin ) ) );
}

//---------------------------------------------------------------------------
/**
 * @brief ���������N���擾����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static int GetResearchTeamRank( const EVENT_WORK* work )
{
  const SAVE_CONTROL_WORK* save;
  const MISC* misc;

  GF_ASSERT( work );
  GF_ASSERT( work->gameData );

  save = GAMEDATA_GetSaveControlWorkConst( work->gameData );
  misc = SaveData_GetMiscConst( save );

  return MISC_CrossComm_GetResearchTeamRank( misc );
}

//---------------------------------------------------------------------------
/**
 * @brief ����Ⴂ�l�����擾����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static int GetCrossCount( const EVENT_WORK* work )
{
  const SAVE_CONTROL_WORK* save;
  const MISC* misc;

  GF_ASSERT( work );
  GF_ASSERT( work->gameData );

  save = GAMEDATA_GetSaveControlWorkConst( work->gameData );
  misc = SaveData_GetMiscConst( save );

  return MISC_CrossComm_GetSuretigaiCount( misc );
}

//---------------------------------------------------------------------------
/**
 * @brief ����񐔂��擾����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------
static int GetThanksCount( const EVENT_WORK* work )
{
  const SAVE_CONTROL_WORK* save;
  const MISC* misc;

  GF_ASSERT( work );
  GF_ASSERT( work->gameData );

  save = GAMEDATA_GetSaveControlWorkConst( work->gameData );
  misc = SaveData_GetMiscConst( save );

  return MISC_CrossComm_GetThanksRecvCount( misc );
}

//---------------------------------------------------------------------------
/**
 * @brief �\�����郁�b�Z�[�WID���擾����
 *
 * @param rank ���������N
 *
 * @return �w�肵�����������N�̏���\�����邽�߂̃��b�Z�[�WID
 */
//---------------------------------------------------------------------------
static u32 GetStringID( int rank )
{
  switch( rank ) {
  case RESEARCH_TEAM_RANK_0: return msg_c03r0101_none_debug_01;
  case RESEARCH_TEAM_RANK_1: return msg_c03r0101_none_debug_02a;
  case RESEARCH_TEAM_RANK_2: return msg_c03r0101_none_debug_02b;
  case RESEARCH_TEAM_RANK_3: return msg_c03r0101_none_debug_02c;
  case RESEARCH_TEAM_RANK_4: return msg_c03r0101_none_debug_02d;
  case RESEARCH_TEAM_RANK_5: return msg_c03r0101_none_debug_02e;
  default: GF_ASSERT(0);
  }

  GF_ASSERT(0);
  return 0;
}

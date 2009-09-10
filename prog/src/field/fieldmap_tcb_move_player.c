#include <gflib.h>
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "fieldmap_tcb.h"
#include "fieldmap.h"
#include "field_player.h"


//==============================================================================
/**
 * @brief TCB���[�N
 */
//==============================================================================
typedef struct
{
  FIELDMAP_WORK* fieldmap;  // ����Ώۃt�B�[���h�}�b�v
  int               frame;  // ���݂̃t���[����
  int            endFrame;  // �ŏI�t���[����
  VecFx32        startPos;  // �ړ��J�n�ʒu
  VecFx32          endPos;  // �ŏI�ʒu
}
TCB_WORK;


//==============================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//==============================================================================
static void TCB_FUNC_MovePlayer( GFL_TCB* tcb, void* work );
static void DeleteTask( GFL_TCB* tcb, TCB_WORK* work );


//------------------------------------------------------------------------------
/**
 * @brief �v���C���[�̈ړ��^�X�N��ǉ�����
 *
 * @param fieldmap ����Ώۃt�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param pos      �ړ�����W
 */
//------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_MovePlayer(
    FIELDMAP_WORK* fieldmap, int frame, VecFx32* pos )
{
  HEAPID       heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*   tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
  TCB_WORK*       work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof(TCB_WORK) );

  // TCB���[�N������
  work->frame    = 0;
  work->endFrame = frame;
  work->fieldmap = fieldmap;
  FIELD_PLAYER_GetPos( player, &work->startPos );
  VEC_Set( &work->endPos, pos->x, pos->y, pos->z );

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_MovePlayer, work, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief TCB����֐�
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_MovePlayer( GFL_TCB* tcb, void* work )
{
  TCB_WORK*    tcbwork = (TCB_WORK*)work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( tcbwork->fieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player );

  // ���@�̍��W���X�V
  {
    VecFx32 v0, v1, v2;
    fx32 w1 = FX_F32_TO_FX32( (float)tcbwork->frame / (float)tcbwork->endFrame );
    fx32 w0 = FX32_ONE - w1;
    GFL_CALC3D_VEC_MulScalar( &tcbwork->startPos, w0, &v0 );
    GFL_CALC3D_VEC_MulScalar( &tcbwork->endPos,   w1, &v1 );
    VEC_Add( &v0, &v1, &v2 );
    FIELD_PLAYER_SetPos( player, &v2 );   // �ړ�
    MMDL_UpdateCurrentHeight( mmdl );   // �ڒn
  }

  // ��莞�ԓ��삵���玩�E����
  if( tcbwork->endFrame < ++tcbwork->frame )
  {
    DeleteTask( tcb, tcbwork );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��j������
 *
 * @param tcb �j������^�X�N
 */
//------------------------------------------------------------------------------------------
static void DeleteTask( GFL_TCB* tcb, TCB_WORK* work )
{ 
  // TCB���[�N��j��
  GFL_HEAP_FreeMemory( GFL_TCB_GetWork( tcb ) );

  // �^�X�N��j��
  GFL_TCB_DeleteTask( tcb );
}

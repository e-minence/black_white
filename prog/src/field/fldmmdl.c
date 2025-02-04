//======================================================================
/**
 * @file  fldmmdl.c
 * @brief  動作モデル
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#define MMDL_SYSTEM_FILE
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

#include "arc_def.h"
#include "arc/fieldmap/fldmmdl_mdlparam.naix"

#include "fieldmap.h"
#include "field/eventwork.h"

//OBJCHRWORK0
#include "../../../resource/fldmapdata/flagwork/work_define.h"

#include "fldmmdl_work.h"

#include "system/main.h"

#ifdef PM_DEBUG
#ifdef _NITRO
//@note　OBJCODE_PARAM この構造体のサイズを変更するとコンバーターの修正まで変更が及びます。
//@note  変更する際は、関係者に通達をお願いします。　20100517 satio
SDK_COMPILER_ASSERT(sizeof(OBJCODE_PARAM) == 28);
#endif
#endif		//PM_DEBUG

#define DAT_HEADER_SIZE  (2)
#define DAT_FOOTER_SIZE (2)
#define REGULAR_MAX  (26)

#define SYS_MDL_PRM_WORK_SIZE ( (sizeof(u16) * REGULAR_MAX ) + DAT_HEADER_SIZE + DAT_FOOTER_SIZE + sizeof(OBJCODE_PARAM) * REGULAR_MAX )

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------

//--------------------------------------------------------------
/// 同一XZ座標に配置可能な動作オブジェレイヤー数
//--------------------------------------------------------------
#define MMDL_POST_LAYER_MAX (16)  //FLDMAPPER_GRIDINFO_MAXと同じ値にしてます

//--------------------------------------------------------------
/// かいりき岩が参照、保存する座標データ数
//--------------------------------------------------------------
#define MMDL_ROCKPOS_MAX (30)
#define ROCKPOS_INIT (0xffffffff) ///<MMDL_ROCKPOS 座標初期化値

//--------------------------------------------------------------
///  エイリアスシンボル
//--------------------------------------------------------------
enum
{
  RET_ALIES_NOT = 0,  //エイリアスではない
  RET_ALIES_EXIST,  //エイリアスとして既に存在している
  RET_ALIES_CHANGE,  //エイリアス変更が必要である
};

//--------------------------------------------------------------
/// ワーク参照OBJコード
//--------------------------------------------------------------
#define WKOBJCODE_START (WKOBJCODE00)
#define WKOBJCODE_END (WKOBJCODE15)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  MMDLポインタ配列
//--------------------------------------------------------------
typedef struct{
  int count;  //格納された有効なポインタ数
  MMDL* mmdl_parray[MMDL_POST_LAYER_MAX];
}MMDL_PARRAY;

//--------------------------------------------------------------
/// MMDL_ROCKPOS
//--------------------------------------------------------------
struct _TAG_MMDL_ROCKPOS
{
  VecFx32 pos;
};

//======================================================================
//  proto
//======================================================================
//MMDLSYS プロセス
static void mmdlsys_DeleteProc( MMDLSYS *mmdlsys );

//MMDL 追加、削除
static MMDL * mmdlsys_AddMMdlCore( MMDLSYS *mmdlsys,
    const MMDL_HEADER *header, int zone_id, const EVENTWORK *ev );
static void mmdl_SetHeaderMoveParam(
    MMDL * mmdl, const MMDL_HEADER *head );
static void mmdl_CallSetHeaderPos(
    MMDL * mmdl, const MMDL_HEADER *head );
static void mmdl_SetHeaderBefore( MMDL * mmdl, const MMDL_HEADER *head,
    const EVENTWORK *ev, const MMDLSYS *mmdlsys );
static void mmdl_SetHeaderAfter(
  MMDL * mmdl, const MMDL_HEADER *head, void *sys );
static void mmdl_SetHeaderPos(MMDL *mmdl,const MMDL_HEADER *head);
static void mmdl_InitWork( MMDL * mmdl, MMDLSYS *sys, int zone_id );
static void mmdl_InitDir( MMDL *mmdl );
static void mmdl_InitCallMoveProcWork( MMDL * mmdl );
static void mmdl_InitMoveWork( const MMDLSYS *mmdlsys, MMDL * mmdl );
static void mmdl_InitMoveProc( const MMDLSYS *mmdlsys, MMDL * mmdl );
static void mmdl_UpdateMove( MMDL * mmdl );
#if 0
static void mmdlsys_CheckSetInitMoveWork( MMDLSYS *mmdlsys );
static void mmdlsys_CheckSetInitDrawWork( MMDLSYS *mmdlsys );
#endif

//MMDL 動作関数
static void mmdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void mmdl_TCB_DrawProc( MMDL * mmdl );

//MMDLSYS 設定、参照
static void mmdlsys_OnStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit );
static void mmdlsys_OffStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit );
static void mmdlsys_IncrementOBJCount( MMDLSYS *mmdlsys );
static void mmdlsys_DecrementOBJCount( MMDLSYS *mmdlsys );

//MMDLSYS ツール
static MMDL * mmdlsys_SearchSpaceMMdl( const MMDLSYS *sys );
static MMDL * mmdlsys_SearchAlies(
  const MMDLSYS *mmdlsys, int obj_id, int zone_id );

//MMDL ツール
static void mmdl_AddTCB( MMDL *mmdl, const MMDLSYS *sys );
static void mmdl_DeleteTCB( MMDL *mmdl );
static void mmdl_InitDrawWork( MMDL *mmdl );
static void mmdl_InitCallDrawProcWork( MMDL * mmdl );
static void mmdl_InitDrawEffectFlag( MMDL * mmdl );
static void mmdl_ClearWork( MMDL *mmdl );
static int mmdl_CheckHeaderAlies(
    const MMDL *mmdl, int h_zone_id, int max,
    const MMDL_HEADER *head );
static MMDL * mmdl_SearchOBJIDZoneID(
    const MMDLSYS *mmdlsys, int obj_id, int zone_id );
static void mmdl_InitDrawStatus( MMDL * mmdl );
static void mmdl_SetDrawDeleteStatus( MMDL * mmdl );
static void mmdl_ChangeAliesOBJ(
  MMDL *mmdl, const MMDL_HEADER *head, int zone_id );
static void mmdl_ChangeOBJAlies(
  MMDL * mmdl, int zone_id, const MMDL_HEADER *head );

//OBJCODE_PARAM
static void mmdlsys_InitOBJCodeParam( MMDLSYS *mmdlsys, HEAPID heapID );
static void mmdlsys_DeleteOBJCodeParam( MMDLSYS *mmdlsys );
static void mmdlsys_LoadSetMMdlOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, MMDL *mmdl );

//parts
static u16 WorkOBJCode_GetOBJCode( const EVENTWORK *ev, u16 code );
static const MMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code );
static const MMDL_DRAW_PROC_LIST * DrawProcList_GetList( MMDL_DRAWPROCNO no );
static BOOL MMdlHeader_CheckAlies( const MMDL_HEADER *head );
static int MMdlHeader_GetAliesZoneID( const MMDL_HEADER *head );
static BOOL mmdlsys_CheckEventFlag( EVENTWORK *evwork, u16 flag_no );

//MMDL_ROCKPOS
static BOOL mmdl_rockpos_CheckSetPos( const MMDL_ROCKPOS *rockpos );
static int rockpos_GetPosNumber( const u16 zone_id, const u16 obj_id );
static int mmdl_rockpos_GetPosNumber( const MMDL *mmdl );
static BOOL mmdl_rockpos_CheckPos( const MMDL *mmdl );
static BOOL mmdl_rockpos_GetPos( const MMDL *mmdl, VecFx32 *pos );

//======================================================================
//  フィールド動作モデル　システム
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS システム作成
 * @param  heapID  HEAPID
 * @param  max  動作モデル最大数
 * @param rockpos MMDL_ROCKPOS 怪力岩が参照、保存するデータ
 * @retval  MMDLSYS* 作成されたMMDLSYS*
 */
//--------------------------------------------------------------
MMDLSYS * MMDLSYS_CreateSystem(
    HEAPID heapID, u32 max, MMDL_ROCKPOS *rockpos )
{
  MMDLSYS *mmdlsys;
  mmdlsys = GFL_HEAP_AllocClearMemory( heapID, MMDLSYS_SIZE );
  mmdlsys->pMMdlBuf = GFL_HEAP_AllocClearMemory( heapID, max*MMDL_SIZE );
  mmdlsys->mmdl_max = max;
  mmdlsys->sysHeapID = heapID;
  mmdlsys->rockpos = rockpos;
  mmdlsys->tcb_pri = MMDL_TCBPRI_STANDARD;
  mmdlsys_InitOBJCodeParam( mmdlsys, heapID );

  {
    u32 size;
    size = SYS_MDL_PRM_WORK_SIZE;

    mmdlsys->pMdlPrmWork = GFL_HEAP_AllocClearMemory( heapID, size );
  }

  return( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS システム開放
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_FreeSystem( MMDLSYS *mmdlsys )
{
  mmdlsys_DeleteOBJCodeParam( mmdlsys );
  GFL_HEAP_FreeMemory( mmdlsys->pMdlPrmWork );
  GFL_HEAP_FreeMemory( mmdlsys->pMMdlBuf );
  GFL_HEAP_FreeMemory( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS システムが所持するモデルパラメータをクリア
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_ClearSysOBJCodeParam( MMDLSYS *mmdlsys )
{
  u32 size;
  size = SYS_MDL_PRM_WORK_SIZE;
  MI_CpuClear8(mmdlsys->pMdlPrmWork, size);
}

//======================================================================
//  フィールド動作モデル　システム　プロセス
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS システム 動作プロセスセットアップ
 * @param  mmdlsys  MMDLSYS*
 * @param  heapID  プロセス用HEAPID
 * @param  pG3DMapper FLDMAPPER
 * @param  pNOGRIDMapper FLDNOGRID_MAPPER
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetupProc( MMDLSYS *mmdlsys, HEAPID heapID,
    GAMEDATA *gdata, FIELDMAP_WORK *fieldmap,
    const FLDMAPPER *pG3DMapper, FLDNOGRID_MAPPER* pNOGRIDMapper )
{
  mmdlsys->heapID = heapID;
  mmdlsys->pG3DMapper = pG3DMapper;
  mmdlsys->gdata = gdata;
  mmdlsys->fieldMapWork = fieldmap;
  
  mmdlsys->pTCBSysWork = GFL_HEAP_AllocMemory(
    heapID, GFL_TCB_CalcSystemWorkSize(mmdlsys->mmdl_max) );
  mmdlsys->pTCBSys = GFL_TCB_Init( mmdlsys->mmdl_max, mmdlsys->pTCBSysWork );
  
  mmdlsys->arcH_res = GFL_ARC_OpenDataHandle( ARCID_MMDL_RES, heapID );
  
  // ノーグリッド移動設定
  mmdlsys->pNOGRIDMapper = pNOGRIDMapper;
  mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS システム　動作プロセス削除
 * @param  mmdlsys    MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_DeleteProc( MMDLSYS *mmdlsys )
{
  GFL_TCB_Exit( mmdlsys->pTCBSys );
  GFL_HEAP_FreeMemory( mmdlsys->pTCBSysWork );
  GFL_ARC_CloseDataHandle( mmdlsys->arcH_res );
  mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 全てのプロセス削除
 * @param  mmdlsys    MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteProc( MMDLSYS *mmdlsys )
{
//  MMDLSYS_DeleteMMdl( mmdlsys );
  
  { //いずれfldmmdl.c内で処理する。
    MMDLSYS_DeleteDraw( mmdlsys );
    mmdlsys->targetCameraAngleYaw = NULL;
  }
  
  mmdlsys_DeleteProc( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 動作モデル更新
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_UpdateProc( MMDLSYS *mmdlsys )
{
  GFL_TCBSYS *tcbsys = mmdlsys->pTCBSys;
  GF_ASSERT( tcbsys != NULL );
  GFL_TCB_Main( tcbsys );
  MMDL_BLACTCONT_Update( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS Vブランク処理
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_VBlankProc( MMDLSYS *mmdlsys )
{
  if( mmdlsys->pBlActCont != NULL ){
    MMDL_BLACTCONT_ProcVBlank( mmdlsys );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS 強制VBlank処理
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 * @attention 内部でVブランク待ちを行います。
 * MMDL_BLACTCONT_IsThereReserve()で追加要素が無くなるまで戻りません。
 * その間、メイン処理が止まりますので呼ぶ際は注意して下さい。
 */
//--------------------------------------------------------------
void MMDLSYS_ForceWaitVBlankProc( MMDLSYS *mmdlsys )
{
  while( MMDL_BLACTCONT_IsThereReserve(mmdlsys) == TRUE ){
    MMDLSYS_UpdateProc( mmdlsys );
    OS_WaitInterrupt( TRUE, OS_IE_V_BLANK );
    MMDLSYS_VBlankProc( mmdlsys );
    D_MMDL_DPrintf( "MMDLSYS_ForceVBlankProc()\n" );
  }
}

//======================================================================
//  フィールド動作モデル　システム　描画プロセス
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS システム　描画プロセスセットアップ
 * @param  mmdlsys  MMDLSYS*
 * @param  heapID  プロセス用HEAPID
 * @param  pG3DMapper FLDMAPPER
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetupDrawProc( MMDLSYS *mmdlsys, const u16 *angleYaw )
{
  mmdlsys->targetCameraAngleYaw = angleYaw;
  MMDLSYS_SetCompleteDrawInit( mmdlsys, TRUE );
}

//======================================================================
//  フィールド動作モデル追加、削除
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデルを追加　コア部分
 * @param  mmdlsys      MMDLSYS *
 * @param  header    追加する情報を纏めたMMDL_HEADER *
 * @param  zone_id    ゾーンID
 * @param ev EVENTWORK*
 * @retval  MMDL  追加されたMMDL *
 * @attention ev==NULL状態の際に
 * WKOBJCODE00等のワーク参照型OBJコードが来るとエラーとなる。
 */
//--------------------------------------------------------------
static MMDL * mmdlsys_AddMMdlCore( MMDLSYS *mmdlsys,
    const MMDL_HEADER *header, int zone_id, const EVENTWORK *ev )
{
  MMDL *mmdl;
  MMDL_HEADER header_data = *header;
  const MMDL_HEADER *head = &header_data;
  mmdl = mmdlsys_SearchSpaceMMdl( mmdlsys );
  GF_ASSERT( mmdl != NULL );
  mmdl_InitWork( mmdl, mmdlsys, zone_id );
  mmdl_SetHeaderBefore( mmdl, head, ev, mmdlsys );
  mmdl_InitDir( mmdl );
  
  if( mmdl_rockpos_CheckPos(mmdl) == TRUE ){
    MMDL_OnStatusBit( mmdl,
        MMDL_STABIT_FELLOW_HIT_NON |
        MMDL_STABIT_HEIGHT_GET_OFF );
  }
  
  if( MMDLSYS_CheckStatusBit(mmdlsys,MMDLSYS_STABIT_MOVE_INIT_COMP) ){
    mmdl_InitMoveWork( mmdlsys, mmdl );
    mmdl_InitMoveProc( mmdlsys, mmdl );
  }
  
  if( MMDLSYS_CheckStatusBit(mmdlsys,MMDLSYS_STABIT_DRAW_INIT_COMP) ){
    mmdl_InitDrawWork( mmdl );
  }
  
  mmdlsys_IncrementOBJCount( (MMDLSYS*)MMDL_GetMMdlSys(mmdl) );
  
  mmdl_SetHeaderAfter( mmdl, head, NULL );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデルを追加
 * @param  mmdlsys      MMDLSYS *
 * @param  header    追加する情報を纏めたMMDL_HEADER *
 * @param  zone_id    ゾーンID
 * @retval  MMDL  追加されたMMDL *
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdl(
  MMDLSYS *mmdlsys, const MMDL_HEADER *header, int zone_id )
{
  MMDL *mmdl;
  mmdl = mmdlsys_AddMMdlCore( mmdlsys, header, zone_id, NULL );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 動作モデルを追加。必要最低限な情報からヘッダーを作成して追加。
 * 足りない情報は0でクリアされている。必要あれば追加後に各自で設定する。
 * @param mmdlsys MMDLSYS
 * @param x 初期X座標 グリッド単位
 * @param z 初期Z座標 グリッド単位
 * @param dir 初期方向。DIR_UP等
 * @param id OBJ ID
 * @param code OBJコード。HERO等
 * @param move 動作コード。MV_RND等
 * @param zone_id ZONE_ID
 * @retval MMDL 追加されたMMDL*
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdlParam( MMDLSYS *mmdlsys,
    s16 gx, s16 gz, u16 dir,
    u16 id, u16 code, u16 move, int zone_id )
{
  MMDL *mmdl;
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *gridpos;
  MI_CpuClear8( &head, sizeof(MMDL_HEADER) );
  
  head.id = id;
  head.obj_code = code;
  head.move_code = move;
  head.dir = dir;
  head.pos_type = MMDL_HEADER_POSTYPE_GRID;
  gridpos = (MMDL_HEADER_GRIDPOS*)head.pos_buf;
  gridpos->gx = gx;
  gridpos->gz = gz;
  
  mmdl = MMDLSYS_AddMMdl( mmdlsys, &head, zone_id );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデルを追加　複数
 * @param  mmdlsys      MMDLSYS *
 * @param  header    追加する情報を纏めたMMDL_HEADER *
 * @param  zone_id    ゾーンID
 * @param  count    header要素数
 * @retval  nothing
 * @note イベントフラグが立っているヘッダーは追加しない。
 */
//--------------------------------------------------------------
void MMDLSYS_SetMMdl( MMDLSYS *mmdlsys,
  const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork )
{
  GF_ASSERT( count > 0 );
  GF_ASSERT( header != NULL );
  
  D_MMDL_DPrintf( "MMDLSYS_SetMMdl Count %d\n", count );
  do{
    if( MMdlHeader_CheckAlies(header) == TRUE ||
        mmdlsys_CheckEventFlag(eventWork,header->event_flag) == FALSE ){
      mmdlsys_AddMMdlCore( mmdlsys, header, zone_id, eventWork );
    }else{
      D_MMDL_DPrintf( "ADD STOP MMDL OBJID=%d,EVENT FLAG=%xH\n",
          header->id, header->event_flag );
    }
    
    header++;
    count--;
  }while( count );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデルを追加　ヘッダー内の指定OBJID
 * @param  mmdlsys      MMDLSYS *
 * @param  header    追加する情報を纏めたMMDL_HEADER *
 * @param  zone_id    ゾーンID
 * @param  count    header要素数
 * @param id 追加するOBJ ID
 * @retval  nothing
 * @note イベントフラグが立っているヘッダーは追加しない。
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdlHeaderID( MMDLSYS *mmdlsys,
  const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork,
  u16 objID )
{
  MMDL *mmdl;

  GF_ASSERT( count > 0 );
  GF_ASSERT( header != NULL );
  
  D_MMDL_DPrintf( "MMDLSYS_SetMMdlHeaderID Count %d\n", count );
  
  do{
    if( MMdlHeader_CheckAlies(header) == FALSE )
    {
      if( header->id == objID )
      {
        if( mmdlsys_CheckEventFlag(
              eventWork,header->event_flag) == FALSE )
        {
          mmdl = mmdlsys_AddMMdlCore( mmdlsys, header, zone_id, eventWork );
          break;
        }
      }
    }
    
    header++;
    count--;
  }while( count );
  
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデルを削除
 * @param  mmdl    削除するMMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_Delete( MMDL * mmdl )
{
  const MMDLSYS *mmdlsys;
  
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
    MMDL_CallDrawDeleteProc( mmdl );
  }
  
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVEPROC_INIT) ){
    MMDL_CallMoveDeleteProc( mmdl );
    mmdl_DeleteTCB( mmdl );
  }
  
  mmdlsys_DecrementOBJCount( (MMDLSYS*)(mmdl->pMMdlSys) );
  mmdl_ClearWork( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデルを削除　イベントフラグ OBJ非表示フラグをONに。
 * @param  mmdl    削除するMMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DeleteEvent( MMDL * mmdl, EVENTWORK *evwork )
{
  EVENTWORK_SetEventFlag( evwork, MMDL_GetEventFlag(mmdl) );
  MMDL_Delete( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 現在発生しているフィールド動作モデルを全て削除
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteMMdl( const MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
    if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
      MMDL_CallDrawDeleteProc( mmdl );
    }
    
    if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVEPROC_INIT) ){
      //MMDL_CallMoveDeleteProc( mmdl );
      //mmdl_DeleteTCB( mmdl );
    }
    
    mmdlsys_DecrementOBJCount( (MMDLSYS*)(mmdl->pMMdlSys) );
    mmdl_ClearWork( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　ヘッダー情報反映　動作関連
 * @param  mmdl    設定するMMDL * 
 * @param  head    反映する情報を纏めたMMDL_HEADER *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderMoveParam(
    MMDL * mmdl, const MMDL_HEADER *head )
{
  MMDL_SetOBJID( mmdl, head->id );
  MMDL_SetMoveCode( mmdl, head->move_code );
  MMDL_SetEventType( mmdl, head->event_type );
  MMDL_SetEventFlag( mmdl, head->event_flag );
  MMDL_SetEventID( mmdl, head->event_id );
  mmdl->dir_head = head->dir;
  mmdl->dir_move = head->dir;
  MMDL_SetParam( mmdl, head->param0, MMDL_PARAM_0 );
  MMDL_SetParam( mmdl, head->param1, MMDL_PARAM_1 );
  MMDL_SetParam( mmdl, head->param2, MMDL_PARAM_2 );
  MMDL_SetMoveLimitX( mmdl, head->move_limit_x );
  MMDL_SetMoveLimitZ( mmdl, head->move_limit_z );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　ヘッダーから
 * @param  mmdl    設定するMMDL * 
 * @param  head    反映する情報を纏めたMMDL_HEADER *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_CallSetHeaderPos(
    MMDL * mmdl, const MMDL_HEADER *head )
{
  // 座標タイプにより、位置の初期化方法を変更
  if( head->pos_type == MMDL_HEADER_POSTYPE_GRID )
  {
    mmdl_SetHeaderPos( mmdl, head );
  }
  else
  {
    MMDL_SetRailHeaderBefore( mmdl, head );
  }
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　ヘッダー情報反映
 * @param  mmdl    設定するMMDL * 
 * @param  head    反映する情報を纏めたMMDL_HEADER *
 * @param  ev      EVENTWORK*
 * @retval  nothing
 * @attention ev==NULL状態の際に
 * WKOBJCODE00等のワーク参照型OBJコードが来るとエラーとなる。
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderBefore(
    MMDL * mmdl, const MMDL_HEADER *head,
    const EVENTWORK *ev, const MMDLSYS *mmdlsys )
{
  u16 obj_code;
  const OBJCODE_PARAM *param;
  
  mmdl_SetHeaderMoveParam( mmdl, head );
  
  obj_code = WorkOBJCode_GetOBJCode( ev, head->obj_code );
  MMDL_SetOBJCode( mmdl, obj_code );
  mmdlsys_LoadSetMMdlOBJCodeParam( mmdlsys, obj_code, mmdl );
  
  param = &mmdl->objcode_param;
  
  if( obj_code == STOPPER ){ //サイズ設定。STOPPERはサイズ指定アリ
    mmdl->gx_size = head->param0;
    mmdl->gz_size = head->param1;
    
    if( mmdl->gx_size == 0 ){
      mmdl->gx_size++;
    }
    
    if( mmdl->gz_size == 0 ){
      mmdl->gz_size++;
    }
  }else{
    mmdl->gx_size = param->size_width;
    mmdl->gz_size = param->size_depth;
  }
  
  mmdl->offset_x = param->offs_x; //オフセット
  mmdl->offset_y = param->offs_y;
  mmdl->offset_z = param->offs_z;
  
  mmdl_CallSetHeaderPos( mmdl, head );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　ヘッダー情報反映  動作モデルの登録完了後処理
 * @param  mmdl    設定するMMDL * 
 * @param  head    反映する情報を纏めたMMDL_HEADER *
 * @param  fsys    FIELDSYS_WORK *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderAfter(
  MMDL * mmdl, const MMDL_HEADER *head, void *sys )
{
  // 座標タイプにより、位置の初期化方法を変更
  if( head->pos_type == MMDL_HEADER_POSTYPE_RAIL )
  {
    MMDL_SetRailHeaderAfter( mmdl, head );
  }
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　座標系初期化
 * @param  mmdl    MMDL * 
 * @param  head    反映する情報を纏めたMMDL_HEADER *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderPos( MMDL *mmdl, const MMDL_HEADER *head )
{
  fx32 set_y;
  VecFx32 vec;
  int pos,set_gx,set_gz;
  const MMDL_HEADER_GRIDPOS *gridpos;
  
  GF_ASSERT( head->pos_type == MMDL_HEADER_POSTYPE_GRID );
  
  gridpos = (const MMDL_HEADER_GRIDPOS*)head->pos_buf;
  
  if( mmdl_rockpos_CheckPos(mmdl) == FALSE ){
    set_gx = gridpos->gx;
    set_gz = gridpos->gz;
    set_y = gridpos->y;
  }else{
    mmdl_rockpos_GetPos( mmdl, &vec );
    set_gx = SIZE_GRID_FX32( vec.x );
    set_gz = SIZE_GRID_FX32( vec.z );
    set_y = vec.y;
  }
  
  vec.x = GRID_SIZE_FX32( set_gx ) + MMDL_VEC_X_GRID_OFFS_FX32;
  MMDL_SetInitGridPosX( mmdl, gridpos->gx );
  MMDL_SetOldGridPosX( mmdl, set_gx );
  MMDL_SetGridPosX( mmdl, set_gx );
  
  pos = SIZE_GRID_FX32( gridpos->y );    //pos設定はfx32型で来る。
  MMDL_SetInitGridPosY( mmdl, pos );
  
  vec.y = set_y;
  pos = SIZE_GRID_FX32( set_y );
  MMDL_SetOldGridPosY( mmdl, pos );
  MMDL_SetGridPosY( mmdl, pos );
  
  vec.z = GRID_SIZE_FX32( set_gz ) + MMDL_VEC_Z_GRID_OFFS_FX32;
  MMDL_SetInitGridPosZ( mmdl, gridpos->gz );
  MMDL_SetOldGridPosZ( mmdl, set_gz );
  MMDL_SetGridPosZ( mmdl, set_gz );
  
  MMDL_SetVectorPos( mmdl, &vec );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　ワーク初期化
 * @param  mmdl    MMDL * 
 * @param  sys      MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitWork( MMDL * mmdl, MMDLSYS *sys, int zone_id )
{
  mmdl->pMMdlSys = sys;
  MMDL_SetZoneID( mmdl, zone_id );
  
  MMDL_OnStatusBit( mmdl, MMDL_STABIT_USE );
  MMDL_OnMoveBit( mmdl,
    MMDL_MOVEBIT_HEIGHT_GET_ERROR |  //高さ取得が必要
    MMDL_MOVEBIT_ATTR_GET_ERROR );  //アトリビュート取得が必要
  
  if( MMDL_CheckAliesEventID(mmdl) == TRUE ){
    MMDL_SetStatusBitAlies( mmdl, TRUE );
  }
  
  MMDL_FreeAcmd( mmdl );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル　方向初期化
 * @param mmdl 
 * @retval
 */
//--------------------------------------------------------------
static void mmdl_InitDir( MMDL *mmdl )
{
  MMDL_SetForceDirDisp( mmdl, MMDL_GetDirHeader(mmdl) );
  MMDL_SetDirMove( mmdl, MMDL_GetDirHeader(mmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル 動作関数初期化
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitCallMoveProcWork( MMDL * mmdl )
{
  mmdl->move_proc_list =
    MoveProcList_GetList( MMDL_GetMoveCode(mmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL 動作初期化に行う処理纏め
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitMoveWork( const MMDLSYS *mmdlsys, MMDL *mmdl )
{
  mmdl_InitCallMoveProcWork( mmdl );
  mmdl_AddTCB( mmdl, mmdlsys );
  MMDL_OnMoveBit( mmdl,
      MMDL_MOVEBIT_MOVE_START |
      MMDL_MOVEBIT_ATTR_GET_ERROR |
      MMDL_MOVEBIT_HEIGHT_GET_ERROR );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル 動作初期化
 *  @param  mmdlsys
 *  @param  mmdl 
 */
//--------------------------------------------------------------
static void mmdl_InitMoveProc( const MMDLSYS *mmdlsys, MMDL * mmdl )
{
  if( !MMDL_CheckStatusBit(mmdl,MMDL_STABIT_RAIL_MOVE) )
  {
    MMDL_InitMoveProc( mmdl );
  }
  else
  {
    MMDL_InitRailMoveProc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル動作
 *
 *  @param  mmdl 
 */
//--------------------------------------------------------------
static void mmdl_UpdateMove( MMDL * mmdl )
{
  GF_ASSERT( mmdl );
  if( !MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) )
  {
    MMDL_UpdateMove( mmdl );
  }
  else
  {
    MMDL_UpdateRailMove( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS 動作初期化を行っていない動作モデルに対して初期化処理をセット
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
#if 0
static void mmdlsys_CheckSetInitMoveWork( MMDLSYS *mmdlsys )
{
  u32 i = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&i) == TRUE ){
    if( MMDL_CheckStatusBit(mmdl,  //初期化関数呼び出しまだ
      MMDL_STABIT_MOVEPROC_INIT) == 0 ){
      mmdl_InitMoveProc( mmdl );
    }
  }
}
#endif

//--------------------------------------------------------------
/**
 * MMDLSYS 描画初期化を行っていない動作モデルに対して初期化処理をセット
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
#if 0
static void mmdlsys_CheckSetInitDrawWork( MMDLSYS *mmdlsys )
{
  u32 i = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&i) == TRUE ){
    if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == FALSE ){
      mmdl_InitDrawWork( mmdl );
    }
  }
}
#endif

//======================================================================
//  MMDLSYS PUSH POP
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS 動作モデル 退避
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Push( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == FALSE ){
    D_MMDL_DPrintf( "WARNING!! 動作モデル 描画未初期化\n" );
    GF_ASSERT( 0 );
    return;
  }
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    {
      mmdl_DeleteTCB( mmdl );
      MMDL_CallDrawPushProc( mmdl );
      
      MMDL_OnMoveBit( mmdl,
        MMDL_MOVEBIT_DRAW_PUSH | //描画処理を退避した
        MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER ); //復帰処理が必要
    }
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS 動作モデル復帰
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Pop( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    {  //動作処理復帰
      mmdl_InitMoveWork( mmdlsys, mmdl ); //ワーク初期化
      
      //初期化関数呼び出しまだ
      if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVEPROC_INIT) == 0 ){
        mmdl_InitMoveProc( mmdlsys, mmdl );
      }
      
      //復元関数呼び出しが必要
      if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER) ){
        MMDL_CallMovePopProc( mmdl );
        MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
      }
    }
    
    {  //描画処理復帰
      if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == FALSE ){
        mmdl_InitDrawWork( mmdl );
      }
      
      if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_DRAW_PUSH) ){
        MMDL_CallDrawPopProc( mmdl );
        MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_DRAW_PUSH );
      }
    }

    { //リカバリー
      mmdl_InitDrawEffectFlag( mmdl );
    }
  }
}

//======================================================================
//  MMDL 動作関数
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL 動作部分実行
 * @param  mmdl  MMDL*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateMoveProc( MMDL *mmdl )
{
  mmdl_UpdateMove( mmdl );
  
  if( MMDL_CheckStatusBitUse(mmdl) == TRUE ){
    mmdl_TCB_DrawProc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL TCB 動作関数
 * @param  tcb    GFL_TCB *
 * @param  work  tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_TCB_MoveProc( GFL_TCB * tcb, void *work )
{
  MMDL *mmdl = (MMDL *)work;
  MMDL_UpdateMoveProc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL TCB 動作関数から呼ばれる描画関数
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_TCB_DrawProc( MMDL * mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys(mmdl);
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
    MMDL_UpdateDraw( mmdl );
  }
}

//======================================================================
//  MMDL アニメーションコマンド
//======================================================================
//--------------------------------------------------------------
/**
 * アニメーションコマンドが可能かチェック
 * @param  mmdl    対象となるMMDL * 
 * @retval  int      TRUE=可能。FALSE=無理
 */
//--------------------------------------------------------------
BOOL MMDL_CheckPossibleAcmd( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) == 0 ){
    return( FALSE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE) ){
    return( FALSE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) ){ //コマンド中
    if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD_END) == 0 ){
      return( FALSE ); //コマンド終了していない
    }
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * アニメーションコマンドセット
 * @param  mmdl    対象となるMMDL * 
 * @param  code    実行するコード。AC_DIR_U等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmd( MMDL * mmdl, u16 code )
{
  GF_ASSERT( code < ACMD_MAX && "MMDL ACMD CODE ERROR" );
  MMDL_SetAcmdCode( mmdl, code );
  MMDL_SetAcmdSeq( mmdl, 0 );
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ACMD );
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * コマンドセット
 * @param  mmdl    対象となるMMDL * 
 * @param  code    実行するコード。AC_DIR_U等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetLocalAcmd( MMDL * mmdl, u16 code )
{
  MMDL_SetAcmdCode( mmdl, code );
  MMDL_SetAcmdSeq( mmdl, 0 );
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * アニメーションコマンド終了チェック。
 * @param  mmdl    対象となるMMDL * 
 * @retval  int      TRUE=終了
 */
//--------------------------------------------------------------
BOOL MMDL_CheckEndAcmd( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) == 0 ){
    return( TRUE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD_END) == 0 ){
    return( FALSE );
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * アニメーションコマンド終了チェックと開放。
 * アニメーションコマンドが終了していない場合は開放されない。
 * @param  mmdl    対象となるMMDL * 
 * @retval  BOOL  TRUE=終了している。
 */
//--------------------------------------------------------------
BOOL MMDL_EndAcmd( MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) == 0 ){
    return( TRUE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD_END) == 0 ){
    return( FALSE );
  }
  
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD|MMDL_MOVEBIT_ACMD_END );
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * アニメーションコマンド開放。
 * アニメーションコマンドが終了していなくとも強制開放。
 * @param  mmdl    対象となるMMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_FreeAcmd( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD|MMDL_MOVEBIT_ACMD_END );
  MMDL_SetAcmdCode( mmdl, ACMD_NOT );
  MMDL_SetAcmdSeq( mmdl, 0 );
}

//======================================================================
//  MMDLSYS パラメタ設定、参照
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS ステータスビットチェック
 * @param  mmdlsys  MMDLSYS*
 * @param  bit  MMDLSYS_STABIT
 * @retval  u32  (status bit & bit)
 */
//--------------------------------------------------------------
u32 MMDLSYS_CheckStatusBit(
  const MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
  return( (mmdlsys->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ステータスビット ON
 * @param  mmdlsys  MMDLSYS*
 * @param  bit  MMDLSYS_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_OnStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
  mmdlsys->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDLSYS ステータスビット OFF
 * @param  mmdlsys  MMDLSYS*
 * @param  bit  MMDLSYS_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_OffStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
  mmdlsys->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * mmdlSYS 動作モデル最大数を取得
 * @param  mmdlsys  MMDLSYS*
 * @retval  u16  最大数
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlMax( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->mmdl_max );
}

//--------------------------------------------------------------
/**
 * mmdlSYS 現在発生している動作モデルの数を取得
 * @param  mmdlsys  MMDLSYS*
 * @retval  u16  発生数
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlCount( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->mmdl_count );
}

//--------------------------------------------------------------
/**
 * MMDLSYS TCBプライオリティを取得
 * @param  mmdlsys  MMDLSYS*
 * @retval  u16  TCBプライオリティ
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetTCBPriority( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->tcb_pri );
}

//--------------------------------------------------------------
/**
 * MMDLSYS HEAPID取得
 * @param  mmdlsys  MMDLSYS*
 * @retval  HEAPID HEAPID
 */
//--------------------------------------------------------------
HEAPID MMDLSYS_GetHeapID( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->heapID );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 現在発生しているOBJ数を1増加
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_IncrementOBJCount( MMDLSYS *mmdlsys )
{
  mmdlsys->mmdl_count++;
}

//--------------------------------------------------------------
/**
 * MMDLSYS 現在発生しているOBJ数を1減少
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_DecrementOBJCount( MMDLSYS *mmdlsys )
{
  mmdlsys->mmdl_count--;
  GF_ASSERT( mmdlsys->mmdl_count >= 0 );
}

//--------------------------------------------------------------
/**
 * MMDLSYS GFL_TCBSYS*取得
 * @param  mmdlsys  MMDLSYS*
 * @retval  GFL_TCBSYS*
 */
//--------------------------------------------------------------
GFL_TCBSYS * MMDLSYS_GetTCBSYS( MMDLSYS *mmdlsys )
{
  return( mmdlsys->pTCBSys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS リソースアーカイブハンドル取得
 * @param mmdlsys MMDLSYS*
 * @retval ARCHANDLE*
 */
//--------------------------------------------------------------
ARCHANDLE * MMDLSYS_GetResArcHandle( MMDLSYS *mmdlsys )
{
  return( mmdlsys->arcH_res );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_BLACTCONTセット
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetBlActCont(
  MMDLSYS *mmdlsys, MMDL_BLACTCONT *pBlActCont )
{
  mmdlsys->pBlActCont = pBlActCont;
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_BLACTCONT取得
 * @param  mmdlsys MMDLSYS
 * @retval  MMDL_BLACTCONT*
 */
//--------------------------------------------------------------
MMDL_BLACTCONT * MMDLSYS_GetBlActCont( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->pBlActCont != NULL );
  return( mmdlsys->pBlActCont );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_G3DOBJCONTセット
 * @param  mmdlsys  MMDLSYS
 * @param objcont MMDL_G3DOBJCONT*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetG3dObjCont(
  MMDLSYS *mmdlsys, MMDL_G3DOBJCONT *objcont )
{
  mmdlsys->pObjCont = objcont;
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_G3DOBJCONT取得
 * @param  mmdlsys  MMDLSYS
 * @param objcont MMDL_G3DOBJCONT*
 * @retval  nothing
 */
//--------------------------------------------------------------
MMDL_G3DOBJCONT * MMDLSYS_GetG3dObjCont( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->pObjCont != NULL );
  return( mmdlsys->pObjCont );
}

//--------------------------------------------------------------
/**
 * MMDLSYS FLDMAPPER取得
 * @param  mmdlsys  MMDLSYS
 * @retval  FLDMAPPER* FLDMAPPER*
 */
//--------------------------------------------------------------
const FLDMAPPER * MMDLSYS_GetG3DMapper( const MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->pG3DMapper != NULL);
  return( mmdlsys->pG3DMapper );
}

//--------------------------------------------------------------
/**
 *  @brief  ノーグリッド動作マッパーの取得
 *  @param  mmdlsys   動作モデルシステム
 *  @return　マッパー
 */
//--------------------------------------------------------------
FLDNOGRID_MAPPER * MMDLSYS_GetNOGRIDMapper( const MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys );
  return mmdlsys->pNOGRIDMapper;
}

//--------------------------------------------------------------
/**
 * MMDLSYS FIELDMAP_WORK取得
 * @param mmdlsys MMDLSYS
 * @retval fieldMapWork FIELDMAP_WORK
 */
//--------------------------------------------------------------
void * MMDLSYS_GetFieldMapWork( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->fieldMapWork != NULL );
  return( mmdlsys->fieldMapWork );
}

//--------------------------------------------------------------
/**
 * MMDLSYS GAMEDATA取得
 * @param mmdlsys MMDLSYS
 * @retval GAMEDATA*
 */
//--------------------------------------------------------------
GAMEDATA * MMDLSYS_GetGameData( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->gdata != NULL );
  return( mmdlsys->gdata );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 目標となるカメラ水平方向角度を取得。
 * セットされていない場合は0を返す
 * @param mmdlsys MMDLSYS
 * @retval u16
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetTargetCameraAngleYaw( const MMDLSYS *mmdlsys )
{
  if( mmdlsys->targetCameraAngleYaw != NULL ){
    return( *mmdlsys->targetCameraAngleYaw );
  }
  return( 0 );
}

//======================================================================
//  MMDL　パラメタ参照、設定
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL ステータスビットON
 * @param  mmdl  mmdl
 * @param  bit    MMDL_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBit( MMDL *mmdl, MMDL_STABIT bit )
{
  mmdl->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL ステータスビットOFF
 * @param  mmdl  mmdl
 * @param  bit    MMDL_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBit( MMDL *mmdl, MMDL_STABIT bit )
{
  mmdl->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL ステータスビット取得
 * @param  mmdl    MMDL * 
 * @retval  MMDL_STABIT ステータスビット
 */
//--------------------------------------------------------------
MMDL_STABIT MMDL_GetStatusBit( const MMDL * mmdl )
{
  return( mmdl->status_bit );
}

//--------------------------------------------------------------
/**
 * MMDL ステータスビットチェック
 * @param  mmdl  MMDL*
 * @param  bit  MMDL_STABIT
 * @retval  u32  (status bit & bit)
 */
//--------------------------------------------------------------
u32 MMDL_CheckStatusBit( const MMDL *mmdl, MMDL_STABIT bit )
{
  return( (mmdl->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * MMDL 動作ビット　取得
 * @param  mmdl  MMDL *
 * @retval  u32    動作ビット
 */
//--------------------------------------------------------------
MMDL_MOVEBIT MMDL_GetMoveBit( const MMDL * mmdl )
{
  return( mmdl->move_bit );
}

//--------------------------------------------------------------
/**
 * MMDL 動作ビット　ON
 * @param  mmdl  MMDL *
 * @param  bit    立てるビット MMDL_MOVEBIT_NON等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit )
{
  mmdl->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL 動作ビット　OFF
 * @param  mmdl  MMDL *
 * @param  bit    下ろすビット MMDL_MOVEBIT_NON等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit )
{
  mmdl->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL 動作ビット　チェック
 * @param  mmdl  MMDL *
 * @param  bit    チェックするビット MMDL_MOVEBIT_NON等
 * @retval  nothing
 */
//--------------------------------------------------------------
u32 MMDL_CheckMoveBit( const MMDL * mmdl, MMDL_MOVEBIT bit )
{
  return( (mmdl->move_bit & bit) );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ IDセット
 * @param  mmdl  MMDL * 
 * @param  id    obj id
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJID( MMDL * mmdl, u16 obj_id )
{
  mmdl->obj_id = obj_id;
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID取得
 * @param  mmdl  MMDL *
 * @retval  u16    OBJ ID
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJID( const MMDL * mmdl )
{
  return( mmdl->obj_id );
}

//--------------------------------------------------------------
/**
 * MMDL ZONE IDセット
 * @param  mmdl  MMDL *
 * @param  zone_id  ゾーンID
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetZoneID( MMDL * mmdl, u16 zone_id )
{
  mmdl->zone_id = zone_id;
}

//--------------------------------------------------------------
/**
 * MMDL ZONE ID取得
 * @param  mmdl  MMDL *
 * @retval  int    ZONE ID
 */
//--------------------------------------------------------------
u16 MMDL_GetZoneID( const MMDL * mmdl )
{
  return( mmdl->zone_id );
}

//--------------------------------------------------------------
/**
 * MMDL OBJコードセット
 * @param  mmdl      MMDL * 
 * @param  code      セットするコード
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJCode( MMDL * mmdl, u16 code )
{
  mmdl->obj_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL OBJコード取得
 * @param  mmdl      MMDL * 
 * @retval  u16        OBJコード
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJCode( const MMDL * mmdl )
{
  return( mmdl->obj_code );
}

//--------------------------------------------------------------
/**
 * MMDL 動作コードセット
 * @param  mmdl      MMDL * 
 * @param  code      動作コード
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveCode( MMDL * mmdl, u16 code )
{
  mmdl->move_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL 動作コード取得
 * @param  mmdl      MMDL * 
 * @retval  u32        動作コード
 */
//--------------------------------------------------------------
u16 MMDL_GetMoveCode( const MMDL * mmdl )
{
  return( mmdl->move_code );
}

//--------------------------------------------------------------
/**
 * MMDL イベントタイプセット
 * @param  mmdl    MMDL * 
 * @param  type    Event Type
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventType( MMDL * mmdl, u16 type )
{
  mmdl->event_type = type;
}

//--------------------------------------------------------------
/**
 * MMDL イベントタイプ取得
 * @param  mmdl    MMDL * 
 * @retval  u32      Event Type
 */
//--------------------------------------------------------------
u16 MMDL_GetEventType( const MMDL * mmdl )
{
  return( mmdl->event_type );
}

//--------------------------------------------------------------
/**
 * MMDL イベントフラグセット
 * @param  mmdl    MMDL * 
 * @param  flag    Event Flag
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventFlag( MMDL * mmdl, u16 flag )
{
  mmdl->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * MMDL イベントフラグ取得
 * @param  mmdl    MMDL * 
 * @retval  u16      Event Flag
 */
//--------------------------------------------------------------
u16 MMDL_GetEventFlag( const MMDL * mmdl )
{
  return( mmdl->event_flag );
}

//--------------------------------------------------------------
/**
 * MMDL イベントIDセット
 * @param  mmdl    MMDL *
 * @param  id      Event ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventID( MMDL * mmdl, u16 id )
{
  mmdl->event_id = id;
}

//--------------------------------------------------------------
/**
 * MMDL イベントID取得
 * @param  mmdl    MMDL * 
 * @retval  u16      Event ID
 */
//--------------------------------------------------------------
u16 MMDL_GetEventID( const MMDL * mmdl )
{
  return( mmdl->event_id );
}

//--------------------------------------------------------------
/**
 * MMDL イベントIDがエイリアスかどうかチェック
 * @param  mmdl    MMDL * 
 * @retval  int      TRUE=エイリアスIDである FALSE=違う
 */
//--------------------------------------------------------------
BOOL MMDL_CheckAliesEventID( const MMDL * mmdl )
{
  u16 id = (u16)MMDL_GetEventID( mmdl );
  
  if( id == SP_SCRID_ALIES ){
    return( TRUE );
  }

  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ヘッダー指定方向取得
 * @param  mmdl    MMDL * 
 * @retval  u32      DIR_UP等
 */
//--------------------------------------------------------------
u32 MMDL_GetDirHeader( const MMDL * mmdl )
{
  return( mmdl->dir_head );
}

//--------------------------------------------------------------
/**
 * MMDL 表示方向セット　強制
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetForceDirDisp( MMDL * mmdl, u16 dir )
{
  mmdl->dir_disp_old = mmdl->dir_disp;
  mmdl->dir_disp = dir;
}

//--------------------------------------------------------------
/**
 * MMDL 表示方向セット
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirDisp( MMDL * mmdl, u16 dir )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_PAUSE_DIR) == 0 ){
    mmdl->dir_disp_old = mmdl->dir_disp;
    mmdl->dir_disp = dir;
  }
}

//--------------------------------------------------------------
/**
 * MMDL 表示方向取得
 * @param  mmdl  MMDL * 
 * @retval  u16   DIR_UP等
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDisp( const MMDL * mmdl )
{
  return( mmdl->dir_disp );
}

//--------------------------------------------------------------
/**
 * MMDL 過去表示方向取得
 * @param  mmdl      MMDL * 
 * @retval  dir        DIR_UP等
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDispOld( const MMDL * mmdl )
{
  return( mmdl->dir_disp_old );
}

//--------------------------------------------------------------
/**
 * MMDL 移動方向セット
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirMove( MMDL * mmdl, u16 dir )
{
  mmdl->dir_move_old = mmdl->dir_move;
  mmdl->dir_move = dir;
}

//--------------------------------------------------------------
/**
 * MMDL 移動方向取得
 * @param  mmdl      MMDL * 
 * @retval  u16    DIR_UP等
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMove( const MMDL * mmdl )
{
  return( mmdl->dir_move );
}

//--------------------------------------------------------------
/**
 * MMDL 過去移動方向取得
 * @param  mmdl      MMDL * 
 * @retval  u16  DIR_UP等
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMoveOld( const MMDL * mmdl )
{
  return( mmdl->dir_move_old );
}

//--------------------------------------------------------------
/**
 * MMDL 表示、移動方向セット
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirAll( MMDL * mmdl, u16 dir )
{
  MMDL_SetDirDisp( mmdl, dir );
  MMDL_SetDirMove( mmdl, dir );
}

//--------------------------------------------------------------
/**
 * MMDL ヘッダー指定パラメタセット
 * @param  mmdl  MMDL * 
 * @param  param  パラメタ
 * @param  no    セットするパラメタ番号　MMDL_PARAM_0等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetParam( MMDL *mmdl, u16 param, MMDL_H_PARAM no )
{
  switch( no ){
  case MMDL_PARAM_0: mmdl->param0 = param; break;
  case MMDL_PARAM_1: mmdl->param1 = param; break;
  case MMDL_PARAM_2: mmdl->param2 = param; break;
  default: GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * MMDL ヘッダー指定パラメタ取得
 * @param  mmdl      MMDL *
 * @param  param      MMDL_PARAM_0等
 * @retval  u16 パラメタ
 */
//--------------------------------------------------------------
u16 MMDL_GetParam( const MMDL * mmdl, MMDL_H_PARAM param )
{
  switch( param ){
  case MMDL_PARAM_0: return( mmdl->param0 );
  case MMDL_PARAM_1: return( mmdl->param1 );
  case MMDL_PARAM_2: return( mmdl->param2 );
  }
  
  GF_ASSERT( 0 );
  return( 0 );
}

//--------------------------------------------------------------
/**
 * MMDL 移動制限Xセット
 * @param  mmdl      MMDL * 
 * @param  x        移動制限
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitX( MMDL * mmdl, s16 x )
{
  mmdl->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * MMDL 移動制限X取得
 * @param  mmdl    MMDL * 
 * @retval  s16      移動制限X
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitX( const MMDL * mmdl )
{
  return( mmdl->move_limit_x );
}

//--------------------------------------------------------------
/**
 * MMDL 移動制限Zセット
 * @param  mmdl      MMDL * 
 * @param  z        移動制限
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitZ( MMDL * mmdl, s16 z )
{
  mmdl->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * MMDL 移動制限Z取得
 * @param  mmdl    MMDL * 
 * @retval  s16    移動制限z
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitZ( const MMDL * mmdl )
{
  return( mmdl->move_limit_z );
}

//--------------------------------------------------------------
/**
 * MMDL 描画ステータスセット
 * @param  mmdl    MMDL * 
 * @param  st      DRAW_STA_STOP等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDrawStatus( MMDL * mmdl, u16 st )
{
  mmdl->draw_status = st;
}

//--------------------------------------------------------------
/**
 * MMDL 描画ステータス取得
 * @param  mmdl    MMDL * 
 * @retval  u16      DRAW_STA_STOP等
 */
//--------------------------------------------------------------
u16 MMDL_GetDrawStatus( const MMDL * mmdl )
{
  return( mmdl->draw_status );
}

//--------------------------------------------------------------
/**
 * MMDL MMDLSYS *取得
 * @param  mmdl      MMDL * 
 * @retval  MMDLSYS  MMDLSYS *
 */
//--------------------------------------------------------------
MMDLSYS * MMDL_GetMMdlSys( const MMDL *mmdl )
{
  return( mmdl->pMMdlSys );
}

//--------------------------------------------------------------
/**
 * MMDL 動作関数用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * 動作用ワークはsize分、0で初期化。
 * @param  mmdl  MMDL * 
 * @param  size  必要なワークサイズ
 * @retval  void*  初期化されたワーク
 */
//--------------------------------------------------------------
void * MMDL_InitMoveProcWork( MMDL * mmdl, int size )
{
  void *work;
  GF_ASSERT( size <= MMDL_MOVE_WORK_SIZE );
  work = MMDL_GetMoveProcWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL 動作関数用ワーク取得。
 * @param  mmdl  MMDL * 
 * @retval  void*  ワーク
 */
//--------------------------------------------------------------
void * MMDL_GetMoveProcWork( MMDL * mmdl )
{
  return( mmdl->move_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL 動作補助関数用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * @param  mmdl  MMDL * 
 * @param  size  必要なワークサイズ
 * @retval  void*  初期化されたワーク
 */
//--------------------------------------------------------------
void * MMDL_InitMoveSubProcWork( MMDL * mmdl, int size )
{
  u8 *work;
  
  GF_ASSERT( size <= MMDL_MOVE_SUB_WORK_SIZE );
  work = MMDL_GetMoveSubProcWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL 動作補助関数用ワーク取得
 * @param  mmdl  MMDL * 
 * @retval  void*  ワーク*
 */
//--------------------------------------------------------------
void * MMDL_GetMoveSubProcWork( MMDL * mmdl )
{
  return( mmdl->move_sub_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL 動作コマンド用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * @param  mmdl  MMDL * 
 * @param  size  必要なワークサイズ
 * @retval  void*  初期化されたワーク
 */
//--------------------------------------------------------------
void * MMDL_InitMoveCmdWork( MMDL * mmdl, int size )
{
  u8 *work;
  
  GF_ASSERT( size <= MMDL_MOVE_CMD_WORK_SIZE );
  work = MMDL_GetMoveCmdWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL 動作コマンド用ワーク取得
 * @param  mmdl  MMDL *
 * @retval  void*  ワーク*
 */
//--------------------------------------------------------------
void * MMDL_GetMoveCmdWork( MMDL * mmdl )
{
  return( mmdl->move_cmd_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL 描画関数用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * @param  mmdl  MMDL * 
 * @param  size  必要なワークサイズ
 * @retval  void*  初期化されたワーク
 */
//--------------------------------------------------------------
void * MMDL_InitDrawProcWork( MMDL * mmdl, int size )
{
  u8 *work;
  
  GF_ASSERT( size <= MMDL_DRAW_WORK_SIZE );
  work = MMDL_GetDrawProcWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL 描画関数用ワーク取得
 * @param  mmdl  MMDL * 
 * @retval  void*  ワーク
 */
//--------------------------------------------------------------
void * MMDL_GetDrawProcWork( MMDL * mmdl )
{
  return( mmdl->draw_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL 動作初期化関数実行
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveInitProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->init_proc );
  if( mmdl->move_proc_list->init_proc != NULL ){
    mmdl->move_proc_list->init_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 動作関数実行
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->move_proc );
  if( mmdl->move_proc_list->move_proc != NULL ){
    mmdl->move_proc_list->move_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 削除関数実行
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveDeleteProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->delete_proc );
  if( mmdl->move_proc_list->delete_proc != NULL ){
    mmdl->move_proc_list->delete_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 復帰関数実行
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMovePopProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->recover_proc );
  if( mmdl->move_proc_list->recover_proc != NULL ){
    mmdl->move_proc_list->recover_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 描画初期化関数実行
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawInitProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->init_proc );
  if( mmdl->draw_proc_list->init_proc != NULL ){
    mmdl->draw_proc_list->init_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 描画関数実行
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->draw_proc );
  if( mmdl->draw_proc_list->draw_proc != NULL ){
    mmdl->draw_proc_list->draw_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 描画削除関数実行
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawDeleteProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->delete_proc );
  if( mmdl->draw_proc_list->delete_proc != NULL ){
    mmdl->draw_proc_list->delete_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 描画退避関数実行
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPushProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->push_proc );
  if( mmdl->draw_proc_list->push_proc != NULL ){
    mmdl->draw_proc_list->push_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 描画復帰関数実行
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPopProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->pop_proc );
  if( mmdl->draw_proc_list->pop_proc != NULL ){
    mmdl->draw_proc_list->pop_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 描画取得関数実行
 * @param  mmdl  MMDL*
 * @param  state  取得関数に与える情報
 * @retval  nothing
 */
//--------------------------------------------------------------
u32 MMDL_CallDrawGetProc( MMDL *mmdl, u32 state )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->get_proc );
  if( mmdl->draw_proc_list->get_proc != NULL ){
    return( mmdl->draw_proc_list->get_proc(mmdl,state) );
  }
  return( 0 );
}

//--------------------------------------------------------------
/**
 * MMDL アニメーションコマンドコードセット
 * @param  mmdl  MMDL * 
 * @param  code  AC_DIR_U等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdCode( MMDL * mmdl, u16 code )
{
  mmdl->acmd_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL アニメーションコマンドコード取得
 * @param  mmdl  MMDL * 
 * @retval  u16  AC_DIR_U等
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdCode( const MMDL * mmdl )
{
  return( mmdl->acmd_code );
}

//--------------------------------------------------------------
/**
 * MMDL アニメーションコマンドシーケンスセット
 * @param  mmdl  MMDL * 
 * @param  no    シーケンス
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdSeq( MMDL * mmdl, u16 no )
{
  mmdl->acmd_seq = no;
}

//--------------------------------------------------------------
/**
 * MMDL アニメーションコマンドシーケンス増加
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_IncAcmdSeq( MMDL * mmdl )
{
  mmdl->acmd_seq++;
}

//--------------------------------------------------------------
/**
 * MMDL アニメーションコマンドシーケンス取得
 * @param  mmdl  MMDL * 
 * @retval  u16 シーケンス
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdSeq( const MMDL * mmdl )
{
  return( mmdl->acmd_seq );
}

//--------------------------------------------------------------
/**
 * MMDL 現在のマップアトリビュートをセット
 * @param  mmdl  MMDL *
 * @param  attr  セットするアトリビュート
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttr( MMDL * mmdl, u32 attr )
{
  mmdl->now_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL 現在のマップアトリビュートを取得
 * @param  mmdl  MMDL *
 * @retval  u32    マップアトリビュート
 */
//--------------------------------------------------------------
u32 MMDL_GetMapAttr( const MMDL * mmdl )
{
  return( mmdl->now_attr );
}

//--------------------------------------------------------------
/**
 * MMDL 過去のマップアトリビュートをセット
 * @param  mmdl  MMDL *
 * @param  attr  セットするアトリビュート
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttrOld( MMDL * mmdl, u32 attr )
{
  mmdl->old_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL 過去のマップアトリビュートを取得
 * @param  mmdl  MMDL *
 * @retval  u32    マップアトリビュート
 */
//--------------------------------------------------------------
u32 MMDL_GetMapAttrOld( const MMDL * mmdl )
{
  return( mmdl->old_attr );
}

//--------------------------------------------------------------
/**
 * MMDL エイリアス時のゾーンID取得。
 * ※エイリアス時はイベントフラグが指定ゾーンID
 * @param  mmdl  MMDL *
 * @retval  u16 ゾーンID
 */
//--------------------------------------------------------------
u16 MMDL_GetZoneIDAlies( const MMDL * mmdl )
{
  GF_ASSERT( MMDL_CheckStatusBitAlies(mmdl) == TRUE );
  return( MMDL_GetEventFlag(mmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL 初期座標 グリッドX座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16 X座標
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosX( const MMDL * mmdl )
{
  return( mmdl->gx_init );
}

//--------------------------------------------------------------
/**
 * MMDL 初期座標 グリッドX座標セット
 * @param  mmdl  MMDL * 
 * @param  x    セットする座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_init = x;
}

//--------------------------------------------------------------
/**
 * MMDL 初期座標 Y座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    Y
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosY( const MMDL * mmdl )
{
  return( mmdl->gy_init );
}

//--------------------------------------------------------------
/**
 * MMDL 初期座標 Y座標セット
 * @param  mmdl  MMDL * 
 * @param  y    セットする座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_init = y;
}

//--------------------------------------------------------------
/**
 * MMDL 初期座標 z座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    z座標
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosZ( const MMDL * mmdl )
{
  return( mmdl->gz_init );
}

//--------------------------------------------------------------
/**
 * MMDL 初期座標 z座標セット
 * @param  mmdl  MMDL * 
 * @param  z    セットする座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_init = z;
}

//--------------------------------------------------------------
/**
 * MMDL 過去座標　X座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    X座標
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosX( const MMDL * mmdl )
{
  return( mmdl->gx_old );
}

//--------------------------------------------------------------
/**
 * MMDL 過去座標 X座標セット
 * @param  mmdl  MMDL * 
 * @param  x    セットする座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_old = x;
}

//--------------------------------------------------------------
/**
 * MMDL 過去座標 Y座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    Y
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosY( const MMDL * mmdl )
{
  return( mmdl->gy_old );
}

//--------------------------------------------------------------
/**
 * MMDL 過去座標 Y座標セット
 * @param  mmdl  MMDL * 
 * @param  y    セットする座標
 * @retval  s16    y座標
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_old = y;
}

//--------------------------------------------------------------
/**
 * MMDL 過去座標 z座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    z座標
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosZ( const MMDL * mmdl )
{
  return( mmdl->gz_old );
}

//--------------------------------------------------------------
/**
 * MMDL 過去座標 z座標セット
 * @param  mmdl  MMDL * 
 * @param  z    セットする座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_old = z;
}

//--------------------------------------------------------------
/**
 * MMDL 現在グリッド座標
 * @param  mmdl  MMDL * 
 * @retval  MMDL_GRIDPOS
 */
//--------------------------------------------------------------
void MMDL_GetGridPos( const MMDL * mmdl, MMDL_GRIDPOS* pos )
{
  pos->gx = mmdl->gx_now;
  pos->gy = mmdl->gy_now;
  pos->gz = mmdl->gz_now;
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 X座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    X座標
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosX( const MMDL * mmdl )
{
  return( mmdl->gx_now );
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 X座標セット
 * @param  mmdl  MMDL * 
 * @param  x    セットする座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_now = x;
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 X座標増加
 * @param  mmdl  MMDL * 
 * @param  x    足す値
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_now += x;
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 Y座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    Y
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosY( const MMDL * mmdl )
{
  return( mmdl->gy_now );
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 Y座標セット
 * @param  mmdl  MMDL * 
 * @param  y    セットする座標
 * @retval  s16    y座標
 */
//--------------------------------------------------------------
void MMDL_SetGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_now = y;
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 Y座標増加
 * @param  mmdl  MMDL * 
 * @param  y    足す値
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_now += y;
}

//--------------------------------------------------------------
/**
 * MMDL 過去座標 z座標取得
 * @param  mmdl  MMDL * 
 * @retval  s16    z座標
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosZ( const MMDL * mmdl )
{
  return( mmdl->gz_now );
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 z座標セット
 * @param  mmdl  MMDL * 
 * @param  z    セットする座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_now = z;
}

//--------------------------------------------------------------
/**
 * MMDL 現在座標 z座標増加
 * @param  mmdl  MMDL * 
 * @param  z    足す値
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_now += z;
}

//--------------------------------------------------------------
/**
 * MMDL 実座標ポインタ取得
 * @param  mmdl  MMDL * 
 * @retval VecFx32*
 */
//--------------------------------------------------------------
const VecFx32 * MMDL_GetVectorPosAddress( const MMDL * mmdl )
{
  return( &mmdl->vec_pos_now );
}

//--------------------------------------------------------------
/**
 * MMDL 実座標取得
 * @param  mmdl  MMDL * 
 * @param  vec    座標格納先
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_pos_now;
}

//--------------------------------------------------------------
/**
 * MMDL 実座標セット
 * @param  mmdl  MMDL * 
 * @param  vec    セット座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_pos_now = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL 実座標Y値取得
 * @param  mmdl  MMDL * 
 * @retval  fx32  高さ
 */
//--------------------------------------------------------------
fx32 MMDL_GetVectorPosY( const MMDL * mmdl )
{
  return( mmdl->vec_pos_now.y );
}

//--------------------------------------------------------------
/**
 * MMDL 表示座標オフセット取得
 * @param  mmdl  MMDL * 
 * @param  vec    セット座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_draw_offs;
}

//--------------------------------------------------------------
/**
 * MMDL 表示座標オフセットセット
 * @param  mmdl  MMDL * 
 * @param  vec    セット座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_draw_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL 外部指定表示座標オフセット取得
 * @param  mmdl  MMDL * 
 * @param  vec    セット座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorOuterDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_draw_offs_outside;
}

//--------------------------------------------------------------
/**
 * MMDL 外部指定表示座標オフセットセット
 * @param  mmdl  MMDL * 
 * @param  vec    セット座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorOuterDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_draw_offs_outside = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL アトリビュート変化座標オフセット取得
 * @param  mmdl  MMDL * 
 * @param  vec    セット座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorAttrDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_attr_offs;
}

//--------------------------------------------------------------
/**
 * MMDL アトリビュート変化座標オフセットセット
 * @param  mmdl  MMDL * 
 * @param  vec    セット座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorAttrDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_attr_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL 高さ(グリッド単位)を取得
 * @param  mmdl  MMDL *
 * @retval  s16    高さ。H_GRID単位
 * @attention 色々あって廃止予定。実行すると無条件でアサート。
 * グリッドY座標はMMDL_GetGridPosY()を使用してください。
 * 実座標からグリッド値が欲しい場合はMMDL_GetVectorPosY()から変換して下さい。
 */
//--------------------------------------------------------------
#if 0
s16 MMDL_GetHeightGrid( const MMDL * mmdl )
{
  fx32 y = MMDL_GetVectorPosY( mmdl );
  s16 gy = SIZE_GRID_FX32( y );
  GF_ASSERT( 0 );
  return( gy );
}
#endif

//--------------------------------------------------------------
/**
 * MMDL MMDL_BLACTCONTを取得
 * @param  mmdl  MMDL *
 * @retval  MMDL_BLACTCONT*
 */
//--------------------------------------------------------------
MMDL_BLACTCONT * MMDL_GetBlActCont( MMDL *mmdl )
{
  return( MMDLSYS_GetBlActCont((MMDLSYS*)MMDL_GetMMdlSys(mmdl)) );
}

//--------------------------------------------------------------
/**
 * MMDL 横グリッドサイズを取得
 * @param mmdl MMDL*
 * @retval u8 グリッドサイズ
 */
//--------------------------------------------------------------
u8 MMDL_GetGridSizeX( const MMDL *mmdl )
{
  return( mmdl->gx_size );
}

//--------------------------------------------------------------
/**
 * MMDL 奥グリッドサイズを取得
 * @param mmdl MMDL*
 * @retval u8 グリッドサイズ
 */
//--------------------------------------------------------------
u8 MMDL_GetGridSizeZ( const MMDL *mmdl )
{
  return( mmdl->gz_size );
}

//--------------------------------------------------------------
/**
 * MMDL 管理表設定オフセット座標を取得
 * @param mmdl MMDL*
 * @param pos 座標格納先
 * @retval
 */
//--------------------------------------------------------------
void MMDL_GetControlOffsetPos( const MMDL *mmdl, VecFx32 *pos )
{
  pos->x = NUM_FX32( mmdl->offset_x );
  pos->y = NUM_FX32( mmdl->offset_y );
  pos->z = NUM_FX32( mmdl->offset_z );
}

//======================================================================
//  MMDLSYS ステータス操作
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS 描画処理が初期化されているかどうかチェック
 * @param  mmdlsys  MMDLSYS *
 * @retval  BOOL  TRUE=初期化されている
 */
//--------------------------------------------------------------
BOOL MMDLSYS_CheckCompleteDrawInit( const MMDLSYS *mmdlsys )
{
  if( MMDLSYS_CheckStatusBit(
      mmdlsys,MMDLSYS_STABIT_DRAW_INIT_COMP) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 描画処理初期化完了セット
 * @param  mmdlsys  MMDLSYS*
 * @param  flag  TRUE=初期化完了
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetCompleteDrawInit( MMDLSYS *mmdlsys, BOOL flag )
{
  if( flag == TRUE ){
    mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
  }else{
    mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS 影を付ける、付けないのセット
 * @param  mmdlsys MMDLSYS *
 * @param  flag  TRUE=影を付ける FALSE=影を付けない
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetJoinShadow( MMDLSYS *mmdlsys, BOOL flag )
{
  if( flag == FALSE ){
    mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_SHADOW_JOIN_NOT );
  }else{
    mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_SHADOW_JOIN_NOT );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS 影を付ける、付けないのチェック
 * @param  mmdlsys MMDLSYS *
 * @retval  BOOL TRUE=付ける
 */
//--------------------------------------------------------------
BOOL MMDLSYS_CheckJoinShadow( const MMDLSYS *mmdlsys )
{
  if( MMDLSYS_CheckStatusBit(mmdlsys,MMDLSYS_STABIT_SHADOW_JOIN_NOT) ){
    return( FALSE );
  }
  return( TRUE );
}

//======================================================================
//  MMDLSYS プロセス操作
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデル全体の動作を完全停止。
 * 動作処理、描画処理を完全停止する。アニメーションコマンドにも反応しない。
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_StopProc( MMDLSYS *mmdlsys )
{
  mmdlsys_OnStatusBit( mmdlsys,
    MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDLSYS_StopProc()の解除。
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_PlayProc( MMDLSYS *mmdlsys )
{
  mmdlsys_OffStatusBit( mmdlsys,
    MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデル全体の動作を一時停止
 * 固有の動作処理（ランダム移動等）を一時停止する。
 * アニメーションコマンドには反応する。
 * @param  mmdlsys  MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_PauseMoveProc( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_CheckMoveBitRejectPauseMove(mmdl) == FALSE ){
      MMDL_OnMoveBitMoveProcPause( mmdl );
    }
  }
  
  mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_PAUSE_ALL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデル全体の一時停止を解除
 * @param  mmdlsys  MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_ClearPauseMoveProc( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;

  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    MMDL_OffMoveBitMoveProcPause( mmdl );
  }
  
  mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_PAUSE_ALL );
}

//======================================================================
//  MMDL ステータスビット関連
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデルから
 * フィールド動作モデルシステムのビットチェック
 * @param  mmdl    MMDL*
 * @param  bit      MMDLSYS_STABIT_DRAW_INIT_COMP等
 * @retval  u32      0以外 bitヒット
 */
//--------------------------------------------------------------
u32 MMDL_CheckMMdlSysStatusBit(
  const MMDL *mmdl, MMDLSYS_STABIT bit )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  return( MMDLSYS_CheckStatusBit(mmdlsys,bit) );
}

//--------------------------------------------------------------
/**
 * MMDL 使用チェック
 * @param  mmdl  MMDL *
 * @retval  BOOL  TRUE=使用中
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitUse( const MMDL *mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作中にする
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMove( MMDL *mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作中を解除
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMove( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作中チェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=動作中
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMove( const MMDL *mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作開始にする
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMoveStart( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作開始を解除
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMoveStart( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作開始チェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=移動動作開始
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMoveStart( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_START) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作終了にする
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMoveEnd( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作終了を解除
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMoveEnd( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL 移動動作終了チェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=移動終了
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMoveEnd( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_END) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 描画初期化完了にする
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitCompletedDrawInit( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL 描画初期化完了を解除
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitCompletedDrawInit( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL 描画初期化完了チェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=描画初期化完了
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitCompletedDrawInit( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_DRAW_PROC_INIT_COMP) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 非表示フラグをチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=非表示　FALSE=表示
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitVanish( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_VANISH) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 非表示フラグを設定
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=非表示　FALSE=表示
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitVanish( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_VANISH );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_VANISH );
  }
}

//--------------------------------------------------------------
/**
 * MMDL OBJ同士の当たり判定フラグを設定
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=ヒットアリ　FALSE=ヒットナシ
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitFellowHit( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_FELLOW_HIT_NON );
  }else{
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_FELLOW_HIT_NON );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 動作中フラグのセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=動作中　FALSE=停止中
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitMove( MMDL * mmdl, int flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBitMove( mmdl );
  }else{
    MMDL_OffMoveBitMove( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 話しかけ可能チェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=可能 FALSE=不可
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitTalk( MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_TALK_OFF) ){
    return( FALSE );
  }
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * MMDL 話しかけ不可フラグをセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=不可 FALSE=可能
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitTalkOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_TALK_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_TALK_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 動作ポーズ
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMoveProcPause( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL 動作ポーズ解除
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMoveProcPause( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL 動作ポーズチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=動作ポーズ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMoveProcPause( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_PAUSE_MOVE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ステータスビット 描画処理初期化完了チェック
 * @param  mmdl    MMDL *
 * @retval  BOOL TRUE=完了。FALSE=まだ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckCompletedDrawInit( const MMDL * mmdl )
{
  const MMDLSYS *mmdlsys;
  
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == FALSE ){
    return( FALSE );
  }
  
  if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 高さ取得を禁止する
 * @param  mmdl  MMDL *
 * @param  int    TRUE=高さ取得OFF FALSE=ON
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitHeightGetOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 高さ取得が禁止されているかチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=禁止
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitHeightGetOFF( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_HEIGHT_GET_OFF) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ゾーン切り替え時の削除禁止
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=禁止 FALSE=禁止しない
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitNotZoneDelete( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ZONE_DEL_NOT );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ZONE_DEL_NOT );
  }
}

//--------------------------------------------------------------
/**
 * MMDL エイリアスフラグをセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=エイリアス FALSE=違う
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAlies( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ALIES );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ALIES );
  }
}

//--------------------------------------------------------------
/**
 * MMDL エイリアスフラグをチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=エイリアス FALSE=違う
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAlies( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ALIES) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 浅瀬エフェクトセットフラグをセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=セット　FALSE=クリア
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitShoalEffect( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_EFFSET_SHOAL );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_EFFSET_SHOAL );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 浅瀬エフェクトセットフラグをチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=セット　FALSE=違う
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitShoalEffect( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_EFFSET_SHOAL) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL アトリビュートオフセット設定OFFセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=セット　FALSE=クリア
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAttrOffsetOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ATTR_OFFS_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ATTR_OFFS_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL アトリビュートオフセット設定OFFチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=OFF　FALSE=違う
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAttrOffsetOFF( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_OFFS_OFF) ){
    return( TRUE );
  }
  return( FALSE );
}

#if 0 //old pl
//--------------------------------------------------------------
/**
 * MMDL 橋移動フラグセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=セット　FALSE=クリア
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitBridge( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_BRIDGE );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_BRIDGE );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 橋移動フラグチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=橋　FALSE=違う
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitBridge( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_BRIDGE) ){
    return( TRUE );
  }
  return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * MMDL 映りこみフラグセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=セット　FALSE=クリア
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitReflect( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_REFLECT_SET );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_REFLECT_SET );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 映りこみフラグチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=セット　FALSE=無し
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitReflect( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_REFLECT_SET) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL アニメーションコマンドチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=コマンドアリ　FALSE=無し
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitAcmd( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 動作ポーズ拒否フラグセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=セット　FALSE=クリア
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitRejectPauseMove( MMDL *mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_REJECT_PAUSE_MOVE );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_REJECT_PAUSE_MOVE );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 動作ポーズ拒否フラグチェック
 * @param  mmdl  MMDL *
 * @retval BOOL TRUE=セット　FALSE=無し
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitRejectPauseMove( const MMDL *mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_REJECT_PAUSE_MOVE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 拡張高さ反応フラグをセット
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=セット　FALSE=クリア
 * @retval  nothing
 */
//--------------------------------------------------------------
#if 0 //wb null
void MMDL_SetStatusBitHeightExpand( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_HEIGHT_EXPAND );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_HEIGHT_EXPAND );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 拡張高さ反応フラグチェック
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=拡張高さに反応する　FALSE=無し
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitHeightExpand( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_HEIGHT_EXPAND) ){
    return( TRUE );
  }
  return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * MMDL アトリビュート取得を停止
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=停止
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAttrGetOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ATTR_GET_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ATTR_GET_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL アトリビュート取得を停止　チェック
 * @param  mmdl  MMDL *
 * @retval  int  TRUE=停止
 */
//--------------------------------------------------------------
int MMDL_CheckStatusBitAttrGetOFF( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_GET_OFF) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 非セーブ対象にする
 * @param mmdl MMDL*
 * @param flag TRUE=非セーブ FALSE=セーブされる
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitNotSave( MMDL *mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_NOT_SAVE );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_NOT_SAVE );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 移動エフェクトを出さない
 * @param mmdl MMDL*
 * @param flag TRUE=出さない FALSE=出す
 * @retval nothing
 * 
 * 社内BTS1625対処の為の追加処理。
 * 他の影響を考え、現状は土煙エフェクトのみが対象
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitNonCreateMoveEffect( MMDL *mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_NON_CREATE_MOVE_EFFECT );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_NON_CREATE_MOVE_EFFECT );
  }
}

//--------------------------------------------------------------
/**
 * MMDL 移動エフェクトを出さない　チェック
 * @param mmdl MMDL*
 * @retval BNOOL TRUE=出さない FALSE=出す
 * 
 * 社内BTS1625対処の為の追加処理。
 * 他の影響を考え、現状は土煙エフェクトのみが対象
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitNonCreateMoveEffect( const MMDL *mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_NON_CREATE_MOVE_EFFECT) ){
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  MMDLSYS ツール
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS 使用しているフィールド動作モデルを探す。
 * @param  mmdlsys  MMDLSYS *
 * @param  mmdl  MMDL*格納先
 * @param  no  検索開始ワークno。先頭から検索する際は初期値0を指定。
 * @retval BOOL TRUE=動作モデル取得した FALSE=noから終端まで検索し取得無し
 * @note 引数noは呼び出し後、取得位置+1の値になる。
 * @note ※例：OBJ ID 1番の動作モデルを探す。
 * u32 no=0;
 * MMDL *mmdl;
 * while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
 *     if( MMDL_GetOBJID(mmdl) == 1 ){
 *       break;
 *     }
 * }
 */
//--------------------------------------------------------------
BOOL MMDLSYS_SearchUseMMdl(
  const MMDLSYS *mmdlsys, MMDL **mmdl, u32 *no )
{
  u32 max = MMDLSYS_GetMMdlMax( mmdlsys );
  
  if( (*no) < max ){
    MMDL *check_obj = &(((MMDLSYS*)mmdlsys)->pMMdlBuf[*no]);
    
    do{
      (*no)++;
      if( MMDL_CheckStatusBit(check_obj,MMDL_STABIT_USE) ){
        *mmdl = check_obj;
        return( TRUE );
      }
      check_obj++;
    }while( (*no) < max );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 指定されたグリッドX,Z座標にいるOBJ全てをMMDLポインタ配列に取得
 * @param  sys      MMDLSYS *
 * @param  x      検索グリッドX
 * @param  z      検索グリッドZ
 * @param  old_hit    TURE=過去座標も判定する
 * @param array   見つけたMMDLポインタを最大16個分格納する構造体型
 * @return  見つけた配列数 
 */
//--------------------------------------------------------------
static int MMDLSYS_SearchGridPosArray(
  const MMDLSYS *sys, s16 x, s16 z, BOOL old_hit, MMDL_PARRAY *array )
{
  u32 no = 0;
  MMDL *mmdl;
  
  MI_CpuClear8( array, sizeof(MMDL_PARRAY) );
  
  while( MMDLSYS_SearchUseMMdl(sys,&mmdl,&no) == TRUE ){
    if( MMDL_HitCheckXZ(mmdl,x,z,old_hit) == TRUE ){
      array->mmdl_parray[array->count++] = mmdl;
    
      if( array->count >= MMDL_POST_LAYER_MAX ){
        GF_ASSERT_MSG( 0, "同一座標OBJ個数オーバー\n" ); 
        break;  //これ以上格納できない
      }
    }
  }
  
  return array->count;
}

//--------------------------------------------------------------
/**
 * MMDLSYS 指定されたグリッドX,Z座標にいるOBJを取得(立体交差を考えない版)
 * @param  sys      MMDLSYS *
 * @param  x      検索グリッドX
 * @param  z      検索グリッドZ
 * @param  old_hit    TURE=過去座標も判定する
 * @retval  MMDL  x,z位置にいるMMDL * 。NULL=その座標にOBJはいない
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchGridPos(
  const MMDLSYS *sys, s16 x, s16 z, BOOL old_hit )
{
  u32 num = 0;
  MMDL_PARRAY array;

  num = MMDLSYS_SearchGridPosArray(sys, x, z, old_hit , &array);
  
  if( num ){
    return array.mmdl_parray[0];
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 指定されたグリッドX,Z,and Y座標にいるOBJを取得(立体交差を考慮する)
 * @param  sys      MMDLSYS *
 * @param  x        検索グリッドX
 * @param  z        検索グリッドZ
 * @param height  検索Y座標値 fx32型
 * @param h_diff  Y値で許容する振れ幅(絶対値がこの指定値未満のY差分を持つOBJのみを返す)
 *
 * @param  old_hit    TURE=過去座標も判定する
 * @retval  MMDL  x,z位置にいるMMDL * 。NULL=その座標にOBJはいない
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchGridPosEx(
  const MMDLSYS *sys, s16 x, s16 z, fx32 height, fx32 y_diff, BOOL old_hit )
{
  u32 i = 0,num = 0;
  MMDL_PARRAY array;
  
  num = MMDLSYS_SearchGridPosArray(sys, x, z, old_hit , &array);
  
  if( num == 0 ){
    return NULL;
  }
  
  for(i = 0;i < num;i++){
    fx32 y,diff;
    diff = MMDL_GetVectorPosY( array.mmdl_parray[i] ) - height;
    if(diff < 0){
      diff = FX_Mul(diff,FX32_CONST(-1));
    }
    if(diff < y_diff) {
      return array.mmdl_parray[i];
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 動作コードに一致するMMDL *を検索
 * @param  mmdlsys    MMDLSYS *
 * @param  mv_code    検索する動作コード
 * @retval  MMDL *  NULL=存在しない
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchMoveCode( const MMDLSYS *mmdlsys, u16 mv_code )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_GetMoveCode(mmdl) == mv_code ){
      return( mmdl );
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJ IDに一致するMMDL *を検索
 * @param  mmdlsys    MMDLSYS *
 * @param  id    検索するOBJ ID
 * @retval  MMDL *  NULL=存在しない
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchOBJID( const MMDLSYS *mmdlsys, u16 id )
{
  u32 no = 0;
  MMDL *mmdl;

  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_CheckStatusBitAlies(mmdl) == FALSE ){
      if( MMDL_GetOBJID(mmdl) == id ){
        return( mmdl );
      }
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS 自機OBJを探す
 * @param mmdlsys MMDLSYS*
 * @retval MMDL* NULL=存在しない
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchMMdlPlayer( MMDLSYS *mmdlsys )
{
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, MMDL_ID_PLAYER );
  GF_ASSERT( mmdl != NULL );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデルの空きを探す
 * @param  sys      MMDLSYS *
 * @retval  MMDL  空きのMMDL*　空きが無い場合はNULL
 */
//--------------------------------------------------------------
static MMDL * mmdlsys_SearchSpaceMMdl( const MMDLSYS *sys )
{
  int i,max;
  MMDL *mmdl;
  
  i = 0;
  max = MMDLSYS_GetMMdlMax( sys );
  mmdl = ((MMDLSYS*)sys)->pMMdlBuf;
  
  do{
    if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) == 0 ){
      return( mmdl );
    }
    
    mmdl++;
    i++;
  }while( i < max );
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデル　エイリアスを探す
 * @param  mmdlsys      MMDLSYS *
 * @param  obj_id    一致するOBJ ID
 * @param  zone_id    一致するZONE ID
 * @retval  MMDL  一致するMMDL*　一致無し=NULL
 */
//--------------------------------------------------------------
static MMDL * mmdlsys_SearchAlies(
  const MMDLSYS *mmdlsys, int obj_id, int zone_id )
{
  u32 no = 0;
  MMDL * mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
    if( MMDL_CheckStatusBitAlies(mmdl) == TRUE ){
      if( MMDL_GetOBJID(mmdl) == obj_id ){
        if( MMDL_GetZoneIDAlies(mmdl) == zone_id ){
          return( mmdl );
        }
      }
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS フィールド動作モデル ゾーン更新時の動作モデル削除
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteZoneUpdateMMdl( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
    //本来であれば更にエイリアスチェックが入る
    if( MMDL_CheckStatusBit(
        mmdl,MMDL_STABIT_ZONE_DEL_NOT) == 0 ){
      if( MMDL_GetOBJID(mmdl) == 0xff ){
        GF_ASSERT( 0 );
      }
      MMDL_Delete( mmdl );
    }
  }
}

//======================================================================
//  MMDL ツール
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　TCB動作関数追加
 * @param  mmdl  MMDL*
 * @param  sys    MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_AddTCB( MMDL *mmdl, const MMDLSYS *sys )
{
  int pri,code,offs;
  GFL_TCB * tcb;
  
  offs = 0;
  pri = MMDLSYS_GetTCBPriority( sys );
  code = MMDL_GetMoveCode( mmdl );
  
  if( code == MV_PAIR || code == MV_TR_PAIR ){
    offs = MMDL_TCBPRI_OFFS_PAIR;
  }else if( MMDL_GetOBJID(mmdl) == MMDL_ID_PLAYER ){
    offs = MMDL_TCBPRI_OFFS_PLAYER;
  }
  
  pri += offs;
  GF_ASSERT( pri >= 0 );

  tcb = GFL_TCB_AddTask( MMDLSYS_GetTCBSYS((MMDLSYS*)sys),
      mmdl_TCB_MoveProc, mmdl, (u32)pri );
  GF_ASSERT( tcb != NULL );
  
  mmdl->pTCB = tcb;
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　TCB動作関数削除
 * @param  mmdl  MMDL*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_DeleteTCB( MMDL *mmdl )
{
#if 0 //tcb追加されず削除される事もある
  GF_ASSERT( mmdl->pTCB );
  GFL_TCB_DeleteTask( mmdl->pTCB );
  mmdl->pTCB = NULL;
#else
  if( mmdl->pTCB != NULL ){
    GFL_TCB_DeleteTask( mmdl->pTCB );
    mmdl->pTCB = NULL;
  }
#endif
}

//--------------------------------------------------------------
/**
 * MMDL 現在発生しているフィールド動作モデルのOBJコードを参照
 * @param  mmdl  MMDL * 
 * @param  code  チェックするコード。HERO等
 * @retval  BOOL  TRUE=mmdl以外にもcodeを持っている奴がいる
 */
//--------------------------------------------------------------
BOOL MMDL_SearchUseOBJCode( const MMDL *mmdl, u16 code )
{
  u32 no = 0;
  u16 check_code;
  MMDL *cmmdl;
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&cmmdl,&no) == TRUE ){
    if( cmmdl != mmdl ){
      check_code = MMDL_GetOBJCode( cmmdl );
      if( check_code != OBJCODEMAX && check_code == code ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL 座標、方向を初期化。
 * @param  mmdl  MMDL *
 * @param  vec    初期化座標
 * @param  dir    方向 DIR_UP等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_InitPosition( MMDL * mmdl, const VecFx32 *vec, u16 dir )
{
  int grid = SIZE_GRID_FX32( vec->x );
  MMDL_SetGridPosX( mmdl, grid );
  
  grid = SIZE_GRID_FX32( vec->y );
  MMDL_SetGridPosY( mmdl, grid );
  
  grid = SIZE_GRID_FX32( vec->z );
  MMDL_SetGridPosZ( mmdl, grid );
  
  MMDL_SetVectorPos( mmdl, vec );
  MMDL_UpdateGridPosCurrent( mmdl );
  
  MMDL_SetForceDirDisp( mmdl, dir );
  
  MMDL_FreeAcmd( mmdl );
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE|MMDL_MOVEBIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL 座標、方向を初期化。グリッド版
 * @param  mmdl  MMDL *
 * @param  vec    初期化座標
 * @param  dir    方向 DIR_UP等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_InitGridPosition( MMDL * mmdl, s16 gx, s16 gy, s16 gz, u16 dir )
{
  VecFx32 pos;
  pos.x = GRID_SIZE_FX32( gx ) + MMDL_VEC_X_GRID_OFFS_FX32;
  pos.y = GRID_SIZE_FX32( gy );
  pos.z = GRID_SIZE_FX32( gz ) + MMDL_VEC_Z_GRID_OFFS_FX32;
  MMDL_InitPosition( mmdl, &pos, dir );
}

//--------------------------------------------------------------
/**
 * MMDL ヘッダーから座標タイプ、座標、動作コードを変更
 * @param  mmdl  MMDL *
 * @param  head MMDL_HEADAER
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeMoveParam( MMDL *mmdl, const MMDL_HEADER *head )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  MMDL_CallMoveDeleteProc( mmdl );
  mmdl_SetHeaderMoveParam( mmdl, head );
  mmdl_CallSetHeaderPos( mmdl, head );
  mmdl_InitCallMoveProcWork( mmdl );
  mmdl_InitMoveProc( mmdlsys, mmdl );
  mmdl_SetHeaderAfter( mmdl, head, NULL );
}

//--------------------------------------------------------------
/**
 * MMDL 動作コードヘッダーから座標タイプ、座標、コードを変更
 * @param  mmdl  MMDL *
 * @param  code MV_RND等
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeMoveCode( MMDL *mmdl, u16 code )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  MMDL_CallMoveDeleteProc( mmdl );
  MMDL_SetMoveCode( mmdl, code );
  mmdl_InitCallMoveProcWork( mmdl );
  mmdl_InitMoveProc( mmdlsys, mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL 描画初期化に行う処理纏め
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitDrawWork( MMDL *mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == FALSE ){
    return; //描画システム初期化完了していない
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_HEIGHT_GET_ERROR) ){
    MMDL_UpdateCurrentHeight( mmdl );
  }
  
  MMDL_SetDrawStatus( mmdl, 0 );
  
  if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == FALSE ){
    mmdl_InitCallDrawProcWork( mmdl );
    MMDL_CallDrawInitProc( mmdl );
    MMDL_OnMoveBitCompletedDrawInit( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル 描画関数初期化
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitCallDrawProcWork( MMDL * mmdl )
{
  const MMDL_DRAW_PROC_LIST *list;
  const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
  list = DrawProcList_GetList( prm->draw_proc_no );
  mmdl->draw_proc_list = list;
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル関連エフェクトのフラグ初期化。
 * エフェクト関連のフラグ初期化をまとめる。
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitDrawEffectFlag( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl,
    MMDL_MOVEBIT_SHADOW_SET |
    MMDL_MOVEBIT_SHADOW_VANISH |
    MMDL_MOVEBIT_EFFSET_SHOAL |
    MMDL_MOVEBIT_REFLECT_SET );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ IDを変更
 * @param  mmdl  MMDL *
 * @param  id    OBJ ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeOBJID( MMDL * mmdl, u16 id )
{
  MMDL_SetOBJID( mmdl, id );
  MMDL_OnMoveBitMoveStart( mmdl );
  mmdl_InitDrawEffectFlag( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ワーク消去
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_ClearWork( MMDL *mmdl )
{
  GFL_STD_MemClear( mmdl, MMDL_SIZE );
}

//--------------------------------------------------------------
/**
 * MMDL 指定されたフィールド動作モデルがエイリアス指定かどうかチェック
 * @param  mmdl    MMDL *
 * @param  h_zone_id  headを読み込むゾーンID
 * @param  max      head要素数
 * @param  head    MMDL_H
 * @retval  int      RET_ALIES_NOT等
 */
//--------------------------------------------------------------
static int mmdl_CheckHeaderAlies(
    const MMDL *mmdl, int h_zone_id, int max,
    const MMDL_HEADER *head )
{
  u16 obj_id;
  int zone_id;
  
  while( max ){
    obj_id = head->id;
    
    if( MMDL_GetOBJID(mmdl) == obj_id ){
      //エイリアスヘッダー
      if( MMdlHeader_CheckAlies(head) == TRUE ){
        //エイリアスの正規ゾーンID
        zone_id = MMdlHeader_GetAliesZoneID( head );
        //対象エイリアス
        if( MMDL_CheckStatusBitAlies(mmdl) == TRUE ){
          if( MMDL_GetZoneIDAlies(mmdl) == zone_id ){
            return( RET_ALIES_EXIST );  //Aliesとして既に存在
          }
        }else if( MMDL_GetZoneID(mmdl) == zone_id ){
          return( RET_ALIES_CHANGE );    //Alies対象である
        }
      }else{ //通常ヘッダー
        if( MMDL_CheckStatusBitAlies(mmdl) == TRUE ){
          //生存エイリアスと一致
          if( MMDL_GetZoneIDAlies(mmdl) == h_zone_id ){
            return( RET_ALIES_CHANGE );
          }
        }
      }
    }
    
    max--;
    head++;
  }
  
  return( RET_ALIES_NOT );
}

//--------------------------------------------------------------
/**
 * MMDL 指定されたゾーンIDとOBJ IDを持つMMDL *を取得。
 * @param  mmdlsys    MMDLSYS *
 * @param  obj_id  OBJ ID
 * @param  zone_id  ゾーンID
 * @retval  MMDL * MMDL *
 */
//--------------------------------------------------------------
static MMDL * mmdl_SearchOBJIDZoneID(
    const MMDLSYS *mmdlsys, int obj_id, int zone_id )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_GetOBJID(mmdl) == obj_id &&
      MMDL_GetZoneID(mmdl) == zone_id ){
      return( mmdl );
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル描画初期化に行う処理
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitDrawStatus( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
  mmdl_InitDrawEffectFlag( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル描画削除時に行う処理
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetDrawDeleteStatus( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　エイリアスから正規OBJへの変更
 * @param  mmdl    MMDL * 
 * @param  head    対象のMMDL_H
 * @param  zone_id    正規のゾーンID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_ChangeAliesOBJ(
  MMDL *mmdl, const MMDL_HEADER *head, int zone_id )
{
  GF_ASSERT( MMDL_CheckStatusBitAlies(mmdl) == TRUE );
  MMDL_SetStatusBitAlies( mmdl, FALSE );
  MMDL_SetZoneID( mmdl, zone_id );
  MMDL_SetEventID( mmdl, head->event_id );
  MMDL_SetEventFlag( mmdl, head->event_flag );
}

//--------------------------------------------------------------
/**
 * MMDL フィールド動作モデル　正規OBJからエイリアスへの変更
 * @param  mmdl    MMDL * 
 * @param  head    対象のMMDL_H
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_ChangeOBJAlies(
  MMDL * mmdl, int zone_id, const MMDL_HEADER *head )
{
  GF_ASSERT( MMdlHeader_CheckAlies(head) == TRUE );
  MMDL_SetStatusBitAlies( mmdl, TRUE );
  MMDL_SetEventID( mmdl, head->event_id );
  MMDL_SetEventFlag( mmdl, MMdlHeader_GetAliesZoneID(head) );
  MMDL_SetZoneID( mmdl, zone_id );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデルの同一チェック用のMMDL_CHECKSAME_DATA初期化
 * @param  mmdl  MMDL *
 * @param outData 初期化するmmdlの情報格納先
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_InitCheckSameData( const MMDL *mmdl, MMDL_CHECKSAME_DATA *outData )
{
  outData->id = MMDL_GetOBJID( mmdl );
  outData->code = MMDL_GetOBJCode( mmdl );
  outData->zone_id = MMDL_GetZoneID( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL_CHECKSAME_DATAを参照しフィールド動作モデルの同一チェック。
 * 死亡、入れ替わりが発生しているかチェックする。
 * @param  mmdl  MMDL *
 * @param data MMDL_SAMEDATA*
 * @retval BOOL  TRUE=同一。FALSE=同一ではない
 */
//--------------------------------------------------------------
BOOL MMDL_CheckSameData(
    const MMDL * mmdl, const MMDL_CHECKSAME_DATA *data )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) ){
    if( MMDL_GetOBJID(mmdl) == data->id &&
        MMDL_GetOBJCode(mmdl) == data->code &&
        MMDL_GetZoneID(mmdl) == data->zone_id ){
      return( TRUE );
    }
  }
   
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL_CHECKSAME_DATAを参照しフィールド動作モデルの同一チェック。
 * 死亡、入れ替わりが発生しているかチェックする。
 * OBJコードの入れ替わりのみは同一とする。
 * @param  mmdl  MMDL *
 * @param data MMDL_SAMEDATA*
 * @retval BOOL  TRUE=同一。FALSE=同一ではない
 */
//--------------------------------------------------------------
BOOL MMDL_CheckSameDataIDOnly(
    const MMDL * mmdl, const MMDL_CHECKSAME_DATA *data )
{
  if( MMDL_CheckSameData(mmdl,data) == FALSE ){
    if( MMDL_GetOBJCode(mmdl) == data->code ){
      return( TRUE );
    }
  }
   
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * OBJコードを変更する
 * @param mmdl MMDL*
 * @param code 変更するコード。HERO等
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeOBJCode( MMDL *mmdl, u16 code )
{
  const MMDLSYS *mmdlsys;
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
    if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == TRUE ){
      MMDL_CallDrawDeleteProc( mmdl );
    }
  }
  
  MMDL_SetOBJCode( mmdl, code );
  mmdlsys_LoadSetMMdlOBJCodeParam( mmdlsys, code, mmdl );
  
  { //新規OBJCODE_PARAM
    const OBJCODE_PARAM *param = &mmdl->objcode_param;
    mmdl->gx_size = param->size_width;
    mmdl->gz_size = param->size_depth;
    mmdl->offset_x = param->offs_x; //オフセット
    mmdl->offset_y = param->offs_y;
    mmdl->offset_z = param->offs_z;
  }
  
  MMDL_OffMoveBitCompletedDrawInit( mmdl );
  mmdl_InitDrawStatus( mmdl );
  mmdl_InitDrawWork( mmdl );
}

//======================================================================
//  OBJCODE_PARAM
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM 初期化
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_InitOBJCodeParam( MMDLSYS *mmdlsys, HEAPID heapID )
{
#if 0
  u8 *p = GFL_ARC_LoadDataAlloc( ARCID_MMDL_PARAM, 
      NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin, heapID );
  mmdlsys->pOBJCodeParamBuf = p;
  mmdlsys->pOBJCodeParamTbl = (const OBJCODE_PARAM*)(&p[OBJCODE_PARAM_TOTAL_NUMBER_SIZE]);
#endif
  
  mmdlsys->arcH_param = GFL_ARC_OpenDataHandle( ARCID_MMDL_PARAM, heapID );
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM 削除
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_DeleteOBJCodeParam( MMDLSYS *mmdlsys )
{
#if 0
  GFL_HEAP_FreeMemory( mmdlsys->pOBJCodeParamBuf );
  mmdlsys->pOBJCodeParamBuf = NULL;
#endif
  
  GFL_ARC_CloseDataHandle( mmdlsys->arcH_param );
}

#if 0
//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM 取得
 * @param  mmdlsys  MMDLSYS *
 * @param  code  取得するOBJコード
 * @retval  OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDLSYS_GetOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code )
{
  GF_ASSERT( mmdlsys->pOBJCodeParamTbl != NULL );
  code = MMDL_TOOL_OBJCodeToDataNumber( code );
  return( &(mmdlsys->pOBJCodeParamTbl[code]) );
}
#endif

//--------------------------------------------------------------
/**
 * システムにOBJCODE_PARAMをロードする
 * @note　　常駐でメモリを持つことでアクセススピードを高める
 *
 * @param   mmdlsys  MMDLSYS *
 * @param   list_id   エリア別リストＩＤ
 * 
 * @retval  none
 */
//--------------------------------------------------------------
void MMDLSYS_SetSysOBJCodeParam( MMDLSYS *mmdlsys, int list_id)
{
  NOZOMU_Printf("list_id %d\n",list_id);
	GFL_ARC_LoadData( mmdlsys->pMdlPrmWork, ARCID_MMDL_LIST, list_id );
  
  {
    u16 *data = (u16*)mmdlsys->pMdlPrmWork;
    u16 num = data[0];
    GF_ASSERT_MSG(num <= REGULAR_MAX, "%d メモリに入る人数を超えました", num);
  }
}

//--------------------------------------------------------------
/**
 * システムからOBJCODE_PARAMを取得する
 * @note　　常駐でメモリを持つことでアクセススピードを高める
 *
 * @param   mmdlsys  MMDLSYS *
 * @param   idx           システムが保持するパラメータテーブルのインデックス
 * @param   outParam      パラメータ格納バッファ
 *
 * @retval  none
 */
//--------------------------------------------------------------
void MMDLSYS_GetSysOBJCodeParam(
    const MMDLSYS *mmdlsys, const u32 idx, OBJCODE_PARAM *outParam)
{
	u16 *data = (u16*)mmdlsys->pMdlPrmWork;
  u16 num = data[0];
  if (idx < num)
  {
    u16 ofs = ((num+1)/2) * 2;  //アライメントを加味したオブセット
    OBJCODE_PARAM *prm = (OBJCODE_PARAM *)&data[ofs+2];  //+2はヘッダとフッタの分
    *outParam = prm[idx];
  }
  else
  {
    GF_ASSERT_MSG(0,"ERROR idx=%d num = %",idx,num);
  }
}

//--------------------------------------------------------------
/**
 * システムからOBJCODE_PARAMを取得する
 * @note　　常駐でメモリを持つことでアクセススピードを高める
 * 
 * @param  mmdlsys  MMDLSYS *
 *
 * @retval  登録した項目数
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetSysOBJCodeParamNum( const MMDLSYS *mmdlsys)
{
	u16 *data = (u16*)mmdlsys->pMdlPrmWork;
  u16 num = data[0];
  NOZOMU_Printf("OBJ_NUM %d\n",num);
  return num;
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM ロード
 * @param  mmdlsys  MMDLSYS *
 * @param  code  取得するOBJコード
 * @param  outParam 格納先
 * @retval nothing
 * @attention アーカイブデータからロードを行いoutParamに格納します。
 * ・処理が重い為、連続呼びは控えて下さい。
 * ・可能であればMMDL_GetOBJCodeParam()を使用してください。
 * @note 呼び出し速度 約270ms〜300ms
 */
//--------------------------------------------------------------
void MMDLSYS_LoadOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, OBJCODE_PARAM *outParam )
{
  {
    int i;
    u16 *data = (u16*)mmdlsys->pMdlPrmWork;
    u16 num = data[0];
    if (num < REGULAR_MAX)
    {
      u16 ofs = ((num+1)/2) * 2; //アライメントを加味したオブセット
      OBJCODE_PARAM *prm = (OBJCODE_PARAM *)&data[ofs+2]; //+2はヘッダとフッタの分
      for(i=0;i<num;i++)
      {
        if ( code == data[1+i] )
        {
//          NOZOMU_Printf("Find code:%d\n",code);
          *outParam = prm[i];
          return;
        }
      }
    }
  }

  { //既にロード済みの同一コードを持つ動作モデルを探す
    u32 no = 0;
    MMDL *mmdl;
    
    while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
      if( MMDL_GetOBJCode(mmdl) == code ){
        *outParam = mmdl->objcode_param;        
        D_MMDL_DPrintf(
            "MMDLSYS_LoadOBJCodeParam() CODE %xH 既存データアリ\n", code );
        return;
      }
    }
  }

  {
    u16 size = sizeof( OBJCODE_PARAM );
    u16 offs = OBJCODE_PARAM_TOTAL_NUMBER_SIZE +
      (size * MMDL_TOOL_OBJCodeToDataNumber(code));
  
    GFL_ARC_LoadDataOfsByHandle( mmdlsys->arcH_param, 
        NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin,
        offs, size, outParam );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAMを動作モデルにセット
 * @param  mmdlsys  MMDLSYS *
 * @param  code  取得するOBJコード
 * @param  mmdl ロード先MMDL
 * @retval nothing
 */
//--------------------------------------------------------------
static void mmdlsys_LoadSetMMdlOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, MMDL *mmdl )
{
  //ステータスビットを一時的に降ろし
  //MMDLSYS_LoadOBJCodeParam()既出検索の対象から外す
  MMDL_OffStatusBit( mmdl, MMDL_STABIT_USE );
  
  //ロード
  MMDLSYS_LoadOBJCodeParam( mmdlsys, code, &mmdl->objcode_param );
  
  //元に戻す
  MMDL_OnStatusBit( mmdl, MMDL_STABIT_USE );
} 

//--------------------------------------------------------------
/**
 * MMDL MMDLに設定されているOBJCODE_PARAM取得
 * @param  mmdl  MMDL*
 * @param OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDL_GetOBJCodeParam( const MMDL *mmdl )
{
#if 0
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  return( MMDLSYS_GetOBJCodeParam(mmdlsys,code) );
#else  
  return( &(mmdl->objcode_param) );
#endif
}

//--------------------------------------------------------------
/**
 * MMDL OBJCODE_PARAMからOBJCODE_PARAM_BUF_BBD取得
 * @param
 * @retval
 */
//--------------------------------------------------------------
const OBJCODE_PARAM_BUF_BBD * MMDL_TOOL_GetOBJCodeParamBufBBD(
    const OBJCODE_PARAM *param )
{
  return (const OBJCODE_PARAM_BUF_BBD*)param->buf;
}

//--------------------------------------------------------------
/**
 * MMDL OBJCODE_PARAMからOBJCODE_PARAM_BUF_MDL取得
 * @param
 * @retval
 */
//--------------------------------------------------------------
const OBJCODE_PARAM_BUF_MDL * MMDL_TOOL_GetOBJCodeParamBufMDL(
    const OBJCODE_PARAM *param )
{
  return (const OBJCODE_PARAM_BUF_MDL*)param->buf;
}

//======================================================================
//  OBJCODE_PARAM 横幅、奥行、オフセット
//======================================================================

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * 指定されたOBJコードがワーク参照型ならばワーク内OBJコードにして変更
 * 違う場合はそのままで返す。
 * @param  fsys  FIELDSYS_WORK *
 * @param  code  OBJコード。HERO等
 * @retval  int    チェックされたOBJコード
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetWorkOBJCode( const EVENTWORK *ev, u16 code )
{
  return( WorkOBJCode_GetOBJCode(ev,code) );
}

//--------------------------------------------------------------
/**
 * 指定されたOBJコードがワーク参照型ならばワーク内OBJコードに変更。
 * 違う場合はそのままで返す。
 * @param  fsys  FIELDSYS_WORK *
 * @param  code  OBJコード。HERO等
 * @retval  int    チェックされたOBJコード
 */
//--------------------------------------------------------------
static u16 WorkOBJCode_GetOBJCode( const EVENTWORK *ev, u16 code )
{
  if( code >= WKOBJCODE_START && code <= WKOBJCODE_END ){
    if( ev == NULL ){
      GF_ASSERT( 0 );
      code = BOY1; //無難なコードに強制変更
    }else{
      u16 *work;
      code -= WKOBJCODE_START;
      code += OBJCHRWORK0;
      work = EVENTWORK_GetEventWorkAdrs( (EVENTWORK*)ev, code );
      code = *work;
    }
  }
  
  return( code );
}

//--------------------------------------------------------------
/**
 * 指定された動作コードの関数リストを取得
 * @param  code  動作コード
 * @retval MMDL_MOVE_PROC_LIST
 */
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code )
{
  GF_ASSERT( code < MV_CODE_MAX );
  return( DATA_FieldOBJMoveProcListTbl[code] );
}

//--------------------------------------------------------------
/**
 * 指定されたOBJコードの描画関数リストを取得
 * @param  code  OBJコード
 * @retval  MMDL_DRAW_PROC_LIST*
 */
//--------------------------------------------------------------
static const MMDL_DRAW_PROC_LIST * DrawProcList_GetList(
    MMDL_DRAWPROCNO no )
{
  GF_ASSERT( no < MMDL_DRAWPROCNO_MAX );
  return( DATA_MMDL_DRAW_PROC_LIST_Tbl[no] );
}

//--------------------------------------------------------------
/**
 * MMDL_HEADER エイリアスチェック
 * @param  head  FIELD_OBJ_H
 * @retval  int    TRUE=エイリアス　FALSE=違う
 */
//--------------------------------------------------------------
static BOOL MMdlHeader_CheckAlies( const MMDL_HEADER *head )
{
  u16 id = head->event_id;
  if( id == SP_SCRID_ALIES ){ return( TRUE ); }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * フィールドOBJヘッダー　エイリアス時のゾーンID取得。
 * エイリアス時はイベントフラグがゾーンIDになる
 * @param  head  FIELD_OBJ_H
 * @retval  int    ゾーンID
 */
//--------------------------------------------------------------
static int MMdlHeader_GetAliesZoneID( const MMDL_HEADER *head )
{
  GF_ASSERT( MMdlHeader_CheckAlies(head) == TRUE );
  return( (int)head->event_flag );
}

//--------------------------------------------------------------
/**
 * イベントフラグチェック
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL mmdlsys_CheckEventFlag( EVENTWORK *evwork, u16 flag_no )
{
#if 0 //
  FIELDMAP_WORK *fieldMap = mmdlsys->fieldMapWork;
  GF_ASSERT( fieldMap != NULL );
  
  if( fieldMap == NULL ){
    return( FALSE );
  }
  
  return( FALSE ); //FIELDMAP_WORKの扱いが確定していない。
  
  {
    GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
    return( EVENTWORK_CheckEventFlag(ev,flag_no) );
  }
#else
  return( EVENTWORK_CheckEventFlag(evwork,flag_no) );
#endif
}

//--------------------------------------------------------------
/**
 * 指定されたタイプがトレーナータイプであれば
 * EV_TYPE_TRAINERもしくはEV_TYPE_TRAINER_EYEALLで返す。
 * それ以外のタイプはそのまま返す
 * @param type EV_TYPE_TRAINER等
 * @retval u16 EV_TYPE_TRAINER等
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetTrainerEventType( u16 type )
{
  switch( type ){
  case EV_TYPE_TRAINER_KYORO:
  case EV_TYPE_TRAINER_SPIN_STOP_L:
  case EV_TYPE_TRAINER_SPIN_STOP_R:
  case EV_TYPE_TRAINER_SPIN_MOVE_L:
  case EV_TYPE_TRAINER_SPIN_MOVE_R:
  case EV_TYPE_TRAINER_DASH_ALTER:
  case EV_TYPE_TRAINER_DASH_REACT:
  case EV_TYPE_TRAINER_DASH_ACCEL:
    type = EV_TYPE_TRAINER;
    break;
  }
  
  return( type );
}

//======================================================================
//  動作関数ダミー
//======================================================================
//--------------------------------------------------------------
/**
 * 動作初期化関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveInitProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * 動作関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * 動作削除関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveDeleteProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * 動作復帰関数ダミー
 * @param  MMDL *  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveReturnProcDummy( MMDL * mmdl )
{
}

//======================================================================
//  描画関数ダミー
//======================================================================
//--------------------------------------------------------------
/**
 * 描画初期化関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  int      TRUE=初期化成功
 */
//--------------------------------------------------------------
void MMDL_DrawInitProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画削除関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawDeleteProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画退避関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawPushProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画復帰関数ダミー
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawPopProcDummy( MMDL * mmdl )
{
}

//======================================================================
//  MMDL_ROCKPOS,かいりき岩が参照、保存する座標データ
//======================================================================
//--------------------------------------------------------------
/// 参照位置データ
//--------------------------------------------------------------
#include "../../../resource/fldmapdata/eventdata/define/total_header.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../../../resource/fldmapdata/pushrock/rockpos.cdat"

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS ワークサイズを取得
 * @param nothing
 * @retval u32 ワークサイズ
 */
//--------------------------------------------------------------
u32 MMDL_ROCKPOS_GetWorkSize( void )
{
  return( sizeof(MMDL_ROCKPOS)*MMDL_ROCKPOS_MAX );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS ワーク初期化
 * @param p MMDL_ROCKPOS*
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_ROCKPOS_Init( void *p )
{
  MI_CpuFill32( p, ROCKPOS_INIT, sizeof(MMDL_ROCKPOS)*MMDL_ROCKPOS_MAX );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS 座標がセットされているかどうか
 * @param rockpos MMDL_ROCKPOS
 * @retval  BOOL TRUE=セット済み
 */
//--------------------------------------------------------------
static BOOL mmdl_rockpos_CheckSetPos( const MMDL_ROCKPOS *rockpos )
{
  if( (u32)rockpos->pos.x != ROCKPOS_INIT ||
      (u32)rockpos->pos.y != ROCKPOS_INIT ||
      (u32)rockpos->pos.z != ROCKPOS_INIT ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS 指定IDが使用する座標ワーク位置。
 * @param zone_id ZONE ID
 * @param obj_id OBJ ID
 * @retval int 使用する座標ワークの位置番号。MMDL_ROCKPOS_MAX=無し。
 */
//--------------------------------------------------------------
static int rockpos_GetPosNumber( const u16 zone_id, const u16 obj_id )
{
  u16 z,o;
  int i = 0;
  
  do{
    z = DATA_MMDL_PushRockPosNum[i][0];
    o = DATA_MMDL_PushRockPosNum[i][1];
    
    D_MMDL_DPrintf( "怪力岩 座標ワーク検索 zone_id = %xH, obj_id = %xH,  data zone_id = %xH, data obj_id = %xH\n", zone_id, obj_id, z, o );

    if( z == zone_id && o == obj_id ){
      return( DATA_MMDL_PushRockPosNum[i][2] );
    }
    
    i++;
  }while( (z != ROCKPOS_DATA_END && o != ROCKPOS_DATA_END) );
  
  return( MMDL_ROCKPOS_MAX );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS 指定の動作モデルが座標を保持しているかどうか
 * @param mmdl  MMDL*
 * @retval int 保持している位置番号。MMDL_ROCKPOS_MAX=保持無し。
 */
//--------------------------------------------------------------
static int mmdl_rockpos_GetPosNumber( const MMDL *mmdl )
{
  int no = MMDL_ROCKPOS_MAX;
  
  if( MMDL_CheckOBJCodeKairiki(MMDL_GetOBJCode(mmdl)) == TRUE ){
    u16 zone_id = MMDL_GetZoneID( mmdl );
    u16 obj_id = MMDL_GetOBJID( mmdl );
    no = rockpos_GetPosNumber( zone_id, obj_id );
  }
  
  return( no );
}

//--------------------------------------------------------------
/**
 * 指定OBJの座標が専用ワークに保存されているか
 * @param mmdl MMDL*
 * @retval  BOOL TRUE=保存あり。FALSE=無し。もしくは初期値のまま。
 */
//--------------------------------------------------------------
static BOOL mmdl_rockpos_CheckPos( const MMDL *mmdl )
{
  int no = mmdl_rockpos_GetPosNumber( mmdl );
  
  if( no != MMDL_ROCKPOS_MAX ){
    const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    const MMDL_ROCKPOS *rockpos = &mmdlsys->rockpos[no];
    return( mmdl_rockpos_CheckSetPos(rockpos) );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * 指定の動作モデルが保存している座標を取得
 * @param mmdl MMDL*
 * @param pos 座標保存先
 * @retval BOOL TRUE=取得 FALSE=取得できない。
 * @note かいりき岩OBJのみが対象。違う場合はアサート。
 */
//--------------------------------------------------------------
static BOOL mmdl_rockpos_GetPos( const MMDL *mmdl, VecFx32 *pos )
{
  int no = mmdl_rockpos_GetPosNumber( mmdl );
  
  if( no != MMDL_ROCKPOS_MAX ){
    const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    const MMDL_ROCKPOS *rockpos = &mmdlsys->rockpos[no];
    
    if( mmdl_rockpos_CheckSetPos(rockpos) ){
      *pos = rockpos->pos;
      return( TRUE );
    }
  }
  
  GF_ASSERT( 0 );
  return( FALSE );
}
 
//--------------------------------------------------------------
/**
 * 指定の動作モデルの座標をMMDL_ROCKPOSに保存。
 * @param mmdl MMDL*
 * @retval nothing
 * @note かいりき岩OBJのみが対象。
 */
//--------------------------------------------------------------
void MMDL_ROCKPOS_SavePos( const MMDL *mmdl )
{
  int no = mmdl_rockpos_GetPosNumber( mmdl );
  
  if( no != MMDL_ROCKPOS_MAX ){
    VecFx32 pos;
    const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    MMDL_ROCKPOS *rockpos = &mmdlsys->rockpos[no];
    MMDL_GetVectorPos( mmdl, &pos );
    rockpos->pos = pos;
    return;
  }
  
  GF_ASSERT( 0 && "ERROR NOT ROCK OF SAVE" );
}

//--------------------------------------------------------------
/**
 * 指定位置にかいりき岩OBJが落ちているか
 * @param mmdlsys MMDLSYS*
 * @param pos チェックする座標
 * @retval BOOL TRUE=岩が落ちている
 */
//--------------------------------------------------------------
BOOL MMDLSYS_ROCKPOS_CheckRockFalled(
    const MMDLSYS *mmdlsys, const VecFx32 *pos )
{
  s16 gx,gz;
  MMDL *mmdl;
  
  gx = SIZE_GRID_FX32( pos->x );
  gz = SIZE_GRID_FX32( pos->z );
  
  mmdl = MMDLSYS_SearchGridPosEx(
      mmdlsys, gx, gz, pos->y, GRID_FX32*2, FALSE );
  
  if( mmdl != NULL ){
    if( mmdl_rockpos_CheckPos(mmdl) == TRUE ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * OBJコードは怪力で押すタイプかチェック
 * @param code OBJコード
 * @retval BOOL TRUE=怪力で押せる
 */
//--------------------------------------------------------------
BOOL MMDL_CheckOBJCodeKairiki( const u16 code )
{
  if( code == ROCK || code == BIGROCK || code == JUNK || code == BIGROCK2 ){
    return( TRUE );
  }
  return( FALSE );
}

//======================================================================
//
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  ヘッダーにグリッドマップポジションを設定
 *  @param  head    ヘッダー
 *  @param  gx      ｘグリッド座標
 *  @param  gz      ｚグリッド座標
 *  @param  y       ｙ座標
 */
//--------------------------------------------------------------
void MMDLHEADER_SetGridPos( MMDL_HEADER* head, u16 gx, u16 gz, int y )
{
  MMDL_HEADER_GRIDPOS* pos;
  
  GF_ASSERT( head );
  head->pos_type = MMDL_HEADER_POSTYPE_GRID;
  pos = (MMDL_HEADER_GRIDPOS*)head->pos_buf;

  pos->gx = gx;
  pos->gz = gz;
  pos->y  = y;
}

//--------------------------------------------------------------
/**
 *  @brief  ヘッダーにレールマップポジションを設定
 *  @param  head    ヘッダー
 *  @param  index   インデックス
 *  @param  front   フロントグリッド座標
 *  @param  side    サイドグリッド座標
 */
//--------------------------------------------------------------
void MMDLHEADER_SetRailPos( MMDL_HEADER* head, u16 index, u16 front, u16 side )
{
  MMDL_HEADER_RAILPOS* pos;
  
  GF_ASSERT( head );
  head->pos_type = MMDL_HEADER_POSTYPE_RAIL;
  pos = (MMDL_HEADER_RAILPOS*)head->pos_buf;

  pos->rail_index = index;
  pos->front_grid = front;
  pos->side_grid  = side;
}


//======================================================================
//  debug
//======================================================================
//----
#ifdef DEBUG_MMDL
//----
#include "arc/fieldmap/fldmmdl_objcodestr.naix"

//--------------------------------------------------------------
/**
 * デバッグ　動作モデル　OBJコード文字列を取得(ASCII)
 * @param  code OBJコード
 * @param  heapID buf領域確保用HEAPID
 * @retval  u8* 文字列が格納されたu8*。使用後GFL_HEAP_FreeMemory()が必要。
 * 文字列長はDEBUG_OBJCODE_STR_LENGTH。
 */
//--------------------------------------------------------------
u8 * DEBUG_MMDL_GetOBJCodeString( u16 code, HEAPID heapID )
{
  u8 *buf;
  
  code = MMDL_TOOL_OBJCodeToDataNumber( code );
  
  buf = GFL_HEAP_AllocClearMemoryLo(
      heapID, DEBUG_OBJCODE_STR_LENGTH );
  
  GFL_ARC_LoadDataOfs( buf, ARCID_DEBUG_MMDL_OBJCODESTR,
      NARC_fldmmdl_objcodestr_fldmmdl_objcodestr_bin,
      DEBUG_OBJCODE_STR_LENGTH * code, DEBUG_OBJCODE_STR_LENGTH );
  
  return( buf );
}

//--------------------------------------------------------------
/**
 * 動作モデル情報をプリント
 * @param mmdl 対象の動作モデル
 * @param f_str 事前にプリントする文字列 NULL=なし
 * @param e_str MMDLプリント後にプリントする文字列 NULL=なし
 * @retval nothing
 */
//--------------------------------------------------------------
void DEBUG_MMDL_PrintState(
    const MMDL *mmdl, const char *f_str, const char *e_str )
{
  if( f_str != NULL ){
    OS_TPrintf( "%s\n", f_str );
  }
  
  D_MMDL_Printf( "MMDL ID %d : CODE %xH : MOVE CODE =%xH : DIR %d : GX = %d GY = %d GZ = %d : EV TYPE %d EV FLAG %d : PARAM 0(%d) 1(%d) 2(%d)\n",
      MMDL_GetOBJID(mmdl),
      MMDL_GetOBJCode(mmdl),
      MMDL_GetMoveCode(mmdl),
      MMDL_GetDirDisp(mmdl),
      MMDL_GetGridPosX(mmdl),
      MMDL_GetGridPosY(mmdl),
      MMDL_GetGridPosZ(mmdl),
      MMDL_GetEventType(mmdl),
      MMDL_GetEventFlag(mmdl),
      MMDL_GetParam(mmdl,MMDL_PARAM_0),
      MMDL_GetParam(mmdl,MMDL_PARAM_1),
      MMDL_GetParam(mmdl,MMDL_PARAM_2)
      );
      
  if( e_str != NULL ){
    OS_TPrintf( "%s\n", e_str );
  }
}

#ifdef DEBUG_ONLY_FOR_kagaya
#define DEBUG_SPEED_CHECK_ENABLE  //デバッグ速度計測を有効にする
#include "debug_speed_check.h"

#if 0
void DEBUG_MMDLSYS_CheckSpeed( MMDLSYS *mmdlsys )
{
  OBJCODE_PARAM param;
  MMDL *mmdl = MMDLSYS_SearchMMdlPlayer( mmdlsys );
  u16 code;

  code = MMDL_GetOBJCode( mmdl );
#if 0  
  if( code == HERO ){
    code = HEROINE;
  }else{
    code = HERO;
  }
#endif  
  INIT_CHECK();
  SET_CHECK( "MMDL CHECK PARAM_START" );
  SET_CHECK( "MMDL CHECK PARAM0" );
  
  MMDLSYS_LoadOBJCodeParam( mmdlsys, code, &param );
  
  SET_CHECK( "MMDL CHECK PARAM1" );
  SET_CHECK( "MMDL CHECK PARAM_END" );
  PUT_CHECK();

  {
    OSTick end_tick;
    TAIL_CHECK( &end_tick );
    OS_TPrintf("MMDL top_tick:%ld", end_tick);
    OS_TPrintf(" MMDL micro sec:%ld\n", OS_TicksToMicroSeconds( end_tick ) );
  }

  GF_ASSERT( 0 && "MMDL CHECK END" );
}
#endif
#endif

//----
#endif //DEBUG_MMDL
//----

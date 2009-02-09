//======================================================================
/**
 * @file	fldmmdl.c
 * @brief	フィールド動作モデル
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include "fldmmdl.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------

//--------------------------------------------------------------
///	エイリアスシンボル
//--------------------------------------------------------------
enum
{
	RET_ALIES_NOT = 0,	//エイリアスではない
	RET_ALIES_EXIST,	//エイリアスとして既に存在している
	RET_ALIES_CHANGE,	//エイリアス変更が必要である
};

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLDMMDLSYS構造体
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDLSYS
{
	u32 status_bit;					///<ステータスビット
	u16 fmmdl_max;					///<FLDMMDL最大数
	s16 fmmdl_count;				///<フィールド動作モデル現在数
	u16 tcb_pri;					///<TCBプライオリティ
	HEAPID heapID;					///<ヒープID
	FLDMMDL *pFldMMdlBuf;			///<FLDMMDLワーク *
	void *pTCBSysWork;				///<TCBワーク
	GFL_TCBSYS *pTCBSys;			///<TCBSYS*
	FLDMMDL_BLACTCONT *pBlActCont;	///<FLDMMDL_BLACTCONT
	ARCHANDLE *pArcHandle;			///<アーカイブハンドル
	
	const FLDMAPPER *pG3DMapper;	///<FLDMAPPER
	FIELD_MAIN_WORK *pFldMainWork;	///<FIELD_MAIN_WORK
}FLDMMDLSYS;

#define FLDMMDLSYS_SIZE (sizeof(FLDMMDLSYS)) ///<FLDMMDLSYSサイズ

//--------------------------------------------------------------
///	FLDMMDL構造体
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDL
{
	u32 status_bit;				///<ステータスビット
	u32 move_bit;				///<動作ビット
	
	u16 obj_id;					///<OBJ ID
	u16 zone_id;				///<ゾーン ID
	u16 obj_code;				///<OBJコード
	u16 move_code;				///<動作コード
	u16 event_type;				///<イベントタイプ
	u16 event_flag;				///<イベントフラグ
	u16 event_id;				///<イベントID
	u16 dir_head;				///<FLDMMDL_H指定方向
	u16 dir_disp;				///<現在向いている方向
	u16 dir_move;				///<現在動いている方向
	u16 dir_disp_old;			///<過去の動いていた方向
	u16 dir_move_old;			///<過去の動いていた方向
	
	int param0;					///<ヘッダ指定パラメタ
	int param1;					///<ヘッダ指定パラメタ
	int param2;					///<ヘッダ指定パラメタ
	
	u16 acmd_code;				///<アニメーションコマンドコード
	u16 acmd_seq;				///<アニメーションコマンドシーケンス
	u16 draw_status;			///<描画ステータス
	s16 move_limit_x;			///<X方向移動制限
	s16 move_limit_z;			///<Z方向移動制限
	s16 gx_init;				///<初期グリッドX
	s16 gy_init;				///<初期グリッドY
	s16 gz_init;				///<初期グリッドZ
	s16 gx_old;					///<過去グリッドX
	s16 gy_old;					///<過去グリッドY
	s16 gz_old;					///<過去グリッドZ
	s16 gx_now;					///<現在グリッドX
	s16 gy_now;					///<現在グリッドY
	s16 gz_now;					///<現在グリッドZ
	VecFx32 vec_pos_now;		///<現在実数座標
	VecFx32 vec_draw_offs;		///<表示座標オフセット
	VecFx32 vec_draw_offs_outside;	///<外部指定表示座標オフセット
	VecFx32 vec_attr_offs;		///<アトリビュートによる座標オフセット
	u32 now_attr;				///<現在のマップアトリビュート
	u32 old_attr;				///<過去のマップアトリビュート
	
	GFL_TCB *pTCB;				///<動作関数TCB*
	const FLDMMDLSYS *pFldMMdlSys;///<FLDMMDLSYS*
	
	FLDMMDL_MOVE_PROC_INIT move_init_proc;	///<初期化関数
	FLDMMDL_MOVE_PROC move_proc;			///<動作関数
	FLDMMDL_MOVE_PROC_DEL move_delete_proc;	///<削除関数
	FLDMMDL_DRAW_PROC_INIT draw_init_proc;	///<描画初期化関数
	FLDMMDL_DRAW_PROC draw_proc;			///<描画関数
	FLDMMDL_DRAW_PROC_DEL draw_delete_proc;	///<描画削除関数
	FLDMMDL_DRAW_PROC_PUSH draw_push_proc;	///<描画退避関数
	FLDMMDL_DRAW_PROC_POP draw_pop_proc;	///<描画復帰関数
	
	///動作関数用ワーク
	u8 move_proc_work[FLDMMDL_MOVE_WORK_SIZE];
	///動作サブ関数用ワーク
	u8 move_sub_proc_work[FLDMMDL_MOVE_SUB_WORK_SIZE];
	///動作コマンド用ワーク
	u8 move_cmd_proc_work[FLDMMDL_MOVE_CMD_WORK_SIZE];
	///描画関数用ワーク
	u8 draw_proc_work[FLDMMDL_DRAW_WORK_SIZE];
	
	GFL_BBDACT_ACTUNIT_ID blActID;
}FLDMMDL;

#define FLDMMDL_SIZE (sizeof(FLDMMDL)) ///<FLDMMDLサイズ

//--------------------------------------------------------------
///	FLDMMDL_HEADER_LOAD_FILE構造体
//--------------------------------------------------------------
typedef struct
{
	int zone;
	int add_max;			//登録最大数
	int add_count;			//追加数
	const FLDMMDLSYS *sys;	//FLDMMDLSYS *
	FLDMMDL_HEADER *head;		//ヘッダー
}FLDMMDL_HEADER_LOAD_FILE;

///FLDMMDL_HEADER_LOAD_FILEサイズ
#define FLDMMDL_HEADER_LOAD_FILE_SIZE (sizeof(FLDMMDL_HEADER_LOAD_FILE))

//======================================================================
//	proto
//======================================================================
//FLDMMDL 追加、削除
static void FldMMdl_SetHeader(
	FLDMMDL * fmmdl, const FLDMMDL_HEADER *head, void *sys );
static void FldMMdl_SetHeaderPos( FLDMMDL *fmmdl, const FLDMMDL_HEADER *head );
static void FldMMdl_InitWork( FLDMMDL * fmmdl, const FLDMMDLSYS *sys );
static void FldMMdl_InitMoveProcWork( FLDMMDL * fmmdl );
static void FldMMdl_InitMoveWork( FLDMMDL * fmmdl );

//FLDMMDL 動作関数
static void FldMMdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void FldMMdl_TCB_DrawProc( FLDMMDL * fmmdl );

//FLDMMDLSYS 設定、参照
static void FldMMdlSys_OnStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit );
static void FldMMdlSys_OffStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit );
static void FldMMdlSys_IncrementOBJCount( FLDMMDLSYS *fmmdlsys );
static void FldMMdlSys_DecrementOBJCount( FLDMMDLSYS *fmmdlsys );

//FLDMMDLSYS ツール
static FLDMMDL * FldMMdlSys_SearchSpaceFldMMdl( const FLDMMDLSYS *sys );
static FLDMMDL * FldMMdlSys_SearchAlies(
	const FLDMMDLSYS *fos, int obj_id, int zone_id );
static void FldMMdlSys_AddFldMMdlTCB(
	const FLDMMDLSYS *sys, FLDMMDL * fmmdl );

//FLDMMDL ツール
static void FldMMdl_InitDrawWork( FLDMMDL *fmmdl );
static void FldMMdl_InitDrawProcWork( FLDMMDL * fmmdl );
static void FldMMdl_InitDrawEffectFlag( FLDMMDL * fmmdl );
static void FldMMdl_ClearWork( FLDMMDL *fmmdl );
static int FldMMdl_CheckHeaderAlies(
		const FLDMMDL *fmmdl, int h_zone_id, int max,
		const FLDMMDL_HEADER *head );
static FLDMMDL * FldMMdl_SearchOBJIDZoneID(
		const FLDMMDLSYS *fos, int obj_id, int zone_id );
static void FldMMdl_InitDrawStatus( FLDMMDL * fmmdl );
static void FldMMdl_SetDrawDeleteStatus( FLDMMDL * fmmdl );
static void FldMMdl_ChangeAliesOBJ(
	FLDMMDL *fmmdl, const FLDMMDL_HEADER *head, int zone_id );
static void FldMMdl_ChangeOBJAlies(
	FLDMMDL * fmmdl, int zone_id, const FLDMMDL_HEADER *head );

//parts
static u16 WorkOBJCode_GetOBJCode( void *fsys, int code );
static const FLDMMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code );
static const FLDMMDL_DRAW_PROC_LIST * DrawProcList_GetList( u16 code );
static BOOL FldMMdlHeader_CheckAlies( const FLDMMDL_HEADER *head );
static int FldMMdlHeader_GetAliesZoneID( const FLDMMDL_HEADER *head );

//======================================================================
//	フィールド動作モデル　システム
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS システム作成
 * @param	fsys	FIELDSYS_WORK *
 * @param	max	処理するOBJ最大数
 * @retval	FLDMMDLSYS*	追加されたフィールド動作モデルシステム*
 */
//--------------------------------------------------------------
FLDMMDLSYS * FLDMMDLSYS_Create(
	FIELD_MAIN_WORK *pFldMainWork,
	const FLDMAPPER *pG3DMapper, HEAPID heapID, int max )
{
	FLDMMDLSYS *fos;
	
	fos = GFL_HEAP_AllocClearMemory( heapID, FLDMMDLSYS_SIZE );
	fos->pFldMMdlBuf = GFL_HEAP_AllocClearMemory( heapID, max*FLDMMDL_SIZE );
	fos->fmmdl_max = max;
	fos->heapID = heapID;
	fos->pFldMainWork = pFldMainWork;
	fos->pG3DMapper = pG3DMapper;
	
	fos->pTCBSysWork = GFL_HEAP_AllocMemory(
		heapID, GFL_TCB_CalcSystemWorkSize(max) );
	fos->pTCBSys = GFL_TCB_Init( max, fos->pTCBSysWork );
	return( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS システム削除
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_Delete( FLDMMDLSYS *fos )
{
	GFL_TCB_Exit( fos->pTCBSys );
	GFL_HEAP_FreeMemory( fos->pTCBSys );
	GFL_HEAP_FreeMemory( fos->pFldMMdlBuf );
	GFL_HEAP_FreeMemory( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 全てを削除
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteAll( FLDMMDLSYS *fos )
{
	FLDMMDLSYS_DeleteMMdl( fos );
	FLDMMDLSYS_DeleteDraw( fos );
	FLDMMDLSYS_Delete( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 動作モデル更新
 * @param	fos	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_UpdateMove( FLDMMDLSYS *fos )
{
	GFL_TCBSYS *tcbsys = fos->pTCBSys;
	if( tcbsys != NULL ){
		GFL_TCB_Main( tcbsys );
	}
}

//======================================================================
//	フィールド動作モデル追加、削除
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS フィールド動作モデルを追加
 * @param	fos			FLDMMDLSYS *
 * @param	head		追加する情報を纏めたFLDMMDL_HEADER *
 * @param	zone_id		ゾーンID
 * @retval	FLDMMDL	追加されたFLDMMDL *
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_AddFldMMdl(
	const FLDMMDLSYS *fos, const FLDMMDL_HEADER *header, int zone_id )
{
	FLDMMDL *fmmdl;
	FLDMMDL_HEADER headdata = *header;
	FLDMMDL_HEADER *head = &headdata;
	
	fmmdl = FldMMdlSys_SearchSpaceFldMMdl( fos );
	GF_ASSERT( fmmdl != NULL );
	
	FldMMdl_SetHeader( fmmdl, head, NULL );
	FldMMdl_InitWork( fmmdl, fos );
	FLDMMDL_SetZoneID( fmmdl, zone_id );
	
	FldMMdl_InitMoveWork( fmmdl );
	FldMMdl_InitDrawWork( fmmdl );
	
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
	
	FldMMdlSys_AddFldMMdlTCB( fos, fmmdl );
	FldMMdlSys_IncrementOBJCount( (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys(fmmdl) );
	
	return( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデルを削除
 * @param	fmmdl		削除するFLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_Delete( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos;
	
	fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		FLDMMDL_CallDrawDeleteProc( fmmdl );
	}
	
	FLDMMDL_CallMoveDeleteProc( fmmdl );
	GFL_TCB_DeleteTask( fmmdl->pTCB );
	FldMMdlSys_DecrementOBJCount( (FLDMMDLSYS*)(fmmdl->pFldMMdlSys) );
	FldMMdl_ClearWork( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 現在発生しているフィールド動作モデルを全て削除
 * @param	fmmdl		削除するFLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteMMdl( const FLDMMDLSYS *fos )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) ){
		FLDMMDL_Delete( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル　ヘッダー情報反映
 * @param	fmmdl		設定するFLDMMDL * 
 * @param	head		反映する情報を纏めたFLDMMDL_HEADER *
 * @param	fsys		FIELDSYS_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SetHeader(
	FLDMMDL * fmmdl, const FLDMMDL_HEADER *head, void *sys )
{
	FLDMMDL_SetOBJID( fmmdl, head->id );
	FLDMMDL_SetOBJCode( fmmdl, WorkOBJCode_GetOBJCode(sys,head->obj_code) );
	FLDMMDL_SetMoveCode( fmmdl, head->move_code );
	FLDMMDL_SetEventType( fmmdl, head->event_type );
	FLDMMDL_SetEventFlag( fmmdl, head->event_flag );
	FLDMMDL_SetEventID( fmmdl, head->event_id );
	fmmdl->dir_head = head->dir;
	FLDMMDL_SetParam( fmmdl, head->param0, FLDMMDL_PARAM_0 );
	FLDMMDL_SetParam( fmmdl, head->param1, FLDMMDL_PARAM_1 );
	FLDMMDL_SetParam( fmmdl, head->param2, FLDMMDL_PARAM_2 );
	FLDMMDL_SetMoveLimitX( fmmdl, head->move_limit_x );
	FLDMMDL_SetMoveLimitZ( fmmdl, head->move_limit_z );
	
	FldMMdl_SetHeaderPos( fmmdl, head );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル　座標系初期化
 * @param	fmmdl		FLDMMDL * 
 * @param	head		反映する情報を纏めたFLDMMDL_HEADER *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SetHeaderPos( FLDMMDL *fmmdl, const FLDMMDL_HEADER *head )
{
	int pos;
	VecFx32 vec;
	
	pos = head->gx;
	vec.x = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_X_GRID_OFFS_FX32;
	FLDMMDL_SetInitGridPosX( fmmdl, pos );
	FLDMMDL_SetOldGridPosX( fmmdl, pos );
	FLDMMDL_SetGridPosX( fmmdl, pos );
	
	pos = head->gy;		//pos設定はfx32型で来る。
	vec.y = (fx32)pos;
	pos = SIZE_GRID_FX32( pos );
	FLDMMDL_SetInitGridPosY( fmmdl, pos );
	FLDMMDL_SetOldGridPosY( fmmdl, pos );
	FLDMMDL_SetGridPosY( fmmdl, pos );
	
	pos = head->gz;
	vec.z = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_Z_GRID_OFFS_FX32;
	FLDMMDL_SetInitGridPosZ( fmmdl, pos );
	FLDMMDL_SetOldGridPosZ( fmmdl, pos );
	FLDMMDL_SetGridPosZ( fmmdl, pos );
	
	FLDMMDL_SetVectorPos( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル　ワーク初期化
 * @param	fmmdl		FLDMMDL * 
 * @param	sys			FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitWork( FLDMMDL * fmmdl, const FLDMMDLSYS *sys )
{
	FLDMMDL_OnStatusBit( fmmdl,
		FLDMMDL_STABIT_USE |				//使用中
		FLDMMDL_STABIT_HEIGHT_GET_ERROR |	//高さ取得が必要
		FLDMMDL_STABIT_ATTR_GET_ERROR );	//アトリビュート取得が必要
	
	if( FLDMMDL_CheckAliesEventID(fmmdl) == TRUE ){
		FLDMMDL_SetStatusBitAlies( fmmdl, TRUE );
	}
	
	fmmdl->pFldMMdlSys = sys;
	FLDMMDL_SetForceDirDisp( fmmdl, FLDMMDL_GetDirHeader(fmmdl) );
	FLDMMDL_SetDirMove( fmmdl, FLDMMDL_GetDirHeader(fmmdl) );
	FLDMMDL_FreeAcmd( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル 動作関数初期化
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitMoveProcWork( FLDMMDL * fmmdl )
{
	const FLDMMDL_MOVE_PROC_LIST *list;
	list = MoveProcList_GetList( FLDMMDL_GetMoveCode(fmmdl) );
	fmmdl->move_init_proc = list->init_proc;
	fmmdl->move_proc = list->move_proc;
	fmmdl->move_delete_proc = list->delete_proc;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作初期化に行う処理纏め
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitMoveWork( FLDMMDL * fmmdl )
{
	FldMMdl_InitMoveProcWork( fmmdl );
	FLDMMDL_InitMoveProc( fmmdl );
}

//======================================================================
//	FLDMMDL 動作関数
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL TCB 動作関数
 * @param	tcb		GFL_TCB *
 * @param	work	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_TCB_MoveProc( GFL_TCB * tcb, void *work )
{
	FLDMMDL *fmmdl = (FLDMMDL *)work;
	FLDMMDL_UpdateMove( fmmdl );
	
	if( FLDMMDL_CheckStatusBitUse(fmmdl) == FALSE ){
		return;
	}
	
	FldMMdl_TCB_DrawProc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL TCB 動作関数から呼ばれる描画関数
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_TCB_DrawProc( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys(fmmdl);
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		FLDMMDLSYS_UpdateDraw( fmmdl );
	}
}

//======================================================================
//	FLDMMDLSYS パラメタ設定、参照
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS ステータスビットチェック
 * @param	fmmdlsys	FLDMMDLSYS*
 * @param	bit	FLDMMDLSYS_STABIT
 * @retval	u32	(status bit & bit)
 */
//--------------------------------------------------------------
u32 FLDMMDLSYS_CheckStatusBit(
	const FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit )
{
	return( (fmmdlsys->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ステータスビット ON
 * @param	fmmdlsys	FLDMMDLSYS*
 * @param	bit	FLDMMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_OnStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit )
{
	fmmdlsys->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ステータスビット OFF
 * @param	fmmdlsys	FLDMMDLSYS*
 * @param	bit	FLDMMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_OffStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit )
{
	fmmdlsys->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FMMDLSYS 動作モデル最大数を取得
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	u16	最大数
 */
//--------------------------------------------------------------
u16 FLDMMDLSYS_GetFldMMdlMax( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->fmmdl_max );
}

//--------------------------------------------------------------
/**
 * FMMDLSYS 現在発生している動作モデルの数を取得
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	u16	発生数
 */
//--------------------------------------------------------------
u16 FLDMMDLSYS_GetFldMMdlCount( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->fmmdl_count );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS TCBプライオリティを取得
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	u16	TCBプライオリティ
 */
//--------------------------------------------------------------
u16 FLDMMDLSYS_GetTCBPriority( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->tcb_pri );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS HEAPID取得
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	HEAPID HEAPID
 */
//--------------------------------------------------------------
HEAPID FLDMMDLSYS_GetHeapID( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->heapID );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 現在発生しているOBJ数を1増加
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_IncrementOBJCount( FLDMMDLSYS *fmmdlsys )
{
	fmmdlsys->fmmdl_count++;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 現在発生しているOBJ数を1減少
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_DecrementOBJCount( FLDMMDLSYS *fmmdlsys )
{
	fmmdlsys->fmmdl_count--;
	GF_ASSERT( fmmdlsys->fmmdl_count >= 0 );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS アーカイブハンドルセット
 * @param	fmmdlsys	FLDMMDLSYS *
 * @param	handle	ARCHANDLE *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetArcHandle( FLDMMDLSYS *fmmdlsys, ARCHANDLE *handle )
{
	fmmdlsys->pArcHandle = handle;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS アーカイブハンドル取得
 * @param	fmmdlsys FLDMMDLSYS *
 * @retval	ARCHANDLE*　ARCHANDLE *
 */
//--------------------------------------------------------------
ARCHANDLE * FLDMMDLSYS_GetArcHandle( FLDMMDLSYS *fmmdlsys )
{
	GF_ASSERT( fmmdlsys->pArcHandle != NULL );
	return( fmmdlsys->pArcHandle );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS GFL_TCBSYS*取得
 * @param	fos	FLDMMDLSYS*
 * @retval	GFL_TCBSYS*
 */
//--------------------------------------------------------------
GFL_TCBSYS * FLDMMDLSYS_GetTCBSYS( FLDMMDLSYS *fos )
{
	return( fos->pTCBSys );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS FLDMMDL_BLACTCONTセット
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetBlActCont(
	FLDMMDLSYS *fmmdlsys, FLDMMDL_BLACTCONT *pBlActCont )
{
	fmmdlsys->pBlActCont = pBlActCont;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS FLDMMDL_BLACTCONT取得
 * @param	fmmdlsys FLDMMDLSYS
 * @retval	FLDMMDL_BLACTCONT*
 */
//--------------------------------------------------------------
FLDMMDL_BLACTCONT * FLDMMDLSYS_GetBlActCont( FLDMMDLSYS *fmmdlsys )
{
	GF_ASSERT( fmmdlsys->pBlActCont != NULL );
	return( fmmdlsys->pBlActCont );
}

//======================================================================
//	FLDMMDL　パラメタ参照、設定
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL ステータスビットON
 * @param	fmmdl	fmmdl
 * @param	bit		FLDMMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBit( FLDMMDL *fmmdl, FLDMMDL_STABIT bit )
{
	fmmdl->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ステータスビットOFF
 * @param	fmmdl	fmmdl
 * @param	bit		FLDMMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBit( FLDMMDL *fmmdl, FLDMMDL_STABIT bit )
{
	fmmdl->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ステータスビット取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			ステータスビット
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetStatusBit( const FLDMMDL * fmmdl )
{
	return( fmmdl->status_bit );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ステータスビットチェック
 * @param	fmmdl	FLDMMDL*
 * @param	bit	FLDMMDL_STABIT
 * @retval	u32	(status bit & bit)
 */
//--------------------------------------------------------------
u32 FLDMMDL_CheckStatusBit( const FLDMMDL *fmmdl, FLDMMDL_STABIT bit )
{
	return( (fmmdl->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作ビット　取得
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		動作ビット
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetMoveBit( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_bit );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作ビット　ON
 * @param	fmmdl	FLDMMDL *
 * @param	bit		立てるビット FLDMMDL_MOVEBIT_NON等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnMoveBit( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作ビット　OFF
 * @param	fmmdl	FLDMMDL *
 * @param	bit		下ろすビット FLDMMDL_MOVEBIT_NON等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffMoveBit( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作ビット　チェック
 * @param	fmmdl	FLDMMDL *
 * @param	bit		チェックするビット FLDMMDL_MOVEBIT_NON等
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 FLDMMDL_CheckMoveBit( const FLDMMDL * fmmdl, u32 bit )
{
	return( (fmmdl->move_bit & bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ IDセット
 * @param	fmmdl	FLDMMDL * 
 * @param	id		obj id
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOBJID( FLDMMDL * fmmdl, u16 obj_id )
{
	fmmdl->obj_id = obj_id;
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ ID取得
 * @param	fmmdl	FLDMMDL *
 * @retval	u16		OBJ ID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetOBJID( const FLDMMDL * fmmdl )
{
	return( fmmdl->obj_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ZONE IDセット
 * @param	fmmdl	FLDMMDL *
 * @param	zone_id	ゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetZoneID( FLDMMDL * fmmdl, u16 zone_id )
{
	fmmdl->zone_id = zone_id;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ZONE ID取得
 * @param	fmmdl	FLDMMDL *
 * @retval	int		ZONE ID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetZoneID( const FLDMMDL * fmmdl )
{
	return( fmmdl->zone_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJコードセット
 * @param	fmmdl			FLDMMDL * 
 * @param	code			セットするコード
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOBJCode( FLDMMDL * fmmdl, u16 code )
{
	fmmdl->obj_code = code;
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJコード取得
 * @param	fmmdl			FLDMMDL * 
 * @retval	u16				OBJコード
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetOBJCode( const FLDMMDL * fmmdl )
{
	return( fmmdl->obj_code );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作コードセット
 * @param	fmmdl			FLDMMDL * 
 * @param	code			動作コード
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveCode( FLDMMDL * fmmdl, u16 code )
{
	fmmdl->move_code = code;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作コード取得
 * @param	fmmdl			FLDMMDL * 
 * @retval	u32				動作コード
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetMoveCode( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_code );
}

//--------------------------------------------------------------
/**
 * FLDMMDL イベントタイプセット
 * @param	fmmdl		FLDMMDL * 
 * @param	type		Event Type
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetEventType( FLDMMDL * fmmdl, u16 type )
{
	fmmdl->event_type = type;
}

//--------------------------------------------------------------
/**
 * FLDMMDL イベントタイプ取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			Event Type
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetEventType( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_type );
}

//--------------------------------------------------------------
/**
 * FLDMMDL イベントフラグセット
 * @param	fmmdl		FLDMMDL * 
 * @param	flag		Event Flag
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetEventFlag( FLDMMDL * fmmdl, u16 flag )
{
	fmmdl->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * FLDMMDL イベントフラグ取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	u16			Event Flag
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetEventFlag( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_flag );
}

//--------------------------------------------------------------
/**
 * FLDMMDL イベントIDセット
 * @param	fmmdl		FLDMMDL *
 * @param	id			Event ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetEventID( FLDMMDL * fmmdl, u16 id )
{
	fmmdl->event_id = id;
}

//--------------------------------------------------------------
/**
 * FLDMMDL イベントID取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	u16			Event ID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetEventID( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL イベントIDがエイリアスかどうかチェック
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			TRUE=エイリアスIDである FALSE=違う
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckAliesEventID( const FLDMMDL * fmmdl )
{
	u16 id = (u16)FLDMMDL_GetEventID( fmmdl );
	
	if( id == SP_SCRID_ALIES ){
		return( TRUE );
	}

	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ヘッダー指定方向取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			DIR_UP等
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetDirHeader( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_head );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 表示方向セット　強制
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetForceDirDisp( FLDMMDL * fmmdl, u16 dir )
{
	fmmdl->dir_disp_old = fmmdl->dir_disp;
	fmmdl->dir_disp = dir;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 表示方向セット
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDirDisp( FLDMMDL * fmmdl, u16 dir )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_PAUSE_DIR) == 0 ){
		fmmdl->dir_disp_old = fmmdl->dir_disp;
		fmmdl->dir_disp = dir;
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 表示方向取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	u16 	DIR_UP等
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirDisp( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_disp );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去表示方向取得
 * @param	fmmdl			FLDMMDL * 
 * @retval	dir				DIR_UP等
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirDispOld( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_disp_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動方向セット
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDirMove( FLDMMDL * fmmdl, u16 dir )
{
	fmmdl->dir_move_old = fmmdl->dir_move;
	fmmdl->dir_move = dir;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動方向取得
 * @param	fmmdl			FLDMMDL * 
 * @retval	u16		DIR_UP等
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirMove( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_move );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去移動方向取得
 * @param	fmmdl			FLDMMDL * 
 * @retval	u16	DIR_UP等
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirMoveOld( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_move_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 表示、移動方向セット
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDirAll( FLDMMDL * fmmdl, u16 dir )
{
	FLDMMDL_SetDirDisp( fmmdl, dir );
	FLDMMDL_SetDirMove( fmmdl, dir );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ヘッダー指定パラメタセット
 * @param	fmmdl	FLDMMDL * 
 * @param	param	パラメタ
 * @param	no		セットするパラメタ番号　FLDMMDL_PARAM_0等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetParam( FLDMMDL *fmmdl, int param, FLDMMDL_H_PARAM no )
{
	switch( no ){
	case FLDMMDL_PARAM_0: fmmdl->param0 = param; break;
	case FLDMMDL_PARAM_1: fmmdl->param1 = param; break;
	case FLDMMDL_PARAM_2: fmmdl->param2 = param; break;
	default: GF_ASSERT( 0 );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL ヘッダー指定パラメタ取得
 * @param	fmmdl			FLDMMDL *
 * @param	param			FLDMMDL_PARAM_0等
 * @retval	int				パラメタ
 */
//--------------------------------------------------------------
int FLDMMDL_GetParam( const FLDMMDL * fmmdl, FLDMMDL_H_PARAM param )
{
	switch( param ){
	case FLDMMDL_PARAM_0: return( fmmdl->param0 );
	case FLDMMDL_PARAM_1: return( fmmdl->param1 );
	case FLDMMDL_PARAM_2: return( fmmdl->param2 );
	}
	
	GF_ASSERT( 0 );
	return( 0 );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動制限Xセット
 * @param	fmmdl			FLDMMDL * 
 * @param	x				移動制限
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveLimitX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動制限X取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	s16			移動制限X
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetMoveLimitX( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_limit_x );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動制限Zセット
 * @param	fmmdl			FLDMMDL * 
 * @param	z				移動制限
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveLimitZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動制限Z取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	s16		移動制限z
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetMoveLimitZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_limit_z );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画ステータスセット
 * @param	fmmdl		FLDMMDL * 
 * @param	st			DRAW_STA_STOP等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDrawStatus( FLDMMDL * fmmdl, u16 st )
{
	fmmdl->draw_status = st;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画ステータス取得
 * @param	fmmdl		FLDMMDL * 
 * @retval	u16			DRAW_STA_STOP等
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDrawStatus( const FLDMMDL * fmmdl )
{
	return( fmmdl->draw_status );
}

//--------------------------------------------------------------
/**
 * FLDMMDL FLDMMDLSYS *取得
 * @param	fmmdl			FLDMMDL * 
 * @retval	FLDMMDLSYS	FLDMMDLSYS *
 */
//--------------------------------------------------------------
const FLDMMDLSYS * FLDMMDL_GetFldMMdlSys( const FLDMMDL *fmmdl )
{
	return( fmmdl->pFldMMdlSys );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作関数用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * 動作用ワークはsize分、0で初期化。
 * @param	fmmdl	FLDMMDL * 
 * @param	size	必要なワークサイズ
 * @retval	void*	初期化されたワーク
 */
//--------------------------------------------------------------
void * FLDMMDL_InitMoveProcWork( FLDMMDL * fmmdl, int size )
{
	void *work;
	GF_ASSERT( size <= FLDMMDL_MOVE_WORK_SIZE );
	work = FLDMMDL_GetMoveProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作関数用ワーク取得。
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	ワーク
 */
//--------------------------------------------------------------
void * FLDMMDL_GetMoveProcWork( FLDMMDL * fmmdl )
{
	return( fmmdl->move_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作補助関数用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * @param	fmmdl	FLDMMDL * 
 * @param	size	必要なワークサイズ
 * @retval	void*	初期化されたワーク
 */
//--------------------------------------------------------------
void * FLDMMDL_InitMoveSubProcWork( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_MOVE_SUB_WORK_SIZE );
	work = FLDMMDL_GetMoveSubProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作補助関数用ワーク取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	ワーク*
 */
//--------------------------------------------------------------
void * FLDMMDL_GetMoveSubProcWork( FLDMMDL * fmmdl )
{
	return( fmmdl->move_sub_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作コマンド用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * @param	fmmdl	FLDMMDL * 
 * @param	size	必要なワークサイズ
 * @retval	void*	初期化されたワーク
 */
//--------------------------------------------------------------
void * FLDMMDL_InitMoveCmdWork( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_MOVE_CMD_WORK_SIZE );
	work = FLDMMDL_GetMoveCmdWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作コマンド用ワーク取得
 * @param	fmmdl	FLDMMDL *
 * @retval	void*	ワーク*
 */
//--------------------------------------------------------------
void * FLDMMDL_GetMoveCmdWork( FLDMMDL * fmmdl )
{
	return( fmmdl->move_cmd_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画関数用ワーク初期化。
 * sizeがワークサイズを超えていた場合、ASSERT。
 * @param	fmmdl	FLDMMDL * 
 * @param	size	必要なワークサイズ
 * @retval	void*	初期化されたワーク
 */
//--------------------------------------------------------------
void * FLDMMDL_InitDrawProcWork( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_DRAW_WORK_SIZE );
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画関数用ワーク取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	ワーク
 */
//--------------------------------------------------------------
void * FLDMMDL_GetDrawProcWork( FLDMMDL * fmmdl )
{
	return( fmmdl->draw_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作初期化関数実行
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMoveInitProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_init_proc );
	fmmdl->move_init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作関数実行
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMoveProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc );
	fmmdl->move_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 削除関数実行
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMoveDeleteProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_delete_proc );
	fmmdl->move_delete_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 復帰関数実行
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMovePopProc( FLDMMDL * fmmdl )
{
	const FLDMMDL_MOVE_PROC_LIST *list =
		MoveProcList_GetList( FLDMMDL_GetMoveCode(fmmdl) );
	list->return_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画初期化関数実行
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawInitProc( FLDMMDL * fmmdl )
{
	fmmdl->draw_init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画関数実行
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawProc( FLDMMDL * fmmdl )
{
#ifndef FLDMMDL_PL_NULL
	fmmdl->draw_proc( fmmdl );
#endif
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画削除関数実行
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawDeleteProc( FLDMMDL * fmmdl )
{
	fmmdl->draw_delete_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画退避関数実行
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawPushProc( FLDMMDL * fmmdl )
{
	fmmdl->draw_push_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画復帰関数実行
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawPopProc( FLDMMDL * fmmdl )
{
	fmmdl->draw_pop_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL アニメーションコマンドコードセット
 * @param	fmmdl	FLDMMDL * 
 * @param	code	AC_DIR_U等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetAcmdCode( FLDMMDL * fmmdl, u16 code )
{
	fmmdl->acmd_code = code;
}

//--------------------------------------------------------------
/**
 * FLDMMDL アニメーションコマンドコード取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	u16	AC_DIR_U等
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetAcmdCode( const FLDMMDL * fmmdl )
{
	return( fmmdl->acmd_code );
}

//--------------------------------------------------------------
/**
 * FLDMMDL アニメーションコマンドシーケンスセット
 * @param	fmmdl	FLDMMDL * 
 * @param	no		シーケンス
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetAcmdSeq( FLDMMDL * fmmdl, u16 no )
{
	fmmdl->acmd_seq = no;
}

//--------------------------------------------------------------
/**
 * FLDMMDL アニメーションコマンドシーケンス増加
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_IncAcmdSeq( FLDMMDL * fmmdl )
{
	fmmdl->acmd_seq++;
}

//--------------------------------------------------------------
/**
 * FLDMMDL アニメーションコマンドシーケンス取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	u16 シーケンス
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetAcmdSeq( const FLDMMDL * fmmdl )
{
	return( fmmdl->acmd_seq );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在のマップアトリビュートをセット
 * @param	fmmdl	FLDMMDL *
 * @param	attr	セットするアトリビュート
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMapAttr( FLDMMDL * fmmdl, u32 attr )
{
	fmmdl->now_attr = attr;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在のマップアトリビュートを取得
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		マップアトリビュート
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetMapAttr( const FLDMMDL * fmmdl )
{
	return( fmmdl->now_attr );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去のマップアトリビュートをセット
 * @param	fmmdl	FLDMMDL *
 * @param	attr	セットするアトリビュート
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMapAttrOld( FLDMMDL * fmmdl, u32 attr )
{
	fmmdl->old_attr = attr;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去のマップアトリビュートを取得
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		マップアトリビュート
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetMapAttrOld( const FLDMMDL * fmmdl )
{
	return( fmmdl->old_attr );
}

//--------------------------------------------------------------
/**
 * FLDMMDL エイリアス時のゾーンID取得。
 * ※エイリアス時はイベントフラグが指定ゾーンID
 * @param	fmmdl	FLDMMDL *
 * @retval	u16 ゾーンID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetZoneIDAlies( const FLDMMDL * fmmdl )
{
	GF_ASSERT( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE );
	return( FLDMMDL_GetEventFlag(fmmdl) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 初期座標 グリッドX座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16 X座標
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetInitGridPosX( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_init );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 初期座標 グリッドX座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	x		セットする座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetInitGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_init = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 初期座標 Y座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetInitGridPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_init );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 初期座標 Y座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	y		セットする座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetInitGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_init = y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 初期座標 z座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		z座標
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetInitGridPosZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_init );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 初期座標 z座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	z		セットする座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetInitGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_init = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去座標　X座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		X座標
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetOldGridPosX( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去座標 X座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	x		セットする座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOldGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_old = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去座標 Y座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetOldGridPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去座標 Y座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	y		セットする座標
 * @retval	s16		y座標
 */
//--------------------------------------------------------------
void FLDMMDL_SetOldGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_old = y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去座標 z座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		z座標
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetOldGridPosZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去座標 z座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	z		セットする座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOldGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_old = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 X座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		X座標
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetGridPosX( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_now );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 X座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	x		セットする座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_now = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 X座標増加
 * @param	fmmdl	FLDMMDL * 
 * @param	x		足す値
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_now += x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 Y座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetGridPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_now );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 Y座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	y		セットする座標
 * @retval	s16		y座標
 */
//--------------------------------------------------------------
void FLDMMDL_SetGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_now = y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 Y座標増加
 * @param	fmmdl	FLDMMDL * 
 * @param	y		足す値
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_now += y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 過去座標 z座標取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		z座標
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetGridPosZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_now );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 z座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	z		セットする座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_now = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 現在座標 z座標増加
 * @param	fmmdl	FLDMMDL * 
 * @param	z		足す値
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_now += z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 実座標取得
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		座標格納先
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_pos_now;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 実座標セット
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		セット座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_pos_now = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 実座標Y値取得
 * @param	fmmdl	FLDMMDL * 
 * @retval	fx32	高さ
 */
//--------------------------------------------------------------
fx32 FLDMMDL_GetVectorPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->vec_pos_now.y );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 表示座標オフセット取得
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		セット座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorDrawOffsetPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 表示座標オフセットセット
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		セット座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorDrawOffsetPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 外部指定表示座標オフセット取得
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		セット座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorOuterDrawOffsetPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs_outside;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 外部指定表示座標オフセットセット
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		セット座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorOuterDrawOffsetPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs_outside = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL アトリビュート変化座標オフセット取得
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		セット座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorAttrDrawOffsetPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_attr_offs;
}

//--------------------------------------------------------------
/**
 * FLDMMDL アトリビュート変化座標オフセットセット
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		セット座標
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorAttrDrawOffsetPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_attr_offs = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL 高さ(グリッド単位)を取得
 * @param	fmmdl	FLDMMDL *
 * @retval	s16		高さ。H_GRID単位
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetHeightGrid( const FLDMMDL * fmmdl )
{
	fx32 y = FLDMMDL_GetVectorPosY( fmmdl );
	s16 gy = SIZE_GRID_FX32( y );
	return( gy );
}

//======================================================================
//	FLDMMDLSYS ステータス操作
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS 描画処理が初期化されているかどうかチェック
 * @param	fmmdlsys	FLDMMDLSYS *
 * @retval	BOOL	TRUE=初期化されている
 */
//--------------------------------------------------------------
BOOL FLDMMDLSYS_CheckCompleteDrawInit( const FLDMMDLSYS *fmmdlsys )
{
	if( FLDMMDLSYS_CheckStatusBit(
			fmmdlsys,FLDMMDLSYS_STABIT_DRAW_INIT_COMP) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 影を付ける、付けないのセット
 * @param	fmmdlsys FLDMMDLSYS *
 * @param	flag	TRUE=影を付ける FALSE=影を付けない
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetJoinShadow( FLDMMDLSYS *fos, BOOL flag )
{
	if( flag == FALSE ){
		FldMMdlSys_OnStatusBit( fos, FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}else{
		FldMMdlSys_OffStatusBit( fos, FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 影を付ける、付けないのチェック
 * @param	fmmdlsys FLDMMDLSYS *
 * @retval	BOOL TRUE=付ける
 */
//--------------------------------------------------------------
BOOL FLDMMDLSYS_CheckJoinShadow( const FLDMMDLSYS *fos )
{
	if( FLDMMDLSYS_CheckStatusBit(fos,FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT) ){
		return( FALSE );
	}
	return( TRUE );
}

//======================================================================
//	FLDMMDLSYS プロセス操作
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS フィールド動作モデル全体の動作を完全停止。
 * 動作処理、描画処理を完全停止する。アニメーションコマンドにも反応しない。
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_StopProc( FLDMMDLSYS *fmmdlsys )
{
	FldMMdlSys_OnStatusBit( fmmdlsys,
		FLDMMDLSYS_STABIT_MOVE_PROC_STOP|FLDMMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS FLDMMDLSYS_StopProc()の解除。
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_PlayProc( FLDMMDLSYS *fmmdlsys )
{
	FldMMdlSys_OffStatusBit( fmmdlsys,
		FLDMMDLSYS_STABIT_MOVE_PROC_STOP|FLDMMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS フィールド動作モデル全体の動作を一時停止
 * 固有の動作処理（ランダム移動等）を一時停止する。
 * アニメーションコマンドには反応する。
 * @param	fmmdlsys	FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_PauseMoveProc( FLDMMDLSYS *fmmdlsys )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		FLDMMDL_OnStatusBitMoveProcPause( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS フィールド動作モデル全体の一時停止を解除
 * @param	fmmdlsys	FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_ClearPauseMoveProc( FLDMMDLSYS *fmmdlsys )
{
	u32 no = 0;
	FLDMMDL *fmmdl;

	while( FLDMMDLSYS_SearchUseFldMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		FLDMMDL_OffStatusBitMoveProcPause( fmmdl );
	}
}

//======================================================================
//	FLDMMDL ステータスビット関連
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデルから
 * フィールド動作モデルシステムのビットチェック
 * @param	fmmdl		FLDMMDL*
 * @param	bit			FLDMMDLSYS_STABIT_DRAW_INIT_COMP等
 * @retval	u32			0以外 bitヒット
 */
//--------------------------------------------------------------
u32 FLDMMDL_CheckFldMMdlSysStatusBit(
	const FLDMMDL *fmmdl, FLDMMDLSYS_STABIT bit )
{
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	return( FLDMMDLSYS_CheckStatusBit(fos,bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 使用チェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL	TRUE=使用中
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitUse( const FLDMMDL *fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作中にする
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMove( FLDMMDL *fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作中を解除
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMove( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作中チェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=動作中
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMove( const FLDMMDL *fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作開始にする
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMoveStart( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作開始を解除
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMoveStart( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作開始チェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=移動動作開始
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMoveStart( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE_START) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作終了にする
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作終了を解除
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 移動動作終了チェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=移動終了
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMoveEnd( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE_END) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画初期化完了にする
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitCompletedDrawInit( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画初期化完了を解除
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitCompletedDrawInit( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画初期化完了チェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=描画初期化完了
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitCompletedDrawInit( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(
			fmmdl,FLDMMDL_STABIT_DRAW_PROC_INIT_COMP) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 非表示フラグをチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=非表示　FALSE=表示
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitVanish( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_VANISH) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 非表示フラグを設定
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=非表示　FALSE=表示
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitVanish( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_VANISH );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_VANISH );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ同士の当たり判定フラグを設定
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=ヒットアリ　FALSE=ヒットナシ
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitFellowHit( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_FELLOW_HIT_NON );
	}else{
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_FELLOW_HIT_NON );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作中フラグのセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=動作中　FALSE=停止中
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitMove( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBitMove( fmmdl );
	}else{
		FLDMMDL_OffStatusBitMove( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 話しかけ可能チェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=可能 FALSE=不可
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitTalk( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_TALK_OFF) ){
		return( FALSE );
	}
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 話しかけ不可フラグをセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=不可 FALSE=可能
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitTalkOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_TALK_OFF );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_TALK_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作ポーズ
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMoveProcPause( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作ポーズ解除
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMoveProcPause( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作ポーズチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=動作ポーズ
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMoveProcPause( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_PAUSE_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ステータスビット 描画処理初期化完了チェック
 * @param	fmmdl		FLDMMDL *
 * @retval	BOOL TRUE=完了。FALSE=まだ
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckCompletedDrawInit( const FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos;
	
	fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return( FALSE );
	}
	
	if( FLDMMDL_CheckStatusBitCompletedDrawInit(fmmdl) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 高さ取得を禁止する
 * @param	fmmdl	FLDMMDL *
 * @param	int		TRUE=高さ取得OFF FALSE=ON
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitHeightGetOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_OFF );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 高さ取得が禁止されているかチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=禁止
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitHeightGetOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ゾーン切り替え時の削除禁止
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=禁止 FALSE=禁止しない
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitNotZoneDelete( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ZONE_DEL_NOT );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ZONE_DEL_NOT );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL エイリアスフラグをセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=エイリアス FALSE=違う
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitAlies( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ALIES );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ALIES );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL エイリアスフラグをチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=エイリアス FALSE=違う
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitAlies( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ALIES) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 浅瀬エフェクトセットフラグをセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=セット　FALSE=クリア
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitShoalEffect( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_EFFSET_SHOAL );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_EFFSET_SHOAL );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 浅瀬エフェクトセットフラグをチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=セット　FALSE=違う
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitShoalEffect( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_EFFSET_SHOAL) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL アトリビュートオフセット設定OFFセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=セット　FALSE=クリア
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitAttrOffsetOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ATTR_OFFS_OFF );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ATTR_OFFS_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL アトリビュートオフセット設定OFFチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=OFF　FALSE=違う
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitAttrOffsetOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ATTR_OFFS_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 橋移動フラグセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=セット　FALSE=クリア
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitBridge( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_BRIDGE );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_BRIDGE );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 橋移動フラグチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=橋　FALSE=違う
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitBridge( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_BRIDGE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 映りこみフラグセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=セット　FALSE=クリア
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitReflect( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_REFLECT_SET );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_REFLECT_SET );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 映りこみフラグチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=セット　FALSE=無し
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitReflect( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_REFLECT_SET) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL アニメーションコマンドチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=コマンドアリ　FALSE=無し
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitAcmd( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 拡張高さ反応フラグをセット
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=セット　FALSE=クリア
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitHeightExpand( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_EXPAND );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_EXPAND );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL 拡張高さ反応フラグチェック
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=拡張高さに反応する　FALSE=無し
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitHeightExpand( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_HEIGHT_EXPAND) ){
		return( TRUE );
	}
	return( FALSE );
}

//======================================================================
//	FLDMMDL 動作ビット関連
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL アトリビュート取得を停止
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=停止
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveBitAttrGetOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnMoveBit( fmmdl, FLDMMDL_MOVEBIT_ATTR_GET_OFF );
	}else{
		FLDMMDL_OffMoveBit( fmmdl, FLDMMDL_MOVEBIT_ATTR_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL アトリビュート取得を停止　チェック
 * @param	fmmdl	FLDMMDL *
 * @retval	int	TRUE=停止
 */
//--------------------------------------------------------------
int FLDMMDL_CheckMoveBitAttrGetOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckMoveBit(fmmdl,FLDMMDL_MOVEBIT_ATTR_GET_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//======================================================================
//	FLDMMDLSYS ツール
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS 使用しているフィールド動作モデルを探す。
 * @param	fos	FLDMMDLSYS *
 * @param	fmmdl	FLDMMDL*格納先
 * @param	no	検索開始ワークno。先頭から検索する際は初期値0を指定。
 * @retval	BOOL TRUE=動作モデル取得した FALSE=noから終端まで検索し取得無し。
 * 引数noは呼び出し後、検索位置+1の値になる。
 *
 * ※例：OBJ ID 1番の動作モデルを探す。
 * u32 no=0;
 * FLDMMDL *fmmdl;
 * while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
 * 		if( FLDMMDL_GetOBJID(fmmdl) == 1 ){
 * 			break;
 * 		}
 * }
 */
//--------------------------------------------------------------
BOOL FLDMMDLSYS_SearchUseFldMMdl(
	const FLDMMDLSYS *fos, FLDMMDL **fmmdl, u32 *no )
{
	u32 max = FLDMMDLSYS_GetFldMMdlMax( fos );
	
	if( (*no) < max ){
		FLDMMDL *check_obj = &(((FLDMMDLSYS*)fos)->pFldMMdlBuf[*no]);
		
		do{
			(*no)++;
			if( FLDMMDL_CheckStatusBit(check_obj,FLDMMDL_STABIT_USE) ){
				*fmmdl = check_obj;
				return( TRUE );
			}
			check_obj++;
		}while( (*no) < max );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 指定されたグリッドX,Z座標にいるOBJを取得
 * @param	sys			FLDMMDLSYS *
 * @param	x			検索グリッドX
 * @param	z			検索グリッドZ
 * @param	old_hit		TURE=過去座標も判定する
 * @retval	FLDMMDL	x,z位置にいるFLDMMDL * 。NULL=その座標にOBJはいない
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_SearchGridPos(
	const FLDMMDLSYS *sys, s16 x, s16 z, BOOL old_hit )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(sys,&fmmdl,&no) == TRUE ){
		if( old_hit ){
			if( FLDMMDL_GetOldGridPosX(fmmdl) == x &&
				FLDMMDL_GetOldGridPosZ(fmmdl) == z ){
				return( fmmdl );
			}
		}
		
		if( FLDMMDL_GetGridPosX(fmmdl) == x &&
			FLDMMDL_GetGridPosZ(fmmdl) == z ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS 動作コードに一致するFLDMMDL *を検索
 * @param	fos		FLDMMDLSYS *
 * @param	mv_code		検索する動作コード
 * @retval	FLDMMDL *	NULL=存在しない
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_SearchMoveCode( const FLDMMDLSYS *fos, u16 mv_code )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
		if( FLDMMDL_GetMoveCode(fmmdl) == mv_code ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS OBJ IDに一致するFLDMMDL *を検索
 * @param	fos		FLDMMDLSYS *
 * @param	id		検索するOBJ ID
 * @retval	FLDMMDL *	NULL=存在しない
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_SearchOBJID( const FLDMMDLSYS *fos, u16 id )
{
	u32 no = 0;
	FLDMMDL *fmmdl;

	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
		if( FLDMMDL_CheckStatusBitAlies(fmmdl) == FALSE ){
			if( FLDMMDL_GetOBJID(fmmdl) == id ){
				return( fmmdl );
			}
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS フィールド動作モデルの空きを探す
 * @param	sys			FLDMMDLSYS *
 * @retval	FLDMMDL	空きのFLDMMDL*　空きが無い場合はNULL
 */
//--------------------------------------------------------------
static FLDMMDL * FldMMdlSys_SearchSpaceFldMMdl( const FLDMMDLSYS *sys )
{
	int i,max;
	FLDMMDL *fmmdl;
	
	i = 0;
	max = FLDMMDLSYS_GetFldMMdlMax( sys );
	fmmdl = ((FLDMMDLSYS*)sys)->pFldMMdlBuf;
	
	do{
		if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) == 0 ){
			return( fmmdl );
		}
		
		fmmdl++;
		i++;
	}while( i < max );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS フィールド動作モデル　エイリアスを探す
 * @param	fos			FLDMMDLSYS *
 * @param	obj_id		一致するOBJ ID
 * @param	zone_id		一致するZONE ID
 * @retval	FLDMMDL	一致するFLDMMDL*　一致無し=NULL
 */
//--------------------------------------------------------------
static FLDMMDL * FldMMdlSys_SearchAlies(
	const FLDMMDLSYS *fos, int obj_id, int zone_id )
{
	u32 no = 0;
	FLDMMDL * fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) ){
		if( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
			if( FLDMMDL_GetOBJID(fmmdl) == obj_id ){
				if( FLDMMDL_GetZoneIDAlies(fmmdl) == zone_id ){
					return( fmmdl );
				}
			}
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS フィールド動作モデル　TCB動作関数追加
 * @param	sys		FLDMMDLSYS*
 * @param	fmmdl	FLDMMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_AddFldMMdlTCB(
	const FLDMMDLSYS *sys, FLDMMDL * fmmdl )
{
	int pri,code;
	GFL_TCB * tcb;
	
	pri = FLDMMDLSYS_GetTCBPriority( sys );
	code = FLDMMDL_GetMoveCode( fmmdl );
	
	if( code == MV_PAIR || code == MV_TR_PAIR ){
		pri += FLDMMDL_TCBPRI_OFFS_AFTER;
	}
	
	tcb = GFL_TCB_AddTask( FLDMMDLSYS_GetTCBSYS((FLDMMDLSYS*)sys),
			FldMMdl_TCB_MoveProc, fmmdl, pri );
	GF_ASSERT( tcb != NULL );
	
	fmmdl->pTCB = tcb;
}

//======================================================================
//	FLDMMDL ツール
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL 現在発生しているフィールド動作モデルのOBJコードを参照
 * @param	fmmdl	FLDMMDL * 
 * @param	code	チェックするコード。HERO等
 * @retval	BOOL	TRUE=fmmdl以外にもcodeを持っている奴がいる
 */
//--------------------------------------------------------------
BOOL FLDMMDL_SearchUseOBJCode( const FLDMMDL *fmmdl, u16 code )
{
	u32 no = 0;
	u16 check_code;
	FLDMMDL *cmmdl;
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&cmmdl,&no) == TRUE ){
		if( cmmdl != fmmdl ){
			check_code = FLDMMDL_GetOBJCode( cmmdl );
			if( check_code != OBJCODEMAX && check_code == code ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 座標、方向を初期化。
 * @param	fmmdl	FLDMMDL *
 * @param	vec		初期化座標
 * @param	dir		方向 DIR_UP等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_InitPosition( FLDMMDL * fmmdl, const VecFx32 *vec, u16 dir )
{
	int grid = SIZE_GRID_FX32( vec->x );
	FLDMMDL_SetGridPosX( fmmdl, grid );
	
	grid = SIZE_GRID_FX32( vec->y );
	FLDMMDL_SetGridPosY( fmmdl, grid );
	
	grid = SIZE_GRID_FX32( vec->z );
	FLDMMDL_SetGridPosZ( fmmdl, grid );
	
	FLDMMDL_SetVectorPos( fmmdl, vec );
	FLDMMDL_UpdateGridPosCurrent( fmmdl );
	
	FLDMMDL_SetForceDirDisp( fmmdl, dir );
	
	FLDMMDL_FreeAcmd( fmmdl );
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE|FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 動作コード変更
 * @param	fmmdl	FLDMMDL *
 * @param	code	MV_RND等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_ChangeMoveCode( FLDMMDL *fmmdl, u16 code )
{
	FLDMMDL_CallMoveDeleteProc( fmmdl );
	FLDMMDL_SetMoveCode( fmmdl, code );
	FldMMdl_InitMoveProcWork( fmmdl );
	FLDMMDL_InitMoveProc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 描画初期化に行う処理纏め
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitDrawWork( FLDMMDL *fmmdl )
{
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return; //描画システム初期化完了していない
	}
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_ERROR) ){
		FLDMMDL_UpdateCurrentHeight( fmmdl );
	}
	
	FLDMMDL_SetDrawStatus( fmmdl, 0 );
	#ifndef FLDMMDL_PL_NULL
	FLDMMDL_BlActAddPracFlagSet( fmmdl, FALSE );
	#endif
	
	if( FLDMMDL_CheckStatusBitCompletedDrawInit(fmmdl) == FALSE ){
		FldMMdl_InitDrawProcWork( fmmdl );
		FLDMMDL_CallDrawInitProc( fmmdl );
		FLDMMDL_OnStatusBitCompletedDrawInit( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル 描画関数初期化
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitDrawProcWork( FLDMMDL * fmmdl )
{
	const FLDMMDL_DRAW_PROC_LIST *list;
	u32 code = FLDMMDL_GetOBJCode( fmmdl );
	
	if( code == NONDRAW ){
		list = &DATA_FieldOBJDraw_Non;
	}else{
		#ifndef FLDMMDL_PL_NULL
		list = DrawProcList_GetList( code );
		#else
		list = &DATA_FieldOBJDraw_Non;
		#endif
	}
	
	fmmdl->draw_init_proc = list->init_proc;
	fmmdl->draw_proc = list->draw_proc;
	fmmdl->draw_delete_proc = list->delete_proc;
	fmmdl->draw_push_proc = list->push_proc;
	fmmdl->draw_pop_proc = list->pop_proc;
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル関連エフェクトのフラグ初期化。
 * エフェクト関連のフラグ初期化をまとめる。
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitDrawEffectFlag( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl,
		FLDMMDL_STABIT_SHADOW_SET		|
		FLDMMDL_STABIT_SHADOW_VANISH	|
		FLDMMDL_STABIT_EFFSET_SHOAL		|
		FLDMMDL_STABIT_REFLECT_SET );
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ IDを変更
 * @param	fmmdl	FLDMMDL *
 * @param	id		OBJ ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_ChangeOBJID( FLDMMDL * fmmdl, u16 id )
{
	FLDMMDL_SetOBJID( fmmdl, id );
	FLDMMDL_OnStatusBitMoveStart( fmmdl );
	FldMMdl_InitDrawEffectFlag( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ワーク消去
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ClearWork( FLDMMDL *fmmdl )
{
	GFL_STD_MemClear( fmmdl, FLDMMDL_SIZE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL 指定されたフィールド動作モデルがエイリアス指定かどうかチェック
 * @param	fmmdl		FLDMMDL *
 * @param	h_zone_id	headを読み込むゾーンID
 * @param	max			head要素数
 * @param	head		FLDMMDL_H
 * @retval	int			RET_ALIES_NOT等
 */
//--------------------------------------------------------------
static int FldMMdl_CheckHeaderAlies(
		const FLDMMDL *fmmdl, int h_zone_id, int max,
		const FLDMMDL_HEADER *head )
{
	u16 obj_id;
	int zone_id;
	
	while( max ){
		obj_id = head->id;
		
		if( FLDMMDL_GetOBJID(fmmdl) == obj_id ){
			//エイリアスヘッダー
			if( FldMMdlHeader_CheckAlies(head) == TRUE ){
				//エイリアスの正規ゾーンID
				zone_id = FldMMdlHeader_GetAliesZoneID( head );
				//対象エイリアス
				if( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
					if( FLDMMDL_GetZoneIDAlies(fmmdl) == zone_id ){
						return( RET_ALIES_EXIST );	//Aliesとして既に存在
					}
				}else if( FLDMMDL_GetZoneID(fmmdl) == zone_id ){
					return( RET_ALIES_CHANGE );		//Alies対象である
				}
			}else{ //通常ヘッダー
				if( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
					//生存エイリアスと一致
					if( FLDMMDL_GetZoneIDAlies(fmmdl) == h_zone_id ){
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
 * FLDMMDL 指定されたゾーンIDとOBJ IDを持つFLDMMDL *を取得。
 * @param	fos		FLDMMDLSYS *
 * @param	obj_id	OBJ ID
 * @param	zone_id	ゾーンID
 * @retval	FLDMMDL * FLDMMDL *
 */
//--------------------------------------------------------------
static FLDMMDL * FldMMdl_SearchOBJIDZoneID(
		const FLDMMDLSYS *fos, int obj_id, int zone_id )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
		if( FLDMMDL_GetOBJID(fmmdl) == obj_id &&
			FLDMMDL_GetZoneID(fmmdl) == zone_id ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル描画初期化に行う処理
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitDrawStatus( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
	FldMMdl_InitDrawEffectFlag( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル描画削除時に行う処理
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SetDrawDeleteStatus( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル　エイリアスから正規OBJへの変更
 * @param	fmmdl		FLDMMDL * 
 * @param	head		対象のFLDMMDL_H
 * @param	zone_id		正規のゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ChangeAliesOBJ(
	FLDMMDL *fmmdl, const FLDMMDL_HEADER *head, int zone_id )
{
	GF_ASSERT( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE );
	FLDMMDL_SetStatusBitAlies( fmmdl, FALSE );
	FLDMMDL_SetZoneID( fmmdl, zone_id );
	FLDMMDL_SetEventID( fmmdl, head->event_id );
	FLDMMDL_SetEventFlag( fmmdl, head->event_flag );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデル　正規OBJからエイリアスへの変更
 * @param	fmmdl		FLDMMDL * 
 * @param	head		対象のFLDMMDL_H
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ChangeOBJAlies(
	FLDMMDL * fmmdl, int zone_id, const FLDMMDL_HEADER *head )
{
	GF_ASSERT( FldMMdlHeader_CheckAlies(head) == TRUE );
	FLDMMDL_SetStatusBitAlies( fmmdl, TRUE );
	FLDMMDL_SetEventID( fmmdl, head->event_id );
	FLDMMDL_SetEventFlag( fmmdl, FldMMdlHeader_GetAliesZoneID(head) );
	FLDMMDL_SetZoneID( fmmdl, zone_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL フィールド動作モデルの同一チェック。
 * 死亡、入れ替わりが発生しているかチェックする。
 * @param	fmmdl	FLDMMDL *
 * @param	obj_id	同一とみなすOBJ ID
 * @param	zone_id	同一とみなすZONE ID
 * @retval	int		TRUE=同一。FALSE=同一ではない
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckSameID( const FLDMMDL * fmmdl, u16 obj_id, int zone_id )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) == 0 ){
		return( FALSE );
	}
	
	if( FLDMMDL_GetOBJID(fmmdl) != obj_id ){
		return( FALSE );
	}
	
	if( FLDMMDL_GetZoneID(fmmdl) != zone_id ){
		if( FLDMMDL_CheckStatusBitAlies(fmmdl) == FALSE ){
			return( FALSE );
		}
		
		if( FLDMMDL_GetZoneIDAlies(fmmdl) != zone_id ){
			return( FALSE );
		}
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデルの同一チェック。OBJコード含み
 * 死亡、入れ替わりが発生しているかチェックする。
 * @param	fmmdl	FLDMMDL *
 * @param	code	同一とみなすOBJコード
 * @param	obj_id	同一とみなすOBJ ID
 * @param	zone_id	同一とみなすZONE ID
 * @retval	int		TRUE=同一。FALSE=同一ではない
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckSameIDCode(
		const FLDMMDL * fmmdl, u16 code, u16 obj_id, int zone_id )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) ){
		return( FALSE );
	}
	
	{
		u16 ret = FLDMMDL_GetOBJCode( fmmdl );
		if( ret != code ){
			return( FALSE );
		}
	}
	
	return( FLDMMDL_CheckSameID(fmmdl,obj_id,zone_id) );
}


//======================================================================
//	parts
//======================================================================
//--------------------------------------------------------------
/**
 * 指定されたOBJコードがワーク参照型ならばワーク内OBJコードに変更。
 * 違う場合はそのままで返す。
 * @param	fsys	FIELDSYS_WORK *
 * @param	code	OBJコード。HERO等
 * @retval	int		チェックされたOBJコード
 */
//--------------------------------------------------------------
static u16 WorkOBJCode_GetOBJCode( void *fsys, int code )
{
	if( code >= WKOBJCODE_ORG && code <= WKOBJCODE_END ){
		#ifndef FLDMMDL_PL_NULL
		code -= WKOBJCODE_ORG;
		code = GetEvDefineObjCode( fsys, code );
		#else
		GF_ASSERT( 0 );
		#endif
	}
	
	return( code );
}

//--------------------------------------------------------------
/**
 * 指定された動作コードの関数リストを取得
 * @param	code	動作コード
 * @retval FLDMMDL_MOVE_PROC_LIST
 */
//--------------------------------------------------------------
static const FLDMMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code )
{
	GF_ASSERT( code < MV_CODE_MAX );
	return( DATA_FieldOBJMoveProcListTbl[code] );
}

//--------------------------------------------------------------
/**
 * 指定されたOBJコードの描画関数リストを取得
 * @param	code	OBJコード
 * @retval	FLDMMDL_DRAW_PROC_LIST*
 */
//--------------------------------------------------------------
static const FLDMMDL_DRAW_PROC_LIST * DrawProcList_GetList( u16 code )
{
#ifndef FLDMMDL_PL_NULL
	const FIELD_OBJ_DRAW_PROC_LIST_REG *tbl;
	tbl = DATA_FieldOBJDrawProcListRegTbl;
	
	do{
		if( tbl->code == code ){
			return( tbl->list );
		}
		tbl++;
	}while( tbl->code != OBJCODEMAX );
	
	GF_ASSERT( 0 );
	return( NULL );
#else
	return( NULL );
#endif
}

//--------------------------------------------------------------
/**
 * FLDMMDL_HEADER エイリアスチェック
 * @param	head	FIELD_OBJ_H
 * @retval	int		TRUE=エイリアス　FALSE=違う
 */
//--------------------------------------------------------------
static BOOL FldMMdlHeader_CheckAlies( const FLDMMDL_HEADER *head )
{
	u16 id = head->event_id;
	if( id == SP_SCRID_ALIES ){ return( TRUE ); }
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * フィールドOBJヘッダー　エイリアス時のゾーンID取得。
 * エイリアス時はイベントフラグがゾーンIDになる
 * @param	head	FIELD_OBJ_H
 * @retval	int		ゾーンID
 */
//--------------------------------------------------------------
static int FldMMdlHeader_GetAliesZoneID( const FLDMMDL_HEADER *head )
{
	GF_ASSERT( FldMMdlHeader_CheckAlies(head) == TRUE );
	return( (int)head->event_flag );
}

//======================================================================
//	動作関数ダミー
//======================================================================
//--------------------------------------------------------------
/**
 * 動作初期化関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveInitProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * 動作関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * 動作削除関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDeleteProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * 動作復帰関数ダミー
 * @param	FLDMMDL *	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveReturnProcDummy( FLDMMDL * fmmdl )
{
}

//======================================================================
//	描画関数ダミー
//======================================================================
//--------------------------------------------------------------
/**
 * 描画初期化関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	int			TRUE=初期化成功
 */
//--------------------------------------------------------------
void FLDMMDL_DrawInitProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画削除関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawDeleteProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画退避関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPushProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * 描画復帰関数ダミー
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPopProcDummy( FLDMMDL * fmmdl )
{
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//	仮
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
FIELD_MAIN_WORK * FLDMMDLSYS_GetFieldMainWork( FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->pFldMainWork );
}

void FLDMMDL_SetBlActID( FLDMMDL *fmmdl, GFL_BBDACT_ACTUNIT_ID blActID )
{
	fmmdl->blActID = blActID;
}

GFL_BBDACT_ACTUNIT_ID FLDMMDL_GetBlActID( FLDMMDL *fmmdl )
{
	return( fmmdl->blActID );
}

const FLDMAPPER * FLDMMDLSYS_GetG3DMapper( const FLDMMDLSYS *fos )
{
	return( fos->pG3DMapper );
}

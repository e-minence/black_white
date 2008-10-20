typedef struct _FLD_ACTSYS		FLD_ACTSYS;

//------------------------------------------------------------------
/**
 * @brief	フィールドアクトシステム作成
 */
//------------------------------------------------------------------
extern FLD_ACTSYS*	CreateFieldActSys( GFL_G3D_CAMERA* g3Dcamera, HEAPID heapID );
//------------------------------------------------------------------
/**
 * @brief	フィールドアクトシステム破棄
 */
//------------------------------------------------------------------
extern void	DeleteFieldActSys( FLD_ACTSYS* fldActSys );
//------------------------------------------------------------------
/**
 * @brief	フィールドアクト動作関数
 */
//------------------------------------------------------------------
extern void	MainFieldActSys( FLD_ACTSYS* fldActSys );
//------------------------------------------------------------------
/**
 * @brief	フィールドアクト描画関数
 */
//------------------------------------------------------------------
extern void	DrawFieldActSys( FLD_ACTSYS* fldActSys );


//======================================================================
/**
 * @file	情報バー
 * @brief	画面上に常駐する情報表示画面
 * @author	ariizumi
 * @data	09/02/23
 */
//======================================================================

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

//初期化
//	@param bgplane	BG面
//	@param bgOfs	NCGの読み込むVRAMアドレス(0x1000必要)
//	@param heapId	ヒープID
void	INFOBAR_Init( u8 bgplane , u8 pltNo, HEAPID heapId );
void	INFOBAR_Update( void );
void	INFOBAR_Term( void );


//============================================================================================
/**
 * @file		zknsearch_bmp.h
 * @brief		�}�ӌ������ �a�l�o�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	���W���[�����FZKNSEARCHBMP
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�֘A������
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHBMP_Init( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�֘A�폜
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHBMP_Exit( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v�����g���C��
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHBMP_PrintUtilTrans( ZKNSEARCHMAIN_WORK * wk );


extern void ZKNSEARCHBMP_PutMainPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutRowPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutRowItem( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutNamePage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutNameItem( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutTypePage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutTypeItem( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutColorPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutColorItem( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutFormPage( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHBMP_PutListItem( ZKNSEARCHMAIN_WORK * wk, PRINT_UTIL * util, STRBUF * str );

extern void ZKNSEARCHBMP_PutFormListItem( ZKNSEARCHMAIN_WORK * wk, PRINT_UTIL * util );

extern void ZKNSEARCHBMP_SearchStart( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_SearchComp( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_SearchError( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHBMP_PutResetStart( ZKNSEARCHMAIN_WORK * wk );

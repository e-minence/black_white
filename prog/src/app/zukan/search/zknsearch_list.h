#pragma	once



extern void ZKNSEARCHLIST_MakeRowList( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHLIST_MakeNameList( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHLIST_MakeTypeList( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHLIST_MakeColorList( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHLIST_MakeFormList( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�폜
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHLIST_FreeList( ZKNSEARCHMAIN_WORK * wk );

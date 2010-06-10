//=============================================================================================
/**
 * @file  btl_tables.h
 * @brief �|�P����WB �Ă��Ƃ��ȃT�C�Y�̃e�[�u����VRAM_H�ɒu���ăA�N�Z�X����
 * @author  taya
 *
 * @date  2010.05.27  �쐬
 */
//=============================================================================================
#pragma once


/**
 *  �A���R�[�����O�Ώۂ̃��U������
 */
extern BOOL BTL_TABLES_IsMatchEncoreFail( WazaID waza );

/**
 *  ���̂܂ˎ��s�Ώۂ̃��U����
 */
extern BOOL BTL_TABLES_IsMatchMonomaneFail( WazaID waza );

/**
 *  �v���b�V���[�Ώۃ��U����
 */
extern BOOL BTL_TABLES_IsPressureEffectiveWaza( WazaID waza );


/**
 *  �����ǂ莸�s�Ώۃ��U����
 */
extern BOOL BTL_TABLES_IsSakidoriFailWaza( WazaID waza );

/**
 *  �݂��Âꎸ�s���U����
 */
extern BOOL BTL_TABLES_IsMitidureFailWaza( WazaID waza );


/**
 *  �g���[�X�ł��Ȃ��Ƃ���������
 */
extern BOOL BTL_TABLES_CheckTraceFailTokusei( u16 tokuseiID );

/**
 *  �u�Ȃ肫��v�ł��Ȃ��Ƃ���������
 */
extern BOOL BTL_TABLES_CheckNarikiriFailTokusei( u16 tokuseiID );

/**
 *  �u�Ȃ��܂Â���v�ł��Ȃ��Ƃ���������
 */
extern BOOL BTL_TABLES_CheckNakamaDukuriFailTokusei( u16 tokuseiID );

/**
 *  �u�X�L���X���b�v�v�ł��Ȃ��Ƃ���������
 */
extern BOOL BTL_TABLES_CheckSkillSwapFailTokusei( u16 tokuseiID );


/**
 *  ��΂ɏ���������Ă͂����Ȃ��Ƃ���������
 */
extern BOOL BTL_TABLES_IsNeverChangeTokusei( u16 tokuseiID );

/**
 *  �A���Z�E�X�̃v���[�g����
 */
extern BOOL BTL_TABLES_IsMatchAruseusPlate( u16 itemID );

/**
 *  �C���Z�N�^�̃J�Z�b�g����
 */
extern BOOL BTL_TABLES_IsMatchInsectaCasette( u16 itemID );


/**
 *  ������Ԃ肪����������Ώۂ̂Ƃ���������
 */
extern BOOL BTL_TABLES_IsMatchKatayaburiTarget( u16 tokuseiID );



/**
 *  BG�^�C�v���牮�O���ǂ�������i�_�[�N�{�[���p�j
 */
extern BOOL BTL_TABLES_IsOutdoorBGType( u16 bgType );


extern const WazaID* BTL_TABLES_GetYubiFuruOmmitTable( u32* elems );
extern BOOL BTL_TABLES_IsYubiFuruOmmit( WazaID waza );
extern BOOL BTL_TABLES_CheckItemCallNoEffect( u16 itemID );


/**
 *  �^�[���`�F�b�N�������A�N�Z�X
 */
extern WazaSick  BTL_TABLES_GetTurnCheckWazaSickByOrder( u32 idx );

/**
 *  �����^���n�[�u�Ή��̏�Ԉُ�`�F�b�N
 */
extern WazaSick  BTL_TABLES_GetMentalSickID( u32 idx );

/**
 *  �g�p����Ώۃ|�P�����̑I�����s���K�v�������A�C�e��������
 */
extern BOOL BTL_TABLES_IsNoTargetItem( u16 itemNo );


#ifdef PM_DEBUG
extern int GYubiFuruDebugNumber[ BTL_POS_MAX ];

extern void BTL_TABLES_YubifuruDebugReset( void );
extern void BTL_TABLES_YubifuruDebugSetEnd( void );
extern void BTL_TABLES_YubifuruDebugInc( u8 pos );
#endif

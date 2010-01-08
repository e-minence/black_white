//============================================================================================
/**
 * @file  pms_data.h
 * @bfief �ȈՕ��́i�ȈՉ�b�f�[�^�ƒ�^����g�ݍ��킹�����́j
 * @author  taya
 * @date  05.12.27
 */
//============================================================================================
#ifndef __PMS_DATA_H__
#define __PMS_DATA_H__

//------------------------------------------------------
/**
 *  �֘A�萔
 */
//------------------------------------------------------
#define  PMS_WORD_MAX     (2)       ///< �����Ɋ܂܂��P��̍ő吔
#define  PMS_WORD_NULL    (0xffff)  ///< �P��Ƃ��Ė����Ȓl
#define  PMS_DECO_WIDTH   ( 84 )  ///< �f�R�����̍ő啝�i�h�b�g�j
#define  PMS_NULL_WIDTH   ( 84 )//(96)  ///< NULL�����̕��i�h�b�g�j

//�S�J���f�o�b�O
#ifdef PM_DEBUG
#define  PMS_ALLOPEN (1) 
#endif


//------------------------------------------------------
/**
 *  ���̓^�C�v
 */
//------------------------------------------------------
enum  PMS_TYPE {
  PMS_TYPE_BATTLE_READY,  ///< �ΐ�J�n�O
  PMS_TYPE_BATTLE_WON,  ///< �ΐ폟�����Ƃ�
  PMS_TYPE_BATTLE_LOST, ///< �ΐ핉������
  PMS_TYPE_MAIL,      ///< ���[����������
  PMS_TYPE_UNION,     ///< ���j�I����������


  PMS_TYPE_MAX,
};

//------------------------------------------------------
/**
 *  �f�R�������
 */
//------------------------------------------------------
typedef enum {
  PMS_DECOID_NULL,
  PMS_DECOID_ORG,

  PMS_DECOID_HERO = PMS_DECOID_ORG, ///< �͂��߂܂���
  PMS_DECOID_HERO2, ///< ����ɂ���
  PMS_DECOID_LOVE,  ///< ��������
  PMS_DECOID_FUN,   ///< ���̂���
  PMS_DECOID_YELL,  ///< ����΂�I 
  PMS_DECOID_HAPPY, ///< ���ꂵ��
  PMS_DECOID_TANKS, ///< ���肪�Ƃ�
  PMS_DECOID_HIGHEST, ///< ��������
  PMS_DECOID_SORRY,   ///< ���߂��
  PMS_DECOID_BYEBYE,  ///< �o�C�o�C

  PMS_DECOID_MAX,
}PMS_DECO_ID;

//------------------------------------------------------
/**
 *  �P��^��`
 */
//------------------------------------------------------
typedef u16   PMS_WORD; 
// ��ʂ��� 
//   4 bit -> ���[�J���C�Y�p
//   1 bit -> �P��^�f�R���t���O (1�Ȃ�f�R��)
//  11 bit -> �P��^�f�R���i���o�[

#define PMS_WORD_DECO_MASK ( 0x1 )    ///< �f�R������p�r�b�g�}�X�N
#define PMS_WORD_DECO_BITSHIFT ( 11 )  ///< �f�R������p�r�b�g �I�t�Z�b�g
#define PMS_WORD_NUM_MASK		( 0x7FF )    ///< �P��E�f�R���i���o�[ �r�b�g�}�X�N

#define PMS_WORD_LOCALIZE_MASK			( 0x4 )	///<	LOCALIZE�p�r�b�g�}�X�N
#define PMS_WORD_LOCALIZE_BITSHIFT	( 12 )	///<	LOCALIZE�p�r�b�g�V�t�g

//------------------------------------------------------
/**
 *  ���͌^��`
 *
 *  �\���̓��e�����J����Ă��܂����A
 *  ����Ƀ����o���Q�Ƃ����菑���������肵�Ă͂����܂���B
 *  �K�v�ȏ����̓C���^�[�t�F�C�X�֐���ʂ��Ă̂ݍs���Ă��������B
 *
 *�i���J����Ă���̂́A���̍\���̂��܂ލ\���́i���[�����j��
 * �Z�[�u�f�[�^�p�ɒ�`���Ȃ���΂Ȃ�Ȃ����߂ł��j
 *
 */
//------------------------------------------------------
typedef struct {
  u16       sentence_type;      ///< ���̓^�C�v
  u16       sentence_id;      ///< �^�C�v��ID
  PMS_WORD  word[PMS_WORD_MAX];   ///< �P��ID
}PMS_DATA;






//------------------------------------------------------------------
/**
 * �\���̂��󕶏͂Ƃ��ăN���A����
 *
 * @param   pms
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_Clear( PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * ��ԏ�����
 *
 * @param   pms       ���͌^�̈�ւ̃|�C���^
 * @param   sentence_type ���̓^�C�v
 *
 * �������ł̕��̓^�C�v�́A���͉�ʑJ�ڌ�̃f�t�H���g�l�Ƃ��ĎQ�Ƃ����B
 *   ���͉�ʂł̕ҏW��A�قȂ镶�̓^�C�v�ɂȂ��Ă��邱�Ƃ͂��蓾��B
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_Init( PMS_DATA* pms, u32 sentence_type );

//------------------------------------------------------------------
/**
 * ���j�I�����[���f�t�H���g�������Ƃ��ď���������
 *
 * @param   pms   ���͌^�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void PMSDAT_SetupDefaultUnionMessage( PMS_DATA* pms );


//------------------------------------------------------------------
/**
 * �o�g���^���[�f�t�H���g�������Ƃ��ď���������
 *
 * @param   pms     ���͌^�ւ̃|�C���^
 * @param   msgType   �������^�C�v
 *
 */
//------------------------------------------------------------------
//extern void PMSDAT_SetupDefaultBattleTowerMessage( PMS_DATA* pms, BTWR_PLAYER_MSG_ID msgType );



//------------------------------------------------------------------
/**
 * ���͌^���當����𐶐��B���̕�����͂��̂܂�STR_Print�ɉ񂵂ĕ\���ł���
 *
 * @param   pms     ���͌^�ւ̃|�C���^
 * @param   heapID    �o�b�t�@�����p�q�[�vID
 *
 * @retval  STRBUF*   �������ꂽ��������܂ރo�b�t�@
 */
//------------------------------------------------------------------
extern STRBUF* PMSDAT_ToString( const PMS_DATA* pms, u32 heapID );

//-----------------------------------------------------------------------------
/**
 * �ȈՕ��̓f�[�^����A���̂܂ܕ\���ł��镶����𐶐�����(�^�O���w���)
 *
 * @param   pms     ���͌^�ւ̃|�C���^
 * @param   heapID    �o�b�t�@�����p�q�[�vID
 * @param   wordCount �^�O��
 *
 * @retval  STRBUF*   �������ꂽ��������܂ރo�b�t�@
 */
//-----------------------------------------------------------------------------
extern STRBUF* PMSDAT_ToStringWithWordCount( const PMS_DATA* pms, u32 heapID, u8 wordCount );

//------------------------------------------------------------------
/**
 * ���͌^����A�^�O�W�J�O�̕�������擾�B���̂܂ܕ\���͕s�B
 * ������g���̂́A�����炭�ȈՉ�b���͉�ʂ̂݁B
 *
 * @param   pms
 * @param   heapID
 *
 * @retval  STRBUF*
 */
//------------------------------------------------------------------
extern STRBUF*  PMSDAT_GetSourceString( const PMS_DATA* pms, u32 heapID );

//------------------------------------------------------------------
/**
 * �󕶏͂ɂȂ��Ă��Ȃ����`�F�b�N
 *
 * @param   pms   ���͌^�ւ̃|�C���^
 *
 * @retval  BOOL  TRUE�ŗL��
 */
//------------------------------------------------------------------
extern BOOL PMSDAT_IsEnabled( const PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * ���͂��������Ă��邩�`�F�b�N
 *
 * @param   pms
 *
 * @retval  BOOL
 */
//------------------------------------------------------------------
extern BOOL PMSDAT_IsComplete( const PMS_DATA* pms , const HEAPID heapID );


//------------------------------------------------------------------
/**
 * �ݒ肳��Ă��镶�̓^�C�v���擾
 *
 * @param   pms   ���͌^�ւ̃|�C���^
 *
 * @retval  u32   ���̓^�C�v�ienum PMS_TYPE�j
 */
//------------------------------------------------------------------
extern u32  PMSDAT_GetSentenceType( const PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * �ݒ肳��Ă��镶�̓C���f�b�N�X��Ԃ�
 *
 * @param   pms   ���͌^�ւ̃|�C���^
 *
 * @retval  u32   ���̓C���f�b�N�X�i���̂܂܂ł͎g�������Ȃ��n�Y�B���͉�ʗp�B�j
 */
//------------------------------------------------------------------
extern u32 PMSDAT_GetSentenceID( const PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * �ݒ肳��Ă���P��i���o�[���擾
 *
 * @param   pms   ���͌^�ւ̃|�C���^
 * @param   pos   ���Ԗڂ̒P�ꂩ�H
 *
 * @retval  PMS_WORD  �ȈՉ�b�P��i���o�[
 */
//------------------------------------------------------------------
extern PMS_WORD  PMSDAT_GetWordNumber( const PMS_DATA* pms, int pos );

//------------------------------------------------------------------
/**
 * �ݒ肳��Ă���P��i���o�[�𐶂Ŏ擾 
 *
 * @param   pms   ���͌^�ւ̃|�C���^
 * @param   pos   ���Ԗڂ̒P�ꂩ�H
 *
 * @retval  PMS_WORD  �ȈՉ�b�P��i���o�[
 */
//------------------------------------------------------------------
extern PMS_WORD PMSDAT_GetWordNumberDirect( const PMS_DATA* pms, int pos );

//-----------------------------------------------------------------------------
/**
 *	@brief  �f�R������
 *
 *	@param	pms
 *	@param	pos 
 *
 *	@retval TRUE:�f�R�� FALSE:�f�R���łȂ��i�ʏ�̒P��j
 */
//-----------------------------------------------------------------------------
extern BOOL PMSDAT_IsDecoID( const PMS_DATA* pms, int pos );

//-----------------------------------------------------------------------------
/**
 *	@brief  �P��̃f�R������
 *
 *	@param	const PMS_WORD* word 
 *
 *	@retval TRUE:�f�R��
 */
//-----------------------------------------------------------------------------
extern BOOL PMSW_IsDeco( const PMS_WORD word );

//-----------------------------------------------------------------------------
/**
 *	@brief  PMS_WORD ���� �f�R��ID���擾
 *
 *	@param	const PMS_WORD* word ���[�hID
 *
 *	@retval PMS_DECO_ID �f�R��ID
 */
//-----------------------------------------------------------------------------
extern PMS_DECO_ID PMSW_GetDecoID( const PMS_WORD word );

//------------------------------------------------------------------
/**
 * ���e��r
 *
 * @param   pms1
 * @param   pms2
 *
 * @retval  BOOL    ��v���Ă�����TRUE��Ԃ�
 */
//------------------------------------------------------------------
extern BOOL PMSDAT_Compare( const PMS_DATA* pms1, const PMS_DATA* pms2 );

//------------------------------------------------------------------
/**
 * ���̓f�[�^�̃R�s�[
 *
 * @param   src   �R�s�[���|�C���^
 * @param   dst   �R�s�[��|�C���^
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_Copy( PMS_DATA* dst, const PMS_DATA* src  );


//------------------------------------------------------------------
/**
 * ���̓^�C�v����A���̃^�C�v�Ɋ܂܂�镶�̓p�^�[���̍ő吔��Ԃ�
 *
 * @param   sentence_type
 *
 * @retval  u32
 */
//------------------------------------------------------------------
extern u32  PMSDAT_GetSentenceIdMax( u32 sentence_type );

//------------------------------------------------------------------
/**
 * ���̓^�C�v�A�p�^�[�����Z�b�g
 *
 * @param   pms
 * @param   sentence_type
 * @param   sentence_id
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_SetSentence( PMS_DATA* pms, u32 sentence_type, u32 sentence_id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �P��i���o�[�Z�b�g
 *
 *	@param	pms       ���͌^�ւ̃|�C���^
 *	@param	pos       ���͓��ɃZ�b�g����ʒu
 *	@param	word      �P��i���o�[
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
extern void PMSDAT_SetWord( PMS_DATA* pms, u32 pos, PMS_WORD word );

//-----------------------------------------------------------------------------
/**
 *	@brief  �f�R���i���o�[�Z�b�g
 *
 *	@param	pms       ���͌^�ւ̃|�C���^
 *	@param	pos       ���͓��ɃZ�b�g����ʒu
 *	@param	deco_id   �f�R���i���o�[
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
extern void PMSDAT_SetDeco( PMS_DATA* pms, u32 pos, PMS_DECO_ID deco_id );

//------------------------------------------------------------------
/**
 * ���̓^�C�v��ID����A�g��Ȃ��P��̈���N���A���Ă���
 *
 * @param   pms
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_ClearUnnecessaryWord( PMS_DATA* pms , const HEAPID heapID );

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^���e���s�������`�F�b�N
 *
 *	@param	const PMS_DATA *pms   ���͌^�ւ̃|�C���^
 *	@param  HEAPID                �e���|�����o�b�t�@�쐬�p�q�[�vID
 *
 *	@return TRUE�Ȃ�ΐ���  FALSE�Ȃ�Εs��
 */
//-----------------------------------------------------------------------------
extern BOOL PMSDAT_IsValid( const PMS_DATA *pms, HEAPID heapID );

#ifdef PM_DEBUG
//------------------------------------------------------------------
/**
 * �e�X�g�p�ɓK���ȓ��e���Z�b�g����
 *
 * @param   pms
 */
//------------------------------------------------------------------
extern void PMSDAT_SetDebug( PMS_DATA* pms );   // �Œ�
extern void PMSDAT_SetDebugRandomDeco( PMS_DATA* pms, HEAPID heapID ); //�����_���f�R��
extern void PMSDAT_SetDebugRandom( PMS_DATA* pms ); // �����_��

#endif

#endif

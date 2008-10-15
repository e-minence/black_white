//============================================================================================
/**
 * @file	skb.h
 * @brief	�\�t�g�E�G�A�L�[�{�[�h
 * @author	
 * @date	
 */
//============================================================================================
typedef struct _GFL_SKB GFL_SKB;

#define GFL_SKB_STRLEN_MAX	(24)	//���͕�����ő吔
#define GFL_SKB_STRBUF_SIZ	(sizeof(u16)*(GFL_SKB_STRLEN_MAX+1))//���͕�����o�b�t�@�m�ۃT�C�Y

typedef enum {
	GFL_SKB_BGID_M0 = 0,
	GFL_SKB_BGID_M1,
	GFL_SKB_BGID_M2,
	GFL_SKB_BGID_M3,
	GFL_SKB_BGID_S0,
	GFL_SKB_BGID_S1,
	GFL_SKB_BGID_S2,
	GFL_SKB_BGID_S3,
}GFL_SKB_BGID;

typedef enum {
	GFL_SKB_PALID_0 = 0,
	GFL_SKB_PALID_1,
	GFL_SKB_PALID_2,
	GFL_SKB_PALID_3,
	GFL_SKB_PALID_4,
	GFL_SKB_PALID_5,
	GFL_SKB_PALID_6,
	GFL_SKB_PALID_7,
	GFL_SKB_PALID_8,
	GFL_SKB_PALID_9,
	GFL_SKB_PALID_10,
	GFL_SKB_PALID_11,
	GFL_SKB_PALID_12,
	GFL_SKB_PALID_13,
	GFL_SKB_PALID_14,
	GFL_SKB_PALID_15,
	GFL_SKB_PALID_NONE = 255,
}GFL_SKB_PALID;

typedef enum {
	GFL_SKB_MODE_HIRAGANA = 0,
	GFL_SKB_MODE_KATAKANA,
	GFL_SKB_MODE_ENGNUM,
}GFL_SKB_MODE;

typedef enum {
	GFL_SKB_STRTYPE_STRBUF = 0,
	GFL_SKB_STRTYPE_SJIS,//�z��̑傫���͒���*sizeof(u16)�ɂ��邱��
}GFL_SKB_STRTYPE;

//�@�ݒ��`
//�@������o�b�t�@�͕����񒷂�*sizeof(u16)���m�ۂ��邱��
typedef struct {
	u32				strlen;			//������i�[����
	GFL_SKB_STRTYPE	strtype;		//������i�[�^�C�v

	GFL_SKB_MODE	mode;			//�������[�h
	BOOL			modeChange;		//���[�h�ύX�ۃt���O
	u32				cancelKey;		//���f�L�[����g���K

	GFL_SKB_BGID	bgID;			//�g�pBGVRAMID
	GFL_SKB_PALID	bgPalID;		//�g�pBG�p���b�gID
	GFL_SKB_PALID	bgPalID_on;		//�g�pBG�p���b�gID

}GFL_SKB_SETUP;

//============================================================================================
/**
 *
 * @brief	�V�X�e���N��&�I��
 *	
 */
//============================================================================================
extern GFL_SKB*		GFL_SKB_Create( void* strings, const GFL_SKB_SETUP* setup, HEAPID heapID );
extern void			GFL_SKB_Delete( GFL_SKB* gflSkb );
extern BOOL			GFL_SKB_Main( GFL_SKB* gflSkb );	//FALSE�ŏI��

//SjisCode�i�[�p������o�b�t�@�쐬
//GFL_SKB_STRTYPE_SJIS���g�p����ꍇ�A���̊֐��œ��͍ő�z����m�ۏo����
extern void*	GFL_SKB_CreateSjisCodeBuffer( HEAPID heapID );
extern void		GFL_SKB_DeleteSjisCodeBuffer( void* strbuf );


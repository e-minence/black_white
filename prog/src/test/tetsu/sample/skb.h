//============================================================================================
/**
 * @file	skb.h
 * @brief	�\�t�g�E�G�A�L�[�{�[�h
 * @author	
 * @date	
 */
//============================================================================================
typedef struct _GFL_SKB GFL_SKB;

//�O���t�B�b�N�A�[�J�C�u���e�h�c��`
enum {
	NARC_skb_skb_NCGR = 0,
	NARC_skb_skb_NCLR = 1,
	NARC_skb_skb_1_NSCR = 2,
	NARC_skb_skb_2_NSCR = 3,
	NARC_skb_skb_3_NSCR = 4
};

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

typedef struct {
	void*			strings;		//������i�[�|�C���^
	u32				strlen;			//������i�[����

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
extern GFL_SKB*		GFL_SKB_Boot( HEAPID heapID, const GFL_SKB_SETUP* setup );
extern void			GFL_SKB_Exit( GFL_SKB* gflSkb );	//�����I���������ꍇ�g�p
extern BOOL			GFL_SKB_Main( GFL_SKB* gflSkb );	//FALSE�ŏI��


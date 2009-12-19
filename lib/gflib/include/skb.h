//============================================================================================
/**
 * @file  skb.h
 * @brief �\�t�g�E�G�A�L�[�{�[�h
 * @author
 * @date
 */
//============================================================================================
#pragma once
//
//  �g�p�̍ۂɂ�graphic�t�H���_����skb.narc�����[�g�ɔz�u���Ă�������
//  �i"skb.narc"�p�X�w��Ŏ擾�ł���ꏊ�j
//  �܂��A�V�X�e���t�H���g������ GFL_TEXT_CreateSystem() �����O�ɍs���Ă����Ă��������B
//
//  �g�pBG�ʂ̃R���g���[�����W�X�^�͌Ăяo�����Őݒ肳�ꂽ��ԂɈˑ����܂��B
//
typedef struct _GFL_SKB GFL_SKB;

#define GFL_SKB_STRLEN_MAX  (24)  //���͕�����ő吔
#define GFL_SKB_STRBUF_SIZ  (sizeof(u16)*(GFL_SKB_STRLEN_MAX+1))//���͕�����o�b�t�@�m�ۃT�C�Y

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
  u32       strlen;       //������i�[����
  GFL_SKB_STRTYPE strtype;//������i�[�^�C�v

  GFL_SKB_MODE  mode;     //�������[�h
  BOOL      modeChange;   //���[�h�ύX�ۃt���O
  u16       cancelKey;    //���f�L�[����g���K�i�s�v�Ȃ�0�ł悢�B���̏ꍇ�͒��f�s��)
  u16       decideKey;    //�m��L�[����g���K�i�s�v�Ȃ�0�ł悢�B���̏ꍇ�̓^�b�`�p�l������ł̂݊m��j

  GFL_DISPUT_BGID   bgID;        //�g�pBGVRAMID
  GFL_DISPUT_PALID  bgPalID;     //�g�pBG�p���b�gID
  GFL_DISPUT_PALID  bgPalID_on;  //�g�pBG�p���b�gID

}GFL_SKB_SETUP;

typedef enum {

  GFL_SKB_REACTION_QUIT = 0,  ///< �I������
  GFL_SKB_REACTION_NONE,      ///< ��������
  GFL_SKB_REACTION_INPUT,     ///< �������������͂��ꂽ
  GFL_SKB_REACTION_BACKSPACE, ///< BackSpace���͂��ꂽ
  GFL_SKB_REACTION_PAGE,      ///< �y�[�W�؂�ւ����͂��ꂽ

}GflSkbReaction;
//============================================================================================
/**
 *
 * @brief �V�X�e���N��&�I��
 *
 */
//============================================================================================
extern GFL_SKB*   GFL_SKB_Create( void* strings, const GFL_SKB_SETUP* setup, HEAPID heapID );
extern void     GFL_SKB_Delete( GFL_SKB* gflSkb );
extern GflSkbReaction     GFL_SKB_Main( GFL_SKB* gflSkb );  //FALSE�ŏI��

extern void GFL_SKB_PickStr( GFL_SKB* skb );
extern void GFL_SKB_ReloadStr( GFL_SKB* skb, const STRBUF* strbuf );

//SjisCode�i�[�p������o�b�t�@�쐬
//GFL_SKB_STRTYPE_SJIS���g�p����ꍇ�A���̊֐��œ��͍ő�z����m�ۏo����
extern void*  GFL_SKB_CreateSjisCodeBuffer( HEAPID heapID );
extern void   GFL_SKB_DeleteSjisCodeBuffer( void* strbuf );


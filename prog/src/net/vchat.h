#ifndef __VHCAT_H_
#define __VHCAT_H_
// �{�C�X�`���b�g�֌W�B
// dwc_rap.c ��vchat.c����̂݌Ăяo�����B
// �{�C�X�`���b�g���Ăԏꍇ�́Adwc_rap.h�o�R�ŌĂԁB

extern void myvct_init( int heapID, int codec,int maxEntry );
extern void myvct_setCodec( int codec );

//==============================================================================
/**
 * @brief   �{�C�X�`���b�g���C��
 * @param   offflg   ���𑗂�Ȃ��ꍇ���ꎞ�I�ɃI�t�ɂ���ꍇ TRUE
 * @retval  none
 */
//==============================================================================
extern void myvct_main( BOOL offflg);
extern BOOL myvct_checkData( int aid, void *data, int size );

//==============================================================================
/**
 * ��b�I�����������R�[���o�b�N��ݒ肵�܂��B
 * ���肩��؂�ꂽ�ꍇ���Ăяo����܂��B
 * ���̊֐����Ăяo����钼�O�ɁAvchat.c�p�̃��[�N���������܂��B
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void myvct_setDisconnectCallback( void (*disconnectCallback)() );

//==============================================================================
/**
 * ��b�I���v���������܂��B�܂��ʘb�ł��Ă��Ȃ��Ƃ��͑����ɏI�����܂��B
 * myvct_setDisconnectCallback�Őݒ肳�ꂽ�R�[���o�b�N���Ăяo����܂��B
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void myvct_endConnection();

//==============================================================================
/**
 * ���C�u�����I�������B���̒������b�I�����������R�[���o�b�N���Ă΂�܂��B
 * �ʏ�I�����͂��̊֐��́A��b�I���v������Ăяo����܂��B
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void myvct_free();

//==============================================================================
/**
 * �����̃m�C�Y�J�b�g���x���𒲐����܂��i�O������A�N�Z�X�������̂ŁA������Ɂj
 * @param   d �c �����臒l�������邩�A�グ�邩�i������قǏE���₷���Ȃ�j
 * @retval  none
 */
//==============================================================================
extern void myvct_changeVADLevel(int d);


// ���M�������ǂ�����Ԃ��܂�
extern BOOL myvct_IsSendVoiceAndInc(void);


//==============================================================================
/**
 * VCT�J���t�@�����X�ɏ��҂���
 * @param   bitmap   �ڑ����Ă���CLIENT��BITMAP
 * @param   myAid    ������ID
 * @retval  TRUE �ݒ�ł���
 */
//==============================================================================

extern BOOL myvct_AddConference(int bitmap, int myAid);

//==============================================================================
/**
 * VCT�J���t�@�����X����S���͂���
 * @param   bitmap   �ڑ����Ă���CLIENT��BITMAP
 * @param   myAid    ������ID
 * @retval  TRUE �ݒ�ł���
 * @retval  FALSE ����̂܂� �������͕K�v���Ȃ�
 */
//==============================================================================

extern BOOL myvct_DelConference(int myAid);

extern void VCHAT_PauesFlg(BOOL bPause);

#endif


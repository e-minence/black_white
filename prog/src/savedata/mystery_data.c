//============================================================================================
/**
 * @file	mystery_data.c
 * @date	2006.04.28
 * @author	tamada / mitsuhara
 * @brief	�ӂ����ʐM�p�Z�[�u�f�[�^�֘A
 */
//============================================================================================
#include <gflib.h>
#include "savedata/mystery_data.h"

//============================================================================================
//============================================================================================

extern MYSTERY_DATA * SaveData_GetMysteryData(SAVE_CONTROL_WORK * sv);

#define UNIQ_TYPE			0xEDB88320L
#define MYSTERY_DATA_NO_USED		0x00000000
#define MYSTERY_DATA_MAX_EVENT		2048

#define MYSTERY_MENU_FLAG		(MYSTERY_DATA_MAX_EVENT - 1)

//------------------------------------------------------------------
/**
 * @brief	�ӂ����f�[�^�̒�`
 */
//------------------------------------------------------------------
struct MYSTERY_DATA{
  u8 recv_flag[MYSTERY_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  GIFT_DELIVERY delivery[GIFT_DELIVERY_MAX];		// �z�B���W��
  GIFT_CARD card[GIFT_CARD_MAX];			// �J�[�h���R��
};


//============================================================================================
//
//		��ɃZ�[�u�V�X�e������Ă΂��֐�
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�T�C�Y�̎擾
 * @return	int		�ӂ����Z�[�u�f�[�^�̃T�C�Y
 */
//------------------------------------------------------------------
int MYSTERYDATA_GetWorkSize(void)
{
  return sizeof(MYSTERY_DATA);
}
//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^������
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void MYSTERYDATA_Init(MYSTERY_DATA * fd)
{
#ifdef DEBUG_ONLY_FOR_mituhara
  OS_TPrintf("�ӂ����f�[�^������\n");
  // ���̏�����saveload_system.c��SVDT_Init�ōs���Ă���̂ŉ������Ȃ�
  MI_CpuClearFast(fd, sizeof(MYSTERY_DATA));
#endif


  OS_TPrintf("GIFT_DELIVERY %d\n",sizeof(GIFT_DELIVERY));
  OS_TPrintf("GIFT_CARD %d\n",sizeof(GIFT_CARD));

}

//============================================================================================
//
//		���p���邽�߂ɌĂ΂��A�N�Z�X�֐�
//
//============================================================================================


//------------------------------------------------------------------
/**
 * @brief	�f�[�^���L�����ǂ���
 * @param	gift_type
 * @return	BOOL =TRUE �L��
 */
//------------------------------------------------------------------

static BOOL MYSTERYDATA_IsIn(u16 gift_type)
{
    if((gift_type > MYSTERYGIFT_TYPE_NONE) &&
       (gift_type < MYSTERYGIFT_TYPE_MAX)){
        return TRUE;
    }
    return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�z�B���f�[�^�̎擾
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	index		�z�B���f�[�^�̃C���f�b�N�X�i�O�I���W���j
 * @return	GIFT_DATA	�z�B���f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
GIFT_DELIVERY * MYSTERYDATA_GetDeliData(MYSTERY_DATA * fd, int index)
{
    if((index >= 0) && (index < GIFT_DELIVERY_MAX)){
        if(MYSTERYDATA_IsIn(fd->delivery[index].gift_type)){
            // �f�[�^���L���Ȃ̂Ń|�C���^��Ԃ�
            return &fd->delivery[index];
        }
    }
    return (GIFT_DELIVERY *)NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�̎擾
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	index		���蕨�f�[�^�̃C���f�b�N�X�i�O�I���W���j
 * @return	GIFT_CARD	�J�[�h�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
GIFT_CARD *MYSTERYDATA_GetCardData(MYSTERY_DATA *fd, int index)
{
    if((index >= 0) && (index < GIFT_CARD_MAX)){
        if(MYSTERYDATA_IsIn(fd->card[index].gift_type)){
            return &fd->card[index];
        }
    }
    return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�z�B���f�[�^���Z�[�u�f�[�^�֓o�^
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	p		�f�[�^�ւ̃|�C���^
 * @return	TRUE: �Z�[�u�ł��� : FALSE: �󂫃X���b�g����������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_SetDeliData(MYSTERY_DATA *fd, const void *p, int link)
{
  int i;
    BOOL bRet=FALSE;

  // �Z�[�u�ł���̈悪������΃Z�[�u���s
  if(MYSTERYDATA_CheckDeliDataSpace(fd) == FALSE)	return FALSE;

  // ������ȍ~�͗e�ʓI�ɂ̓Z�[�u�ɐ�������͂�
  
  for(i = 0; i < GIFT_DELIVERY_MAX; i++){
    if(!MYSTERYDATA_IsIn(fd->delivery[i].gift_type)){
      MI_CpuCopy8(p, &fd->delivery[i], sizeof(GIFT_DELIVERY));
      fd->delivery[i].link = link;
      bRet = TRUE;
        break;
    }
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
  return bRet;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^���Z�[�u�f�[�^�֓o�^
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	p		�f�[�^�ւ̃|�C���^
 * @return	TRUE: �Z�[�u�ł��� : FALSE: �󂫃X���b�g����������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_SetCardData(MYSTERY_DATA *fd, const void *p)
{
    int i;
    GIFT_CARD *gc = (GIFT_CARD *)p;
    BOOL bRet = FALSE;

    // �Z�[�u�ł���̈悪������΃Z�[�u���s
    if(MYSTERYDATA_CheckCardDataSpace(fd) == FALSE)	return FALSE;
    // �z�B�����܂ރf�[�^�̏ꍇ�͔z�B�������`�F�b�N
    if(gc->beacon.delivery_flag == TRUE &&
       MYSTERYDATA_CheckDeliDataSpace(fd) == FALSE)	return FALSE;

  // ������ȍ~�͗e�ʓI�ɂ̓Z�[�u�ɐ�������͂�
  
  // �J�[�h���Z�[�u����
    for(i = 0; i < GIFT_CARD_MAX; i++){
        if(!MYSTERYDATA_IsIn(fd->card[i].gift_type)){
#ifdef DEBUG_ONLY_FOR_mituhara
            OS_TPrintf("�J�[�h���Z�[�u���܂��� [%d]\n", i);
#endif
            MI_CpuCopy8(gc, &fd->card[i], sizeof(GIFT_CARD));

      // �z�B�����Z�[�u����
            if(gc->beacon.delivery_flag == TRUE){
                MYSTERYDATA_SetDeliData(fd, (const void *)&gc->gift_type, i);
            }
            bRet = TRUE;
            break;
        }
    }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
    return bRet;
}

//------------------------------------------------------------------
/**
 * @brief	�z�B���f�[�^�𖕏�����
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	index		�z�B���f�[�^�̃C���f�b�N�X
 * @return	TRUE: �Z�[�u�ł��� : FALSE: �󂫃X���b�g����������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_RemoveDeliData(MYSTERY_DATA *fd, int index)
{
    GF_ASSERT(index < GIFT_DELIVERY_MAX);
    fd->delivery[index].gift_type = MYSTERYGIFT_TYPE_NONE;
    fd->delivery[index].link = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�𖕏����� �z�B����  BIT�����Ƃ�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @return	TRUE: �Z�[�u�ł��� : FALSE: �󂫃X���b�g����������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_RemoveCardDataPlusBit(MYSTERY_DATA *fd, int index)
{
    GF_ASSERT(index < GIFT_CARD_MAX);
    fd->card[index].gift_type = MYSTERYGIFT_TYPE_NONE;
    //BIT�����Ƃ�
    MYSTERYDATA_ResetEventRecvFlag(fd, fd->card[index].beacon.event_id);
    // �����N����Ă���J�[�h���ꏏ�ɖ���
    MYSTERYDATA_RemoveCardLinkDeli(fd, index);

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�𖕏�����
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @return	TRUE: �Z�[�u�ł��� : FALSE: �󂫃X���b�g����������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_RemoveCardData(MYSTERY_DATA *fd, int index)
{
    GF_ASSERT(index < GIFT_CARD_MAX);
    fd->card[index].gift_type = MYSTERYGIFT_TYPE_NONE;

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�z�B���f�[�^���Z�[�u�ł��邩�`�F�b�N
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @return	TRUE: �󂫂����� : FALSE: �󂫃X���b�g������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_CheckDeliDataSpace(MYSTERY_DATA *fd)
{
    int i;
    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(!MYSTERYDATA_IsIn(fd->delivery[i].gift_type)){
            return TRUE;
        }
    }
    return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^���Z�[�u�ł��邩�`�F�b�N
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	size		�f�[�^�̃T�C�Y
 * @return	TRUE: �󂫂����� : FALSE: �󂫃X���b�g������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_CheckCardDataSpace(MYSTERY_DATA *fd)
{
  int i;
  for(i = 0; i < GIFT_CARD_MAX; i++){
      if(!MYSTERYDATA_IsIn(fd->card[i].gift_type)){
          return TRUE;
      }
  }
  return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	���蕨�f�[�^�̑��݃`�F�b�N
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	index		���蕨�f�[�^�̃C���f�b�N�X�i�O�I���W���j
 * @return	BOOL	TRUE�̎��A���݂���
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsExistsDelivery(const MYSTERY_DATA * fd, int index)
{
    GF_ASSERT(index < GIFT_DELIVERY_MAX);
  
    if(MYSTERYDATA_IsIn(fd->delivery[index].gift_type)){
        return TRUE;
    }
    return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�����݂��邩�Ԃ�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @return	BOOL	TRUE�̎��A���݂���
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsExistsCard(const MYSTERY_DATA * fd, int index)
{
    GF_ASSERT(index < GIFT_CARD_MAX);

    if(MYSTERYDATA_IsIn(fd->card[index].gift_type)){
        return TRUE;
    }
    return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���ɃJ�[�h�f�[�^�����݂��邩�Ԃ�
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsExistsCardAll(const MYSTERY_DATA *fd)
{
  int i;
  for(i = 0; i < GIFT_CARD_MAX; i++)
    if(MYSTERYDATA_IsExistsCard(fd, i) == TRUE)
      return TRUE;
  return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	�w��̃J�[�h�Ƀ����N����Ă���z�B�������݂��邩
 * @param	��index��0�`2���L��
 * @return	TRUE: �z�B���͑��݂��� FALSE: ���݂��Ȃ�
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_GetCardLinkDeli(const MYSTERY_DATA *fd, int index)
{
    int i;

    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(MYSTERYDATA_IsIn(fd->delivery[i].gift_type)){
            if(fd->delivery[i].link == index){
                return TRUE;
            }
        }
    }
    return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief	�w��J�[�h�Ƀ����N����Ă���z�B�����폜
 * @param	��index��0�`2���L��(������Ή������Ȃ�)
 * @return	NONE
 */
//------------------------------------------------------------------
void MYSTERYDATA_RemoveCardLinkDeli(const MYSTERY_DATA *fd, int index)
{
    int i;

    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(MYSTERYDATA_IsIn(fd->delivery[i].gift_type)){
            if(fd->delivery[i].link == index){
#if (DEBUG_ONLY_FOR_mituhara | DEBUG_ONLY_FOR_ohno)
                OS_TPrintf("�J�[�h�ƈꏏ�� %d �Ԃ̂�������̂������܂���\n", i );
#endif
                MYSTERYDATA_RemoveDeliData((MYSTERY_DATA *)fd, i);
                return;
            }
        }
    }
}

//------------------------------------------------------------------
/**
 * @brief	�w��̃C�x���g�͂��łɂ���������Ԃ�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	num		�C�x���g�ԍ�
 * @return	BOOL	TRUE�̎��A���łɂ�����Ă���
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsEventRecvFlag(MYSTERY_DATA * fd, int num)
{
  GF_ASSERT(num < MYSTERY_DATA_MAX_EVENT);
  return !!(fd->recv_flag[num / 8] & (1 << (num & 7)));
}


//------------------------------------------------------------------
/**
 * @brief	�w��̃C�x���g���������t���O�𗧂Ă�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	num		�C�x���g�ԍ�
 * @return	NONE
 */
//------------------------------------------------------------------
void MYSTERYDATA_SetEventRecvFlag(MYSTERY_DATA * fd, int num)
{
  GF_ASSERT(num < MYSTERY_DATA_MAX_EVENT);
  fd->recv_flag[num / 8] |= (1 << (num & 7));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
}

//------------------------------------------------------------------
/**
 * @brief	�w��̃C�x���g���������t���O�𗎂� �J�[�h�Ɣz�B�����ꏏ�ɏ����������g�p����
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	num		�C�x���g�ԍ�
 * @return	NONE
 */
//------------------------------------------------------------------
void MYSTERYDATA_ResetEventRecvFlag(MYSTERY_DATA * fd, int num)
{
    u8 notbit = (u8)~(1 << (num & 7));
    GF_ASSERT(num < MYSTERY_DATA_MAX_EVENT);
    
#if (DEBUG_ONLY_FOR_mituhara | DEBUG_ONLY_FOR_ohno)
    OS_TPrintf("MYSTERYDATA_ResetEventRecvFlag %x ",  fd->recv_flag[num / 8]);
#endif
  fd->recv_flag[num / 8] &= notbit;
#if (DEBUG_ONLY_FOR_mituhara | DEBUG_ONLY_FOR_ohno)
    OS_TPrintf(" %x \n",  fd->recv_flag[num / 8]);
#endif
    
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
}

//------------------------------------------------------------------
/**
 * @brief	�ӂ����Ȃ�������̂�\���o���邩�H
 * @param	NONE
 * @return	TRUE: �\���@FALSE: �\�����Ȃ�
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsMysteryMenu(MYSTERY_DATA *fd)
{
  return MYSTERYDATA_IsEventRecvFlag(fd, MYSTERY_MENU_FLAG);
}


//------------------------------------------------------------------
/**
 * @brief	�ӂ����Ȃ�������̂̕\���t���O��ON�ɂ���
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
void MYSTERYDATA_SetMysteryMenu(MYSTERY_DATA *fd)
{
  MYSTERYDATA_SetEventRecvFlag(fd, MYSTERY_MENU_FLAG);
}


//============================================================================================
//
//		����ɏ�ʂ̊֐��Q�@��ɂ�������Ă�ł�������
//
//============================================================================================

static MYSTERY_DATA *_mystery_ptr = NULL;

//------------------------------------------------------------------
/**
 * @brief	����ȉ��̊֐����g�����߂ɕK�v�ȏ�����
 * @param	sv		�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	heap_id		���[�N���擾����q�[�v��ID
 * @return	NONE
 */
//------------------------------------------------------------------
void MYSTERYDATA_InitSlot(SAVE_CONTROL_WORK * sv, int heap_id)
{
  LOAD_RESULT result;

  if(_mystery_ptr == NULL){
    _mystery_ptr = SaveData_GetMysteryData(sv);
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MYSTERYDATA)
	SVLD_SetCrc(GMDATA_ID_MYSTERYDATA);
#endif //CRC_LOADCHECK
}

//------------------------------------------------------------------
/**
 * @brief	����ȉ��̊֐����g���I�������̌�n��
 * @param	sv		�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	flag		TRUE: �Z�[�u���� / FALSE: �Z�[�u���Ȃ�
 * @return	NONE
 */
//------------------------------------------------------------------
void MYSTERYDATA_FinishSlot(SAVE_CONTROL_WORK * sv, int flag)
{
  SAVE_RESULT result;
  if(_mystery_ptr){
#if 0
    if(flag == TRUE)
      SaveData_Save(sv);
#endif
    _mystery_ptr = NULL;
  }
}
     
//------------------------------------------------------------------
/**
 * @brief	�X���b�g�Ƀf�[�^�����邩�Ԃ��֐�
 * @param	NONE
 * @return	-1..�f�[�^�Ȃ�: 0�ȏ� �f�[�^index�ԍ�
*/
//------------------------------------------------------------------
int MYSTERYDATA_GetSlotData(void)
{
    int i;
    for(i = 0; i < GIFT_DELIVERY_MAX; i++){
        if(MYSTERYDATA_IsExistsDelivery(_mystery_ptr, i) == TRUE)
            return i;
    }
    return -1;
}

//------------------------------------------------------------------
/**
 * @brief	�X���b�g�Ƀf�[�^�����邩�Ԃ��֐�
 * @param	NONE
 * @return	FALSE..�f�[�^�Ȃ�: TRUE �f�[�^����
*/
//------------------------------------------------------------------
BOOL MYSTERYDATA_CheckSlotData(void)
{
    if(MYSTERYDATA_GetSlotData() == -1){
        return FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�w��ԍ��̃X���b�g�f�[�^�̃^�C�v��Ԃ�
 * @param	index		�X���b�g�̃C���f�b�N�X�ԍ�
 * @return	int		MYSTERYGIFT_TYPE_xxxxx
 *
 * ���C���f�b�N�X�ԍ���MYSTERYDATA_GetSlotData�ŕԂ��ꂽ�l
*/
//------------------------------------------------------------------
int MYSTERYDATA_GetSlotType(int index)
{
    GIFT_DELIVERY *dv;

    dv = MYSTERYDATA_GetDeliData(_mystery_ptr, index);
    if(dv){
        return (int)dv->gift_type;
    }
    return MYSTERYGIFT_TYPE_NONE;
}


//------------------------------------------------------------------
/**
 * @brief	�w��ԍ��̃X���b�g�\���̂ւ̃|�C���^��Ԃ�
 * @param	index		�X���b�g�̃C���f�b�N�X�ԍ�
 * @return	GIFT_PRESENT	�\���̂ւ̃|�C���^
 *
 * ���C���f�b�N�X�ԍ���MYSTERYDATA_GetSlotData�ŕԂ��ꂽ�l
 */
//------------------------------------------------------------------
GIFT_PRESENT *MYSTERYDATA_GetSlotPtr(int index)
{
    GIFT_DELIVERY *dv;

    dv = MYSTERYDATA_GetDeliData(_mystery_ptr, index);
    if(dv){
        return &dv->data;
    }
    return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�w��̃X���b�g����������
 * @param	index		�X���b�g�̃C���f�b�N�X�ԍ�
 * @return	NONE
 *
 * ���C���f�b�N�X�ԍ���MYSTERYDATA_GetSlotData�ŕԂ��ꂽ�l
 */
//------------------------------------------------------------------
void MYSTERYDATA_RemoveSlot(int index)
{
  MYSTERYDATA_RemoveDeliData(_mystery_ptr, index);
}

//------------------------------------------------------------------



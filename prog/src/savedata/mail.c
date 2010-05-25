/**
 *  @file mail.c
 *  @brief  ���[���Z�[�u�f�[�^�@����
 *  @author Miyuki Iwasawa
 *  @date 06.02.09
 *
 *  ���[���Z�[�u�f�[�^���f�菇
 *  1,MAIL_SearchNullID()�ŋ󂫗̈�������A�f�[�^ID�擾
 *  2,MailData_CreateWork()�Ń_�~�[�f�[�^�쐬�p���[�N�G���A���擾
 *  3,MailData_CreateDataFromSave()�Ȃǂ��g���ă��[�N�Ƀf�[�^���\�z
 *  4,MAIL_AddDataFromWork()�Ǝ擾�ς݂̃f�[�^ID,���[�N�̃f�[�^���g���āA
 *  �@�Z�[�u�̈�Ƀf�[�^�𔽉f������
 *  5,�_�~�[���[�N�G���A���J������
 */

#include "gflib.h"
#include "savedata/save_tbl.h"
#include "gamesystem/game_data.h"

#include "savedata/mail.h"
#include "savedata/mail_util.h"
#include "savedata/mystatus.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "pokeicon/pokeicon.h"

#include "system/pms_data.h"

#include "poke_icon.naix"
#include "poke_tool/monsno_def.h"

//==============================================================================================
// �\���̐錾
//==============================================================================================

//=============================================================
///���[���f�[�^�^
// 
//  size=56byte
//
//  �\���̃T�C�Y��4byte���E�ɍ��킹�邽�߁AMAIL_ICON�z��v�f����+1����
//  �p�f�B���O���s���Ă��܂�
//=============================================================
typedef struct _MAIL_DATA{
  u32 writerID; //<�g���[�i�[ID 4                         4
  u8  sex;      //<��l���̐��� 1                         5
  u8  region;   //<���R�[�h 1                             6
  u8  version;  //<�J�Z�b�g�o�[�W���� 1                   7
  u8  design;   //<�f�U�C���i���o�[ 1                     8
  STRCODE name[PERSON_NAME_SIZE+EOM_SIZE];  // 16        24
  u16     padding[3];                       //< �p�f�B���O�U�o�C�g   30
  u16     form_bit;                         // padding�̈���v���`�i����3�̂̃|�P������    32
                                            // �t�H�����ԍ��Ƃ��Ďg�p(5bit�P��)
  PMS_DATA  msg[MAILDAT_MSGMAX];            //<���̓f�[�^ 56
}_MAIL_DATA;

//_MAIL_DATA�\���̂̃T�C�Y�ɕύX����������A
//���l�w�肵�Ă���POKEMON_PARAM�\���̃����o�ɂ��ύX��������̂ŁA
//�`�F�b�N������
#ifdef DEBUG_ONLY_FOR_sogabe
SDK_COMPILER_ASSERT( sizeof(_MAIL_DATA) == 56 );
#endif

//=============================================================
///���[���f�[�^�Z�[�u�f�[�^�u���b�N��`�\����
//=============================================================
typedef struct _MAIL_BLOCK{

  MAIL_DATA paso[MAIL_STOCK_PASOCOM];   // ���[���ő吔20��

}_MAIL_BLOCK;


///�|�P�����A�C�R���̍ő�CGXID(����ŃA�C�R����������Ȃ炱�����ς���K�v������)
#define ICON_CGXID_MAX    (NARC_poke_icon_poke_icon_656_freeze_f_NCGR)

/**
 *  @brief  ���[���f�[�^�T�C�Y�擾
 *
 *  �����[���f�[�^��ʂ̃T�C�Y
 */
int MailData_GetDataWorkSize(void)
{
  return sizeof(MAIL_DATA);
}
/**
 *  @brief  ���[���f�[�^�N���A(�����f�[�^�Z�b�g)
 */
void MailData_Clear(MAIL_DATA* dat)
{
  int i;
  
  dat->writerID = 0;
  dat->sex      = PM_MALE;
  dat->region   = CasetteLanguage;  
  dat->version  = CasetteVersion;   
  dat->design   = MAIL_DESIGN_NULL;

  GFL_STD_MemFill16(dat->name, GFL_STR_GetEOMCode(), sizeof(dat->name));

  dat->form_bit = 0;
  for(i = 0;i < MAILDAT_MSGMAX;i++){
    PMSDAT_Clear(&dat->msg[i]);
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
  SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *  @brief  ���[���f�[�^���L�����ǂ����Ԃ�
 *  @retval FALSE ����
 *  @retval TRUE  �L��
 */
BOOL MailData_IsEnable(MAIL_DATA* dat)
{
  if( dat->design >= MAIL_DESIGN_START &&
    dat->design <= MAIL_DESIGN_END){
    return TRUE;
  }
  return FALSE;
}

/**
 *  @brief  ���[���f�[�^�̃��[�N���擾���ĕԂ�
 *
 *  ���Ăяo�������ӔC�����ĉ�����邱��
 *  
 */
MAIL_DATA* MailData_CreateWork(HEAPID heapID)
{
  MAIL_DATA* p;

  p = GFL_HEAP_AllocMemoryLo(heapID,sizeof(MAIL_DATA)); 
  MailData_Clear(p);

  return p;
}

/**
 *  @brief  ���[���f�[�^�̍\���̃R�s�[
 */
void MailData_Copy(MAIL_DATA* src,MAIL_DATA* dest)
{
  MI_CpuCopy8(src,dest,sizeof(MAIL_DATA));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MAILDATA)
  SVLD_SetCrc(GMDATA_ID_MAILDATA);
#endif //CRC_LOADCHECK
}

/**
 *  @brief  ���[���f�[�^�̓��e��r
 *
 * @retval  BOOL    ��v���Ă�����TRUE��Ԃ�
 */
BOOL MailData_Compare(MAIL_DATA* src1,MAIL_DATA* src2)
{
  int i;
  
  if( (src1->writerID != src2->writerID) ||
    (src1->sex != src2->sex) ||
    (src1->region != src2->region) ||
    (src1->version != src2->version) ||
    (src1->design != src2->design) ||
    (src1->form_bit != src2->form_bit) ){
    return FALSE;
  }
  if( GFL_STD_MemComp( src1->name,src2->name,sizeof(src1->name) ) != 0){
    return FALSE;
  }
  for(i = 0;i < MAILDAT_MSGMAX;i++){
    if(!PMSDAT_Compare( &src1->msg[i], &src2->msg[i] )){
      return FALSE;
    }
  }
  return TRUE;
}

/**
 *  @brief  �f�U�C��No,�|�P�����̃|�W�V�����A�Z�[�u�f�[�^�w�肵�ă��[����V�K�쐬��Ԃɏ�����
 *  @param  dat �f�[�^���쐬����MAIL_DATA�\���̌^�ւ̃|�C���^
 *  @param  design_no ���[���̃C���[�WNo
 *  @param  pos   ���[������������|�P�����̎莝�����̃|�W�V����
 *  @param  save  �Z�[�u�f�[�^�ւ̃|�C���^
 */
void MAILDATA_CreateFromSaveData(MAIL_DATA* dat,u8 design_no,u8 pos, GAMEDATA* gamedata)
{
  MYSTATUS  *my;
  
  MailData_Clear(dat);
  dat->design = design_no;

  //�Z�[�u�f�[�^����
  my    = GAMEDATA_GetMyStatus( gamedata );

  //���@�̖��O
  GFL_STD_MemCopy( MyStatus_GetMyName(my), dat->name, sizeof(dat->name));
  //����
  dat->sex = (u8)MyStatus_GetMySex(my);
  //�g���[�i�[ID
  dat->writerID = MyStatus_GetID(my);

}

/**
 *  @brief  ���[���f�[�^�@�g���[�i�[ID�擾
 */
u32 MailData_GetWriterID(const MAIL_DATA* dat)
{
  return dat->writerID;
}
/**
 *  @brief  ���[���f�[�^�@�g���[�i�[ID�K�p
 */
void MailData_SetWriterID(MAIL_DATA* dat,u32 id)
{
  dat->writerID = id;
}

/**
 *  @brief  ���[���f�[�^�@���C�^�[���擾
 */
STRCODE* MailData_GetWriterName(MAIL_DATA* dat)
{
  return &(dat->name[0]);
}
/**
 *  @brief  ���[���f�[�^�@���C�^�[���K�p
 */
void MailData_SetWriterName(MAIL_DATA* dat,STRCODE* name)
{
  GFL_STD_MemCopy( name, dat->name, BUFLEN_PERSON_NAME);
}

/**
 *  @brief  ���[���f�[�^�@���C�^�[�̐��ʂ��擾
 */
u8  MailData_GetWriterSex(const MAIL_DATA* dat)
{
  return dat->sex;
}
/**
 *  @brief  ���[���f�[�^�@���C�^�[�̐��ʂ�K�p
 */
void MailData_SetWriterSex(MAIL_DATA* dat,const u8 sex)
{
  dat->sex = sex;
}

/**
 *  @brief  ���[���f�[�^�@�f�U�C��No�擾
 */
u8  MailData_GetDesignNo(const MAIL_DATA* dat)
{
  return dat->design;
}
/**
 *  @brief  ���[���f�[�^�@�f�U�C��No�K�p
 */
void MailData_SetDesignNo(MAIL_DATA* dat,const u8 design)
{
  if(design >= MAIL_DESIGN_MAX){
    return;
  }
  dat->design = design;
}

/**
 *  @brief  ���[���f�[�^�@���R�[�h�擾
 */
u8  MailData_GetCountryCode(const MAIL_DATA* dat)
{
  return dat->region;
}
/**
 *  @brief  ���[���f�[�^�@���R�[�h�K�p
 */
void MailData_SetCountryCode(MAIL_DATA* dat,const u8 code)
{
  dat->region = code;
}

/**
 *  @brief  ���[���f�[�^�@�J�Z�b�g�o�[�W�����擾
 */
u8  MailData_GetCasetteVersion(const MAIL_DATA* dat)
{
  return dat->version;
}
/**
 *  @brief  ���[���f�[�^�@�J�Z�b�g�o�[�W�����K�p
 */
void MailData_SetCasetteVersion(MAIL_DATA* dat,const u8 version)
{
  dat->version = version;
}


/**
 *  @brief  ���[���f�[�^�@form_bit�擾
 */
u16 MailData_GetFormBit(const MAIL_DATA* dat)
{
  return dat->form_bit;
}

void MailData_SetFormBit( MAIL_DATA* dat, u16 word )
{
  dat->form_bit = word;
}

/**
 *  @brief  ���[���f�[�^�@�ȈՕ��擾(�C���f�b�N�X�w���)
 */
PMS_DATA* MailData_GetMsgByIndex(MAIL_DATA* dat,u8 index)
{
  if(index < MAILDAT_MSGMAX){
    return &(dat->msg[index]);
  }else{
    return &(dat->msg[0]);
  }
}
/**
 *  @brief  ���[���f�[�^�@�ȈՕ��Z�b�g(�C���f�b�N�X�w���)
 */
void MailData_SetMsgByIndex(MAIL_DATA* dat, const PMS_DATA* pms,u8 index)
{
  if(index >= MAILDAT_MSGMAX){
    return;
  }
  PMSDAT_Copy(&dat->msg[index],pms);
}

/**
 *  @brief  ���[���f�[�^�@�ȈՕ�������擾(�C���f�b�N�X�w��)
 *
 *  @param  dat MAIL_DATA*
 *  @param  index �ȈՕ��C���f�b�N�X
 *  @param  buf   �擾����������|�C���^�̊i�[�ꏊ
 *
 *  @retval FALSE ������̎擾�Ɏ��s(�܂��͊ȈՕ����L���ȃf�[�^�ł͂Ȃ�)
 *  
 *  @li buf�ɑ΂��ē����Ń��������m�ۂ��Ă���̂ŁA�Ăяo�����������I�ɉ�����邱��
 *  @li FALSE���Ԃ����ꍇ�Abuf��NULL�N���A�����
 */
BOOL MailData_GetMsgStrByIndex(const MAIL_DATA* dat,u8 index,STRBUF* buf,HEAPID heapID)
{
  if(index >= MAILDAT_MSGMAX){
    buf = NULL;
    return FALSE;
  }
  
  if(!PMSDAT_IsEnabled(&dat->msg[index])){
    buf = NULL;
    return FALSE;
  }

  buf = PMSDAT_ToString(&dat->msg[index],heapID);
  return TRUE;
}

//=================================================================
//
//=================================================================
//���[�J���֐��v���g�^�C�v
static int mail_GetNullData(MAIL_DATA* array,int num);
static int mail_GetNumEnable(MAIL_DATA* array,int num);
static MAIL_DATA* mail_GetAddress(MAIL_BLOCK* bloc,MAILBLOCK_ID blockID,int dataID);

/**
 *  @brief  �Z�[�u�f�[�^�u���b�N�ւ̃|�C���^���擾
MAIL_BLOCK* SaveData_GetMailBlock(SAVE_CONTROL_WORK* sv)
{
  return SaveControl_DataPtrGet( sv, GMDATA_ID_MAILDATA );
}
 */

//=============================================================================================
/**
 * @brief GAMEDATA���烁�[���f�[�^�u���b�N��Ԃ�
 *
 * @param   gamedata    
 *
 * @retval  MAIL_BLOCK*   
 */
//=============================================================================================
MAIL_BLOCK* GAMEDATA_GetMailBlock( GAMEDATA *gamedata )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gamedata );
  return SaveControl_DataPtrGet( sv, GMDATA_ID_MAILDATA );
  
}
/**
 *  @brief  ���[���Z�[�u�f�[�^�u���b�N�T�C�Y�擾
 *
 *  �����[���f�[�^��ʂ̃T�C�Y�ł͂Ȃ��̂Œ��ӁI
 */
int MAIL_GetBlockWorkSize(void)
{
  return sizeof(MAIL_DATA)*MAIL_STOCK_MAX;
}
/**
 *  @brief  ���[���Z�[�u�f�[�^�u���b�N������
 */
void MAIL_Init(MAIL_BLOCK* dat)
{
  int i = 0;

  for(i = 0;i < MAIL_STOCK_PASOCOM;i++){
    MailData_Clear(&dat->paso[i]);
  }
}

/**
 *  @brief  �󂢂Ă��郁�[���f�[�^ID���擾
 *
 *  @param  id �ǉ����������[���u���b�NID
 *
 *  @return int �f�[�^��ǉ��ł���ꍇ�͎Q��ID
 *        �ǉ��ł��Ȃ��ꍇ�̓}�C�i�X�l���Ԃ�
 */
int MAIL_SearchNullID(MAIL_BLOCK* block,MAILBLOCK_ID id)
{
  return mail_GetNullData(block->paso,MAIL_STOCK_PASOCOM);
}

/**
 *  @brief  ���[���f�[�^���폜
 *
 *  @param  blockID �u���b�N��ID
 *  @param  dataID  �f�[�^ID
 */
void MAIL_DelMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID)
{
  MAIL_DATA* pd = NULL;
  
  pd = mail_GetAddress(block,blockID,dataID);
  if(pd != NULL){
    MailData_Clear(pd);
  }
}

/**
 *  @brief  ���[���f�[�^���Z�[�u�u���b�N�ɒǉ�
 *
 *  �������n����MAIL_DATA�\���̌^�f�[�^�̒��g���Z�[�u�f�[�^�ɔ��f�����̂�
 *  �@�������ȃf�[�^�����Ȃ��悤�ɒ��ӁI
 */
void MAIL_AddMailFormWork(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* src)
{
  MAIL_DATA* pd = NULL;
  
  pd = mail_GetAddress(block,blockID,dataID);
  if(pd != NULL){
    MailData_Copy(src,pd);
  }
}

/**
 *  @brief  �w��u���b�N�ɗL���f�[�^���������邩�Ԃ�
 */
int MAIL_GetEnableDataNum(MAIL_BLOCK* block,MAILBLOCK_ID blockID)
{
  return mail_GetNumEnable(block->paso,MAIL_STOCK_PASOCOM);
}

/**
 *  @brief  ���[���f�[�^�̃R�s�[���擾
 *
 *  �������Ń��������m�ۂ���̂ŁA�Ăяo�������ӔC�����ė̈���J�����邱��
 *  ������ID���w�肵���ꍇ�A��f�[�^��Ԃ�
 */
MAIL_DATA* MAIL_AllocMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,HEAPID heapID)
{
  MAIL_DATA* src = NULL;
  MAIL_DATA* dest;
  
  src = mail_GetAddress(block,blockID,dataID);
  dest = MailData_CreateWork(heapID);
  if(src != NULL){
    MailData_Copy(src,dest);
  }
  return dest;
}

/**
 *  @brief  ���[���f�[�^�̃R�s�[���擾
 *
 *  �����炩���ߊm�ۂ���MAIL_DATA�^�������ɃZ�[�u�f�[�^���R�s�[���Ď擾
 */
void MAIL_GetMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* dest)
{
  MAIL_DATA* src = NULL;
  
  src = mail_GetAddress(block,blockID,dataID);
  if(src == NULL){
    MailData_Clear(dest);
  }else{
    MailData_Copy(src,dest);
  }
}

//=================================================================
//
//=================================================================

/**
 *  @brief  �莝���u���b�N�̋󂫂�T���ĕԂ�
 *
 *  @param  array ���[���f�[�^�z��ւ̃|�C���^
 *  @param  num   �������œn�����z��̗v�f��
 */
static int mail_GetNullData(MAIL_DATA* array,int num)
{
  int i = 0;

  for(i = 0;i < num;i++){
    if(!MailData_IsEnable(&array[i])){
      return i;
    }
  }
  return MAILDATA_NULLID;
}

/**
 *  @brief  �L���f�[�^�̐���T���ĕԂ�
 */
static int mail_GetNumEnable(MAIL_DATA* array,int num)
{
  int i = 0;
  int ct = 0;
  
  for(i = 0;i < num;i++){
    if(MailData_IsEnable(&array[i])){
      ct++;
    }
  }
  return ct;
}

/**
 *  @brief  �w��ID�����u���b�N���̃��[���f�[�^�ւ̃|�C���^��Ԃ�
 */
static MAIL_DATA* mail_GetAddress(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID)
{
  if(dataID < MAIL_STOCK_PASOCOM){
    return &(block->paso[dataID]);
  }
  return NULL;
}

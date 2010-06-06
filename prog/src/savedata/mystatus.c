//============================================================================================
/**
 * @file  mystatus.c
 * @brief ������ԃf�[�^�A�N�Z�X�p�\�[�X
 * @author  tamada GAME FREAK inc.
 * @date  2005.10.27
 */
//============================================================================================
#include <gflib.h>
#include "savedata/mystatus.h"
#include "savedata/wifihistory.h"
#include "mystatus_local.h"
#include "system/main.h"
#include "print/str_tool.h"

#define TR_LOW_MASK   (0xffff)



//============================================================================================
//============================================================================================

//�\���̂ƍ\���̂̃T�C�Y��`�����ꂽ�ꍇ�R���p�C���G���[
SDK_COMPILER_ASSERT( sizeof(MYSTATUS) == MYSTATUS_SAVE_SIZE );

//============================================================================================
//
//  �Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ������ԕێ����[�N�̃T�C�Y�擾
 * @return  int   �T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------
int MyStatus_GetWorkSize(void)
{
  return sizeof(MYSTATUS);
}

//----------------------------------------------------------
/**
 * @brief ������ԕێ����[�N�̊m��
 * @param heapID    �������m�ۂ������Ȃ��q�[�v�w��
 * @return  MYSTATUS  �擾�������[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
MYSTATUS * MyStatus_AllocWork(u32 heapID)
{
  MYSTATUS * my;
  my = GFL_HEAP_AllocMemory(heapID, sizeof(MYSTATUS));
  MyStatus_Init(my);

  return my;
}

//----------------------------------------------------------
/**
 * @brief MYSTATUS�̃R�s�[
 * @param from  �R�s�[��MYSTATUS�ւ̃|�C���^
 * @param to    �R�s�[��MYSTATUS�ւ̃|�C���^
 */
//----------------------------------------------------------
void MyStatus_Copy(const MYSTATUS * from, MYSTATUS * to)
{
  GFL_STD_MemCopy(from, to, sizeof(MYSTATUS));
}

//============================================================================================
//
//  MYSTATUS����̂��߂̊֐�
//
//============================================================================================

//----------------------------------------------------------
/**
 * @brief   ������ԃf�[�^�̏�����
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
void MyStatus_Init(MYSTATUS * my)
{
  GFL_STD_MemClear(my, sizeof(MYSTATUS));

  // �J�Z�b�g�̌���R�[�h�𖄂ߍ���
  my->region_code = PM_LANG;

  // RomCode
  MyStatus_SetRomCode( my, PM_VERSION );

#ifdef PM_DEBUG
  {
    int cont = GFL_UI_KEY_GetCont();

    // �f�o�b�O�ő����̐l�ɂȂ肷�܂���B
    if(cont&PAD_BUTTON_L){
      MyStatus_SetRegionCode(my, LANG_JAPAN );
    }else if(cont&PAD_BUTTON_R){
      MyStatus_SetRegionCode(my, LANG_ENGLISH );
    }else if(cont&PAD_BUTTON_SELECT){
      MyStatus_SetRegionCode(my, LANG_FRANCE   );
    }else if(cont&PAD_BUTTON_START){
      MyStatus_SetRegionCode(my, LANG_ITALY  );
    }else if(cont&PAD_KEY_UP){
      MyStatus_SetRegionCode(my, LANG_GERMANY );
    }else if(cont&PAD_KEY_LEFT){
      MyStatus_SetRegionCode(my, LANG_SPAIN  );
    }
  }

  // �f�t�H���g�������Ă��� (taya)
  {
    static const STRCODE default_name[] = {
      0x30d6, 0x30e9, 0x30c3, 0x30af, 0xffff,
    };
    STRTOOL_Copy( default_name, my->name, NELEMS(my->name) );
  }
#endif
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���O���O�Ŗ��߂��Ă肤���`�F�b�N(�f�[�^�������Ă��Ȃ���Ԃ����`�F�b�N����)
 *
 *  @param  my  My�X�e�[�^�X���[�N
 *
 *  @retval TRUE  �N���A��ԁi�S��0�j
 *  @retval FALSE ���������Ă���
 */
//-----------------------------------------------------------------------------
BOOL MyStatus_CheckNameClear( const MYSTATUS * my )
{
  int i;

  for( i=0; i<PERSON_NAME_SIZE + EOM_SIZE; i++ ){
    if( my->name[ i ] != 0 ){
      return FALSE;
    }
  }

  return TRUE;
}

//----------------------------------------------------------
/**
 * @brief �����̖��O�Z�b�g
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param name  ���O������ւ̃|�C���^
 */
//----------------------------------------------------------
void MyStatus_SetMyName(MYSTATUS * my, const STRCODE * name)
{
  STRTOOL_Copy( name, my->name, PERSON_NAME_SIZE + EOM_SIZE );
}

//----------------------------------------------------------
/**
 * @brief �����̖��O�Z�b�g(STRBUF�Łj
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param str   ���O�ێ��o�b�t�@�ւ̃|�C���^
 */
//----------------------------------------------------------
void MyStatus_SetMyNameFromString(MYSTATUS * my, const STRBUF * str)
{
  GFL_STR_GetStringCode(str, my->name, PERSON_NAME_SIZE + EOM_SIZE);
}

//----------------------------------------------------------
/**
 * @brief �����̖��O�擾
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @return  STRCODE   ���O������ւ̃|�C���^
 */
//----------------------------------------------------------
const STRCODE * MyStatus_GetMyName(const MYSTATUS * my)
{
  return my->name;
}

//----------------------------------------------------------
/**
 * @brief �����̖��O�擾�iSTRCODE�ɃR�s�[�j
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param buf   �R�s�[������STRCODE�ւ̃|�C���^�i�e���œ��e�͊m�ۂ��Ă��鎖���O��j
 * @return  none
 */
//----------------------------------------------------------
void MyStatus_CopyNameStrCode( const MYSTATUS * my, STRCODE *buf, int dest_length )
{
  GF_ASSERT(dest_length >= PERSON_NAME_SIZE + EOM_SIZE);
  STRTOOL_Copy(my->name, buf, dest_length);
}

//----------------------------------------------------------
/**
 * @brief �����̖��O�擾�iSTRBUF�ɃR�s�[�j
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param buf   �R�s�[������STRBUF�ւ̃|�C���^�i�e���œ��e�͊m�ۂ��Ă��鎖���O��j
 * @return  none
 */
//----------------------------------------------------------
void MyStatus_CopyNameString( const MYSTATUS * my, STRBUF *buf )
{
  GFL_STR_SetStringCode(buf, my->name);
}

//----------------------------------------------------------
/**
 * @brief �����̖��O�擾�iSTRBUF�𐶐��j
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param heapID  STRBUF�𐶐�����q�[�v��ID
 * @return  STRBUF  ���O���i�[����STRBUF�ւ̃|�C���^
 */
//----------------------------------------------------------
STRBUF * MyStatus_CreateNameString(const MYSTATUS * my, int heapID)
{
  STRBUF * tmpBuf = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, heapID);
  MyStatus_CopyNameString( my, tmpBuf );
  return tmpBuf;
}


//----------------------------------------------------------
/**
 * @brief �����̂h�c�Z�b�g
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param id    �h�c�w��
 */
//----------------------------------------------------------
void MyStatus_SetID(MYSTATUS * my, u32 id)
{
  my->id = id;
}
//----------------------------------------------------------
/**
 * @brief �����̂h�c�擾
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @return  int   �����̂h�c
 */
//----------------------------------------------------------
u32 MyStatus_GetID(const MYSTATUS * my)
{
  return my->id;
}

//----------------------------------------------------------
/**
 * @brief �����̂h�c�擾(���ʂQ�o�C�g)
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @return  int   �����̂h�c
 */
//----------------------------------------------------------
u16 MyStatus_GetID_Low(const MYSTATUS * my)
{
  return (u16)(my->id&TR_LOW_MASK);
}



//---------------------------------------------------------------------------
/**
 * @brief GTS�EWifi�o�g���^���[�pId�̎擾
 * @param sysdt   �V�X�e���f�[�^�ւ̃|�C���^
 * @param profileId [out]GameSpy�v���t�@�C��ID���o�͂���|�C���^
 * @param FriendKey [out]�Ƃ������R�[�h���o�͂���|�C���^
 * @param u64     �Ƃ������R�[�h�̒l�i���߂Ď擾����FriendKey�������ƕێ��j
 */
//---------------------------------------------------------------------------
s32 MyStatus_GetProfileID( const MYSTATUS* my )
{
  return my->profileID;
}

//---------------------------------------------------------------------------
/**
 * @brief GTS�EWifi�o�g���^���[�pId�̐ݒ�(����̂ݑ���ł���j
 * @param sysdt   �V�X�e���f�[�^�ւ̃|�C���^
 * @param none
 */
//---------------------------------------------------------------------------
void MyStatus_SetProfileID( MYSTATUS* my, s32 profileID )
{
  // GTS�EWifi�o�g���^���[�p��ID�E�Ƃ������R�[�h��
  // �����o�^����Ă��Ȃ������ꍇ�o�^����
  if( my->profileID==0 ){
    // ��x�o�^������AGameSpyId���ύX�ɂȂ��Ă��ς��Ȃ�
    my->profileID = profileID;
  }
}


//----------------------------------------------------------
/**
 * @brief �����̐��ʃZ�b�g
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param sex   �����̐���
 */
//----------------------------------------------------------
void MyStatus_SetMySex(MYSTATUS * my, int sex)
{
  my->sex = sex;
}
//----------------------------------------------------------
/**
 * @brief �����̐��ʎ擾
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @return  sex   �����̐���
 */
//----------------------------------------------------------
u32 MyStatus_GetMySex(const MYSTATUS * my)
{
  return my->sex;
}

//==============================================================================
/**
 * $brief   �������g���[�i�[�Ƃ��Č����鎞�̔ԍ���Ԃ��i���j�I�����[���p�j
 *
 * @param   my    ������ԕێ����[�N�ւ̃|�C���^
 *
 * @retval  u8    �g���[�i�[�Ƃ��Ă̎����̌�����
 */
//==============================================================================
u8 MyStatus_GetTrainerView( const MYSTATUS *my )
{
  return my->trainer_view;
}

//==============================================================================
/**
 * $brief   �������g���[�i�[�Ƃ��Č����鎞�̔ԍ����Z�b�g�i���j�I�����[���p�j
 *
 * @param   my    ������ԕێ����[�N�ւ̃|�C���^
 * @param   view  �g���[�i�[�����ڔԍ�
 *
 * @retval  none
 */
//==============================================================================
void MyStatus_SetTrainerView( MYSTATUS *my, u8 view )
{
  GF_ASSERT( view<MYSTATUS_UNIONVIEW_MAX*2 ); // �j���W����

  my->trainer_view = view;
}


//==============================================================================
/**
 * $brief   �q�n�l�o�[�W�����R�[�h��Ԃ�
 *
 * @param   my    ������ԕێ����[�N�ւ̃|�C���^
 *
 * @retval  u8    0:�_�C�� 1:�p�[��
 */
//==============================================================================
u8  MyStatus_GetRomCode( const MYSTATUS * my )
{
  return my->rom_code;
}

//==============================================================================
/**
 * $brief   �q�n�l�o�[�W�����R�[�h��ݒ肷��
 *
 * @param   my    ������ԕێ����[�N�ւ̃|�C���^
 *
 * @param  rom_code   0:�_�C�� 1:�p�[��
 */
//==============================================================================
void  MyStatus_SetRomCode( MYSTATUS * my, u8 rom_code )
{
  my->rom_code = rom_code;
}


//==============================================================================
/**
 * $brief   ���[�W�����R�[�h��Ԃ�
 *
 * @param   my    ������ԕێ����[�N�ւ̃|�C���^
 *
 * @retval  u8    ���[�W�����R�[�h
 */
//==============================================================================
u8  MyStatus_GetRegionCode( const MYSTATUS * my )
{
  return my->region_code;
}

//==============================================================================
/**
 * $brief   ���[�W�����R�[�h��ݒ肷��
 *
 * @param   my    ������ԕێ����[�N�ւ̃|�C���^
 *
 * @param  region_code
 */
//==============================================================================
void  MyStatus_SetRegionCode( MYSTATUS * my, u8 region_code )
{
  my->region_code = region_code;
}

//----------------------------------------------------------
/**
 * @brief �����̍��R�[�h���擾
 * @param my      ������ԕێ����[�N�ւ̃|�C���^
 * @return  int     ���w��R�[�h
 */
//----------------------------------------------------------
int MyStatus_GetMyNation(const MYSTATUS * my)
{
  return my->nation;
}

//----------------------------------------------------------
/**
 * @brief �����̒n��R�[�h���擾
 * @param my      ������ԕێ����[�N�ւ̃|�C���^
 * @return  int     �n��w��R�[�h
 */
//----------------------------------------------------------
int MyStatus_GetMyArea(const MYSTATUS * my)
{
  return my->area;
}

//----------------------------------------------------------
/**
 * @brief �����̒n��R�[�h��ݒ�
 * @param my      ������ԕێ����[�N�ւ̃|�C���^
 * @param nation  
 * @param area    
 */
//----------------------------------------------------------
void MyStatus_SetMyNationArea(MYSTATUS * my, int nation, int area)
{
  GF_ASSERT(nation < WIFI_NATION_MAX);
  GF_ASSERT(area < WIFI_AREA_MAX);

  my->nation = nation;
  my->area = area;
}



//----------------------------------------------------------
/**
 * @brief myStatus���m���������̂��ǂ����𒲂ׂ�
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @param target  ����̎�����ԕێ����[�N�ւ̃|�C���^
 * @return  ��v������TRUE
 */
//----------------------------------------------------------
BOOL MyStatus_Compare(const MYSTATUS * my, const MYSTATUS * target)
{
  if(0 == GFL_STD_MemComp(my->name, target->name, PERSON_NAME_SIZE)){
        if(my->id == target->id){
            return TRUE;
        }
    }
    return FALSE;
}



//============================================================================================
//  �f�o�b�O�p
//============================================================================================
#ifdef PM_DEBUG
#include "arc_def.h"
#include "debug_message.naix"
#include "msg\debug\msg_d_matsu.h"
//--------------------------------------------------------------
/**
 * @brief   �_�~�[�̖��O���Z�b�g����
 *
 * @param   mystatus    �����
 * @param   heap_id     �e���|�����q�[�v
 */
//--------------------------------------------------------------
void DEBUG_MyStatus_DummyNameSet(MYSTATUS *mystatus, HEAPID heap_id)
{
  GFL_MSGDATA *mm;
  STRBUF *buf;

  mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_matsu_dat, heap_id );

  buf = GFL_MSG_CreateString( mm, DM_MSG_DUMMY_NAME );
  MyStatus_SetMyNameFromString(mystatus, buf);
  GFL_STR_DeleteBuffer(buf);

  GFL_MSG_Delete( mm );
}
#endif  //PM_DEBUG

// �O���Q�ƃC���f�b�N�X����鎞�̂ݗL��(�Q�[�����͖���)
#ifdef CREATE_INDEX
void *Index_Get_Mystatus_Name_Offset(MYSTATUS *my){ return &my->name; }
void *Index_Get_Mystatus_Id_Offset(MYSTATUS *my){ return &my->id; }
void *Index_Get_Mystatus_Sex_Offset(MYSTATUS *my){ return &my->sex; }
#endif

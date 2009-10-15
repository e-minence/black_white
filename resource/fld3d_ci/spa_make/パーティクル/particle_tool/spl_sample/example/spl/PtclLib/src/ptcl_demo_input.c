/*---------------------------------------------
  Utility program codes for particle demo .
  system.
 
 $Log: ptcl_demo_input.c,v $
 Revision 1.2  2004/12/27 04:32:20  okane_shinji
 (none)

 Revision 1.1  2004/09/15 01:23:09  okane_shinji
 �����o�^

 
 *---------------------------------------------*/

#include<nnsys.h>
#include"ptcl_demo.h"

// �L�[����
static u16      ptcl_demo_input_Cont, ptcl_demo_input_Trg;

/*-----------------------------------------------------------------
  Name       : PtclDemo_Key( int key_ )
  Description: �����Ŏw�肳��Ă����L�[��������Ă��邩�ǂ����𔻒肵�܂��B
  Returns    : �L�[��������Ă����TRUE�B�����łȂ����FALSE
 *-----------------------------------------------------------------*/
BOOL PtclDemo_Key( int key_ )
{
    if( ptcl_demo_input_Cont & key_ ){
        return TRUE;
    }
    return FALSE;
}

/*-----------------------------------------------------------------
  Name       : PtclDemo_KeyOneshot( int key_ )
  Description: �����Ŏw�肳��Ă����L�[�����܉����ꂽ���ǂ����𔻒肵�܂��B
  Returns    : �L�[��������Ă����TRUE�B�����łȂ����FALSE
 *-----------------------------------------------------------------*/
BOOL PtclDemo_KeyOneshot( int key_ )
{
    if( ptcl_demo_input_Trg & key_ ){
        return TRUE;
    }
    return FALSE;
}

/*-----------------------------------------------------------------
  Name       : PtclDemo_UpdateInput( )
  Description: ���͂��X�V�B���t���[���Ɉ��Ăт������K�v������܂��B
  Returns    : �Ȃ�
 *-----------------------------------------------------------------*/
void PtclDemo_UpdateInput(void)
{
    u16 ReadData = PAD_Read();
    ptcl_demo_input_Trg  = (u16)(ReadData & (ReadData ^ ptcl_demo_input_Cont)); // �g���K ����
    ptcl_demo_input_Cont = ReadData;                                    //   �x�^ ����
}






// end of file
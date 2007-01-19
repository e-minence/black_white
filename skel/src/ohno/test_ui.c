//============================================================================================
/**
 * @file	test_ui.c
 * @brief	�t�h����e�X�g�p�֐�
 * @author	ohno
 * @date	2006.11.29
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "test_ui.h"


//------------------------------------------------------------------
// �L�[�R���t�B�O�̃e�X�g
//------------------------------------------------------------------

static void _keyConfigTest(void)
{
    GFL_UI_KEY_CUSTOM_TBL key_data[] ={
        //0�Ԗڂ̃R���t�B�O
        {GFL_UI_KEY_END,0,0},		// �I���f�[�^
        //1�Ԗڂ̃R���t�B�O
        {PAD_BUTTON_SELECT,PAD_BUTTON_START,GFL_UI_KEY_CHANGE},		//START SELECT ����
        {PAD_BUTTON_L,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//L��A������
        {PAD_BUTTON_R,0,GFL_UI_KEY_RESET},		//R������
        {GFL_UI_KEY_END,0,0},		// �I���f�[�^
        //2�Ԗڂ̃R���t�B�O
        {PAD_BUTTON_L,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//L��A������
        {PAD_BUTTON_R,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//R��A������
        {PAD_BUTTON_B,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//B��A������
        {GFL_UI_KEY_END,0,0},		// �I���f�[�^
    };

#if 0
	if (GFL_UI_KeyGetTrg() & PAD_BUTTON_X) {

        GFL_UI_KeySetControlModeTbl(key_data);
        GFL_UI_KeySetControlMode(1);
        OS_TPrintf("keyconfig %d change\n",GFL_UI_KeyGetControlMode());
    }
#endif

}

//------------------------------------------------------------------
// �^�b�`�p�l���̃e�X�g
//------------------------------------------------------------------

static void _touchTableTest(void)
{
    int no;
    GFL_UI_TP_HITTBL tp_data[] ={
        {10,33,128,255},  //�E��
        {86,192,0,128},  //����
        {GFL_UI_TP_USE_CIRCLE, 128, 86, 33 },	 //�~�`�^��
        {GFL_UI_TP_HIT_END,0,0,0},		 //�I���f�[�^
        };

    no = GFL_UI_TouchPanelHitTrg(tp_data);
    if(no != GFL_UI_TP_HIT_NONE){
        u32 x=0,y=0;
        GFL_UI_TouchPanelGetPointCont(&x,&y);
        OS_TPrintf("no %d %d %d\n",no,x,y);
    }

}

//------------------------------------------------------------------
// �t�h�̃e�X�g
//------------------------------------------------------------------

void TEST_UI_Main(void)
{
	if (GFL_UI_KeyGetTrg() & PAD_BUTTON_A) {
        OS_TPrintf("a button\n");
	}
    else if (GFL_UI_KeyGetTrg() & PAD_BUTTON_B){
        OS_TPrintf("b button\n");
	}
    else if (GFL_UI_KeyGetTrg() & PAD_BUTTON_R){
        OS_TPrintf("r button\n");
	}
    else if (GFL_UI_KeyGetTrg() & PAD_BUTTON_L){
        OS_TPrintf("l button\n");
	}
    else if (GFL_UI_KeyGetTrg() & PAD_BUTTON_START){
        OS_TPrintf("start button\n");
	}
    else if (GFL_UI_KeyGetTrg() & PAD_BUTTON_SELECT){
        OS_TPrintf("sel button\n");
	}
    {
        u32 x,y;
        if(GFL_UI_TouchPanelGetPointTrg(&x,&y)){
            OS_TPrintf("tx ty %d %d\n", x, y);
        }
    }

    _touchTableTest();
    _keyConfigTest();

}


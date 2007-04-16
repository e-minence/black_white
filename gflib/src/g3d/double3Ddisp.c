//============================================================================================
/**
 *
 * @file	double3Ddisplay.c
 * @brief	���ʂR�c
 *
 */
//============================================================================================
#include "gflib.h"
#include "double3Ddisp.h"

/*
============================================================================================


		���ݎ��p�i�K�ł��B
		�f�B�X�v���C�͊O���ŗ��ʂn�m�ɂ��Ă��������B������܂������ǁB

		��{��NitroSDK�̃T���v���̂܂�
		�T�u���(��R�c���)�̃L���v�`���[�f�[�^�؂�ւ��p��
		�a�f�C�n�a�i���\�[�X���t���Ɏg���Ă���̂��l�b�N��
		���̂܂܂a�f��n�a�i�Ƌ����ł���̂��͋^��

		�T�uVRAM�̓L���v�`���[�̂��߂�
		�a�f�C�n�a�i�Ƃ���256�r�b�g�}�b�v�Ƃ���128k��K�v�Ƃ���B
		�Ȃ̂Ō��󂻂�𖞂����ɂ́AVRAM_C,VRAM_D���Œ�Ŏg�p����邱�ƂɂȂ�B
		VRAM_D���g�p����̂ŁA�ʐM�Ƃ̗����͕s�\


============================================================================================
*/
typedef struct {
	HEAPID		heapID;

	BOOL		flip_flag;	// flip�؂�ւ��t���O
	BOOL		swap;		// SwapBuffers���s�t���O

	GXOamAttr	sOamBak[128];
//	TCB*		vIntrTask;

}GFL_DOUBLE3D;

GFL_DOUBLE3D* dbl3D = NULL;

static void setupSubOAM(void);
//------------------------------------------------------------------
/**
 * @brief	������
 *
 * �L���v�`���[�f�[�^�\���n�a�i��ݒ�
 *
 * @param	heapID	�q�[�v�h�c
 */
//------------------------------------------------------------------
void 
	GFL_G3D_DOUBLE3D_Init
		( HEAPID heapID )
{
	GF_ASSERT( dbl3D == NULL );

	dbl3D = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_DOUBLE3D) );
	dbl3D->heapID = heapID;
	dbl3D->flip_flag = TRUE;	// flip�؂�ւ��t���O
	dbl3D->swap = FALSE;		// SwapBuffers���s�t���O
	setupSubOAM();
//
//	//VBLANK�^�X�N�ݒ�
//	dbl3D->vIntrTask = GFUser_VIntr_CreateTCB( GFL_G3D_DOUBLE3D_VblankIntr, NULL, 0 );
}

//------------------------------------------------------------------
//�n�`�l�g�p�ݒ�
static void setupSubOAM(void)
{
    int     i;
    int     x, y;
    int     idx = 0;

    GXS_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_2D_W256);

    for (i = 0; i < 128; ++i){
        dbl3D->sOamBak[i].attr01 = 0;
        dbl3D->sOamBak[i].attr23 = 0;
    }
    for (y = 0; y < 192; y += 64){
        for ( x = 0; x < 256; x += 64, idx++ ){
            G2_SetOBJAttr(	&dbl3D->sOamBak[idx], x, y, 0, 
							GX_OAM_MODE_BITMAPOBJ, FALSE, GX_OAM_EFFECT_NONE,
							GX_OAM_SHAPE_64x64, GX_OAM_COLOR_16, (y / 8) * 32 + (x / 8), 15, 0);
        }
    }
    DC_FlushRange( &dbl3D->sOamBak[0], sizeof(dbl3D->sOamBak) );
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    GXS_LoadOAM( &dbl3D->sOamBak[0], 0, sizeof(dbl3D->sOamBak) );

}

//------------------------------------------------------------------
/**
 * @brief	�I��
 */
//------------------------------------------------------------------
void 
	GFL_G3D_DOUBLE3D_Exit
		( void )
{
//	GFL_TCB_DeleteTask( dbl3D->vIntrTask );
	GFL_HEAP_FreeMemory( dbl3D );
	dbl3D = NULL;
}


/*
============================================================================================


		�`��̍ہA�ȉ��̊֐���p���āB�e��؂�ւ��ݒ���s���Ă��������B
		
		GFL_G3D_DOUBLE3D_GetFlip
			���݂�flip���擾���邱�Ƃɂ��A�㉺�ɑ�������`���؂�ւ��܂��B
			�i�J�������j

		GFL_G3D_DOUBLE3D_SetSwapFlag
			SwapBuffer�I����ɐݒ肵�Ă�������

		�i��j
		static void g3d_draw( void )
		{
			if( flip_flag ){
				GFL_G3D_CAMERA_Switching( g3DcameraLower );
			} else {
				GFL_G3D_CAMERA_Switching( g3DcameraUpper );
			}
			GFL_G3D_SCENE_Draw( g3Dscene );  
			swap = TRUE;
		}


============================================================================================
*/
//------------------------------------------------------------------
/**
 * @brief	flip�ʃt���O�擾
 *
 * @return	TRUE = �����R�c, FALSE = �オ�R�c
 */
//------------------------------------------------------------------
BOOL
	GFL_G3D_DOUBLE3D_GetFlip
		( void )
{
	return	dbl3D->flip_flag;
}

//------------------------------------------------------------------
/**
 * @brief	swap�I���t���O�ݒ�
 */
//------------------------------------------------------------------
void 
	GFL_G3D_DOUBLE3D_SetSwapFlag
		( void )
{
	dbl3D->swap = TRUE;
}


/*
============================================================================================


		��ʂ̐؂�ւ���VBLANK���Ɏ��s���܂��B
		
		GFL_G3D_DOUBLE3D_VblankIntr( ���ڌďo���p )
		GFL_G3D_DOUBLE3D_VblankIntrTask( TCB�p )

		�̂����ꂩ���Z�b�g�A�b�v���Ă��������B


============================================================================================
*/
static void setupFrame2N( void );
static void setupFrame2N_1( void );
//------------------------------------------------------------------
/**
 * @brief	VBLANK���荞�݊֐�
 */
//------------------------------------------------------------------
#define SPIN_WAIT                      // OS_SpinWait�֐���p������@�Ɨp���Ȃ����@��؂�ւ��܂��B

void
	GFL_G3D_DOUBLE3D_VblankIntr
		( void )
{
    // �摜�̕`����o�b�t�@�̃X���b�v�������������Ă��邱�Ƃ��m�F���܂��B
#ifdef SPIN_WAIT
    if (GX_GetVCount() <= 193) {
        OS_SpinWait(784);
    }
    if (!G3X_IsGeometryBusy() && dbl3D->swap)
#else
    if ((G3X_GetVtxListRamCount() == 0) && dbl3D->swap)
#endif
        // �摜�̕`����X���b�v�������������Ă���ꍇ�A�㉺��ʂ̐؂�ւ����s���܂��B
    {
        if ( dbl3D->flip_flag ){                 // flip�̐؂�ւ�(�㉺��ʂ̐؂�ւ�����)
            setupFrame2N_1();
        } else {
            setupFrame2N();
        }
        dbl3D->swap = FALSE;
        dbl3D->flip_flag = !dbl3D->flip_flag;
    }
}

//------------------------------------------------------------------
//TCB�p
#define SPIN_WAIT                      // OS_SpinWait�֐���p������@�Ɨp���Ȃ����@��؂�ւ��܂��B

void	
	GFL_G3D_DOUBLE3D_VblankIntrTCB
		( TCB* tcb, void* work )
{
	GFL_G3D_DOUBLE3D_VblankIntr();
}

//------------------------------------------------------------------
//���ʔ��f�ݒ�
static void setupFrame2N( void )
{

    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

    (void)GX_ResetBankForSubOBJ();
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForLCDC(GX_VRAM_LCDC_D);
    GX_SetCapture(GX_CAPTURE_SIZE_256x192,
                  GX_CAPTURE_MODE_A,
                  GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    //GX_SetVisiblePlane(GX_GetVisiblePlane() | GX_PLANEMASK_BG0);
    //G2_SetBG0Priority(0);

    GXS_SetGraphicsMode(GX_BGMODE_5);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG2);
    G2S_SetBG2ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256,
                           GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
    //G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(FALSE);
}

//------------------------------------------------------------------
//����ʔ��f�ݒ�
static void setupFrame2N_1( void )
{
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    (void)GX_ResetBankForSubBG();
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForLCDC(GX_VRAM_LCDC_C);
    GX_SetCapture(GX_CAPTURE_SIZE_256x192,
                  GX_CAPTURE_MODE_A,
                  GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_C_0x00000, 16, 0);

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    //GX_SetVisiblePlane(GX_GetVisiblePlane() | GX_PLANEMASK_BG0);
    //G2_SetBG0Priority(0);

    GXS_SetGraphicsMode(GX_BGMODE_5);
    GXS_SetVisiblePlane(GX_PLANEMASK_OBJ);
}

/*
---------------------------------------------------------------------------------------------------

��SwapBuffers�R�}���h�ƃW�I���g���G���W���̓���̐���

�ESwapBuffers�R�}���h��G3_SwapBuffers�֐����Ă񂾂Ƃ��ɁA�W�I���g���R�}���hFIFO�Ɋi�[����
  SwapBuffers�R�}���h�͂ǂ̃^�C�~���O�ŃW�I���g���R�}���hFIFO����Ă΂ꂽ�Ƃ��Ă��A
  ����V�u�����N�̊J�n���ɁA�o�b�t�@�̃X���b�v���������s����܂��B
  (�܂�AV�u�����N�̊J�n���ɂ������s�ł��܂���)
�E���̂��߁A�����`�揈���Ȃǂ�V�u�����N���ԓ��܂ł��ꍞ��ŁA
  SwapBuffers�R�}���h�̎��s��V�u�����N�̊J�n���܂łɂł��Ȃ������ꍇ�A
  ���̃t���[����V�u�����N�J�n���܂ŃW�I���g���G���W���̓r�W�[�̂܂ܖ�1�t���[���҂�������B
  ���̊Ԃ͉摜�̕`���X���b�v�Ȃǂ͂ł��Ȃ��ł��̃t���[���͑O�Ɠ����摜��\������B

s : SwapBuffers�R�}���h���s
S : �o�b�t�@�̃X���b�v�������s
w : SwapBuffers�R�}���h���s����o�b�t�@�̃X���b�v�������s�J�n�܂ł̃E�F�C�g
G : �W�I���g���G���W�����`��Ȃǂ̏�����

                          |
-VBlank(end)------------------------------------------
               +-----+    |     +-----+
               |     |    |     |     |
               |     |    |     |     |
               |  G  |    |     |  G  |
               |     |    |     |     |
               |     |    |     |     |
               |     |    |     |     |
               |     |    |     |     |
               |     |    |     |     |
               +-----+    |     |     |
               |  s  |    |     |     |
               +-----+    |     |     |
               |  w  |    |     |     |
-VBlank(start)-+-----+----+-----+     +-----------
  ��784(cycle) |  S  |    |     |     |  �����̃X���b�v������CPU�T�C�N����784�T�C�N��������܂�
               +-----+    |     +-----+           
    ��check  ��           |     |  s  |  ������check�̕�����G3X_IsGeometryBusy�֐���p����
                          |     +-----+    �X���b�v�������I��������ǂ����𔻒肵�Ă��܂��B�@�@
-VBlank(end)--------------+-----+     +---------
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |  w  |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
-VBlank(start)------------+-----+-----+----------
                          |     |  S  |
                          |     +-----+
                          |
                          |
-VBlank(end)--------------+-----------------------

����肪�����������
    �EV�u�����N���Ńo�b�t�@�̃X���b�v�������������Ă��Ȃ��̂ɁA�㉺��ʂ̐؂�ւ����s�����ꍇ
      �㉺�œ�����ʂ��\������Ă��܂��܂��B

�����_��h������V�u�����N���ł��ׂ�����
    �EV�u�����N�ɓ���O�܂łɁA���̉摜�̕`�悪�I�����Ă��邱�Ƃ��m�F
      �`�悪�I�����Ă���Ȃ�A����V�u�����N�J�n���Ƀo�b�t�@�̃X���b�v���������s����܂��B
    �EV�u�����N���ŁA�o�b�t�@�̃X���b�v�������I���������Ƃ��m�F
      �X���b�v�������I�����Ă���Ȃ�A�㉺��ʂ̐؂�ւ����s���܂��B

    ���̂��߂�
    �EV�u�����N�ɓ���O�ɁA���̉摜�̕`�悪�I�����Ă��邱��
		(����V�u�����N�ŃX���b�v�������m���Ɏn�܂�)
    �EV�u�����N���ŁA�o�b�t�@�̃X���b�v�������I����������
    �����̂Q���m�F����K�v������܂��B

����̓I��V�u�����N���ōs���Ă��鏈��

        �E if(GX_GetVCount() <= 193)        
					�X���b�v��������ɂ�1���C����������Ȃ��̂�
                    ���̎��_��193���C���ɒB���Ă���΁A
                    �X���b�v�����͏I����Ă���Ɣ��f�ł��A
                    OS_SpinWait�֐���784�T�C�N���҂K�v������܂���B
                                                   
        �E OS_SpinWait(784)      
					784�T�C�N���҂��܂��B
        
        �E if (!G3X_IsGeometryBusy() && swap)
					V�u�����N�O�� SwapBuffers �R�}���h�����s�ς݂ŁA
                    �W�I���g���G���W�����r�W�[�łȂ��ꍇ
                    (�摜�̕`�揈����o�b�t�@�̃X���b�v�������s���łȂ�)
                                                
�������̈Ӗ�
    �o�b�t�@�̃X���b�v����(�����_�����O�G���W�����Q�Ƃ���f�[�^��؂�ւ���)�ɂ́A
    CPU�T�C�N����784(392�~2)�T�C�N���K�v�Ȃ��߁A
    �����AV�u�����N�̊J�n���Ƀo�b�t�@�̃X���b�v�������J�n���ꂽ�̂��Ƃ�����A784�T�C�N���҂Ă�
    �o�b�t�@�̃X���b�v�����͏I����Ă���Ɣ��f�ł��܂��B

    V�u�����N�J�n����784�T�C�N���҂��ăW�I���g���G���W�����r�W�[�ȏꍇ
        V�u�����N�O�̉摜�̕`�揈�����x�ꂽ���߁A�o�b�t�@�̃X���b�v�������܂��I�����Ă��Ȃ��B
        �����̂Ƃ��̓X���b�v�������㉺��ʐ؂�ւ�������ׂ��ł͂���܂���B
    V�u�����N�J�n����784�T�C�N���҂��ăW�I���g���G���W�����r�W�[�łȂ��ꍇ
        �o�b�t�@�̃X���b�v�������摜�̕`�揈�����I�����Ă���B
        ���摜�̕`����X���b�v�������I�����Ă���̂ŁA�㉺��ʂ̐؂�ւ�������ׂ��ł��B

�����AV�u�����N���ł̂���784�T�C�N�����ǂ����Ă����������Ȃ��Ǝv���ꍇ��
���� OS_SpinWait(784) �̑O�ɉ����A784�T�C�N���ȏ�̏��������Ă��������B

�������́A���̕��@�̓|���S����1���`�悵�Ă��Ȃ��Ƃ��͎g���Ȃ����@�Ȃ̂ł����A

void VBlankIntr(void)
{
    if ( (G3X_GetVtxListRamCount() == 0) && swap )
    {
        if (flag)  // flip�̐؂�ւ�
        {
            setupFrame2N_1();
        }
        else
        {
            setupFrame2N();
        }

��̂悤�ɏC�����Ă�����h�����Ƃ��ł��܂��B
���̕��@�̓X���b�v�o�b�t�@������VBlank�J�n���Ɏ��s���ꂽ�Ƃ�
���_RAM�Ɋi�[����Ă��钸�_���� 0 �ɂȂ邱�Ƃ�
G3X_GetVtxListRamCount�֐�(���_RAM�Ɋi�[����Ă��钸�_�̐���Ԃ��֐�) �Ŋm�F���邱�Ƃɂ����
�X���b�v�o�b�t�@���������s���ꂽ�Ƃ�����������Ă��܂��B

���ӁF�����A���ɒ������荞�ݏ�����V�u�����N�̊��荞�ݑO�ɔ������āA
      V�u�����N�̊J�n���x��AV�u�����N�̎��Ԃ����ɒZ��(��3���C���ȉ�)�Ȃ��Ă��܂����ꍇ
      �\�����ɏ㉺��ʂ̐؂�ւ����s��ꂽ��A���̃t���[�������O�̉摜���\������邱�Ƃ�����܂��B

---------------------------------------------------------------------------------------------------
*/



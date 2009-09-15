/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MI - 
  File:     mi_uncompress.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/

#include <nitro/types.h>
#include <nitro/mi/uncompress.h>

//****�o�O�΍�****
//  CW �̃o�O�ŁAldrh �� strh �Ƃ������A�n�[�t���[�h�A�N�Z�X���߂�
//  inline assembler �Œʂ�Ȃ��̂ŁA���ɖ��߂̒l�� dcd �ŏ�����
//  �������B�o�O���Ȃ������牺��define �͍폜����B
//#define CW_BUG_FOR_LDRH_AND_STRH

#define UNCOMPRESS_RL16_CODE32

//---- This code will be compiled in ARM-Mode
#include <nitro/code32.h>

//======================================================================
//          ���k�f�[�^�W�J
//======================================================================
//----------------------------------------------------------------------
//          �a�������k�f�[�^�W�J
//
//�E0�Œ�̃r�b�g���l�߂��f�[�^��W�J���܂��B
//�E�f�X�e�B�l�[�V�����A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//             void *srcp     �\�[�X�A�h���X
//             void *destp    �f�X�e�B�l�[�V�����A�h���X
//  MIUnpackBitsParam *paramp   MIUnpackBitsParam�\���̂̃A�h���X
//
//�EMIUnpackBitsParam�\����
//    u16 srcNum              �\�[�X�f�[�^�E�o�C�g��
//    u8  srcBitNum           �P�\�[�X�f�[�^�E�r�b�g��
//    u8  destBitNum          �P�f�X�e�B�l�[�V�����f�[�^�E�r�b�g��
//    u32 destOffset:31       �\�[�X�f�[�^�ɉ��Z����I�t�Z�b�g��
//        destOffset0_On:1    �O�̃f�[�^�ɃI�t�Z�b�g�����Z���邩�ۂ��̃t���O
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UnpackBits( register const void *srcp, register void *destp, register MIUnpackBitsParam *paramp )
{
                stmfd   sp!, {r4-r11, lr}
                sub     sp, sp, #4

                ldrh    r7, [r2, #0]            // r7:  srcNum    = unPackBitsParamp->srcNum

@00:            ldrb    r6, [r2, #2]            // r6:  srcBitNum = unPackBitsParamp->srcBitNum
                rsb     r10, r6, #8             // r10: srcBitNumInv = 8 - srcBitNum
                mov     r14, #0                 // r14: destBak = 0
                ldr     r11, [r2, #4]           // r8:  destOffset0_On
                mov     r8, r11, lsr #31        //          = unPackBitsParamp->destOffset0_On
                ldr     r11,[r2, #4]            //      destOffset = unPackBitsParamp->destOffset
                mov     r11,r11, lsl #1
                mov     r11,r11, lsr #1
                str     r11,[sp, #0]
                ldrb    r2, [r2, #3]            // r2:  destBitNum = unPackBitsParamp->destBitNum
                mov     r3, #0                  // r3:  destBitCount = 0

@01:            subs    r7, r7, #1              //  while (--srcNum >= 0) {
                blt     @06

                mov     r11, #0xff              // r5:  srcMask = 0xff >> srcBitNumInv;
                mov     r5, r11, asr r10
                ldrb    r9, [r0], #1            // r9:  srcTmp = *srcp++;
                mov     r4, #0                  // r4:  srcBitCount = 0;

@02:            cmp     r4, #8                  //      while (srcBitCount < 8) {
                bge     @01

                and     r11, r9, r5             // r12:     destTmp = ((srcTmp & srcMask) >>srcBitCount);
                movs    r12, r11, lsr r4
                cmpeq   r8,  #0
                beq     @04

@03:            ldr     r11, [sp, #0]           //          destTmp += destOffset;
                add     r12, r12, r11
@04:            orr     r14, r14, r12, lsl r3   //          destBak |= destTmp << destBitCount;
                add     r3, r3, r2              //          destBitCount += destBitNum;

                cmp     r3, #0x20               //          if (destBitCount >= 32) {
                blt     @05

                str     r14, [r1], #4           //              *destp++ = destBak;
                mov     r14, #0                 //              destBak = 0;
                mov     r3,  #0                 //              destBitCount = 0;
                                                //          }
@05:            mov     r5, r5, lsl r6          //          srcMask    <<= srcBitNum;
                add     r4, r4, r6              //          srcBitCount += srcBitNum;
                b       @02                     //      }
                                                //  }
@06:            add     sp, sp, #4
                ldmfd   sp!, {r4-r11, lr}
                bx      lr
}

//----------------------------------------------------------------------
//          �k�y�V�V���k�f�[�^�W�������W�J
//
//�ELZ77���k�f�[�^��W�J���A8bit�P�ʂŏ������݂܂��B
//�EVRAM�ɒ��ړW�J���邱�Ƃ͂ł��܂���B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �\��
//        compType:4          ���k�^�C�v�i = 1�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�t���O�f�[�^�t�H�[�}�b�g
//    u8  flags               ���k�^�����k�t���O
//                            �i0, 1�j = �i�����k�f�[�^, ���k�f�[�^�j
//�E�R�[�h�f�[�^�t�H�[�}�b�g�iBig Endian�j
//    u16 length:4            �W�J�f�[�^�� - 3�i��v��3Byte�ȏ㎞�݈̂��k�j
//        offset:12           ��v�f�[�^�I�t�Z�b�g - 1
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UncompressLZ8( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4-r7, lr}
                
                ldr     r5, [r0], #4            // r2:  destCount = *(u32 *)srcp >> 8
                mov     r2, r5, lsr #8          // r0:  srcp += 4
                mov     r7, #0
                tst     r5, #0x0F               // r7:  isExFormat = (*header & 0x0F)? 1 : 0
                movne   r7, #1

@21:            cmp     r2, #0                  //  while (destCount > 0) {
                ble     @26

                ldrb    r14, [r0], #1           // r14: flags = *srcp++
                mov     r4, #8                  //      
@22:            subs    r4, r4, #1              //      for (i = 8; --i >= 0; ) {
                blt     @21

                tst     r14, #0x80              //          if (!(flags & 0x80)) {
                bne     @23

                ldrb    r6, [r0], #1            //              *srcp++;
                swpb    r6, r6, [r1]            // r1:          *destp++;�i�o�C�g�������ݑ΍�j
                add     r1, r1, #1
                sub     r2, r2, #1              //              destCount--;
                b       @25
                                                //          } else {
@23:            ldrb    r5, [r0, #0]            // r3:          length = (*srcp >> 4);
                cmp     r7, #0                  //              if ( ! isExFormat ) { length += 3; }
                moveq   r6, #3
                beq     @23_2
                                                //              else {
                tst     r5, #0xE0               //                  if ( length > 1 ) {
                movne   r6, #1                  //                      length += 1;
                bne     @23_2                   //                  } else {
                
                add     r0, r0, #1              //                      isWide = (length == 1)? 1 : 0;
                and     r6, r5, #0xf            //                      length = (*srcp++ & 0x0F) << 4
                mov     r6, r6, lsl #4
                tst     r5, #0x10
                beq     @23_1                   //                      if ( isWide ) {
                
                mov     r6, r6, lsl #8          //                          length <<= 8;
                ldrb    r5, [r0], #1            //                          length += (*srcp++) << 4;
                add     r6, r6, r5, lsl #4      //                          length += 0xFF + 1;
                add     r6, r6, #0x100          //                      }
@23_1:
                add     r6, r6, #0x11           //                      length += 0xF + 2;
                ldrb    r5, [r0, #0]            //                      length += (*srcp >> 4);
@23_2:                                          //                  }
                add     r3, r6, r5, asr #4      //              }
                add     r0, r0, #1              // r12:         offset = (*srcp++ & 0x0f) << 8;
                and     r5, r5, #0xf
                mov     r12,r5, lsl #8
                ldrb    r6, [r0], #1            //              offset = (offset | *srcp++) + 1;
                orr     r5, r6, r12
                add     r12,r5, #1
                sub     r2, r2, r3              //              destCount -= length;
                                                //              do {
@24:            ldrb    r5, [r1, -r12]          //                  *destp++ = destp[-offset]
                swpb    r5, r5, [r1]            //    �i�o�C�g�������ݑ΍�j
                add     r1, r1, #1
                subs    r3, r3, #1              //              } while (--length > 0);
                bgt     @24
                                                //          }
@25:            cmp     r2, #0                  //          if (destCount <= 0)   break;
                movgt   r14, r14, lsl #1        //          flags <<= 1
                bgt     @22                     //      }
                b       @21                     //  }

@26:            ldmfd   sp!, {r4-r7, lr}
                bx      lr
}


//----------------------------------------------------------------------
//          �k�y�V�V���k�f�[�^�P�U�������W�J
//
//�ELZ77���k�f�[�^��W�J���A16bit�P�ʂŏ������݂܂��B
//�E�f�[�^TCM�⃁�C���������ɂ��W�J�ł��܂����AMI_UncompressLZ77()
//  ���ᑬ�ł��B
//�E���k�f�[�^�͈�v�������2Byte�ȑO��茟���������̂ɂ��ĉ������B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �\��
//        compType:4          ���k�^�C�v�i = 1�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�t���O�f�[�^�t�H�[�}�b�g
//    u8  flags               ���k�^�����k�t���O
//                            �i0, 1�j = �i�����k�f�[�^, ���k�f�[�^�j
//�E�R�[�h�f�[�^�t�H�[�}�b�g�iBig Endian�j
//    u16 length:4            �W�J�f�[�^�� - 3�i��v��3Byte�ȏ㎞�݈̂��k�j
//        offset:12           ��v�f�[�^�I�t�Z�b�g�i >= 2�j - 1
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UncompressLZ16( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4-r11, lr}

                mov     r3,  #0                 // r3:  destTmp = 0
                ldr     r8,  [r0], #4           // r10: destCount = *(u32 *)srcp >> 8
                mov     r10, r8, lsr #8         // r0:  srcp += 4
                mov     r2,  #0                 // r2:  shift = 0
                mov     r11, #0
                tst     r8,  #0x0F              // r11: isExFormat = (*header & 0x0F)? 1 : 0;
                movne   r11, #1
                
@31:            cmp     r10, #0                 //  while (destCount > 0) {
                ble     @36

                ldrb    r6, [r0], #1            // r6:  flags = *srcp++;
                mov     r7, #8                  //      for ( i = 8; --i >= 0; ) {
@32:            subs    r7, r7, #1
                blt     @31

                tst     r6, #0x80               //          if (!(flags & 0x80)) {
                bne     @33

                ldrb    r9, [r0], #1            //              destTmp |= *srcp++ << shift;
                orr     r3, r3, r9, lsl r2
                sub     r10, r10, #1            //              destCount--;

                eors    r2, r2, #8              //              if (!(shift ^= 8)) {
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                streqh  r3, [r1], #2            //              *destp++ = destTmp;
#else
                dcd     0x00c130b2
#endif
                moveq   r3, #0                  //              destTmp = 0;
                b       @35                     //          } else {

@33:            ldrb    r9, [r0, #0]            // r5:          length = (*srcp >> 4) + 3;
                cmp     r11, #0                 //              if ( ! isExFormat ) { length += 3; }
                moveq   r8,  #3
                beq     @33_2
                                                //              else {
                tst     r9, #0xE0               //                  if ( length > 1 ) {
                movne   r8, #1                  //                      length += 1
                bne     @33_2                   //                  } else {
                
                add     r0, r0, #1              //                      isWide = (length == 1)? 1 : 0;
                and     r8, r9, #0xf            //                      length = (*srcp++ & 0x0F) << 4
                mov     r8, r8, lsl #4
                tst     r9, #0x10
                beq     @33_1                   //                      if ( isWide ) {
                
                mov     r8, r8, lsl #8          //                          length <<= 8;
                ldrb    r9, [r0], #1            //                          length += (*srcp++) << 4
                add     r8, r8, r9, lsl #4      //                          length += 0xFF + 1
                add     r8, r8, #0x100          //                      }
@33_1:
                add     r8, r8, #0x11           //                      length += 0xF + 2;
                ldrb    r9, [r0, #0]            //                      length += (*srcp >> 4);
@33_2:                                          //                  }
                add     r5, r8, r9, asr #4      //              }
                ldrb    r9, [r0], #1            // r4:          offset = (*srcp++ & 0x0f) << 8;
                and     r8, r9, #0xf
                mov     r4, r8, lsl #8
                ldrb    r9, [r0], #1            //              offset = (offset | *srcp++) + 1;
                orr     r8, r9, r4
                add     r4, r8, #1
                rsb     r8, r2, #8              // r14:         offset0_8 = (8 - shift)
                and     r9, r4, #1              //                          ^ ((offset & 1) << 3);
                eor     r14, r8, r9, lsl #3
                sub     r10, r10, r5            //              destCount -= length;
                                                //              do {
@34:            eor     r14, r14, #8            //                  offset0_8 ^= 8;
                rsb     r8, r2, #8              //                  destTmp |= (destp[-((offset
                add     r8, r4, r8, lsr #3      //                              + ((8 - shift) >> 3)) >> 1)];
                mov     r8, r8, lsr #1
                mov     r8, r8, lsl #1
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                ldrh    r9, [r1, -r8]
#else
                dcd     0xe11190b8
#endif
                mov     r8, #0xff
                and     r8, r9, r8, lsl r14
                mov     r8, r8, asr r14
                orr     r3, r3, r8, lsl r2
                eors    r2, r2, #8              //                  if (!(shift ^= 8)) {
#ifndef CW_BUG_FOR_LDRH_AND_STRH
                streqh  r3, [r1], #2            //                      *destp++ = destTmp;
#else
                dcd     0x00c130b2
#endif
                moveq   r3, #0                  //                      destTmp = 0;
                                                //                  }
                subs    r5, r5, #1              //              } while (--length > 0);
                bgt     @34                     //          }

@35:            cmp     r10, #0                 //          if (destCount <= 0)   break;
                movgt   r6, r6, lsl #1          //          flags <<= 1
                bgt     @32                     //      }
                b       @31                     //  }

@36:            ldmfd   sp!, {r4-r11, lr}
                bx      lr
}

//----------------------------------------------------------------------
//          �n�t�}�����k�f�[�^�W�J
//
//�E�n�t�}�����k�f�[�^��W�J���A32bit�P�ʂŏ������݂܂��B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 bitSize:4           �P�f�[�^�E�r�b�g�T�C�Y�i�ʏ� 4|8�j
//        compType:4          ���k�^�C�v�i = 2�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�c���[�e�[�u��
//    u8           treeSize        �c���[�e�[�u���T�C�Y/2 - 1
//    TreeNodeData nodeRoot        ���[�g�m�[�h
//
//    TreeNodeData nodeLeft        ���[�g���m�[�h
//    TreeNodeData nodeRight       ���[�g�E�m�[�h
//
//    TreeNodeData nodeLeftLeft    �����m�[�h
//    TreeNodeData nodeLeftRight   ���E�m�[�h
//
//    TreeNodeData nodeRightLeft   �E���m�[�h
//    TreeNodeData nodeRightRight  �E�E�m�[�h
//
//            �E
//            �E
//
//  ���̌�Ɉ��k�f�[�^�{��
//
//�ETreeNodeData�\����
//    u8  nodeNextOffset:6    ���m�[�h�f�[�^�ւ̃I�t�Z�b�g - 1�i2Byte�P�ʁj
//        rightEndFlag:1      �E�m�[�h�I���t���O
//        leftEndzflag:1       ���m�[�h�I���t���O
//                            �I���t���O���Z�b�g����Ă���ꍇ
//                            ���m�[�h�Ƀf�[�^������
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UncompressHuffman( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4-r11, lr}
                sub     sp, sp, #4

                add     r2, r0, #4              // r2:  *treep = (u8 *)srcp + 4
                add     r7, r2, #1              // r7:  *treeStartp = treep + 1
                ldrb    r10,[r0, #0]            // r4:  DataBit = *(u8 *)srcp & 0x0f
                and     r4, r10, #0xf
                mov     r3, #0                  // r3:  destTmp = 0
                mov     r14,#0                  // r14: destTmpCount = 0
                and     r10,r4,  #7             //      destTmpDataNum = 4 + (DataBit & 0x7)
                add     r11,r10, #4
                str     r11,[sp, #0]
                ldr     r10,[r0, #0]            // r12: destCount = *srcp >> 8
                mov     r12,r10, lsr #8
                ldrb    r10,[r2, #0]            // r0:  srcp  = (u32 *)(treep + ((*treep + 1) << 1))
                add     r10,r10, #1
                add     r0, r2, r10, lsl #1
                mov     r2, r7                  //      treep = treeStartp

@11:            cmp     r12, #0                 //  while (destCount > 0) {
                ble     @14

                mov     r8,  #32                // r8:  srcCount = 32;
                ldr     r5, [r0], #4            // r5:  srcTmp = *srcp++;

@12:            subs    r8, r8,  #1             //      while (--srcCount >= 0) {
                blt     @11

                mov     r10,#1                  // r9:      treeShift = (srcTmp >> 31) & 0x1
                and     r9, r10, r5, lsr #31
                ldrb    r6, [r2, #0]            // r6:      treeCheck = *treep
                mov     r6, r6, lsl r9          //          treeCheck <<= treeShift
                mov     r10,r2, lsr #1          //          treep = (u8 *)((((u32 )treep>>1) <<1)
                mov     r10,r10,lsl #1          //                          + (((*treep & 0x3f)+1) <<1)+treeShift)
                ldrb    r11,[r2,#0]
                and     r11,r11,#0x3f
                add     r11,r11,#1
                add     r10,r10,r11,lsl #1
                add     r2, r10,r9

                tst     r6, #0x80               //          if (treeCheck & TREE_END) {
                beq     @13

                mov     r3, r3, lsr r4          //              destTmp >>= DataBit;
                ldrb    r10,[r2, #0]            //              destTmp |= *treep << (32 - DataBit);
                rsb     r11, r4,  #32
                orr     r3, r3, r10, lsl r11
                mov     r2, r7                  //              treep = treeStartp;
                add     r14,r14,#1              //              if (++destTmpCount == destTmpDataNum) {
                ldr     r11,[sp, #0]
                cmp     r14,r11

                streq   r3, [r1], #4            //                  *destp++ = destTmp;
                subeq   r12,r12,  #4            //                  destCount -= 4;
                moveq   r14,#0                  //                  destTmpCount = 0;
                                                //              }
                                                //          }
@13:            cmp     r12,#0                  //          if (destCount <= 0)   break;
                movgt   r5, r5, lsl #1          //          srcTmp <<= 1;
                bgt     @12                     //      }
                b       @11                     //  }

@14:            add     sp, sp, #4
                ldmfd   sp!, {r4-r11, lr}
                bx      lr
}

//----------------------------------------------------------------------
//          ���������O�X���k�f�[�^�W�������W�J
//
//�E���������O�X���k�f�[�^��W�J���A8bit�P�ʂŏ������݂܂��B
//�ENITRO�̏ꍇVRAM�ɒ��ړW�J���邱�Ƃ͂ł��܂���B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �\��
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�t���O�f�[�^�t�H�[�}�b�g
//    u8  length:7            �W�J�f�[�^�� - 1�i�����k���j
//                            �W�J�f�[�^�� - 3�i�A����3Byte�ȏ㎞�݈̂��k�j
//        flag:1              �i0, 1�j = �i�����k�f�[�^, ���k�f�[�^�j
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UncompressRL8( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4, r5, r7}

                ldmia   r0!, {r3}               // r7:  destCount = *(u32 *)srcp >> 8;
                mov     r7, r3, lsr #8          // r0:  srcp += 4;

@41:            cmp     r7, #0                  //  while (destCount > 0) {
                ble     @45

                ldrb    r4, [r0], #1            // r4:  flags = *srcp++;
                ands    r2, r4, #0x7f           // r2:  length = flags & 0x7f;
                tst     r4, #0x80               //      if (!(flags & 0x80)) {
                bne     @43

                add     r2, r2, #1              //          length++;
                sub     r7, r7, r2              //          destCount -= length;
@42:            ldrb    r3, [r0], #1            //          do{
                swpb    r3, r3, [r1]            //              *destp++ = *srcp++;�i�o�C�g�������ݑ΍�j
                add     r1, r1, #1
                subs    r2, r2, #1              //          } while (--length > 0);
                bgt     @42                     //      } else {
                b       @41

@43:            add     r2, r2, #3              //          length += 3;
                sub     r7, r7, r2              //          destCount -= length;
                ldrb    r5, [r0], #1            //          srcTmp  = *srcp++;
@44:            swpb    r4, r5, [r1]            //          do {�i�o�C�g�������ݑ΍�j
                add     r1, r1, #1              //              *destp++ =  srcTmp;
                subs    r2, r2, #1              //          } while (--length > 0);
                bgt     @44                     //      }
                b       @41                     //  }

@45:            ldmfd   sp!, {r4, r5, r7}
                bx      lr
}

//----------------------------------------------------------------------
//          ���������O�X���k�f�[�^�P�U�������W�J
//
//�E���������O�X���k�f�[�^��W�J���A16bit�P�ʂŏ������݂܂��B
//�E�f�[�^TCM��VRAM�ɂ��W�J�ł��܂�
//�E���C��RAM�֓W�J����ꍇ�AMI_UncompressRL8() ���ᑬ�ł��B
//�E�f�[�^TCM��VRAM�֓W�J����ꍇ��MI_UncompressRL8() ��荂���ł��B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �\��
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�t���O�f�[�^�t�H�[�}�b�g
//    u8  length:7            �W�J�f�[�^�� - 1�i�����k���j
//                            �W�J�f�[�^�� - 3�i�A����3Byte�ȏ㎞�݈̂��k�j
//        flag:1              �i0, 1�j = �i�����k�f�[�^, ���k�f�[�^�j
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

#if !defined(UNCOMPRESS_RL16_CODE32)
//---- This code will be compiled in Thumb-Mode
#include <nitro/code16.h>

asm void MI_UncompressRL16( register const void *srcp, register void *destp )
{
                push    {r4-r7}
                sub     sp, #0xc

                mov     r7, #0                  // r7:  destTmp = 0
                ldmia   r0!, {r3}               // r5:  destCount = *(u32 *)srcp >> 8
                lsr     r5, r3, #8              // r0:  srcp += 4

                mov     r4, #0                  // r4:  shift = 0
@51:            cmp     r5, #0                  //  while (destCount > 0) {
                ble     @57

                ldrb    r3, [r0,  #0]           //      flags = *srcp++;
                str     r3, [sp, #4]
                add     r0, #1
                ldr     r3, [sp, #4]            // r2:  length = flags & 0x7f;
                lsl     r2, r3, #25
                lsr     r2, r2, #25

                ldr     r6, [sp, #4]            //      if (!(flags & 0x80)) {
                lsr     r3, r6, #8
                bcs     @54

                add     r2, #1                  //          length++;
                sub     r5, r5, r2              //          destCount -= length;
                                                //          do {
@52:            ldrb    r6, [r0, #0]            //              destTmp |= *srcp++ << shift;
                lsl     r6, r4
                orr     r7, r6
                add     r0, #1
                mov     r3, #8                  //              if (!(shift ^= 8)) {
                eor     r4, r3
                bne     @53
                strh    r7, [r1, #0]            //                  *destp++ = destTmp;
                add     r1, #2
                mov     r7, #0                  //                  destTmp = 0;
                                                //              }
@53:            sub     r2, r2, #1              //          } while (--length > 0);
                bgt     @52
                b       @51                     //      } else {

@54:            add     r2, #3                  //          length += 3;
                sub     r5, r5, r2              //          destCount -= length;
                ldrb    r6, [r0, #0]            //          srcTmp  = *srcp++;
                str     r6, [sp, #8]
                add     r0, #1
                                                //          do {
@55:            ldr     r6, [sp, #8]            //              destTmp |= srcTmp  << shift;
                lsl     r6, r4
                orr     r7, r6
                mov     r3, #8                  //              if (!(shift ^= 8)) {
                eor     r4, r3
                bne     @56
                strh    r7, [r1, #0]            //                  *destp++ = destTmp;
                add     r1, #2
                mov     r7, #0                  //                  destTmp = 0;
                                                //              }
@56:            sub     r2, r2, #1              //          } while (--length > 0);
                bgt     @55                     //      }
                b       @51                     //  }

@57:            add     sp, #0xc
                pop     {r4-r7}
                bx      lr
}

//---- This code will be compiled in Thumb-Mode
#include <nitro/code32.h>
#else  // defined(UNCOMPRESS_RL16_CODE32)
asm void MI_UncompressRL16( register const void *srcp, register void *destp )
{
                stmdb   sp!, {r4-r6}

                ldr     r12, [r0], #4           // load header
                mov     r12, r12, lsr #8        // dest. count
                mov     r4,  #0                 // write back accumulator
                mov     r5,  #8                 // low-high byte switch and shifter

@L1             cmp     r12, #0                 // finished ?
                ble     @L4

                ldrb    r2,  [r0], #1           // load marker byte
                ldrb    r3,  [r0], #1           // load char ie. 1st char
                                                // <- still 1 cycle interlock from the 1st LDRB (LDRB/LDRH impose 2 cycles pipeline interlock!)
                tst     r2,  #0x80              // check if marker has MSB set 
                bne     @L3                     // path decoding str&iacute;ngs of different characters 
                add     r2,  r2,   #1           // adjust counter
                sub     r12, r12,  r2           // decrement dest. count 

@L2             eors    r5,  r5, #8             // toggle low-high byte switch and shifter
                orr     r4,  r4, r3, lsl r5     // OR in the next byte as either low or high byte
                strneh  r4,  [r1], #2           // if halfword is full store it away
                movne   r4,  #0                 // clear accu 
                subs    r2,  r2,   #1           // decrement counter
                ldrneb  r3,  [r0], #1           // preload char for the next loop to have the 2 cycle interlock of LDRB compensated by the pipeline flush imposed by the branch 
                bne     @L2
                b       @L1                     // path decoding str&iacute;ngs of same character 

@L3             sub     r2,  r2, #0x7d          // adjust counter
                sub     r12, r12, r2            // decrement dest. count 

                eors    r5,  r5, #8             // toggle low-high byte switch and shifter
                orr     r4,  r4, r3, lsl r5     // OR in the next byte as either low or high byte 
                strneh  r4,  [r1], #2           // if halfword is full store it away
                movne   r4,  #0                 // and clear accu

                orr     r6,  r3, r3, lsl #8     // the char extended to halfword

@L5             subs    r2,  r2, #2             // sub 2, to handle the repeatitive char loop halfword-wise 
                ble     @L6                     // done or only one more to go, step out to keep the flow for multiple chars as fast as possible
                strh    r6,  [r1], #2           // more than one to go -> just store the double char
                b       @L5

@L6             blt     @L1                     // done 
                eors    r5,  r5, #8             // one more to go, so toggle the switch
                orr     r4,  r4, r3, lsl r5     // OR in the next byte as either low or high byte
                strneh  r4,  [r1], #2           // if halfword is full store it away
                movne   r4,  #0                 // and clear accu
                b       @L1

@L4             ldmia   sp!, {r4-r6}
                bx      lr
}
#endif  // defined(UNCOMPRESS_RL16_CODE32)

//----------------------------------------------------------------------
//          ���������O�X���k�f�[�^�R�Q�������W�J
//
//�E���������O�X���k�f�[�^��W�J���A32bit�P�ʂŏ������݂܂��B
//�EMI_UncompressRL8(), MI_UncompressRL16() ��荂���ł��B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �\��
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�t���O�f�[�^�t�H�[�}�b�g
//    u8  length:7            �W�J�f�[�^�� - 1�i�����k���j
//                            �W�J�f�[�^�� - 3�i�A����3Byte�ȏ㎞�݈̂��k�j
//        flag:1              �i0, 1�j = �i�����k�f�[�^, ���k�f�[�^�j
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UncompressRL32(register const void *srcp,register void *destp)
{
                stmdb   sp!, {r4-r5}

                ldr     r12, [r0], #4           // load header      
                mov     r12, r12, lsr #8        // dest. count
                mov     r4,  #0                 // write back accumulator
                mov     r5,  #32                // shifter reg. for write back accumulator

@L1             cmp     r12, #0                 // finished ?
                ble     @L6

                ldrb    r2,  [r0], #1           // load marker byte
                ldrb    r3,  [r0], #1           // load char ie. 1st char
                                                // <- still 1 cycle interlock from the 1st LDRB (LDRB/LDRH impose 2 cycles pipeline interlock!)
                tst     r2,  #0x80              // check if marker has MSB set                      
                bne     @L3                     // path decoding str&iacute;ngs of different characters  
                add     r2,  r2, #1             // adjust counter
                sub     r12, r12,r2             // decrement dest. count 

@L2             orr     r4,  r4, r3, ror r5     // accumulate (LSL R5 would be more obvious, but then the shifter reg. must be counted up and an add. CMP would be needed)
                subs    r5,  r5, #8             // decrement shifter
                beq     @L5                     // write back branch is only taken every 4th loop, so it's best to jump out (non-taken branch is only 1 cycle compared to 3 for a taken one)
                subs    r2,  r2, #1             // decrement counter
                ldrneb  r3,  [r0], #1           // preload char for the next loop to have the 2 cycle interlock of LDRB compensated by the pipeline flush imposed by the branch        
                bne     @L2
                b       @L1

@L5             str     r4,  [r1], #4           // write back
                mov     r5,  #32                // restore shifter
                mov     r4,  #0                 // reset write back accumulator    
                subs    r2,  r2, #1             // not to jump back saves 2 cycles
                ldrneb  r3,  [r0], #1           // preload char        
                bne     @L2
                b       @L1                     // path decoding str&iacute;ngs of same character

@L3             sub     r2,  r2, #0x7d          // adjust counter
                sub     r12, r12,r2             // decrement dest. count

@L4             orr     r4,  r4, r3, ror r5     // accumulate (the first word)
                subs    r5,  r5, #8             // decrement shifter
                beq     @L7                     // if word is full step out
                subs    r2,  r2, #1             // if not, check if there are still bytes left
                bne     @L4                     // and continue
                b       @L1                     // or be done

@L7             str     r4,  [r1], #4           // write back (the one word that may have chars from the other path in it)

                orr     r4,  r3, r3, lsl #8
                orr     r4,  r4, r4, lsl #16    // the char extended to word

@L8             subs    r2,  r2, #4             // full words are processed now, so decrement counter by 4
                ble     @L9                     // if no full words are left jump out
                str     r4,  [r1], #4           // otherwise write back 
                b       @L8

@L9             mov     r4,  #0                 // clear accu    
                mov     r5,  #32                // reset shifter reg.    
                adds    r2,  r2, #3             // re-adjust counter (0-3)
                beq     @L1                     // 0 then done

@L0             orr     r4,  r4, r3, ror r5     // accumulate (the max. 3 remaining bytes)
                sub     r5,  r5, #8             // decrement shifter
                subs    r2,  r2, #1             // decrement counter
                bne     @L0
                b       @L1

@L6             ldmia   sp!, {r4-r5}
                bx      lr
}

//----------------------------------------------------------------------
//          �����t�B���^�ϊ��̕��� �W�������W�J
//
//�E�����t�B���^�𕜌����A8bit�P�ʂŏ������݂܂��B
//�ENITRO�̏ꍇVRAM�ɒ��ړW�J���邱�Ƃ͂ł��܂���B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �P�ʃr�b�g�T�C�Y
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UnfilterDiff8( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4}
                
                ldmia   r0, {r2}                // r2:  destCount = (u32 *)srcp;
                mov     r3, #0                  // r3:  sum = 0;
                and     r4, r2, #0xF            // r4:  bitSize   = (u32 *)srcp & 0xF;
                mov     r2, r2, lsr #8          //      destCount = (u32 *)srcp >> 8;
                cmp     r4, #1                  //      if (bitSize != 1) {
                bne     @63

@61             // 8bit�P�ʂł̍����v�Z
                add     r0, r0, #3              //          srcp += 4;
                sub     r1, r1, #1
@62                                             //          do {
                ldrb    r4, [r0, #1]!           //              tmp = *(srcp++);
                subs    r2, r2, #1              //              destCount--; �p�C�v���C���̃X�g�[�������̂��߂��̈ʒu�Ŏ��s
                add     r3, r3, r4              //              sum += tmp
                strb    r3, [r1, #1]!           //              *(destp++) = sum;
                bgt     @62                     //          } while ( destCount > 0 );
                b       @65                     //      } else {
                
@63             // 16bit�P�ʂł̍����v�Z
                add     r0, r0, #2              //          
                sub     r1, r1, #2              //          
@64                                             //          do {
                ldrh    r4, [r0, #2]!           //              tmp = *(u16*)srcp; srcp += 2;
                subs    r2, r2, #2              //              destCount -= 2; �p�C�v���C���̃X�g�[�������̂��߂��̈ʒu�Ŏ��s
                add     r3, r3, r4              //              sum += tmp;
                strh    r3, [r1, #2]!           //              *(u16*)destp = sum; destp += 2;
                bgt     @64                     //          } while ( destCount > 0 );
                                                //      }
@65
                ldmfd   sp!, {r4}
                bx      lr
}

//----------------------------------------------------------------------
//          �����t�B���^�ϊ��̕��� �P�U�������W�J
//
//�E�����t�B���^�𕜌����A16bit�P�ʂŏ������݂܂��B
//�E�f�[�^TCM��VRAM�ɂ��W�J�ł��܂�
//�EMI_UnfilterDiff16()��荂���ł��B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �P�ʃr�b�g�T�C�Y
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UnfilterDiff16( register const void *srcp, register void *destp )
{
                stmfd   sp!, {r4, r5}
                
                ldmia   r0, {r2}                // r2:  destCount = (u32 *)srcp;
                mov     r3, #0                  // r3:  sum = 0;
                and     r4, r2, #0xF            // r4:  bitSize   = (u32 *)srcp & 0xF;
                mov     r2, r2, lsr #8          //      destCount = (u32 *)srcp >> 8;
                cmp     r4, #1                  //      if (bitSize != 1) {
                bne     @63

@61             // 8bit�P�ʂł̍����v�Z
                add     r0, r0, #2              //          srcp += 4;
                sub     r1, r1, #2
@62                                             //          do {
                ldrh    r4, [r0, #2]!           //          tmp = *(u16*)srcp; srcp += 2;
                sub     r2, r2, #2              //              destCount -= 2; �p�C�v���C���̃X�g�[�������̂��߂��̈ʒu�Ŏ��s
                add     r3, r3, r4              //              sum += tmp
                and     r5, r3, #0xFF           // r5:          tmp2 = sum & 0xFF;
                add     r3, r3, r4, lsr #8      //              sum += (tmp >> 8);
                add     r5, r5, r3, lsl #8      //              tmp2 += (sum << 8);
                strh    r5, [r1, #2]!           //              *(u16*)destp = tmp2; destp += 2;
                cmp     r2, #1                  // 
                bgt     @62                     //          } while ( destCount > 1 );
                bne     @65                     //          if ( destCount < 1 ) return;
                                                //          else // if (destCount == 1) {
                ldrh    r4, [r0, #2]!           //              tmp = *(u16*)srcp; srcp += 2;
                add     r3, r3, r4              //              sum += tmp;
                and     r5, r3, #0xFF           //              tmp2 = sum & 0xFF
                strh    r5, [r1, #2]!           //              *(u16*)destp = tmp2; destp += 2;
                b       @65                     //          }
                                                //      } else {
@63             // 16bit�P�ʂł̍����v�Z
                add     r0, r0, #2
                sub     r1, r1, #2
@64                                             //          do {
                ldrh    r4, [r0, #2]!           //              tmp = *(u16*)srcp; srcp += 2;
                subs    r2, r2, #2              //              destCount -= 2; �p�C�v���C���̃X�g�[�������̂��߂��̈ʒu�Ŏ��s
                add     r3, r3, r4              //              sum += tmp;
                strh    r3, [r1, #2]!           //              *(u16*)destp = sum; destp += 2;
                bgt     @64                     //          } while ( destCount > 0 );
                                                //      }
@65
                ldmfd   sp!, {r4,r5}
                bx      lr
}

//----------------------------------------------------------------------
//          �����t�B���^�ϊ��̕��� �R�Q�������W�J
//
//�E�����t�B���^�𕜌����A32bit�P�ʂŏ������݂܂��B
//�E�f�[�^TCM��VRAM�ɂ��W�J�ł��܂��B
//  MI_Uncompress8()��荂���ł��B//---- 
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//
//�E�f�[�^�w�b�_
//    u32 :4                  �P�ʃr�b�g�T�C�Y
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_UnfilterDiff32( register const void *srcp,register void *destp )
{
                stmdb   sp!, {r4-r6}

                ldr     r4, [r0], #4            // load header
                mov     r5, #0xff               // byte mask
                mov     r6, r4, lsr #8          // Dest. count
                tst     r4, #1                  // Bitsize 8 or 16?
                mov     r4, #0                  // R4 - accumulator
                beq     @bs16

@L1             ldr     r2, [r0], #4            // load data

                subs    r6, r6, #4              // decrement dest. count (at this stage to prevent pipeline stall)

                add     r4, r2, r4, lsr #24     // 1st sum
                and     r4, r4, r5              // mask

                add     r3, r4, r2, lsr #8      // 2nd sum
                and     r3, r3, r5              // mask
                orr     r4, r4, r3, lsl #8      // accumulate

                add     r3, r3, r2, lsr #16     // 3rd sum
                and     r3, r3, r5              // mask
                orr     r4, r4, r3, lsl #16     // accumulate

                add     r3, r3, r2, lsr #24     // 4th sum
                and     r3, r3, r5              // mask
                orr     r4, r4, r3, lsl #24     // accumulate

                str     r4, [r1], #4            // write back

                bgt     @L1                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr

@bs16
                orr     r5, r5, r5, lsl #8      // extend mask to 16 bit

@L2             ldr     r2, [r0], #4            // load data

                subs    r6, r6, #4              // decrement Dest. count (at this stage to prevent pipeline stall)

                add     r4, r2, r4, lsr #16     // 1st sum
                and     r4, r4, r5              // mask

                add     r3, r4, r2, lsr #16     // 2nd sum
                and     r3, r3, r5              // mask
                orr     r4, r4, r3, lsl #16     // accumulate

                str     r4, [r1], #4            // write back

                bgt     @L2                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr
}

//----------------------------------------------------------------------
//          �����t�B���^�ϊ� �W�������W�J
//
//�E�����t�B���^�ϊ����s���A8bit�P�ʂŏ������݂܂��B
//�ENITRO�̏ꍇVRAM�ɒ��ړW�J���邱�Ƃ͂ł��܂���B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//    u32  size               �\�[�X�T�C�Y
//    BOOL bitsize            �����T�C�Y (TRUE: 16bit, FALSE: 8bit)
//
//�E�f�[�^�w�b�_
//    u32 :4                  �P�ʃr�b�g�T�C�Y
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_FilterDiff8(register const void *srcp, register void *destp, register u32 size, register BOOL bitsize)
{
                stmdb   sp!, {r4-r6}

                add     r4, r3, #1              // u32 :4 : Bit size of unit
                orr     r4, r4, #0x80           // compType:4 : Compression type( = 3)                                
                orr     r4, r4, r2, lsl #8      // destSize:24 :  Data size before compression
                str     r4, [r1], #4            // write header
                mov     r4, #0                  // 1st subtrahend
                mov     r5, #0xff               // byte mask
                tst     r3, #1                  // Bitsize 8 or 16?
                bne     @bs16

@L1             ldrb    r3,[r0],#1              // load data

                subs    r2, r2, #1              // decrement Dest. count (at this stage to prevent pipeline stall)

                sub     r4, r3, r4              // diff
                and     r6, r4, r5              // mask

                mov     r4, r3                  // set as previous value

                strb    r6, [r1], #1            // write back

                bgt     @L1                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr

@bs16
                orr     r5, r5, r5, lsl #8      // extend mask to 16 bit

@L2             ldrh    r3,[r0],#2              // load data

                subs    r2, r2, #2              // decrement Dest. count (at this stage to prevent pipeline stall)

                sub     r4, r3, r4              // 1st diff
                and     r6, r4, r5              // mask

                mov     r4, r3                  // set as previous value

                strh    r6, [r1], #2            // write back

                bgt     @L2                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr
}

//----------------------------------------------------------------------
//          �����t�B���^�ϊ� �P�U�������W�J
//
//�E�����t�B���^�ϊ����s���A16bit�P�ʂŏ������݂܂��B
//�E�f�[�^TCM��VRAM�ɂ��W�J�ł��܂�
//�EMI_FilterDiff8() ��荂���ł��B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//    u32  size               �\�[�X�T�C�Y
//    BOOL bitsize            �����T�C�Y (TRUE: 16bit, FALSE: 8bit)
//
//�E�f�[�^�w�b�_
//    u32 :4                  �P�ʃr�b�g�T�C�Y
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_FilterDiff16(register const void *srcp, register void *destp, register u32 size, register BOOL bitsize)
{
                stmdb   sp!, {r4-r6}

                add     r4, r3, #1              // u32 :4 : Bit size of unit
                orr     r4, r4, #0x80           // compType:4 : Compression type( = 3)                                
                orr     r4, r4, r2, lsl #8      // destSize:24 :  Data size before compression
                str     r4, [r1], #4            // write header
                mov     r4, #0                  // 1st subtrahend
                mov     r5, #0xff               // byte mask
                tst     r3, #1                  // Bitsize 8 or 16?
                bne     @bs16

@L1             ldrh    r3,[r0],#2              // load data

                subs    r2, r2, #2              // decrement Dest. count (at this stage to prevent pipeline stall)

                sub     r4, r3, r4, lsr #8              // 1st diff
                and     r6, r4, r5              // mask

                rsb     r4, r3, r3, lsr #8      // 2nd diff
                and     r4, r4, r5              // mask
                orr     r6, r6, r4, lsl #8      // accumulate

                mov     r4, r3                  // set as previous value

                strh    r6, [r1], #2            // write back

                bgt     @L1                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr

@bs16
                orr     r5, r5, r5, lsl #8      // extend mask to 16 bit

@L2             ldrh    r3,[r0],#2              // load data

                subs    r2, r2, #2              // decrement Dest. count (at this stage to prevent pipeline stall)

                sub     r4, r3, r4              // 1st diff
                and     r6, r4, r5              // mask

                mov     r4, r3                  // set as previous value

                strh    r6, [r1], #2            // write back

                bgt     @L2                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr
}

//----------------------------------------------------------------------
//          �����t�B���^�ϊ� �R�Q�������W�J
//
//�E�����t�B���^�ϊ����s���A32bit�P�ʂŏ������݂܂��B
//�EMI_FilterDiff16() ��肳��ɍ����ł��B
//�E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
//  �o���邾��0�ŋl�߂Ē������ĉ������B
//�E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
//
//�E�����F
//    void *srcp              �\�[�X�A�h���X
//    void *destp             �f�X�e�B�l�[�V�����A�h���X
//    u32  size               �\�[�X�T�C�Y
//    BOOL bitsize            �����T�C�Y (TRUE: 16bit, FALSE: 8bit)
//
//�E�f�[�^�w�b�_
//    u32 :4                  �P�ʃr�b�g�T�C�Y
//        compType:4          ���k�^�C�v�i = 3�j
//        destSize:24         �W�J��̃f�[�^�T�C�Y
//
//�E�߂�l�F�Ȃ�
//----------------------------------------------------------------------

asm void MI_FilterDiff32(register const void *srcp, register void *destp, register u32 size, register BOOL bitsize)
{
                stmdb   sp!, {r4-r6}

                add     r4, r3, #1              // u32 :4 : Bit size of unit
                orr     r4, r4, #0x80           // compType:4 : Compression type( = 3)                                
                orr     r4, r4, r2, lsl #8      // destSize:24 :  Data size before compression
                str     r4, [r1], #4            // write header
                mov     r4, #0                  // 1st subtrahend
                mov     r5, #0xff               // byte mask
                tst     r3, #1                  // Bitsize 8 or 16?
                bne     @bs16

@L1             ldr    r3,[r0],#4               // load data

                subs    r2, r2, #4              // decrement Dest. count (at this stage to prevent pipeline stall)

                sub     r4, r3, r4, lsr #8      // 1st diff
                and     r6, r4, r5              // mask

                rsb     r4, r3, r3, lsr #8      // 2nd diff
                and     r4, r4, r5              // mask
                orr     r6, r6, r4, lsl #8      // accumulate

                mov     r4, r3, lsr #16         // shift higher halfword down

                sub     r3, r4, r3, lsr #8      // 3rd diff
                and     r3, r3, r5              // mask
                orr     r6, r6, r3, lsl #16     // accumulate

                rsb     r3, r4, r4, lsr #8      // 4th diff
                and     r3, r3, r5              // mask
                orr     r6, r6, r3, lsl #24     // accumulate

                str     r6, [r1], #4            // write back

                bgt     @L1                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr

@bs16
                orr     r5, r5, r5, lsl #8      // extend mask to 16 bit

@L2             ldr     r3,[r0],#4              // load data

                subs    r2, r2, #4              // decrement Dest. count (at this stage to prevent pipeline stall)

                sub     r4, r3, r4              // 1st diff
                and     r6, r4, r5              // mask

                mov     r4, r3, lsr #16         // shift higher halfword down

                rsb     r3, r3, r3, lsr #16     // 2nd diff
                and     r3, r3, r5              // mask
                orr     r6, r6, r3, lsl #16     // accumulate

                str     r6, [r1], #4            // write back

                bgt     @L2                     // loop if dest. count not reached

                ldmia   sp!, {r4-r6}
                bx      lr
}

//---- end limitation of processer mode
#include <nitro/codereset.h>

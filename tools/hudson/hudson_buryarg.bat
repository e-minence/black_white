@echo off
REM =================================================================
REM hudson�p��SLR�Ɉ����𖄂ߍ��ރo�b�`
REM �p�r    : prompt.bat cmd hudson_buryarg.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2010/05/12
REM =================================================================

SET PATH_MAIN_TEF=%PROJECT_PROGDIR%bin/ARM9-TS.HYB/Release/main.tef
SET PATH_MAIN_TLF=%PROJECT_PROGDIR%bin/ARM9-TS.HYB/Release/main.tlf

REM �e�X�g�i���o�[ >> hudson�Ŏw��
SET TEST_NO=%1
@echo TEST_NO = %TEST_NO%

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

REM prog�Ɉړ�
cd prog

REM ================================
REM TLF�ɏ�������ł���SRL���Đ���
REM ================================
buryarg.TWL %PATH_MAIN_TLF% %TEST_NO%
touch %PATH_MAIN_TEF%
make


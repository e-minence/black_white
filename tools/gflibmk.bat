
echo off

REM ============================================
REM     ROM�ł���邽�߂Ɍ���̒�`��ۑ�
REM ============================================
SET TMP_PM_DEBUG=%PM_DEBUG%
SET TMP_NITRO_FINALROM=%NITRO_FINALROM%
SET TMP_TWL_FINALROM=%TWL_FINALROM%
SET TMP_PM_DEBUG=%PM_DEBUG%
SET TMP_TWLSDK_PLATFORM=%TWLSDK_PLATFORM%
SET TMP_TARGET_PLATFORM=%TARGET_PLATFORM%



REM ============================================
REM     Release�p��`�w��
REM ============================================
SET PM_DEBUG=yes
SET NITRO_FINALROM=
SET TWL_FINALROM=


REM ============================================
REM     �ʏ�ł��쐬
REM ============================================
call %PROJECT_ROOT%prompt.bat cmd mkgl.bat %1
call %PROJECT_ROOT%nitro_prompt.bat cmd mkgl.bat %1


REM ============================================
REM     ROM�p��`�w��
REM ============================================
SET PM_DEBUG=no
SET NITRO_FINALROM=yes
SET TWL_FINALROM=yes


REM ============================================
REM     ROM�ō쐬
REM ============================================
call %PROJECT_ROOT%prompt.bat cmd mkgl.bat %1
call %PROJECT_ROOT%nitro_prompt.bat cmd mkgl.bat %1


REM ============================================
REM     ��`�����ɖ߂�
REM ============================================
SET PM_DEBUG=%TMP_PM_DEBUG%
SET NITRO_FINALROM=%TMP_NITRO_FINALROM%
SET TWL_FINALROM=%TMP_TWL_FINALROM%
SET TWLSDK_PLATFORM=%TMP_TWLSDK_PLATFORM%
SET TARGET_PLATFORM=%TMP_TARGET_PLATFORM%

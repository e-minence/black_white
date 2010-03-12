#!/usr/bin/ruby
#===================================================================
#
# @brief  make�Ώۂɂ���vpath��f���o��
#
# @data   10.03.08
# @author miyuki iwasawa 
#
#===================================================================

#====================================================
# �ݒ�
#====================================================
# �����R�[�h�w��
$KCODE = 'SJIS'

#====================================================
# �萔�錾
#====================================================
#�w�b�_�t�@�C���o��
DST_FILENAME = "make_vpath"  #�o�̓t�@�C����

#====================================================
# �又�� �J�n
#====================================================

#�J�����g�ɂ���t�@�C�������X�g�A�b�v
pwd =  Dir::entries( Dir::pwd )

# �z��
val = pwd.to_a;
# �t�H���_�Ɍ��I�A����t�H���_��r��
val = val.select{ |i| File::ftype(i) == "directory" && i != "." && i != ".." && i != ".svn" }

# �����񐶐�
str = "RES_DIR = "
val.each{ |i|
  str += i
  str += " "
}
str += "\n\n"

File::open( DST_FILENAME ,"w"){ |file|
  file.print str
  file.print "vpath %imd $(RES_DIR)\n"
  file.print "vpath %itp $(RES_DIR)\n"
  file.print "vpath %ita $(RES_DIR)\n"
  file.print "vpath %ica $(RES_DIR)\n"
  file.print "vpath %ima $(RES_DIR)\n"
  file.print "vpath %iva $(RES_DIR)\n\n"
}


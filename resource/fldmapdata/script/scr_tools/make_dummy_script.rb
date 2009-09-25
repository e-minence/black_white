###############################################################
#
#   �_�~�[�X�N���v�g�t�@�C������
#
#   tamada GAMEFREAK inc.
#   2009.07.15
#
###############################################################

require "fileutils"

class FileNameError < Exception; end

SP_SCRIPT_DUMMY_STRING = <<DUMMY_STRING

//==============================================================
//
//              sp_ZONEIDSYMBOL.ev
//
//    ����X�N���v�g�t�@�C���FZONEIDSYMBOL�p
//
//==============================================================

  .text

  .include  "scr_seq_def.h"
  .include  "ZONEIDSYMBOL_def.h"

//--------------------------------------------------------------
//              ����X�N���v�g
//--------------------------------------------------------------
_SP_EVENT_DATA_END


DUMMY_STRING


SCRIPT_DUMMY_STRING = <<DUMMY_STRING

//======================================================================
//                ZONEIDSYMBOL.ev
//
//    �X�N���v�g�t�@�C���FZONEIDSYMBOL�p
//
//======================================================================

  .text

  .include  "scr_seq_def.h"
  .include  "msg_ZONEIDSYMBOL.h"
  //.include  "ZONEIDSYMBOL.h"

//--------------------------------------------------------------
//               �X�N���v�g�{��
//--------------------------------------------------------------

//--------------------------------------------------------------
//               �X�N���v�g�e�[�u��
//--------------------------------------------------------------
_EVENT_DATA    ev_ZONEIDSYMBOL
_EVENT_DATA_END


/**************************************************************/
/*            ZONE_CHANGE_LABEL                               */
/**************************************************************/
//ev_zone_ZONEIDSYMBOL:
//  _END

/**************************************************************/
/*                                                            */
/**************************************************************/
ev_ZONEIDSYMBOL:
  //�Ƃ肠�����ŏ��̃��x�������������Ă���
  _END

DUMMY_STRING


HEADER_DUMMY_STRING = <<DUMMY_STRING
//======================================================================
//                ZONEIDSYMBOL_def.h
//
//    �X�N���v�g�w�b�_�t�@�C���FZONEIDSYMBOL�p
//
//======================================================================
#ifndef __ZONEIDSYMBOL_DEF_H__
#define __ZONEIDSYMBOL_DEF_H__

//�X�N���v�g�f�[�^ID��`
#define SCRID_ZONEIDSYMBOL_DATA_MAX   (0)   /* �ő吔 */

#endif  /* __ZONEIDSYMBOL_DEF_H__ */

DUMMY_STRING



def make_dummy(path, sym, content)
  if FileTest.exists?(path) then
    puts "#{path} exists."
  else
    puts "�X�N���v�g�t�@�C��#{path} ��������܂���B"
    #puts "scripts for #{sym} not found!!"
    File.open(path, "w"){|file|
      file.puts "#{content.gsub(/ZONEIDSYMBOL/,sym)}"
    }
    puts "�X�N���v�g�t�@�C��#{path} �𐶐����܂����I�I�I."
  end
end


###############################################################
#
#   ��������{��
#
###############################################################
directory = ARGV[0]
symbol = ARGV[1].downcase
unless FileTest.exists?(directory) then
  raise FileNameError, "#{directory}�����݂��܂���"
end
unless FileTest.directory?(directory) then
  raise FileNameError, "#{directory}�̓f�B���N�g���ł͂���܂���"
end

filename = directory + "\\" + symbol + ".ev"
sp_filename = directory + "\\sp_" + symbol + ".ev"
def_filename = directory + "\\" + symbol + "_def.h"

make_dummy(filename, symbol, SCRIPT_DUMMY_STRING)
make_dummy(sp_filename, symbol, SP_SCRIPT_DUMMY_STRING)
make_dummy(def_filename, symbol, HEADER_DUMMY_STRING)




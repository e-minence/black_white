#===============================================================
# WB��ʔ����f�[�^�e�[�u���R���o�[�^
# 09.12.01 iwasawa
#===============================================================

load "encount_common.def"
#require 'jstring'
$KCODE = "SJIS" #�����R�[�h��SJIS�ɐݒ�Bmonsno_hash.rb�̓ǂݍ��݂ɕK�v
require "../../tools/hash/monsno_hash.rb"

arg = $*
$input_f = arg[0]
$output_f = arg[1]

##############################################
#�\���̒�`
$_gene_poke = Struct.new("GeneratePoke", :zone, :monsno, :form, :lv_min, :lv_max, :white, :black, :name )

##############################################
#�O���[�o���ϐ���`

$header_buf = "/*\n\
\t@file\t%s\n\
\t@brief\t��ʔ����|�P�����f�[�^�e�[�u��\n\
\t@author\t���̃t�@�C���̓R���o�[�^����o�͂���Ă��܂�\n\n\
\tfrom resource/encount/generate_enc.rb\n\
*/\n\
#pragma once\n\n"

#��ʔ����e�[�u��
class CGenerateEnc
  @poke_list

  #������
  def initialize
    @poke_list = Array.new
  end

  #�A�[�J�C�u���X�g�o��
  def output f_path
    white_ct = 0
    black_ct = 0
    File.open(f_path,"w"){ |file|
      file.print( $header_buf % [f_path])
      file.print("#if PM_VERSION == VERSION_WHITE\n\n")

      file.print("static const GENERATE_ENCOUNT DATA_GenerateEncount[] = {\n")
      for n in @poke_list do
        if n.white then
          buf = " { ZONE_ID_%s, %d,%d, %d,%d },\t//%s\n" % [n.zone, n.monsno, n.form, n.lv_min, n.lv_max, n.name]
          file.print(buf)
          white_ct += 1
        end
      end
      file.print("};\n\n#else\n\n")
      file.print("static const GENERATE_ENCOUNT DATA_GenerateEncount[] = {\n")
      for n in @poke_list do
        if n.black then
          buf = " { ZONE_ID_%s, %d,%d, %d,%d },\t//%s\n" % [n.zone, n.monsno, n.form, n.lv_min, n.lv_max, n.name]
          file.print(buf)
          black_ct += 1
        end
      end
      file.print("};\n\n#endif\n\n")
    }
    header_path = File::basename(f_path,'.cdat');
    header_path += "_def.h"
    File.open(header_path,"w"){ |file|
      file.print( $header_buf % [header_path])
    
      file.print("#if PM_VERSION == VERSION_WHITE\n\n")
      file.print(" #define GENERATE_ENC_POKE_MAX\t( %d )\n\n" % [white_ct])
      file.print("#else\n\n")
      file.print(" #define GENERATE_ENC_POKE_MAX\t( %d )\n\n" % [black_ct])
      file.print("#endif\n\n")
    }
  end

  #�t�@�C�����[�h
  def src_load f_path
    File.open(f_path){ |file|
     while line = file.gets
       line = line.chomp #�s�����s����菜��
       line = line.gsub("\"","")  #"����菜��
       work = line.split(",")

       @poke_list << $_gene_poke.new
       cp = @poke_list.last()

       cp.zone = work[0].upcase()
       cp.monsno = work[1].to_i
       cp.lv_min = work[2].to_i
       cp.lv_max = work[3].to_i
       cp.form = work[4].to_i
       cp.white = work[5] == "" ? false : true 
       cp.black = work[6] == "" ? false : true
       cp.name = work[7]
     end
    }
  end
end

#---------------------------------------------
# �R���o�[�g���C��
#---------------------------------------------
def convert
  #�R���o�[�g���C��
  c_enc = CGenerateEnc.new()
  c_enc.src_load( $input_f )
  c_enc.output( $output_f )
end

#���s
convert


#------------------------------------------------------------------------------
#
#
# �z�u���f���A�[�J�C�u����
#		
#
# 2009.12 �����\���ύX�i����͕ς���Ă��Ȃ��j
#
#------------------------------------------------------------------------------
KCODE = "SJIS"

require "fileutils"

#C�`���w�b�_�ǂݍ��݂̂���
require "#{ENV["PROJECT_ROOT"]}tools/headerdata.rb"


class TooMuchSymbolError < Exception; end
class SymbolRedefineError < Exception; end
class AnimeIndexError < Exception; end


BUILDMODEL_HEADER = "../../../prog/src/field/field_buildmodel.h"
ANIME_INDEX_FILE  ="wkdir/buildmodel_anime.h"

COL_FILENAME = 1
COL_ANIME_IDX = 8
COL_PROG_IDX = 9
COL_SUBMODEL_IDX = 10
COL_SUBMODEL_X = 11
COL_SUBMODEL_Y = 12
COL_SUBMODEL_Z = 13

#------------------------------------------------------------------------------
# �z�u���f���f�[�^��ێ�����N���X
#------------------------------------------------------------------------------
class BMData
  attr_reader :sym, :anime_id, :prog, :submodel_id, :submodel_pos

  #------------------------------------------------------------------------------
  # �A�j��ID�̎�������
  #------------------------------------------------------------------------------
  def BMData.makeAnimeDictionary(anime_index_file)
    anm_dic = Array.new
    File.open(anime_index_file, "r"){|file|
      file.each{|line|
        if line =~/^#/ then
          next
        end
        column = line.split
        anm_dic << column[0].upcase
      }
    }
    return anm_dic
  end
  def BMData.readBuildModelHeader( filename )
    bmdic = HeaderDataArray.new
    bmdic.load( filename )
    return bmdic
  end

  @@syms = Array.new
  @@anm_dic = self.makeAnimeDictionary(ANIME_INDEX_FILE)
  @@prog_dic = self.readBuildModelHeader( BUILDMODEL_HEADER )

  #------------------------------------------------------------------------------
  #   ����������
  #------------------------------------------------------------------------------
  def initialize( sym, anime_id, prog, submodel, x, y, z )
    if @@syms.index(sym) == nil then
      @@syms << sym
    else
      raise SymbolRedefineError, "�t�@�C��#{sym}�����d��`����Ă��܂�"
    end
    if @@syms.length > 255 then
      raise TooMuchSymbolError, "�z�u���f���̎�ނ������l�i255)�𒴂��Ă��܂��I�I"
    end
    @sym = sym
    @anime_id = getAnimeID( anime_id )
    @prog = getProgID( prog )
    @submodel = submodel
    @submodel_id = nil
    @smx = Integer(x)
    @smy = Integer(y)
    @smz = Integer(z)
  end

  def dump()
    #p self
    output = ""
    output += [ @anime_id, @prog, @submodel_id ].pack("SSS")
    output += [ @smx, @smy, @smz ].pack("sss")
  end
  
  #------------------------------------------------------------------------------
  # �e�z�u���f�����Ƃ̃A�j���w��ID�̃e�[�u���𐶐�����
  # IN: anm_dic   �A�j���f�[�^�̍����e�[�u��
  #     arr         �z�u���f�����Ƃ̃A�j���w��ID�i�z�u���f���Ɠ������я��ł��邱�Ƃ�z��j
  #------------------------------------------------------------------------------
  def getAnimeID(anime_name)
    index = @@anm_dic.index(anime_name.upcase)
    if anime_name == "�~" then
      index = 0xffff
    elsif index == nil then
      raise AnimeIndexError, "�A�j���w�� #{anime_name} ��������܂���ł���"
    end
    return index
  end

  #------------------------------------------------------------------------------
  # �]�����f������ID�ɕϊ�
  #------------------------------------------------------------------------------
  def createSubModelID()
    idx = @@syms.index(@submodel)
    if @submodel == "�~" then
      @submodel_id = 0xffff 
    elsif idx == nil then
      p @@syms
      puts "�]�����f����#{@submodel}��������܂���I�I"
      @submodel_id = 0xffff
    else
      @submodel_id = idx
    end
  end

  #------------------------------------------------------------------------------
  # �v���O�����w��ID�̎擾
  #------------------------------------------------------------------------------
  def getProgID(prog_name)
    if prog_name == "�~" then
      return 0xffff
    elsif prog_name =~/\d+/ then
     return Integer(prog_name)
    else
      return Integer( @@prog_dic.search( "BM_PROG_ID_" + prog_name, @sym ) )
    end
  end

end

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
#���̓t�@�C���iExcel�t�@�C�����^�u��؂�e�L�X�g�ɕϊ��������́j
INPUT_FILE	=	ARGV[0]
#�ˑ��t�@�C�����`����Makefile�ϐ��̃V���{����
DEP_SYMBOL	= ARGV[1]
#�ˑ��t�@�C����`���L�q����t�@�C���̖��O
DEP_FILE	=	ARGV[2]
#�A�[�J�C�u�Ώۃt�@�C�����L�q����t�@�C���̖��O
ARC_FILE	=	ARGV[3]

BIN_FILE  = ARGV[4]



bmdata = Array.new

#���̓t�@�C���ǂݍ��݁ABMData�̔z��𐶐�����
File.open(INPUT_FILE, "r"){|input_file|
  input_file.gets

  input_file.each{|line|
    column = line.split
    if column.length < 2 then break end
    bmdata << BMData.new( column[COL_FILENAME],
                     column[COL_ANIME_IDX], column[COL_PROG_IDX], column[COL_SUBMODEL_IDX],
                     column[COL_SUBMODEL_X], column[COL_SUBMODEL_Y], column[COL_SUBMODEL_Z] ) 

  }
}

#�o�C�i���t�@�C������
output = ""
bmdata.each{|bm|
  bm.createSubModelID()
  output += bm.dump
}
bin_file = File.open(BIN_FILE, "wb")
bin_file.write output
bin_file.close



#�A�[�J�C�u�Ώۃt�@�C�����L�q����t�@�C���̖��O
File.open(ARC_FILE, "w"){|arc_file|
  bmdata.each{|bm|
    nsbmd_name = "#{bm.sym}.nsbmd"
    arc_file.puts "\"nsbmdfiles/#{nsbmd_name}\""
  }
  arc_file.close
}

#�ˑ��t�@�C����`���L�q����t�@�C���̖��O
File.open(DEP_FILE, "w"){|dep_file|
  dep_file.puts "#{DEP_SYMBOL}	=	\\\n"
  bmdata.each{|bm|
    nsbmd_name = "#{bm.sym}.nsbmd"
    dep_file.puts "\t$(RESDIR)/#{nsbmd_name} \\\n"
  }
  dep_file.printf("\n")
}

#imd�����݂��Ȃ��ꍇ�A�_�~�[�𐶐�����
bmdata.each{|bm|
	imd_name = "imdfiles/#{bm.sym}.imd"
  unless FileTest.exists?( imd_name ) then
    puts "\t@echo #{imd_name}�����݂��Ȃ����߁A�_�~�[�t�@�C�����R�s�[���܂�"
    FileUtils.cp "imdfiles/dummy_buildmodel.imd",imd_name
  end
}


#------------------------------------------------------------------------------
#
# �z�u���f���A�[�J�C�u����
#		
#
#------------------------------------------------------------------------------
KCODE = "SJIS"

require "fileutils"

class SymbolRedefineError < Exception; end
class AnimeIndexError < Exception; end

ANIME_INDEX_FILE  ="wkdir/buildmodel_anime.h"

COL_FILENAME = 1
COL_ANIME_IDX = 8
COL_PROG_IDX = 9
COL_SUBMODEL_IDX = 10

#------------------------------------------------------------------------------
# �e�z�u���f�����Ƃ̃A�j���w��ID�̃e�[�u���𐶐�����
# IN: anm_dic   �A�j���f�[�^�̍����e�[�u��
#     arr         �z�u���f�����Ƃ̃A�j���w��ID�i�z�u���f���Ɠ������я��ł��邱�Ƃ�z��j
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def makeAnimeDictionary(anime_index_file)
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

def getAnimeID(anm_dic, anime_name)
  index = anm_dic.index(anime_name.upcase)
  if anime_name == "�~" then
    index = 0xffff
  elsif index == nil then
    raise AnimeIndexError, "�A�j���w�� #{anime_name} ��������܂���ł���"
  end
  return index
end

def getProgID(prog_name)
  if prog_name == "�~" then
    return 0xffff
  elsif prog_name =~/\d+/ then
    return Integer(prog_name)
  end
end

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def makeIndexes(anm_arr, prog_arr, submodel_arr)

  output = ""
  anm_arr.each_index{|idx|
    output += [anm_arr[idx], prog_arr[idx], submodel_arr[idx]].pack("SSS")
  }
  return output
end

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def makeSubmodelIDList(symbols, submodel_list)
  submodel_ids = submodel_list.map{|name|
    idx = symbols.index(name)
    if name == "�~" then
      0xffff 
    elsif idx == nil then
      puts "�]�����f����#{name}��������܂���I�I"
      0xffff
    else
      idx
    end
  }
  return submodel_ids
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

anm_dic = makeAnimeDictionary(ANIME_INDEX_FILE)

input_file = File.open(INPUT_FILE, "r")
dep_file = File.open(DEP_FILE, "w")
arc_file = File.open(ARC_FILE, "w")

dep_file.puts "#{DEP_SYMBOL}	=	\\\n"

check_list = Array.new

syms = Array.new
anime_ids = Array.new
progs = Array.new
submodels = Array.new

input_file.gets

while line = input_file.gets
	column = line.split
	if column.length < 2 then break end
  sym = column[COL_FILENAME]
  if syms.index(sym) == nil then
    syms << sym
  else
    raise SymbolRedefineError, "�t�@�C��#{sym}�����d��`����Ă��܂�"
  end
	nsbmd_name = "#{sym}.nsbmd"
	dep_file.puts "\t$(RESDIR)/#{nsbmd_name} \\\n"
	arc_file.puts "\"nsbmdfiles/#{nsbmd_name}\""
	imd_name = "imdfiles/#{sym}.imd"
	check_list << imd_name unless FileTest.exists? imd_name
  anime_ids << getAnimeID(anm_dic, column[COL_ANIME_IDX])
  progs << getProgID(column[COL_PROG_IDX])
  submodels << column[COL_SUBMODEL_IDX]
  puts "#{line}"
end

submodel_ids = makeSubmodelIDList(syms, submodels)

syms.each_index{|idx|
#  printf("%-16s %04x, %04x %04x\n", syms[idx], anime_ids[idx], progs[idx], submodel_ids[idx])
}
bin_file = File.open(BIN_FILE, "wb")
bin_file.write makeIndexes(anime_ids, progs, submodel_ids)
bin_file.close

dep_file.printf("\n")
check_list.each{|item|
	puts "\t@echo #{item}�����݂��Ȃ����߁A�_�~�[�t�@�C�����R�s�[���܂�"
	FileUtils.cp "imdfiles/dummy_buildmodel.imd",item
}

input_file.close
dep_file.close
arc_file.close


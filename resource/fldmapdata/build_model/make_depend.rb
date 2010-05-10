#------------------------------------------------------------------------------
#
#		�z�u���f���̃G���A��depend�t�@�C��
#
#		2009.04.27		tamada
#		2010.05.08		tomoya �@depend�t�@�C���𐶐����鏈���ɕύX
#
#------------------------------------------------------------------------------
require "fileutils"

INPUT_FILE	= ARGV[0]			#���̓t�@�C����
OUTPUT_SYM	= ARGV[1]			#�o�̓t�@�C���V���{��
OUTPUT_DIR	= ARGV[2]			#�o�͐�f�B���N�g����
OUTPUT_DEPEND	= ARGV[3]	  #�o�̓f�y���h�t�@�C��
NSBMD_DIR = ARGV[4] #NSBMD�̊i�[����Ă���f�B���N�g��
BMINFO_DIR = ARGV[5] #BMINFO�̊i�[����Ă���f�B���N�g��


CONVERTER = "$(PROJECT_ROOT)/tools/binlinker.rb"

book = Array.new
sheet = Array.new
book << sheet

#book�ɑ΂��ăV�[�g���Ƃɕ��f���ĕۑ�
File.open(INPUT_FILE, "r"){|input_file|
	while line = input_file.gets
		column = line.split
		#�J�������Ȃ����ʃV�[�g�Ɣ���
		if column.length < 2 then
			if sheet.length > 1
				sheet = Array.new
				book << sheet
			else
				break
			end
		end
		sheet << line
	end
}

class InputFileError < Exception; end
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
class Bmodel
	attr :name
	attr :flag

	def initialize(name,flag)
		@name = name
		@flag = flag
	end
end

def make_all_index(first_page)
	bmodel_list = Array.new
	check_list = Hash.new
	count = 0
	first_page.each{|line|
		if count > 0 then
			column = line.split
			name = column[1].downcase
			flag = if column[6] == "�풓" then true else false end
			if check_list.has_key?(name) then
				raise InputFileError, "#{name}���d�����Ă��܂�"
			end
			check_list[name] = true
			bmodel_list << Bmodel.new(name, flag)
		end
		count += 1
	}
	return bmodel_list
end

bmodel_list = make_all_index(book[0])
bmodel_list.each{|bmodel|
#	puts "#{bmodel.name} #{bmodel.flag == true ? 1 : 0}"
}

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
book.delete_at(0)		#1�y�[�W�ڍ폜
arc_list = File.open("#{OUTPUT_SYM}.list","w")
  STDERR.puts "#{book.length}"

depend_list = File.open("#{OUTPUT_DEPEND}","w")

depend_obj_list = File.open("#{OUTPUT_DEPEND}_obj","w")

depend_obj_list.write "#{OUTPUT_SYM}_OBJ = "

book.each_index{|sheet_index|
	filename = sprintf("%s/%s%03d.bin", OUTPUT_DIR, OUTPUT_SYM, sheet_index+1)
  STDERR.puts "#{filename}"
	arc_list.puts "\"#{filename}\""
  depend_obj_list.write "#{filename} "
	#puts "#{filename}"

  #filename�𐶐����邽�߂̈ˑ��֌W�̏������W
  bm_id_array = Array.new
  nsbmd_array = Array.new
  count = 0

  #�܂��A�풓�͕K�������B
  bmodel_list.each_index{|index|

    bmodel = bmodel_list[index]

    if bmodel.flag == true then
      bm_id_array << "#{BMINFO_DIR}/bminfo_#{index}.dat " #BMID
      nsbmd_array << "#{NSBMD_DIR}/#{bmodel.name}.nsbmd " #NSBMD
      count += 1
    end
    
  }
  
  sheet = book[sheet_index]
  sheet.delete_at(0) #1�s�ڍ폜
  sheet.each{|line|
    column = line.split
    #���X�g�ɂ�������
    bmodel_list.each_index{|index|

      bmodel = bmodel_list[index]

      if bmodel.name == column[1] then
        bm_id_array << "#{BMINFO_DIR}/bminfo_#{index}.dat " #BMID
        nsbmd_array << "#{NSBMD_DIR}/#{bmodel.name}.nsbmd " #NSBMD
        count += 1
      end
      
    }
  }
  #�ˑ��֌W���������ށB
  depend_list.puts "#{filename}: #{bm_id_array} #{nsbmd_array}"
  depend_list.puts "\t@echo #{filename} output..."
  depend_list.puts "\t@ruby #{CONVERTER} #{bm_id_array} #{nsbmd_array} #{filename} AB"
  depend_list.puts ""
}


arc_list.close
depend_list.close
depend_obj_list.close







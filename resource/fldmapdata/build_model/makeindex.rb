#------------------------------------------------------------------------------
#
#		�z�u���f���̃G���A�ʓo�^���X�g����
#
#		2009.04.27		tamada
#
#------------------------------------------------------------------------------
require "fileutils"

INPUT_FILE	= ARGV[0]			#���̓t�@�C����
OUTPUT_SYM	= ARGV[1]			#�o�̓t�@�C���V���{��
OUTPUT_DIR	= ARGV[2]			#�o�͐�f�B���N�g����


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
arc_list = File.open("#{OUTPUT_DIR}/#{OUTPUT_SYM}.list","w")
  STDERR.puts "#{book.length}"
book.each_index{|sheet_index|
	filename = sprintf("%s/%s%03d.bin", OUTPUT_DIR, OUTPUT_SYM, sheet_index+1)
  STDERR.puts "#{filename}"
	arc_list.puts "\"#{filename}\""
	#puts "#{filename}"
	File.open(filename , "wb"){|outfile|
		entrys = Hash.new
		sheet = book[sheet_index]
		sheet.delete_at(0) #1�s�ڍ폜
		sheet.each{|line|
			column = line.split
			entrys[column[1]] = true
		}
		count = 0
		bmodel_list.each_index{|index|
			bmodel = bmodel_list[index]
			if bmodel.flag == true || entrys.has_key?(bmodel.name) then
				outfile.write([index].pack("S"))
				#printf("%3d %3d %-5s %-20s\n", count, index, bmodel.flag, bmodel.name)
				count += 1
			end
		}
	}
}
arc_list.close








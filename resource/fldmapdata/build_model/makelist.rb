require "fileutils"

INPUT_FILE	=	ARGV[0]
DEP_FILE	=	ARGV[1]
ARC_FILE	=	ARGV[2]

input_file = File.open(INPUT_FILE, "r")
dep_file = File.open(DEP_FILE, "w")
arc_file = File.open(ARC_FILE, "w")

dep_file.puts "DEPEND_FILES	=	\\\n"

check_list = Array.new

input_file.gets

while line = input_file.gets
	column = line.split
	if column.length < 2 then break end
	nsbmd_name = "#{column[1]}.nsbmd"
	dep_file.puts "\t$(RESDIR)/#{nsbmd_name} \\\n"
	arc_file.puts "\"nsbmdfiles/#{nsbmd_name}\""
	imd_name = "imdfiles/#{column[1]}.imd"
	check_list << imd_name unless FileTest.exists? imd_name
end

dep_file.printf("\n")
check_list.each{|item|
	puts "\t@echo #{item}�����݂��Ȃ����߁A�_�~�[�t�@�C�����R�s�[���܂�"
	FileUtils.cp "imdfiles/dummy_buildmodel.imd",item
}
input_file.close
dep_file.close
arc_file.close
puts "#{check_list}"

$KCODE = "SJIS"

csv_file = open(ARGV.shift,"r")

h_file_name = ARGV.shift
h_file = open(h_file_name,"w")

h_file.printf("//======================================================================\n")
h_file.printf("/**\n")
h_file.printf(" * @file	%s\n",h_file_name)
h_file.printf(" * @brief	���l��`�t�@�C��\n")
h_file.printf(" * @note	���̃t�@�C���̓R���o�[�^�[����o�͂���܂�\n")
h_file.printf(" * @note	�X�N���v�g���Q�Ƃ��܂�\n")
h_file.printf(" *\n")
h_file.printf(" */\n")
h_file.printf("//======================================================================\n")
h_file.printf("\n")

h_file.printf("#pragma once\n")
h_file.printf("\n")

#1�s��΂�
csv_file.gets
index = 0
while line = csv_file.gets
  #END���o
  if line =~/^#END/ then
		break
	end
  
  column = line.split ","

  h_file.printf("#define %s (%d)\n",column[0].upcase, index)

  index += 1
end


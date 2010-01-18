$KCODE = "SJIS"

csv_file = open(ARGV.shift,"r")

dat_file_name = ARGV.shift
dat_file = open(dat_file_name,"w")

dat_file.printf("//======================================================================\n")
dat_file.printf("/**\n")
dat_file.printf(" * @file	%s\n",dat_file_name)
dat_file.printf(" * @brief	�D�������n�a�i��`�t�@�C��\n")
dat_file.printf(" * @note	���̃t�@�C���̓R���o�[�^�[����o�͂���܂�\n")
dat_file.printf(" *\n")
dat_file.printf(" */\n")
dat_file.printf("//======================================================================\n")
dat_file.printf("\n")

#1�s��΂�
csv_file.gets

#�g���[�i�[
dat_file.printf("static const TR_DATA TrData[TRAINER_MAX] = {\n")
while line = csv_file.gets
  #TR_END���o
  if line =~/^#TR_END/ then
		break
	end
  
  column = line.split ","

  dat_file.printf("\t{%s, %s, {%s, %s}},\n",column[0].upcase, column[1].upcase, column[2] ,column[3].chomp("\r").chomp("\n"))

end
dat_file.printf("};\n")
dat_file.printf("\n")

#���b
dat_file.printf("static const NONE_TR_DATA LongTalkerData[LONG_TALKER_MAX] = {\n")
while line = csv_file.gets
  #TR_END���o
  if line =~/^#LONG_END/ then
		break
	end
  
  column = line.split ","

  dat_file.printf("\t{%s, {%s, %s}},\n",column[0].upcase, column[1], column[2].chomp("\r").chomp("\n"))

end
dat_file.printf("};\n")
dat_file.printf("\n")

#���ʘb
dat_file.printf("static const NONE_TR_DATA ShortTalkerData[SHORT_TALKER_MAX] = {\n")
while line = csv_file.gets
  #TR_END���o
  if line =~/^#SHORT_END/ then
		break
	end
  
  column = line.split ","

  dat_file.printf("\t{%s, {%s, %s}},\n",column[0].upcase, column[1], column[2].chomp("\r").chomp("\n"))

end
dat_file.printf("};\n")

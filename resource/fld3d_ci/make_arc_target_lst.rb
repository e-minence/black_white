$KCODE = "SJIS"

def CollectStr(vec, str)
  if str == "NONE" then
  elsif str == nil then
    #�G���[
    p "ERROR::�w�蕶���񂪂���܂���"
    exit -1
  else
    ret_idx = vec.index(str)
    if ret_idx == nil then
      #�G���g���[
      fix_str = str
      fix_str = fix_str.sub(/.imd/,".nsbmd")
      fix_str = fix_str.sub(/.ica/,".nsbca")
      fix_str = fix_str.sub(/.ima/,".nsbma")
      fix_str = fix_str.sub(/.itp/,".nsbtp")
      fix_str = fix_str.sub(/.ita/,".nsbta")
      fix_str = fix_str.sub(/.iva/,".nsbva")
      vec << fix_str
    else
#      printf("ERROR::�w�蕶����͊��ɂ��� %s\n",str)
#      exit -1
    end
  end
end

csv_file = open(ARGV.shift,"r")
list_file = open(ARGV.shift,"w")

ary = Array.new
ary.clear
#1�s�ǂݔ�΂�
csv_file.gets
while line = csv_file.gets
  #END���o
  if line =~/^#END/ then
		break
	end
  column = line.split ","

  #�f�[�^���W
  CollectStr(ary,column[1]) #SPA
  CollectStr(ary,column[4])  #MDL_1
  CollectStr(ary,column[9])  #MDL_2

  CollectStr(ary,column[5])  #MDL_1ANM_1
  CollectStr(ary,column[6])  #MDL_1ANM_2
  CollectStr(ary,column[7])  #MDL_1ANM_3
  
  CollectStr(ary,column[10])   #MDL_2ANM_1
  CollectStr(ary,column[11])  #MDL_2ANM_2
  CollectStr(ary,column[12])  #MDL_2ANM_3

end

ary.each{ |i|
  list_file.printf("\"%s\"\n",i)
}


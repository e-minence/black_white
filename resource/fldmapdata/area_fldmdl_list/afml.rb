$KCODE = "SJIS"

def MakeObjCodeVec( file, vec )
  entry = false
  while line = file.gets
    column = line.split ","
    if entry == true then
      vec << column[0].chomp("\n").chomp("\r")
      entry = false
    else
      if column[0].index("#OBJ") == 0 then
        entry = true
      else
        entry = false
      end
    end
  end  
end

AreaHash = Hash.new
AreaHash.clear

#�]�[���e�[�u����ǂݍ���
zone_csv = open(ARGV.shift,"r")

#��s�ǂݔ�΂�
zone_csv.gets
while line = zone_csv.gets
  #END���o
  if line =~/^END/ then
		break
	end
  column = line.split ","

  #�}�g���b�N�X�h�c���uwb�v�̍���Ώ̂ɂ���
  if column[4] == "wb" then
    #�G���A���擾
    area = column[2]
    #�]�[���擾
    zone = column[0]
    #�擾�G���A�̓v�b�V���ς݂��H
    
    if AreaHash.key?(area) == false then     #���v�b�V��
      zone_ary =  Array.new
      zone_ary.clear
    else
      #�|�b�v
      zone_ary = AreaHash[area]
    end
    #�擾�]�[�����v�b�V��
    zone_ary << zone
    #�v�b�V��
    AreaHash[area] = zone_ary
  end  
end

#MEV�t�@�C�����J���āA�n�a�i�R�[�h���G���A�ʂɊi�[���Ă���

AreaHash.each{|key,val|
  str = key + ".txt"
  text = open(str,"w")
  vec = Array.new
  vec.clear
  val.each{|zone|  
    pass = "../eventdata/data/" + zone + ".mev"
    mev = open(pass,"r")
    MakeObjCodeVec( mev, vec )
  }
  #�d���폜
  vec.uniq!
  vec.each{|code|
    text.printf("%s\n", code)
  }
}

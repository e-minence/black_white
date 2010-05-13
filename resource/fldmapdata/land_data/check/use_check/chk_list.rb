class MATRIX_DATA
  attr_reader:x
  attr_reader:z
  attr_reader:Prefix
  def initialize(x,z,pre)
    @x = x
    @z = z
    @Prefix = pre
  end
end

def MakeMatHash(csv, hash)
  file = open( csv, "r" )
  #1�s��΂�
  line = file.gets

  while line = file.gets
    #END���o
    if line =~/^#END/ then
		  break
	  end
    column = line.split ","

    data = MATRIX_DATA.new(column[1].to_i, column[2].to_i, column[3])
    hash[ column[0] ] = data
  end

end

def MakeBlock(ary, data)
  for n in(0...data.z) do
    for m in(0...data.x) do
      str = data.Prefix + sprintf("%02d",m) + "_" + sprintf("%02d",n)
      ary << str
    end
  end
end

def MakeAry( csv, ary )
  file = open( csv, "r" )
  #3�s��΂�
  line = file.gets
  line = file.gets
  line = file.gets

  while line = file.gets
    #END���o
    if line =~/^#END/ then
		  break
	  end
    column = line.split ","

    ary << column[1]
    if column[2] == "MAPTYPE_CROSS" then
      ex = column[1] + "_ex"
      ary << ex
    end
  end
end

def MakeAryFromReplace( csv, hash, master_ary )
  ary_block = Array.new
  ary_matrix = Array.new

  file = open( csv, "r" )
  #4�s��΂�
  line = file.gets
  line = file.gets
  line = file.gets
  line = file.gets
  
  while line = file.gets
    #END���o
    if line =~/^\n/ then
		  break
	  end
    #END���o
    if line =~/^#END/ then
		  break
	  end

    column = line.split ","

    if column[1] == "MAPBLOCK" then
      for n in(0...5) do
        str = column[3+n]
        rc = ary_block.include?(str)
        if (str.length != 0) && (rc == false) && str != "\n" then
          ary_block << str
        end
      end
    elsif column[1] == "MATRIX" then
      for n in(0...5) do
        str = column[3+n]
        rc = ary_matrix.include?(str)
        if (str.length != 0) && (rc == false) && str != "\n" then
          ary_matrix << str
        end
      end
    end
  end

  ary_block.each{|i|
    rc = master_ary.include?(i)
    if rc == false then
      master_ary << i
    end
  }

  #�}�g���b�N�X����u���b�N���쐬
  printf("matrix_from\n")
  ary_matrix.each{ |i|
    MakeBlock( master_ary, hash[i] )
  }
end

def MakeFieldMapBlock(csv_fld, w, ary)
  file = open( csv_fld, "r" );
  z = 0
  #2�s��΂�
  line = file.gets
  line = file.gets
  while line = file.gets
    #END���o
    if line =~/^#END/ then
		  break
	  end

    column = line.split ","

    for x in (0...w) do
      str = column[1+x].chomp("\n").chomp("\r")
      if str == "1" then
        map_str = "map" + sprintf("%02d_%02d",x,z)
        ary << map_str
      end
    end
    z += 1
  end
end

csv01 = ARGV[0]
csv02 = ARGV[1]
csv03 = ARGV[2]
csv04 = ARGV[3]
csv_rep = ARGV[4]
csv_matrix =  ARGV[5]
csv_fld_matrix =  ARGV[6]

result_file = open( "result.txt", "w" )

#�}�g���b�N�X�f�[�^�쐬
mat_hash = Hash.new
MakeMatHash(csv_matrix, mat_hash)

#�f�B���N�g����bin�t�@�C�����X�g�A�b�v
bin_ary = Dir.glob("../../land_res/*.bin")
#�f�B���N�g����imd�t�@�C�����X�g�A�b�v
imd_ary = Dir.glob("../../land_res/*.imd")
#�f�B���N�g����3dmd�t�@�C�����X�g�A�b�v
dmd_ary = Dir.glob("../../land_res/*.3dmd")
#��������H
bin_ary.each{|i|
  i.sub!(/..\/..\/land_res\//,"")
  i.sub!(/\.bin/,"")
}
#��������H
imd_ary.each{|i|
  i.sub!(/..\/..\/land_res\//,"")
  i.sub!(/\.imd/,"")
}
#��������H
dmd_ary.each{|i|
  i.sub!(/..\/..\/land_res\//,"")
  i.sub!(/\.3dmd/,"")
}


all_ary = bin_ary | imd_ary | dmd_ary

maplist_ary = Array.new

MakeAry( csv01, maplist_ary )
MakeAry( csv02, maplist_ary )
MakeAry( csv03, maplist_ary )
MakeAry( csv04, maplist_ary )
#�[���R�s�[
ary2 = Marshal.load(Marshal.dump(maplist_ary))

#�u�������}�b�v��z��Ɋi�[
rep_ary = Array.new
MakeAryFromReplace( csv_rep, mat_hash, rep_ary )
maplist_ary = maplist_ary | rep_ary
#�d�����폜
maplist_ary.uniq!

#�[���R�s�[
ary = Marshal.load(Marshal.dump(maplist_ary))

printf("ary_size = %d\n", ary.length)
printf("all_size = %d\n", all_ary.length)

use_ary = Array.new
ary.each{|i|
  rc = all_ary.include?(i)
  if rc == true then
    all_ary.delete(i)
    use_ary << i
  end  
}

printf("use_size = %d\n", use_ary.length)

use_ary.each{|i|
  rc = ary.include?(i)
  if rc == true then
    ary.delete(i)
  end  
}

printf("rest res_size = %d\n", all_ary.length)
printf("not_use_size = %d\n", ary.length)

printf("rest res is ...\n")
all_ary.each{|i|
  p i
  result_file.printf("%s\n",i)
}

printf("\n\n")

printf("not use is ...\n")
ary.each{|i|
  p i
}




#-----------------------------------------------------------------------------------------
#�}�b�v���X�g�ɂ͂��邪�A�}�b�v�}�g���b�N�X�A���v���C�X���琶�����Ă��Ȃ��u���b�N�𒲂ׂ�
#-----------------------------------------------------------------------------------------

check_ary = Array.new
fld_ary = Array.new

#���Ɂu_ex�v�������ڂ�T���āA�u_ex�v������
ary2.each{|i|
  i.sub!(/_ex/,"")
  check_ary << i
}

#�d���폜
check_ary.uniq!

w = mat_hash["WB"].x
MakeFieldMapBlock(csv_fld_matrix, w, fld_ary)

fld_ary.each{|i|
  rc = check_ary.include?(i)
  if rc == true then
    check_ary.delete(i)
  end
}


matrix_ary = Array.new
mat_hash.each{|key,val|
  if val.Prefix != "map" then
    MakeBlock(matrix_ary, val)
  end
}

use_ary.clear
matrix_ary.each{|i|
  rc = check_ary.include?(i)
  if rc == true then
    check_ary.delete(i)
    use_ary << i
  end
}

rep_ary.each{|i|
  rc = check_ary.include?(i)
  if rc == true then
    check_ary.delete(i)
  end
}

printf("rest_maplistdump\n")
check_ary.each{|i|
  p i
}
printf("rest_maplistdump_end\n")

use_ary.each{|i|
  rc = use_ary.include?(i)
  if rc == true then
    matrix_ary.delete(i)
  end
}

printf("rest_matrixdump\n")
matrix_ary.each{|i|
  p i
}
printf("rest_matrixdump_end\n")


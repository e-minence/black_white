###################################################################################
# ARC�t�H���_����naix�őS�\�[�X�R�[�h��GREP���A����������Ȃ��������̂��
# �쐬���F2010/6/4 �쐬�ҁF�ۍ� ����
###################################################################################

ARC_PATH = "c:/home/pokemon_wb/prog/arc"
SRC_PATH = "c:/home/pokemon_wb/prog/src"
SCRIPT_PATH = "c:/home/pokemon_wb/resource/fldmapdata/script"

# �ċA�I��C�t�@�C�������o
def recursive_search(path, word)
  Dir::foreach(path) do |v|
    next if v == "." or v == ".." or v == ".svn"
    if path =~ /\/$/
      v = path + v
    else
      v = path + "/" + v
    end

    ext = File::extname(v)
    if ext == ".c" || ext == ".h" || ext == ".cdat" || ext == ".ev"
      f = open(v)
      tbl = f.read
      if tbl.include?( word ) == true
        $hitflag = true;
        return;
      end
      f.close
    end

    #�f�B���N�g���̏ꍇ�͍ċA
    if FileTest::directory?(v)
      recursive_search(v,word) #�ċA�Ăяo�� ���J�[�V�u�@�R�[��/Recursive Call
    end
  end
end

#======================================================

list = Array.new

# naix�t�@�C���������X�g�A�b�v
Dir::foreach(ARC_PATH) {|filename|
  if File::extname(filename) == ".naix"
    list.push( filename )
  end
}

#puts list

puts "�S�v�f��=" + list.size.to_s


$hitflag = false

puts "�g���Ă��Ȃ���"

list.each{|i|
  $hitflag = false
  recursive_search( SRC_PATH, i )
  recursive_search( SCRIPT_PATH, i )
  #�Ȃ�������o��
  if $hitflag == false
    puts i
  end
}

=begin
=end

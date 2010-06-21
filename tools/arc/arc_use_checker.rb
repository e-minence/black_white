###################################################################################
# naix�t�@�C���Œ�`����Ă���ID����S�\�[�X�R�[�h���Œ�`����Ă��Ȃ����̂𒊏o
# �Enaix��ID��150�s�ȓ��̃t�@�C���Ɍ���
#
# �쐬���F2010/6/4 �쐬�ҁF�ۍ� ����
###################################################################################

ARC_PATH = "c:/home/pokemon_wb/prog/arc"
SRC_PATH = "c:/home/pokemon_wb/prog/src"
LINE_MAX = 150

#�\���̒�`
SData = Struct.new('Data', :filename, :tblname)

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
    if ext == ".c" || ext == ".h" || ext == ".cdat"
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

# �S�Ă�naix�ɑ΂��ď���
Dir::foreach(ARC_PATH) {|filename|
  if File::extname(filename) == ".naix"

    lineno = 0
    path =  ARC_PATH + "/" + filename;

    #�s���𒲂ׂ�
    File.open( path, "r" ){|f|
      while line = f.gets
        if f.eof?
          lineno = f.lineno
        end
      end
    }
    
    #����s�ȏ�Ȃ�X�L�b�v
    next if lineno >= LINE_MAX
#    puts lineno

    #PUSH
    File.open( path, "r" ){|f|
      f.each{|i|
        if i.include?(" = ") == true
          i = i.strip
          i = i.sub(/ = \d*,/, "")
          i = i.sub(/ = \d*/, "")
          list.push( SData.new(filename,i) );
        end
      }
    }
  end
}

#list.push( SData.new( "test.naix" , "NARC_guru2_2d_cook_s_obj0_NCLR" ) )
#list.push( SData.new( "test.naix" , "NARC_guru2_2d_record_s_NCLR" ) )
#list.push( SData.new( "test.naix" , "NARC_guru2_2d_record_s2_NCLR" ) )

#puts list

puts "�S�v�f��=" + list.size.to_s

$hitflag = false

puts "�g���Ă��Ȃ���"

list.each{|i|
  $hitflag = false
  recursive_search( SRC_PATH, i.tblname )
  #�Ȃ������o��
  if $hitflag == false
    #�t�@�C�������o��
    puts i.filename + " >> " + i.tblname
  end
}


=begin
=end

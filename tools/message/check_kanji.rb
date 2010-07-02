###################################################################################
#
# �쐬���F2010/7/2 �쐬�ҁF�ۍ� ����
###################################################################################
#! ruby -Ks

require 'kconv'

#�����R�[�h�w��
$KCODE = 'SJIS'

SRC_MESSAAGE = "../../resource/message/src"

# �Ђ炪�Ȃ̒��Ɋ������Ȃ��� �ꍀ�ڂ��`�F�b�N
def check_elem( str )
  #�^�O���O��
  check = str.gsub(/\[.*\]/, '')
  #��������
  if /[��-��]/ =~ check
    #���ʏo��
    res = str.gsub(/<lang.*">/, '')
    res = res.gsub(/<\/lan.*>/, '')
    puts res
  end
end   

# gmm���̂Ђ炪�Ȃ̒��Ɋ������Ȃ����`�F�b�N
def check_kanji( gmm_file )
  puts "====================================================================="
  puts gmm_file + " > "
  File.open( gmm_file, "r" ){|f|
    str = f.read
    str = Kconv.tosjis(str)
    
    check = ""
    b_head = true
    str.each_line{|l|
    
      if b_head == true
        if l.include?("\"JPN\"")
          check = l
          b_head = false
        end
      end
      
      if b_head == false
        # ����ȊO�s�ǉ�
        if check != l
          check += l
        end
        
        if l.include?("/language")
          # �`�F�b�N
          check_elem( check )
          # �I������
          check = ""
          b_head = true
        end
      end
     
    }
  }
end

# �ċA�I��GMM���J��
def recursive_search(path)
  Dir::foreach(path) do |v|
    next if v == "." or v == ".." or v == ".svn"
    if path =~ /\/$/
      v = path + v
    else
      v = path + "/" + v
    end

    ext = File::extname(v)
    if ext == ".gmm"
      check_kanji( v )
    end

    #�f�B���N�g���̏ꍇ�͍ċA
    if FileTest::directory?(v)
      recursive_search(v) #�ċA�Ăяo�� ���J�[�V�u�@�R�[��/Recursive Call
    end
  end
end


#check_kanji( "bag.gmm" )

recursive_search( SRC_MESSAAGE ) 

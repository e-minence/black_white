###########################################
#
# use_source_check.rb 
# �g�p����Ă��Ȃ��\�[�X�`�F�b�N
#
###########################################


PROGDIR = ENV["PROJECT_PROGDIR"].chomp

def debug_puts arg
  #puts arg
end


#�t�@�C�����z��
class FILE_ARRAY

  attr :array

  def initialize 
    @array = Array.new
  end

  def set path

    add_file = 0

    #*.c or *.h
    if path =~ /.*\.c \\*/ then
      add_file = 1
    end
    if path =~ /.*\.h \\*/ then
      add_file = 1
    end

    #�ǉ��̕K�v�Ȃ��B
    if add_file == 0 then
      return 
    end
    

    path = path.sub( /.*=/, "" )
    path = path.sub( /\s/, "" )
    path = path.sub( /\t/, "" )
    path = path.sub( /\\/, "" ).chomp

    #BaseName��ۑ�
    path = File.basename( path )

    #debug_puts( "set #{path}\n" )

    @array << path
    
  end


  def hit check_path

    check_file = 0

    #*.c or *.h
    if check_path =~ /.*\.c$/ then
      check_file = 1
    end
    if check_path =~ /.*\.h$/ then
      check_file = 1
    end

    #check�̕K�v�Ȃ��B
    if check_file == 0 then
      return 2
    end
    

    check_path = check_path.sub( /.*=/, "" )
    check_path = check_path.sub( /\s/, "" )
    check_path = check_path.sub( /\t/, "" )
    check_path = check_path.sub( /\\/, "" ).chomp

    #BaseName���`�F�b�N
    check_path = File.basename( check_path )
    
    @array.each{|filename|

      if check_path == filename  then
        return 1
      end
    }
    return 0
  end

  def uniq_array

    @array = @array.uniq
    
  end

  def listDump

    debug_puts( "check list\n" )
    @array.each{|filename|
      debug_puts( "#{filename}\n" )
    }
  end
  
end


#1Directory���̃t�@�C�����`�F�b�N
def checkDir( tag_str, path, array )

  puts "#{tag_str}check #{path} ...\n"
  puts "#{tag_str}{"

  Dir.glob("#{path}/*"){|file|
    
    if File.ftype( file ) == "file" then

      if array.hit( file ) == 0 then
        puts "#{tag_str}\t[no_use] #{file}\n"
      end
      
    elsif File.ftype( file ) == "directory" then

      #�ċA
      sub_tag_str = "#{tag_str}\t"
      checkDir( sub_tag_str, file, array )
      
    end
    
  }

  puts "#{tag_str}}\n"
  
end



#�܂��Adepend��make_prog_files����\�[�X���X�g���쐬
all_file_array  = FILE_ARRAY.new


File.open( "#{PROGDIR}/make_prog_files" ){|file|

  file.each{|line|
    all_file_array.set( line )
  }
  
}

Dir.glob( "#{PROGDIR}/depend/ARM9-TS.HYB/Release/*.d" ){|filename|

  File.open( filename ){|file|
    file.each{|line|
      all_file_array.set( line )
    }
  }
}

#�d���f�[�^��r��
all_file_array.uniq_array()
all_file_array.listDump()

#include�ȉ�
checkDir( "", "#{PROGDIR}/include/", all_file_array )

#src�ȉ�
checkDir( "", "#{PROGDIR}/src/", all_file_array )

#arc�ȉ�
checkDir( "", "#{PROGDIR}/arc/", all_file_array )




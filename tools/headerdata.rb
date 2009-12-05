#============================================================================
#
#
#   �w�b�_�ǂݍ��݁���`�Q��
#
#
#
#   2009.12.05  tamada  eventdata�ɗF�炭�񂪒ǉ������@�\��ėp��
#
#
#   C�`���̃w�b�_�t�@�C�������߂��A�����Œ�`����Ă����`���ɂ���
#   ���l�Ƃ��ĕԂ��܂��B
#
#
#   �g�����F
#   require "#{ENV["PROJECT_ROOT"]}tools/headerdata.rb"
#
#   headers = HeaderDataArray.new
#   heeaders.load( �K�؂ȃw�b�_�t�@�C�� )
#   heeaders.load( �K�؂ȃw�b�_�t�@�C�� )
#   heeaders.load( �K�؂ȃw�b�_�t�@�C�� )
#   �c
#
#   begin
#     value = headers.search( "���������`��"�A"�G���[�\���p�̌����ꏊ�w��" )
#   rescue => errors
#     # ������Ȃ��ꍇ�AHeaderSearchError���Ԃ�
#   end
#
#   ���ӓ_�F
#   �R�����g// /* �` */�̉��߂�������Əo���Ă���킯�ł͂Ȃ����߁A
#   �����ɂ������ꍇ���ǂݍ���ł��܂��B�܂��A�R�����g�̈ʒu�ɂ���Ă�
#   �K�؂ɉ��߂ł��Ȃ��ꍇ������
#
#============================================================================
#------------------------------------------------------------------------------
# �w�b�_�[�f�[�^�G���[�N���X
#------------------------------------------------------------------------------
class HeaderSearchError < Exception; end

#------------------------------------------------------------------------------
#�w�b�_�[���������N���X
#------------------------------------------------------------------------------
class HeaderData
  
  def initialize( filename )

    #��`���
    @define         = Hash.new("none")
    @enum_in        = 0
    @enum_count     = 0

    #�w�b�_�[��ǂݍ��݁@�萔���擾����
    load( filename )
  end


  #�ǂݍ���
  def load( filename )
    File.open(filename){|file|
      file.each{|line|

        #�ǉ��C���N���[�h
        if line =~ /#include[\s]+\"([^\"]+)\"/
          next_file = Regexp.last_match[1]
         debug_puts "hit include #{next_file}";
          #���̃t�@�C�����݂�����H
          if File.exist?(next_file)
            debug_puts "include #{next_file}";
            load( next_file )
          end
        end
        
        #�^�̒�`SEARCH
        # define
        if line =~ /#define[\s\t]+([^\s\t]+)[\s\t\(]+([xabcdefABCDEF\d]+)/
          
          #���K�\���Ƀ}�b�`�����������擾
          debug_puts "define #{Regexp.last_match[1]} = #{Regexp.last_match[2]}"
          index = Regexp.last_match[1]
          number = Regexp.last_match[2]
          
          #�܂������ĂȂ��Ƃ��ɂ��������
          if @define.member?(index) == false
            
            if number =~ /x/
              @define[ index ] = number.hex
            else
              @define[ index ] = number.to_i
            end

          end
        end

        # enum
        if @enum_in == 0
          
          if line =~ /enum/
            @enum_in = 1
            @enum_count = 0
          end

        else

          #�I���`�F�b�N
          if line =~ /\}/
            @enum_in = 0
          #����
          else
            if line =~ /[\s\t]+([^\s\t,]+)/
              @enum_name = Regexp.last_match[1]

              if line =~ /[\s\t]+[^\s\t,]+[\s\t]*=[\D]*([\d]+)/
                @enum_count = Regexp.last_match[1].to_i
              end

              #�܂������ĂȂ��Ƃ��ɂ��������
              if @define.member?(index) == false
            
                debug_puts "enum #{@enum_name} = #{@enum_count}"
                @define[ @enum_name ] = @enum_count
              end

              @enum_count += 1
            end
          end

        end
      }
    }
  end


  #��`��T�� 
  #�߂�l
  #0or1 , ���l
  def search name
    if @define.has_key?( name ) then
      @define[name]
    else
      nil
    end
  end
  
end


#------------------------------------------------------------------------------
#�w�b�_�[�f�[�^�z��N���X
#------------------------------------------------------------------------------
class HeaderDataArray

  def initialize
    @headerArray = Hash.new
  end

  def load( filename )
    #�ǂݍ��ݍς݂̂��͓̂ǂݍ��܂Ȃ�
    unless @headerArray.has_key?( filename )
      @headerArray[ filename ] = HeaderData.new( filename )
    end
  end

  def getCount
    return @headerArray.length
  end

  #�S��SEARCH
  def search( name, error_comment )

    #���l�N���X�Ȃ炻�̂܂ܕԂ�
    if name.instance_of?( String ) == false
      return name
    end

    #����������Ȃ�A���̂܂ܕԂ�
    if name =~ /^[+-]*\d/
      debug_puts "number out #{name}"
      return name.to_i
    end

    #�]���ȃX�y�[�X�Ȃǔj��
    name = name.gsub( /\s/,"" )
    
    @headerArray.each do |key, headerData|
      value = headerData.search( name )
      if value != nil
        return value
      end
    end

    puts "#{error_comment}����[#{name}]���݂���܂���" 
    raise HeaderSearchError, "#{error_comment}����[#{name}]���݂���܂���" 
  end
end




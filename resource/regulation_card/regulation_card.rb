#!ruby -Ks
#!/usr/bin/ruby
#
# �J�b�v���{�ΐ탋�[���͔ԍ��Ǘ��ɂ���
# 2010.01.06 k.ohno
# ���M�����[�V�������o�C�i���ɂ���v���O����
# 2009.06.04 k.ohno
#
#

require "jcode"
require 'nkf'

require "#{ENV["PROJECT_ROOT"]}resource/shooter_item/shooter_item_hash.rb" #�V���[�^�[�̃n�b�V���e�[�u�����擾

COL_NO = 0    #No
COL_CUPNAME = 1    #�J�b�v��
COL_RULENAME = 2    #�ΐ탋�[��
COL_PROGRAM_LAVEL = 3    #�v���O�����Ŏg�����x��
COL_NUM_LO = 4    #�Q��������
COL_NUM_HI = 5    #�Q�������
COL_LEVEL = 6    #�Q�����x��
COL_LEVEL_RANGE = 7    #���x���͈�
COL_LEVEL_TOTAL = 8    #���x�����v
COL_BOTH_POKE = 9    #�����|�P����
COL_BOTH_ITEM = 10    #�����ǂ���
COL_VETO_POKE = 11    #�Q���֎~�|�P����
COL_VETO_ITEM = 12    #�������݋֎~����
COL_MUST_POKE = 13    #�K�{�|�P����
COL_MUST_POKE_FORM = 14    #�K�{�|�P�����t�H����
COL_SHOOTER = 15    #�V���[�^�[
COL_VETO_SHOOTER_ITEM = 16 #�V���[�^�[�֎~����
COL_TIME_VS = 17    #�ΐ펞��
COL_TIME_COMMAND = 18    #���͎���
COL_NICKNAME = 19    #�j�b�N�l�[���\��
COL_CAMERA = 20    #�J�������[�h
COL_DUMMY = 21        #���g�p�ł����A�Z�[�u�f�[�^���ς�邽�ߏ������ɂ��̂܂܂ɂ��Ă����܂�
COL_SHOW_POKE = 22    #�|�P������������
COL_TIME_SHOW_POKE = 23 #�|�P����������������
COL_BTL_TYPE = 24 #�o�g���^�C�v
COL_BTL_COUNT = 25 #�퓬�K���
COL_ROM_CODE = 26 #ROM�R�[�h
COL_WORLDCUP_NAME = 27 #��
COL_WORLDCUP_NO = 28 #���ԍ�
COL_LANG_CODE = 29 #����R�[�h
COL_START_YEAR = 30 #�J�n�N
COL_START_MONTH = 31 #�J�n��
COL_START_DAY = 32 #�J�n��
COL_END_YEAR = 33 #�I���N
COL_END_MONTH = 34 #�I����
COL_END_DAY = 35 #�I����
COL_BGM_MODE  = 36 #�ȃ��[�h
COL_SAME_MATCH = 37 #�����l�Ƃ̎���



POKENUM_MAX_BYTE = (656/8)  ##���̂��炢�ɑ����邩�� �W�Q�o�C�g
ITEMNUM_MAX_BYTE = (608/8)  ##���̂��炢�ɂӂ��邩��
SHOOTER_ITEMNUM_MAX_BYTE = 7  

class RegulationBin
  
  def initialize
    
    @HashCupRange = {"�V���O���o�g��"=>0 ,"�_�u���o�g��"=>1 ,"�g���v���o�g��"=>2,
      "���[�e�[�V�����o�g��"=>3, "�}���`�o�g��"=>4, "�V���[�^�[�o�g��"=>5 , "�f�o�b�O�o�g��"=>6 }
    @HashRuleRange = {"���x���T�O"=>0 ,"��������Ȃ�"=>1 ,"�X�^���_�[�h"=>2, "�����_���}�b�`"=>3, "�o�g���T�u�E�F�C"=>4, "�t���b�g"=>5}
    @HashLevelRange = {"�Ȃ�"=>0 ,"�ȏ�"=>1 ,"�ȉ�"=>2, "�ȏ�␳"=>3, "�S�␳"=>4, "�ȉ��␳"=>5}
    @HashOKNG = {"NG"=>0,"OK"=>1, "MANUAL"=>2}
    @HashONOFF = {"OFF"=>0,"ON"=>1, "MANUAL"=>2}
    @HashBATTLE = {"SINGLE"=>0,"DOUBLE"=>1, "TRIPLE"=>2, "ROTATION"=>3, "MULTI"=>4}

    @HashCOUNTRY = {"JAPAN"=>1,"ENGLISH"=>2, "FRANCE"=>3, "ITALY"=>4, "GERMANY"=>5, "SPAIN"=>7, "KOREA"=>8}
    @HashROM = {"ALL"=>0xffffffff,"SAPPHIRE"=>0x2, "RUBY"=>0x4, "EMERALD"=>0x08, "RED"=>0x10,
      "GREEN"=>0x20, "GOLD"=>0x80, "SILVER"=>0x100,"DIAMOND"=>0x800, "PEARL"=>0x1000, "PLATINUM"=>0x2000,
      "COLOSSEUM"=>0x10000, "WHITE"=>0x100000, "BLACK"=>0x200000}
    @HashBGM = { "TRAINER"=>0,"WCS"=>1 }
    @HashCAMERA = { "NORMAL"=>0, "MOVE"=>1, "STOP"=>2 }
    @TempNum = "0"
    @HashPokeGmm = Hash.new
    @HashItemGmm = Hash.new
  end
  
  
  def bitingmm(names, outFH , max, gmmhash)
    bitarray = Array.new
    maxmai = max - 1
    
    for i in 0..maxmai
      bitarray.push(0)
    end
    
    byteorder = Array.new([1,2,4,8,16,32,64,128])
    
    
    arrayname = names.split(',')
    
    arrayname.each{|monsname|
      
      keyno = gmmhash.index(monsname).to_i
      if keyno == nil
        p "�s���ȃ|�P�������A�C�e���������̂Œ�~���܂� " + monsname
        exit(-1)
      end
      bitarray[keyno / 8]  = bitarray[keyno / 8] + byteorder[keyno % 8]
    }
    for i in 0..maxmai
      num = bitarray[i]
      outFH.write([num].pack("c"))
    end
    
    
    
  end
  
  
  
  def bitinrom(names, outFH , gmmhash)
    
    keynoall=0
    arrayname = names.split(',')
    
    arrayname.each{|romname|
      keyno = gmmhash[romname]
      if keyno == nil
        p "�s����ROM�R�[�h���������̂Œ�~���܂� " + romname
        exit(-1)
      end
      keynoall  = keynoall + keyno
    }

    outFH.write([keynoall].pack("l"))
    
  end
  
  
  
  
  def valuefunc(index , value, outFH)
    case index
    when COL_NO    #No
    when COL_CUPNAME    #�J�b�v��
      num = @HashCupRange[value]
      outFH.write([num].pack("c"))
    when COL_RULENAME    #�ΐ탋�[��
      num = @HashRuleRange[value]
      outFH.write([num].pack("c"))
    when COL_NUM_LO    #�Q��������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_NUM_HI    #�Q�������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_LEVEL    #�Q�����x��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_LEVEL_RANGE    #���x���͈�
      num = @HashLevelRange[value]
      outFH.write([num].pack("c"))
    when COL_LEVEL_TOTAL    #���x�����v
      num = value.to_i
      outFH.write([num].pack("s"))
    when COL_BOTH_POKE    #�����|�P����
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_BOTH_ITEM    #�����ǂ���
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_VETO_POKE    #�Q���֎~�|�P����
      if value =~ /�Ȃ�/
        num = 0
        for i in 1..POKENUM_MAX_BYTE
          outFH.write([num].pack("c"))
        end
      else
        bitingmm(value,outFH, POKENUM_MAX_BYTE ,@HashPokeGmm)
      end        
    when COL_VETO_ITEM    #�������݋֎~����
      if value =~ /�Ȃ�/
        num = 0
        for i in 1..ITEMNUM_MAX_BYTE 
          outFH.write([num].pack("c"))
        end
      else
        bitingmm(value,outFH, ITEMNUM_MAX_BYTE ,@HashItemGmm)
      end        
    when COL_MUST_POKE    #�K�{�|�P����
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashPokeGmm.index(value).to_i
        outFH.write([keyno].pack("s"))
      end
    when COL_MUST_POKE_FORM    #�K�{�|�P�����t�H����
      num = value.to_i
      outFH.write([num].pack("c")) 
    when COL_SHOOTER    #�V���[�^�[
      num = @HashONOFF[value]
      outFH.write([num].pack("c"))
    when COL_VETO_SHOOTER_ITEM #�V���[�^�[�֎~����
      if value =~ /�Ȃ�/
        num = 0
        for i in 1..SHOOTER_ITEMNUM_MAX_BYTE
          outFH.write([num].pack("c"))
        end
      else
        bitingmm(value,outFH, SHOOTER_ITEMNUM_MAX_BYTE ,$shooter_item_hash)
      end 
    when COL_TIME_VS    #�ΐ펞��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_TIME_COMMAND    #���͎���
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_NICKNAME    #�j�b�N�l�[���\��
      num = @HashONOFF[value]
      outFH.write([num].pack("c"))
    when COL_CAMERA    #�J�������[�h
      num = @HashCAMERA[value]
      outFH.write([num].pack("c"))
    when COL_DUMMY    #���g�p�ł����A�Z�[�u�f�[�^���ς�邽�ߏ������ɂ��̂܂܂ɂ��Ă����܂�
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_SHOW_POKE    #�|�P������������
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_TIME_SHOW_POKE    #�|�P����������������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_BTL_TYPE  #�o�g���^�C�v
      num = @HashBATTLE[value]
      outFH.write([num].pack("c"))
    when COL_BTL_COUNT    #�퓬�K���
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_WORLDCUP_NAME    #�J�b�v��
      msg = NKF.nkf("-m0 -S --utf16",value)
      i = 0
      mojibyte1 = 0
      msg.each_byte{ | mojibyte |
        if i <= 144 #�������I�[�o�[�����珑�����܂Ȃ�
          if i % 2 == 0
            mojibyte1 = mojibyte
          else
            #���s�R�[�h�Ȃ珑��������
            if mojibyte == 0x5e && mojibyte1 == 0x00
              outFH.putc( 0xfe )
              outFH.putc( 0xff )
            else
              outFH.putc( mojibyte )
              outFH.putc( mojibyte1 )
            end
          end
        end
        i=i+1
      }
      outFH.putc(0xff)  ##�I�[
      outFH.putc(0xff)
      num = 144 - msg.length
      k=0
      while k < num
        outFH.putc(0)
        k=k+1
      end
    when COL_ROM_CODE  #ROM�R�[�h
      if value == "ALL"
        num = 0xffffffff
        outFH.write([num].pack("l"))
      else
        bitinrom(value, outFH, @HashROM)
      end
    when COL_WORLDCUP_NO    #�J�b�vNo
      num = value.to_i
      outFH.write([num].pack("s"))
    when COL_LANG_CODE  #����R�[�h
      num = @HashCOUNTRY[value]
      outFH.write([num].pack("c"))
    when COL_START_YEAR  #�J�n�N
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_START_MONTH  #�J�n��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_START_DAY  #�J�n��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_END_YEAR  #�I���N
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_END_MONTH  #�I����
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_END_DAY  #�I����
      num = value.to_i
      outFH.write([num].pack("c"))
      
      ##�X�e�[�^�X��0
      num = 0
      outFH.write([num].pack("c"))

    when COL_BGM_MODE #�Ȏw��
      num = @HashBGM[value]
      p num
      p value
      outFH.write([num].pack("c"))

    when COL_SAME_MATCH #�����l�Ƃ̎���
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
      
    else
    end
    
    
  end

  
  
  def programfunc(index , value, outFH)
    case index
    when COL_NO               # No
      @TempNum = value
      
    when COL_PROGRAM_LAVEL    # Program
      num = value.to_i
      num = @TempNum.to_i
      outFH.puts( "#define REG_" + value + " " +  num.to_s)
    else
    end
  end

  def headerplus(outHeader)
    outHeader.puts("//���M�����[�V�����̃f�[�^�����o�����x���ł�")
    outHeader.puts("//regulation.rb regulation.xls ����̎��������ł�")
    outHeader.puts("#pragma once")
  end
  
  
  def convline( tabfile, outfile,outheader )
    
    outHeader = File.new(outheader,"w")
    
    headerplus(outHeader)
    
    #��U���s�R�[�h�����H���邽�߂�
    #�o�b�t�@�Ɏ󂯎��
    fpr = File.new(tabfile,"r")

    lineindex=0
    fpr.each{ |line|
      index = 0
      if line =~ /^\d+/
      elsif line =~ /^No/
        next
      else
        break
      end
      outFH = File.new("regulation" + sprintf("%03d",lineindex) + ".bin","wb")
      tabarray = line.split(/\t+/)
      tabarray.each{ |value|
        valuefunc(index, value, outFH)
        programfunc(index, value, outHeader)
        index = index + 1
      }
      outFH.close
      lineindex = lineindex + 1
    }
    fpr.close
    outHeader.close
    
  end

  def gmmread( gmmfile , hashgmm)
    fpr = File.new(gmmfile,"r")
    pokeno = 0
    fpr.each{ |line|
      if line =~ /<row\s+id="MONSNAME_(\d+)">/
        pokeno = $1
      elsif line =~ /<row\s+id="ITEMNAME_(\d+)">/
        pokeno = $1
      elsif pokeno != 0
        line = NKF.nkf('-m0 -Ws',line)
        if line =~ /<language.name=.JPN.*>(.*)<\/language>/
          monsname = $1
          hashgmm.store(pokeno, monsname)
        end
        pokeno = 0
      end
    }
    fpr.close
  end
  
  
  def allgmmread(pokegmm,itemgmm)
  
    gmmread(pokegmm, @HashPokeGmm)
    gmmread(itemgmm, @HashItemGmm)
  end
  
  
  
end

begin
  
  
  
  
  dep = RegulationBin.new
  dep.allgmmread(ARGV[3],ARGV[4])
  dep.convline( ARGV[0],ARGV[1],ARGV[2] )
  
  
  exit(0)
end

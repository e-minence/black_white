#!/usr/bin/ruby -Ks
# �t�@�C�����e���r���Ĉ���Ă�����R�s�[����
#  > diffcopy.rb �R�s�[���t�@�C��... �R�s�[��f�B���N�g��
#    �� �R�s�[���t�@�C���͕����w��ł���
#  ���̃c�[���͓��삪�d���̂���_
#  �o���i��diffcopy��shell�ŏ�����Ă���̂ő����ł���
#  �������naix�̓��t��ǂݔ�΂��Ĕ�r���Ă��܂�

require 'ftools'


def naixcheck(baseHandle, targetHandle)
  
  baseHandle.each {|baseline|
    targetline = targetHandle.gets()
    if /^\tUpdate:/ =~ baseline
    elsif targetline == baseline
    else
      return false
    end
  }
  return true
end

begin
  exit 1 if ARGV.size < 2

  destdir = ARGV.pop
  exit 1 unless FileTest.directory?(destdir)
  destdir = destdir.sub(/\/$/, '') # �Ō�� / ������Ύ��

  ARGV.each do |sfile|
    basename = File.basename(sfile)
    extname = File.extname(sfile)
    target = destdir + '/' + basename
    nIgnore = false
    if FileTest.exist?(target)      # �R�s�[�悪���ɑ��݂��A
      if extname == ".naix" #naix�Ȃ�Ǝ�����
        baseHandle = File::open(basename)
        targetHandle = File::open(target)
        nIgnore = naixcheck(baseHandle, targetHandle)
        baseHandle.close
        targetHandle.close
      elsif File.compare(sfile, target)   # ���e��������������
        nIgnore = true
      else
      end
    end
    if nIgnore == false
      #���R�s�[����
      File.copy(sfile, target)
      puts "copied: #{basename} -> #{target}"
    end
  end


end

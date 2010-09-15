$KCODE = "SJIS"

class SeachIF_SW
  @StartCodeAry
  @EndCodeAry
  def initialize
    @StartCodeAry = ["IF","SWITCH"]
    @EndCodeAry = ["ENDIF","ENDSWITCH"]
  end

  def SearchStartCode(str)
    fix_str = Marshal.load(Marshal.dump(str))
    fix_str.gsub!(/ /,"")
    fix_str.gsub!(/\t/,"")

    if fix_str.length == 0 then
      return -1
    end

    @StartCodeAry.each_with_index{|code, idx|
      n = fix_str.index(code)
      if n != nil then
        if n == 0 then
          #���o
          return idx
        end
      end
      return -1
    }
  end

   def SearchEndCode(str, idx)
    fix_str = Marshal.load(Marshal.dump(str))
    fix_str.gsub!(/ /,"")
    fix_str.gsub!(/\t/,"")
  
    code = @EndCodeAry[idx]

    n = fix_str.index(code)
    if n != nil then
      if n == 0 then
        #���o
        return true
      end
    end
    return false
  end

end

def IsRet(str)
  fix_str = Marshal.load(Marshal.dump(str))
  fix_str.gsub!(/ /,"")
  fix_str.gsub!(/\t/,"")

  if fix_str == "_RET" then
    return true
  end

  return false
end


class SubLabelHash
  @ifsw
  @label_hash
  @if_count
  def initialize
    @ifsw = SeachIF_SW.new
    @label_hash = Hash.new
    @if_count = 0
  end

  def MakeHash(filename)
    file = open(filename,"r")

    mode = 0
    label = ""
    ary = Array.new
    while line = file.gets
      str = line.chomp("\n").chomp("\r")
      case mode
      when 0 then     #���x���T��
        rc = str.include?(":")
        if rc == true then
          m = /:/.match(str)
          label = m.pre_match
          ary.clear       #�z��N���A
          mode = 1        #���x���I�����o�J�n
        end
      when 1 then         #���x���I���T���i_RET�j
        retval = @ifsw.SearchStartCode(str)
        if retval == 0 then
          #ENDIF�T�����J�n
          mode = 2
          if @if_count != 0 then
            p "ERROR!  IF �J�E���^�G���["
            exit(-1)
          end
          @if_count += 1
        elsif retval == 1 then
          #ENDSWITCH�T�����J�n
          mode = 3
        end

        rc = IsRet(str)
        if rc == true then  #_RET���o�@���x���I��
          #���Ƀ��x�����邩���`�F�b�N
          rc = @label_hash.key?(label)
          if rc == true then
            printf("���x�������łɂ���@%s\n",label)
            exit(-1)
          end  
          #���ߍ��񂾕����z����n�b�V���ɓo�^
          copy_ary = Marshal.load(Marshal.dump(ary))
          @label_hash[label] = copy_ary
          #���̃��x���T���J�n
          mode = 0
        end
      when 2 then         #ENDIF�T��
        rc = @ifsw.SearchEndCode(str, 0)
        if rc == true then
          @if_count -= 1
          if @if_count == 0 then
            #_RET�T���ɖ߂�
            mode = 1
          end
        else
          retval = @ifsw.SearchStartCode(str)
          if retval == 0 then
            #IF�݂��Ă��܂���
            @if_count += 1
          end
        end 
      when 3 then         #ENDSWITCH�T��
        rc = @ifsw.SearchEndCode(str, 1)
        if rc == true then
          #_RET�T���ɖ߂�
          mode = 1
        end
      end

      if mode != 0 then
        ary << str
      end
    end

    file.close

    return GetHash()
  end

  def Dump()
    #�_���v���Ă݂�
    p "============================"
    @label_hash.each{|key, val|
      printf("--%s--\n",key)
      val.each{|i|
        p i
      }
    }
  end

  def GetHash()
    return @label_hash
  end

end   #class


def _SearchCode(code_ary, str, find_code_ary)
  code_ary.each{|i|
    target = str.index(i)
    if target != nil then     #��������
      if find_code_ary != nil then
        find_code_ary << i
      end  
      return true
    end
  }
  return false
end

def CallFunc(hash, code_ary, call_label, call_find_code_ary)
  local_ary = Array.new

  #�n�b�V�����烉�x���Ɉ�v����z����擾
  ary = hash[call_label]
  if ary == nil then
    printf("not_label %s\n",call_label)
    return
  end

  #�z�������
  ary.each_with_index{|str,idx|
    local_ary.clear
    #�����R�[�h���ׂ�
    rc = _SearchCode(code_ary, str, local_ary)
    if rc == true then
      #_CALL���H
      not_space_str = str.gsub(/\s/,"")
      target = not_space_str.index("_CALL")
      if (target != nil)&&(target == 0) then     #��������
#        printf("str = %s\n",str)
        m = /_/.match(str)
        fix_str = "_" + m.post_match
        m = /\s/.match(fix_str)
        if m != nil then
          call_str = m.pre_match
          label_str = m.post_match
          call_str.gsub!(/\s/,"")
          label_str.gsub!(/\s/,"")
          if call_str == "_CALL" then
#           printf("CALL�𔭌�\n")
            CallFunc(hash, code_ary, label_str, call_find_code_ary)
          end
        end
      else        #_CALL �ȊO
#        printf("CALL���Ŕ���\n")
        make_str = call_label + "-->" + local_ary[0] + "::" + idx.to_s
        call_find_code_ary << make_str
      end
    end
  }
end

def CallFuncParent(hash, code_ary, str, call_find_code_ary)
  #_CALL���H
  not_space_str = str.gsub(/\s/,"")
  target = not_space_str.index("_CALL")
  if (target != nil)&&(target == 0) then     #��������
#    printf("PARENT CALL�𔭌� %s\n",str)
    m = /_/.match(str)
    fix_str = "_" + m.post_match
    rc = fix_str.index("\s")
    if rc =! nil then
      m = /\s/.match(fix_str)
      if m != nil then
        call_str = m.pre_match
        label_str = m.post_match
        call_str.gsub!(/\s/,"")
        label_str.gsub!(/\s/,"")
#       printf("PARENT CALL���� %s\n",call_str)
        if call_str == "_CALL" then
          CallFunc(hash, code_ary, label_str, call_find_code_ary)
        end
      else
#        printf("%s �͏��O���܂�\n",fix_str)
      end
    else
#      printf("%s �͏��O���܂�\n",fix_str)
    end
  end
end

def GetCallLabel(str)
  #_CALL���H
  not_space_str = str.gsub(/\s/,"")
  target = not_space_str.index("_CALL")
  if (target != nil)&&(target == 0) then     #��������
#    printf("PARENT CALL�𔭌� %s\n",str)
    m = /_/.match(str)
    fix_str = "_" + m.post_match
    rc = fix_str.index("\s")
    if rc =! nil then
      m = /\s/.match(fix_str)
      if m != nil then
        call_str = m.pre_match
        label_str = m.post_match
        call_str.gsub!(/\s/,"")
        label_str.gsub!(/\s/,"")
#       printf("PARENT CALL���� %s\n",call_str)
        if call_str == "_CALL" then
          return label_str
        end
      else
#        printf("%s �͏��O���܂�\n",fix_str)
      end
    else
#      printf("%s �͏��O���܂�\n",fix_str)
    end
  end

  return nil
end


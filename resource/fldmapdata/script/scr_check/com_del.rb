#�R�����g���폜���āA�ďo�͂��� saito

$KCODE = "SJIS"

#ret: 0�q�b�g�����@1:�u//�v���q�b�g 2:�u/*�v���q�b�g
def CheckCommentStartCode(sra_sta, sra_sra)
  if (sra_sta == nil)&&(sra_sra == nil) then
    #�R�����g����
    return 0
  elsif (sra_sta == nil)&&(sra_sra != nil) then
    #�u//�v���q�b�g
    return 1
  elsif (sra_sta != nil)&&(sra_sra == nil) then
    #�u/*�v���q�b�g
    return 2
  elsif (sra_sta > sra_sra ) then
    #�u//�v���D��q�b�g
    return 1
  elsif (sra_sta < sra_sra ) then
    #�u/*�v���D��q�b�g
    return 2
  end

  return 0
end

def CheckComSraSta_StaSra(str, sra_sta)
  sra_sta_local = sra_sta
  begin
    sta_sra = str.index("*/")
    if sta_sra != nil then   #��������
      #�u*/�v�܂ō폜
      if sra_sta_local >= sta_sra then
        printf("�p�[�X�G���[")
        exit -1
      end  
      size = sta_sra - sra_sta_local + 2
      str[sra_sta_local,size] = ""
    else                      #������Ȃ�����
      #�ȍ~���폜�����̍s��
      l = str.length
      size = l - sra_sta_local
      str[sra_sta_local,size] = ""  #todo
      return 1      #�u*/�v�������[�h�ŏI���
    end

    sra_sta_local = str.index("/*")
  end while sra_sta_local != nil

  return 0          #�ʏ탂�[�h�ŏI���
end

#printf("%s --> %s\n",ARGV[0],ARGV[1])
ev_file = open(ARGV[0],"r")
tmp_file = open(ARGV[1],"w")

mode = 0
while line = ev_file.gets
  str = line.chomp("\n").chomp("\r")

  #���[�h�ŕ���
  if mode == 0 then               #�ʏ탂�[�h
    #�u/*�v������
    sra_sta = str.index("/*")
    #�u//�v������
    sra_sra = str.index("//")
    #�ǂ�������ɏo�Ă������H
    rc = CheckCommentStartCode(sra_sta, sra_sra)

    if rc == 1 then       #�u//�v�̏ꍇ�A�ȍ~���폜�����̍s��
      l = str.length
      size = l-sra_sra
      str[sra_sra,size] = "" #todo
      if sra_sra != 0 then    #�s�J�n���u//�v�łȂ��Ƃ��̓v�����g����
        tmp_file.printf("%s\n",str)
      end
      mode = 0
      next
    elsif rc == 2 then    #�u/*�v�̏ꍇ�A�u*/�v������
      mode = CheckComSraSta_StaSra(str, sra_sta)
      tmp_file.printf("%s\n",str)
    else                  #�R�����g����
      tmp_file.printf("%s\n",str)
    end
  else                        #�u*/�v���������[�h
    #�u*/�v������
#    printf("�R�����g�s %s\n",str)
    sta_sra = str.index("*/")
    if sta_sra == nil then    #������Ȃ������̍s�͑S���R�����g�Ȃ̂ŏ����Ȃ�
      next
    else                      #�����u*/�v�܂ł��폜
      size = sta_sra+2
      str[0,size] = ""
      tmp_file.printf("%s\n",str)
      mode = 0
    end
  end
end


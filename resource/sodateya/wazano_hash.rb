#===================================================================================
# �Z�ԍ��n�b�V��
#
# $wazano_hash[ �Z�� ] �ŋZ�ԍ����擾
#===================================================================================
$wazano_hash = Hash::new

# �Z�n�b�V���쐬
file = File.open( "../../prog/include/waza_tool/wazano_def.h", "r" )
name = String::new
file.each do |line|
  if line.index("WAZANO_MAX")!=nil then break end
  if line.index("//")!=nil then
    i0 = line.index("//") + 2
    i1 = line.length - 1
    name = line[i0..i1]
    name.strip!
  elsif line.index("define")!=nil then
    i0 = line.index("(") + 1
    i1 = line.index(")") - 1
    str = line[i0..i1]
    no = str.to_i
    $wazano_hash[name] = no
  end
end
file.close


#-----------------------------------------------------------------------------------
# @brief �Z�i���o�[���擾����
# @param name �Z��
# @return �Z�i���o�[
#-----------------------------------------------------------------------------------
def GetWazaNo( name )
  no = $wazano_hash[name]
  if no==nil then
    abort( "[#{name}]�͋Z�n�b�V���ɓo�^����Ă��܂���B" )
  end
  return no
end

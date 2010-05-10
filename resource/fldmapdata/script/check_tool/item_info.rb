
#----------------------------------------------------------------------------------
# @brief  �A�C�e�������擾����
# @param  label ���x����
# @return �w�胉�x���̃A�C�e����
#----------------------------------------------------------------------------------
def GetItemName( label )

	file = File::open("itemsym.h", "r");
	file_line = file.readlines
	file.close

	file_line.each do |line|
		if line =~ /#define.*#{label}.*\/\/(.*)/ then 
			return $1.strip # �O��̋󔒂��폜
		end
	end

	abort("error: �A�C�e��(#{label})�͒�`����Ă��܂���B")
end

#----------------------------------------------------------------------------------
# @brief  �]�[��ID���擾����
# @param  filename .ev �t�@�C����
# @return �]�[��ID
#----------------------------------------------------------------------------------
def GetZoneID_byEvFileName( filename )
  zone_id = File.basename( filename, ".ev" )
  zone_id = zone_id.upcase
  return zone_id
end


class ItemInfo

  def initialize
    @file_name  = nil
    @zone_id    = nil
    @zone_name  = nil
    @item_label = nil
    @item_name  = nil
  end

  attr_accessor :file_name, :zone_id, :zone_name, :item_label, :item_name
end

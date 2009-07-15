###############################################################
#
#   tamada GAMEFREAK inc.
#   2009.07.15
#
###############################################################

require "fileutils"

SCRMSG_PATH = "../../message/src/script/"


def readIDs( filename )
  zone_ids = Array.new
  File.open(filename){|file|
    file.gets #先頭行読み捨て
    while line = file.gets
      column = line.split
      id = column[0].downcase
      if id == "end" then break end
      if zone_ids.index(id) != nil then
        raise Exception, "#{id}が複数あります"
      end
      zone_ids << id
    end
  }
  return zone_ids
end

def makeGMMs( zone_ids )
  zone_ids.each{|id|
    target = SCRMSG_PATH + id + ".gmm"
    unless FileTest.exists?(target) then
      FileUtils.cp("dummyfiles/dummy.gmm", target)
      system ("svn add " + target)
      puts "#{id}.gmm not found. made it!!"
    end
  }
end

ids = readIDs( ARGV[0] )
makeGMMs( ids )


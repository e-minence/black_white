###############################################################
#
#   tamada GAMEFREAK inc.
#   2009.07.15
#
###############################################################

require "fileutils"

SCRMSG_PATH = "../../message/src/script/"
SCRIPT_PATH = "../script/"


###############################################################
###############################################################

#--------------------------------------------------------------
#--------------------------------------------------------------
def readIDs( filename )
  zone_ids = Array.new
  File.open(filename){|file|
    file.gets #�擪�s�ǂݎ̂�
    while line = file.gets
      column = line.split
      id = column[0].downcase
      if id == "end" then break end
      if zone_ids.index(id) != nil then
        raise Exception, "#{id}����������܂�"
      end
      zone_ids << id
    end
  }
  return zone_ids
end

#--------------------------------------------------------------
#--------------------------------------------------------------
def makeGMMs( zone_ids )
  zone_ids.each{|id|
    target = SCRMSG_PATH + id + ".gmm"
    unless FileTest.exists?(target) then
      FileUtils.cp("dummyfiles/dummy.gmm", target)
      #system ("svn add " + target)
      puts "#{id}.gmm not found. made it!!"
    end
  }
end

#--------------------------------------------------------------
#--------------------------------------------------------------
def makeScripts( zone_ids )
  zone_ids.each{|id|
    scrfile = SCRIPT_PATH + id + ".ev"
    sp_scrfile = SCRIPT_PATH + "sp_" + id + ".ev"
    command = "ruby ../script/scr_tools/make_dummy_script.rb ../script #{id}"
      system (command)
    unless FileTest.exists?(scrfile) then
      puts "#{command}"
      system (command)
      #FileUtils.cp("dummyfiles/dumm
    end
  }
end

def makeScriptsIndex( zone_ids )
  File.open("../script/zone_script.list","w"){|file|
    file.puts "#���������t�@�C�� zone_script.list �擪"
    file.puts "SCRIPTFILES = \\"
    zone_ids.each{|id|
      file.puts "\t#{id}.ev \\"
      file.puts "\tsp_#{id}.ev \\"
    }
    file.puts "\n"
    file.puts "#���������t�@�C�� zone_script.list ����"
  }

  File.open("../script/zone_script_bin.list","w"){|file|
    file.puts "#���������t�@�C�� zone_script_bin.list �擪"
    zone_ids.each{|id|
      file.puts "\"#{id}.ev\""
      file.puts "\"sp_#{id}.ev\""
    }
    file.puts "#���������t�@�C�� zone_script_bin.list ����"
  }
end

###############################################################
###############################################################
ids = readIDs( ARGV[0] )
makeGMMs( ids )
makeScripts( ids )
makeScriptsIndex( ids )


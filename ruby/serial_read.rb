# Simplest ruby program to read from arduino serial, using the SerialPort gem
# (http://rubygems.org/gems/serialport)
# To get access to the port : sudo chmod a+rw /dev/ttyUSB0
 
require "serialport"
require "google_drive"        # sudo gem install google_drive
require "google/api_client"


# ------------------- Class Definition -------------------

class Weather
  attr_accessor :snapshot_date
  attr_accessor :humidity
  attr_accessor :temperature
  attr_accessor :snapshot_date

  def initialize()
    @snapshot_date = ""
    @humidity      = ""
    @temperature   = ""
  end
end


# ------------------- Support Function -------------------

def write_to_file(line)

t = Time.now()
t = t.strftime("%Y-%m-%d %H:%M:%S") + ";"
#puts t
line = t.to_s + line
puts "full line: " + line

  open('weather.csv', 'a') do |f|
    f.puts line
  end

end


def write_to_gsheet(weather)

  # Write in the Console
  puts "Date: " + weather.snapshot_date.to_s
  puts "  T*: " + weather.temperature.to_s + "  -  H%: " + weather.humidity.to_s
  puts "---------------------------\n"


  # Initialize the session
  session = GoogleDrive.saved_session("gsheet.json")  # API keys

  # Load the sheet and go to the next empty row
  ws  = session.spreadsheet_by_key("1G6PDL8AycQ4HPYaC-Em3Wf5KJWOmf-NzfvNLYPU3Fws").worksheets[0]
  row = get_next_empty_row(ws)

  ws[row, 1] = weather.snapshot_date
  ws[row, 2] = weather.humidity
  ws[row, 3] = weather.temperature
  ws.save

end


def get_next_empty_row(ws)

  empty_line = false
  row = 0

  while empty_line == false
    row += 1

    if ws[row, 1] == ""
      empty_line = true
    end      
  end

  return row
end





#########################################################################################
# ------------------------------------  Main Program  -----------------------------------
#########################################################################################


# params for serial port
port_str = "/dev/ttyUSB0"   # Arduino Uno
#port_str = "/dev/ttyACM0"  # Arduino Micro
baud_rate = 9600
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE
 
sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

puts "\n----- Weather Station -----\n\n"

line = "" # Build the line 1 char at the time, use the \n as EOL

weather = Weather.new()

# Read serial port forever
while(true)

   while (char = sp.gets) do       # see note 2
      
      if char == ";"         # Structure is xx.xx;xx.xx\n 1st value is humidity, 2nd is temp 
        t = Time.now()
        weather.snapshot_date = t.strftime("%Y-%m-%d %H:%M:%S")   
        weather.humidity = line
        line = ""
      
      elsif char == "\n"
        weather.temperature = line
        write_to_gsheet(weather)
        line = ""
      
      else
        line += char

      end 

    end

end
 
sp.close                       #see note 1


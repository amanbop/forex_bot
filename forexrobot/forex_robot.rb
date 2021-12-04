module ForexRobot
  class PrepareData
  require 'smarter_csv'
  require 'gnuplot'
  
  
  def load_tick_data_csv(filepath)
    #my_chunk_size = 10
    #my_key_mapping = {:date => :first, :bid => :second, :ask=>:third}
    @data = SmarterCSV.process(filepath)
  end
  
      
  def parse_date_time(data=[])
    @data_with_date_time = []
    data.each_with_index do |row,idx|
      @data_with_date_time[idx] = row
      @data_with_date_time[idx][:year] = row[:date].to_s[0..3]
      @data_with_date_time[idx][:month] = row[:date].to_s[4..5]
      @data_with_date_time[idx][:day] = row[:date].to_s[6..7]
      @data_with_date_time[idx][:hour] = row[:date].to_s[8..9]
      @data_with_date_time[idx][:minute] = row[:date].to_s[10..11]
      @data_with_date_time[idx][:second] = row[:date].to_s[12..13]
      @data_with_date_time[idx][:subsecond] = row[:date].to_s[14..17]
    end
  end
  
  def calc_spread(data=[])
    @tick_data_with_spread = []
    data.each_with_index do |row,idx|
      @tick_data_with_spread[idx] = row
      @tick_data_with_spread[idx][:spread] = ((row[:ask]-row[:bid])*10000).round(2)
    end
  end
  
  def calc_ask_change(data=[])
    @tick_data_with_ask_change = []
    @tick_data_with_ask_change[0] = data[0]
    @tick_data_with_ask_change[0][:ask_change] = 0
    data.each_with_index do |row,idx|
      if idx > 0      
        @tick_data_with_ask_change[idx] = row
        @tick_data_with_ask_change[idx][:ask_change] = ((row[:ask]-data[idx-1][:ask])*10000).round(2)
      end
    end
  end#calc_ask_change(data=[])
  
  
  def calc_ask_accel(data=[])
    @tick_data_with_ask_accel = []
    @tick_data_with_ask_accel[0] = data[0]
    @tick_data_with_ask_accel[0][:ask_accel] = 0
    data.each_with_index do |row,idx|
      if idx > 0      
        @tick_data_with_ask_accel[idx] = row
        @tick_data_with_ask_accel[idx][:ask_accel] = ((row[:ask_change]-data[idx-1][:ask_change])).round(2)
      end
    end
  end#calc_ask_accel(data=[])
  
  def pluck_range(args)
    #code
  end
  
  def plot_data(data,element,title,xlabel,ylabel,point_or_line,width,data_title)
    element_data = data.collect {|d| d[element] }
    date_data = data.collect {|d| d[:hour]+d[:minute]+d[:second] }
    data_to_plot = [date_data, element_data]
    draw_graph(data_to_plot,title,xlabel,ylabel,point_or_line,width,data_title)    
  end  
  
  
  def draw_graph(data=[],title,xlabel,ylabel,point_or_line,width,data_title)
    Gnuplot.open do |gp|
      Gnuplot::Plot.new( gp ) do |plot|  
        plot.title  title
        plot.xlabel xlabel
        plot.ylabel ylabel
        plot.data << Gnuplot::DataSet.new(data) do |ds|
          ds.with = point_or_line
          ds.linewidth = width
          ds.title = data_title
        end
      end#Gnuplot::Plot.new( gp ) do |plot|  
    end#Gnuplot.open do |gp|    
  end#def plot_data(data=[])
  
  def data
    @data
  end


  def show_data(data=[],number_of_rows)  
    @show_data = data[1..number_of_rows].each_with_index { |row, idx|   puts idx.to_s+ ' '+row.inspect }
  end
    
  end#class PrepareData
end#module ForexRobot


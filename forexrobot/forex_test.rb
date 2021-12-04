require 'bigdecimal'
require 'bigdecimal/util'
require 'gnuplot'


filepath = '/home/adminuser/Documents/forex_development/data_download/GBPUSD/GBPUSD1d.txt'
filepath = '/home/adminuser/Documents/forex_development/data_download/GBPUSD/GBPUSD1d_100l.txt'
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
  
    plot.title  "GBP USD 1d"
    plot.ylabel "Price"
    plot.xlabel "Date"

    f = File.open(filepath)
    start_from_row = 11950
    rows_to_process = 100
    end_at_row = start_from_row + rows_to_process
    rcounter = 0
    date = []
    bid = []
    ask = []
    avgp = []
    spread = []
    askchange = []
    askchange[0] = 0
    
    puts '#  '+'Date            ' + 'Bid      ' + 'Ask      ' + 'Avg      ' + 'Spread'+'Askchange'
    #puts 'Running'
      f.each_line.with_index do |row, ri|
        
            if (ri >= start_from_row) and (ri < end_at_row)
              cols = row.split(',')
              date[rcounter] = cols[0]
              bid[rcounter] = cols[1].to_f
              ask[rcounter] = cols[2].to_f
              avgp[rcounter] = ((ask[rcounter]+bid[rcounter])/2).round(5)
              spread[rcounter] = ((ask[rcounter]-bid[rcounter])*10000).round(2)
              askchange[rcounter] = ((ask[rcounter-1]-ask[rcounter])*10000).round(4) if ((ri > start_from_row) and (ri < end_at_row))
              puts ri.to_s+' '+ date[rcounter].to_s + '  ' + bid[rcounter].to_s + '  '+ ask[rcounter].to_s+ '  '+avgp[rcounter].to_s+ '  '+ spread[rcounter].to_s+'  '+askchange[rcounter].to_s
            rcounter +=1
            end
      end
      #plot.data << Gnuplot::DataSet.new(bid) do |ds|
      #  ds.with = "lines"
      #  ds.linewidth = 1
      #end
      #plot.data << Gnuplot::DataSet.new(ask) do |ds|
      #  ds.with = "lines"
      #  ds.linewidth = 1
      #end
      #plot.data << Gnuplot::DataSet.new(spread) do |ds|
      #  ds.with = "lines"
      #  ds.linewidth = 1
      #end
      plot.data << Gnuplot::DataSet.new(askchange) do |ds|
        ds.with = "points"
        ds.linewidth = 1
      end

  end#Gnuplot::Plot.new( gp ) do |plot|  
end#Gnuplot.open do |gp|

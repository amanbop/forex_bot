require 'gnuplot'
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
  
    plot.title  "Array Test"
    plot.ylabel "Y axis"
    plot.xlabel "X Axis"
    vals1 = [2,5,4,2,2,1,3,1,3]
    vals2 = [3,6,2,4,1,2,1,3,5]
    vals3 = [0.6,0.5,0.5,0.4,0.9,0.2,0.2,0.9]
    
    
    plot.data << Gnuplot::DataSet.new(vals1) do |ds|
      ds.with = "lines"
      ds.linewidth = 4
    end
    plot.data << Gnuplot::DataSet.new(vals2) do |ds|
      ds.with = "lines"
      ds.linewidth = 4
    end    
    plot.data << Gnuplot::DataSet.new(vals3) do |ds|
      ds.with = "lines"
      ds.linewidth = 4
    end    
    
  end  
end

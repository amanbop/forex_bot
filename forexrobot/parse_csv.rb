class ForexRobot
require 'smarter_csv'
require 'gnuplot'




#my_chunk_size = 10
#my_key_mapping = {:date => :first, :bid => :second, :ask=>:third}

@tick_data = SmarterCSV.process(filepath)

@tick_data[1..10].each_with_index { |row, idx|   puts idx.to_s+ ' '+row.inspect }

end


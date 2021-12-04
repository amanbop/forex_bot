filename = '../AUD_CAD_Week1.csv'
f = File.open(filename)
f.each_with_index do |row,row_index|
AudCadWeek12015Rate.create(:tid=>row[0],
                               :dealable=>row[1],
                               :currency_pair=>row[2],
                               :date_time=>row[3],
                               :bid=>row[4],
                               :ask=>row[5]
                               )

end
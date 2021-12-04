load 'forex_robot.rb'
filepath = '/home/adminuser/Documents/forex_development/data_download/GBPUSD/GBPUSD1d_100l.txt'
number_of_rows = 10
c = ForexRobot::PrepareData.new
raw_data = c.load_tick_data_csv(filepath)
data = c.parse_date_time(raw_data)
data_with_spread = c.calc_spread(data)
data_with_ask_change = c.calc_ask_change(data_with_spread)
data_with_ask_accel = c.calc_ask_accel(data_with_ask_change)
c.plot_data(data,:ask,'GBPUSD','Price','Date','linespoints',2,'ask')
#c.plot_data(data,:ask,'GBPUSD','Price','Date','line',1)
#c.plot_data(data_with_ask_change,:ask_change,'GBPUSD','Price','Date','point',1)
c.show_data(data_with_ask_change,number_of_rows)

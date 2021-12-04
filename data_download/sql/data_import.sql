LOAD DATA LOCAL INFILE 'AUD_CAD_Week1.csv'
INTO TABLE aud_cad_week12015_rates
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\r\n'
IGNORE 1 LINES
(tid,dealable,currency_pair,date_time,bid,ask);

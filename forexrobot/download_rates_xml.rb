require 'oga'
require 'nokogiri'
require 'open-uri'
url = 'http://rates.fxcm.com/RatesXML3'

doc = Nokogiri::XML(open(url))
rate_feed = doc.css("Rates Rate")
rates =  rate_feed.collect{|m| m.children.reject{|c| c.name =="text"}.collect{|c| {c.name=>c.text}}}
puts rates[0..3]


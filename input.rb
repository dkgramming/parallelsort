INPUT_SIZE = Integer(ARGV[0])

puts INPUT_SIZE

INPUT_SIZE.times do
  print "#{Integer(rand * 100)} "
end

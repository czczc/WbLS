#!/usr/bin/env ruby -wKU

# desc = 'kaon_105MeV_py90'
desc = 'muon_140-240MeV_py0'

range = (1..1)

range.each do |i|
  ii = sprintf("%03d", i)
  cmd  = %{root -b -q 'stats.C(}
  cmd += %{"../tree/#{desc}/tree#{ii}.root",}
  cmd += %{"../stats/#{desc}/stats#{ii}.root")'}
  puts cmd
  system(cmd)
end


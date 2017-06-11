--[[ret=l_dir("/home/c6s")
for k,v in ipairs(ret) do
   print(k,v)
end
io.write(inputStr)]]
ss = tostring(inputStr)
send("from lua script echos :" .. ss)
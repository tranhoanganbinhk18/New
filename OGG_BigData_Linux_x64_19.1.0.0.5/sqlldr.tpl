# 
# File Names
#
controlfile ?target.ctl
runfile     ?target.run

#
# Run File Template
#
sqlldr userid=?pw control=?target log=?target direct=true
#
# Control File Template
#
unrecoverable
load data
infile ?source.dat
truncate
into table ?target

# 
# File Names
#
controlfile ?target.fmt
runfile     ?target.bat

#
# Run File Template
#
# Substitute your database name for <db>
#
bcp <db>..?target in ?source.dat -U ?user -P ?pw -f ?target.fmt -e ?target.err
#
# Control File Template
#
# The value below must specify the BCP version, not the Sybase Adaptive Server
# or Microsoft SQL Server version.
# "bcp -v" can be used to determine the correct version number.
#
12.0

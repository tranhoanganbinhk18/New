#!/bin/bash -x
#
# run java user-exit driver, with a custom "message formatter" which simply sends the same text message over & over to the server.
#  see: dirprm/javaue.properties

# java -classpath ../javaue/ggue.jar:./dirprm -Dlog4j.configuration=log4j.properties -DGGS_USEREXIT_CONF=./dirprm/javaue.properties.wmq gg.Console -op 1 -tx 4000  -producer

java -classpath activemq510.jar:../javaue/ggue.jar:./dirprm -Dlog4j.configuration=log4j.properties -DGGS_USEREXIT_CONF=./dirprm/javaue.properties.activemq gg.Console -op 1 -tx 4000  -producer




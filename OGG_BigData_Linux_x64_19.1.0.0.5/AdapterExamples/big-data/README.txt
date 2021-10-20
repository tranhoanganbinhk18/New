Sample prm and properties for all handlers.
Sample trail file "tr000000000" is available within "AdapterExamples/trail/" directory of Adapters installation.

Example command to add REPLICAT from ggsci prompt:
ADD REPLICAT <NAME>, EXTTRAIL <TRAIL FILE LOCATION>

NOTE: "gg.classpath" should be updated with valid directory path containing the jar files, depending on the handler being used.

Directory Structure:
---------------------------------
flume:
  |--> Sample prm, properties and rpc-client properties.
hbase
  |--> Sample prm and properties.
hdfs
  |--> Sample prm and properties.
kafka
  |--> Sample prm, properties and kafka-producer properties.
metadata_provider
  |--> avro
         |--> Sample Avro mdp configuration using hdfs handler with sample Avro schema definition files.
  |--> hive
         |--> Sample Hive mdp configuration using hdfs handler with sample Hive create table script.
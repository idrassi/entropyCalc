# entropyCalc
A tool to calculate the entropy of a file

Two methods are implemented:
  - Bytes frequency which is the most used method
  - GZip compression which is more often used in the academic world and which tend to give a better estimate on most files.

A conservative approach is to take the minimum value returned by both methods.


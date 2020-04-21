# entropyCalc
A tool to calculate the entropy of a file

Four methods are implemented:
  - Bytes frequency which is the most used method
  - GZip compression which is more often used in the academic world and which tend to give a better estimate on most files.
  - LZMA compression which is similar to GZip method but uses LZMA compression insteaf
  - Minimum of all above methods 

A conservative approach is to take the minimum value returned by the three first methods.

![EntropyCalc screenshot on Windows](https://i.imgur.com/DlNNOkj.png)


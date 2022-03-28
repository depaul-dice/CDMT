
# Content-defined Merkle Tree #

This includes the Content-Defined Merkle Tree implementation as well as experiment result for the reproducibility purpose. 
The purpose of this data structure is to make a robust data structure against chunk-shift and compare and contrast two binary datasets quicker (O(logN), where N is the number of chunks).

## Content-Defined Chunking method (CDC) ##

To do the comparison over two big binary data, and if one wants to identify where the differences are, it is usually smarter to compare them after diving the data into smaller chunks. This allows the multithreading to be used, and robust against many troubles made over the network, to give some examples. The naive approach to tackle this problem is to cut the chunk at a certain length. However, if the chunks of two big binary data are very similar yet different, after one insertion or one deletion of a character in between, the following chunks after the insertion/deletion are going to be all different. This is far from ideal. To resolve the issue, CDC is used. CDC cuts the big data into chunks after a certain set of data appears, such as 13 0s in a row, in the big binary data. This allows the chunks to be rubust against the character(s) insertion/deletion, and allows to identify the similarity of two big binary data relatively faster.



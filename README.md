
# Content-Defined Merkle Tree #

This includes the Content-Defined Merkle Tree implementation as well as experiment result for the reproducibility purpose. 
The purpose of this data structure is to make a robust data structure against chunk-shift and compare and contrast two binary datasets quicker (O(logN), where N is the number of chunks).

## Content-Defined Chunking method (CDC) ##

To do the comparison over two big binary data, and if one wants to identify where the differences are, it is usually smarter to compare them after diving the data into smaller chunks. This allows the multithreading to be used, and robust against many troubles made over the network, to give some examples. The naive approach to tackle this problem is to cut the chunk at a certain length. However, if the chunks of two big binary data are very similar yet different, after one insertion or one deletion of a character in between, the following chunks after the insertion/deletion are going to be all different. This is far from ideal. To resolve the issue, CDC is used. CDC cuts the big data into chunks after a certain set of data appears, such as 13 0s in a row, in the big binary data. This allows the chunks to be rubust against the character(s) insertion/deletion, and allows to identify the similarity of two big binary data relatively faster.

## Merkle Tree ##

Merkle Tree (a.k.a. hash tree) is introduced to do the faster comparison of two big data (could be in directory structure) in a situation where the bandwidth is very small. To explain about the tree, each internal node has a hash value, and that hash value is created through concatenating avalue of the children, then hashing them. This data structure allow O(logN) comparison over the network because, whenever the parents nodes' hashes turns out to be the same, all the children nodes hashes have to be the same, except when hash collision happen with a very low chance.

## Chunk-Shift ##

This is the concept which happens when you combine Merkle Tree with CDC. Since Merkle Tree creates internal nodes for the fixed number of children, whenever byte-shift happens, the internal nodes can be created off of different children. Hence after the byte-shift, the whole tree could be changed and make Merkle Tree useless. 

## Solution (CDMT) ##

What we should do at high level is to make the number of children dependent on the hash value of the internal node. Let's say we create an internal node whenever the hash value of the window lasts in 2 0s. Then, we get the hash value of the last two children nodes, and keep on including nodes until the window hash ends with 2 0s. This would make the tree to be robust against Chunk-Shift. 

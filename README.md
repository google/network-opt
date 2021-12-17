# network-opt
Source code for our AAAI 2022 paper, "***Search Strategies for Topological Network Optimization***."

If you decide to use this software in your research, please cite the corresponding paper:

<pre>
@inproceedings{Moffitt2022,
  title     = {Search strategies for topological network optimization},
  author    = {Michael D. Moffitt},
  booktitle = {Proceedings of the 36\textsuperscript{th} AAAI Conference on Artificial Intelligence},
  year      = {2022}
}
</pre>

Example usage of the tool:

<pre>
g++ -Wall network_opt.cpp -o network_opt -O3
time ./network_opt 8 4 1 E12
</pre>

<p align="center">
<img src="network-opt.png">
</p>

Source code for our [AAAI 2022](https://aaai-2022.virtualchair.net/poster_aaai21) paper, "[***Search Strategies for Topological Network Optimization***](https://www.aaai.org/AAAI22Papers/AAAI-21.MoffittM.pdf)."

Example usage of the tool:

<pre>
make
time ./network_opt OPT 1 4 8 E12 SQRT
</pre>

Example output from the tool:
<pre>
 Command: ./network_opt OPT 1 4 8 E12 SQRT
Solution: (1|2.2)+((1.2+(1.5+1.8+(2.7|3.9)))|3.3)
 Network: N()[N({0,4})][N(6)[N()[N(1)][N()[N()[N(2)][N(3)][N({5,7})]]]]]
  Target: 2.82842712474619
   Total: 2.828428882438316 (155903/55120)
    Cost: 1.758e-06

real    0m1.727s
user    0m1.719s
sys     0m0.018s
</pre>

If you decide to use this software in your research, please cite the corresponding paper:

<pre>
@inproceedings{Moffitt2022,
  title     = {Search strategies for topological network optimization},
  author    = {Michael D. Moffitt},
  booktitle = {Proceedings of the 36\textsuperscript{th} AAAI Conference on Artificial Intelligence},
  year      = {2022}
}
</pre>

This is not an officially supported Google product.

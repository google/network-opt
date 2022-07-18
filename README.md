<p align="center">
<img src="img/network-opt.png">
</p>

Source code for our [AAAI 2022](https://aaai-2022.virtualchair.net/poster_aaai21) paper, "[***Search Strategies for Topological Network Optimization***](https://doi.org/10.1609/aaai.v36i9.21271)."

## Example usage

<pre>
make
time ./network_opt OPT 1 4 8 E12 SQRT
</pre>

## Example output

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

## How to cite?

<pre>
@article{Moffitt_2022,
  title = {Search Strategies for Topological Network Optimization},
  volume = {36},
  url = {https://ojs.aaai.org/index.php/AAAI/article/view/21271},
  number = {9},
  journal = {Proceedings of the AAAI Conference on Artificial Intelligence},
  author = {Moffitt, Michael D.},
  year = {2022},
  month = {Jun.},
  pages = {10299-10308}
}
</pre>

## Disclaimer

This is not an officially supported Google product.

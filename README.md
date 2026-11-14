***I wrote a comprehensive blog explaining light culling in Vulkan in detail. Read it [here](https://amrhmorsy.github.io/blog/2026/LightCulling/).***

<br>
<br>
<h2>Light Culling Demo</h2>
<br>
<br>

<p align="center">
  <img src="Results/Bistro.png" width="98%"/>
    <br>
  <i>Bistro scene</i>
</p>
<br>
<p align="center">
  <img src="Results/Sponza.png" width="98%"/>
    <br>
  <i>Sponza scene</i>
</p>
<br>
<p align="center">
  <img src="Results/sibenik.png" width="98%"/>
    <br>
  <i>Sibenik scene</i>
</p>


<br>

A real-time application in <b>C++</b> and <b>Vulkan</b> that demonstrates light culling. It features: 

- A simple shading model that supports lighting
- A simple UI with a performance panel displaying metrics such as FPS, triangles count, and time-per-frame.
- A simple UI for tweaking different parameters. The available parameters are:
  
    - A flag to display albedo
    - A flag to display normal
    - A flag to display depth
    - A flag to inspect heat map
    - A flag to enable and disable light culling
    - A flag to enable and disable 2.5D light culling
    - A knob to edit the number of depth cells per tile
    - A flag to enable and disable stochastic light culling
    - A flag to adjust stochastic parameters

<br>
<br>

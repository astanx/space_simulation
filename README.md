https://github.com/user-attachments/assets/3ee465eb-00da-4448-86c8-a077777bcb80

<h1 align="center">Custom C++ Engine for Celestial Body Simulation, Atmospheric Modelling, and Real-Time Visualization</h1>

<h2>Project Overview </h2>
The project implements a custom C++ simulation engine specialized for celestial body dynamics. It combines:

 - N-body gravitational dynamics.
 - Keplerian orbital elements for solar-system bodies.
 - Rotational, tidal, and gravity-field models.
 - A 3D atmospheric fluid dynamics solver on a pressure–latitude–longitude grid with conserved momentum.
 - GPU-accelerated (OpenCL) and CPU multi-threaded backends.

The simulation currently models the Solar System.
The engine is structured as a modular real-time application rather than a pure scientific library, enabling interactive exploration while keeping scientific precision and accuracy.

<h2>Celestial Mechanics, Rotational Dynamics and Object Modelling </h2>
<h3>Object Hierarchy</h3>
Each body is either pure Object or OrbitalObject
Object has properties of any basic body, like:
<ul>
 <li>Mass</li>
 <li>Velocity</li>
</ul>
OrbitalObject extends Object class and adds properties such as:
<ul>
  <li>Orbit(owns KeplerElements)</li>
</ul>

<h3>Integrators</h3>
<h4>Hybrid Wisdom-Holman Integrator</h4>
My version of WH Integrator contains:
<ul>
  <li>WH Integrator for hierarchical orbital bodies</li>
  <li>Leapfrog for other bodies</li>
</ul>

Each step can be divided into Half-Kick or Drift
The sequence looks like
1. Half-Kick
2. Drift
3. Half-Kick

<h5>Celestial Mechanics</h5>
Half Kick applies basic <a href="https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation">Newton's law of universal gravitation</a> for each body.
For CPU pipeline it uses <p>$$\boldsymbol{O}(\frac{N^2 - N}{2})$$</p>

algorithm version to speed it up, while GPU pipeline uses basic 
<p>$$\boldsymbol{O}(N^2)$$</p> algorithm to prevent unnecessary kernel runs.

Drift advances <a href="https://en.wikipedia.org/wiki/Mean_anomaly">Mean anomaly</a> each step
<p>
  $$
  M_{n+1} = M_{n} + \eta\Delta t 
  $$
</p>
Where 
<p>
  $$
  \eta = \sqrt{\frac{\mu}{\alpha^3}}
  $$
</p>
<a href="https://en.wikipedia.org/wiki/Newton%27s_method">Newton-Raphson</a> iteration
<p>
  $$
  E_{n+1} 
  = E_n - \frac{E_n - e\sin(E_n)-M}
  {1-e\cos(E_n)}
  $$
</p>
to solve <a href="https://en.wikipedia.org/wiki/Kepler%27s_equation">Kepler's equation</a>
<p>
  $$
  M = E - e\sin(E)
  $$
</p>
and then solves equations to predict updated position.
<p>
  $$
  pos_{orb} = \begin{bmatrix}
  \alpha(\cos(E) - e)\\
  \alpha\sqrt(1-(e^2)) * \sin(E)\\
  0
  \end{bmatrix}
  $$
</p>

<p>
  $$
  pos = pos_{central} + R_3(\Omega) R_1(i) R3(\omega) pos_{orb}
  $$
</p>


<h5>Rotational Dynamics</h5>
Half Kick calculates torque by combining gravitational torque for every body and <a href="https://en.wikipedia.org/wiki/Tidal_force">tidal torque</a> for bodies with defined properties, it assumes constant tidal properties, such like constant tidal factor.

Tidal torque:
<p>
$$
\boldsymbol{\tau}_{\mathrm{tidal}}
=
-\frac{3}{2}
\frac{k_2}{Q}
\frac{\mu^2r^5}{d^6}
(\omega-\mathbf{n})
$$
</p>
Drift uses quaternion kinematics to represent rotation of the body.

The rotation quaternion is calculated from the current angular velocity:

$$
\theta = |\omega|\Delta t
$$

where:
- $\boldsymbol{\omega}$ is the body's angular velocity vector
- $\theta$ is the rotation angle during the timestep

The rotation axis is:

$$
\hat{\mathbf{u}} =
\frac{\boldsymbol{\omega}}
{|\boldsymbol{\omega}|}
$$

The rotation quaternion is:

$$
q_{rot}=
(
\cos\frac{\theta}{2},
\hat{\mathbf{u}}\sin\frac{\theta}{2}
)
$$

The new orientation is obtained by:

$$
q_{n+1}=q_{n}q_{rot}
$$

<h2>Atmospheric Modelling</h2>
<h3>Grid</h3>
At it base it uses spherical pressure coordinate grid:
<p>
  $$
  (i, j, k)
  $$
</p>

Where
<ul>
  <li>i = longitude</li>
  <li>j = latitude</li>
  <li>k = pressure level</li>
</ul>

<h3>Initialization</h3>
At the beggining the grid is initialized with input data:
<ul>
  <li>Temperature T</li>
<li>Relative humidity RH</li>
<li>Specific humidity q</li>
<li>
Wind velocity:
<ul>
  <li>u (east-west)</li>
  <li>$$\nu$$ (north-south)</li>
  <li>w (vertical)</li>
</ul>
</li>
<li>Geopotential height $$\phi$$</li>
<li>
Cloud species:
<ul>
  <li>cloud ice</li>
  <li>cloud liquid</li>
  <li>rain</li>
  <li>snow</li>
</ul>
  </li>
<li>Ozone mixing ratio</li>
</ul>

This data is used to calculate:

<p>
  $$
  L_i = r\cos(j)\Delta i 
  $$
  $$
  L_j = r\Delta j
  $$
  $$
  L_k = \frac{h_{k+1} - h_{k-1}}{2}
  $$
</p>

and then volume of the cell is calculated:
<p>
  $$
  V = L_i L_j L_k
  $$
</p>

Initial density calculations:
<p>
  $$
  r_m = \frac{q}{1 - q}
  $$
  $$
  e = \frac{r_m p}{0.622 + r_m}
  $$
</p>

Where:
<p>
  <ul>
    <li>$$q$$ = specific humidity</li>
    <li>$$p$$ = pressure</li>
    <li>$$e$$ = water vapor pressure</li>
  </ul>
</p>

Ideal gas equation:
<p>
  $$
  \rho = \frac{p_d}{R_d T} + \frac{e}{R_v T}
  $$
</p>

Where:
<p>
  <ul>
    <li>$$p_d = p - e$$</li>
    <li>$$R_d$$ = dry air constant</li>
    <li>$$R_v$$ = water vapor gas constant</li>
  </ul>
</p>


<h3>Integration</h3>
Each step total flux is calculated for each face:
<p>
  $$
  f = \rho v A
  $$
</p>


<h2>Rendering and Visualization</h2>
<h3>Hapke BRDF Model</h3>
It is used to calculate the light reflected from the moon towards the planet.
For each reflector body the set of parameters is defined:
<ul>
  <li> $$w$$ = single scattering albedo</li>
  <li> $$\theta$$ = macroscopic roughness angle (radians)</li>
  <li> $$h$$ = opposition effect width</li>
  <li> $$b0$$ = strength of the opposition effect</li>
  <li> $$h_{cb}$$ = width of the coherent backscatter opposition effect</li>
  <li> $$b0_{cb}$$ = strength of the coherent backscatter opposition effect</li>
  <li> $$b$$ = asymmetry parameter</li>
  <li> $$c$$ = weighting between backward and forward scattering</li>
</ul>

The light is calculated as:
<p>
  $$
  C = hapkeBRDF() \frac{L}{4 \pi d^2}
  $$
</p>
Where
<ul>
  <li>$$hapkeBRDF$$ = function, <a href="https://kernelo-mistis.gitlabpages.inria.fr/planet-gllim-front-end/rst/scientific_doc/photometric_models/hapke.html">full function here</a></li>
  <li>$$L$$ = Light Luminocity</li>
  <li>$$d$$ = distance between object and light source</li>
</ul>

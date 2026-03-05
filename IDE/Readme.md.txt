# FROG IDE Architecture

<p align="center">
<b>FROG — Free Open Graphical Language</b><br>
Open Source Graphical Dataflow Programming Environment
</p>

---

<h2>Overview</h2>

The FROG IDE is designed as a modular graphical development environment for building and executing dataflow programs.

The architecture separates clearly:

<ul>
<li>User interface editing</li>
<li>Graphical program representation</li>
<li>Compilation</li>
<li>Execution runtime</li>
</ul>

This separation ensures that the language itself remains independent from the IDE implementation and allows FROG programs to run on many targets.

---

<h2>High-Level Architecture</h2>

<p align="center">

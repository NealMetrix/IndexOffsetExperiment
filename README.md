# IndexOffsetExperiment
An OpenGL test to see if I can color shapes by switching the bound shader at the different points of the index buffer

This is a OpenGL experiment that uses C++ and OpenGL
  C++
  OpenGL
It is actually one of my many experiments that I have where I try and explore how OpenGL works. I am generally not 
trying to do good programming practice here. Experiments I make like this are often used to explore what is possible
with a framework, library, or specification. It is more about me exploring the structure of the code so I know how
it works.

This experiment was mostly to see if I could figure out how the offsets work with OpenGL glDrawElements draw call and to see
if I could use this to change the color of a single shape that I am drawing. So I built 2 shaders and had one shader active for
the 1st 3 indices then I used glUseProgram on a different shader and then used the same draw call again, this time with an offset
so it started off after the 1st 3 vertices. It did take some getting used to to figure out exactly how to specify the offset so
the drawcall started halfway through my index buffer, but overall this test went pretty smoothy.

Q1: How do they roll?
    They roll by updating their position and simultaneously updating their 
rotation in toward their direction they're rolling.

Q2: Do the marbles now stay contained within the ground plane? 
What four normals did you use?

Yes the marbles now stay contained, the four normals I use:
(-1,0,0)
(-1,0,0)
(0,0,-1)
(0,0,-1)

Q3: Knowing just the locations of the two spheres, how can we compute 
the two normals? If we have marble point L1 and marble point L2, 
what is the equation to give us our two normals N1 and N2?

we caculate the normals point from one balls center to the opposing ball.
This done by caluculating the normal vector in regard from one ball to 
the next, ie: vec(N1) = L1 - L2, vec(N2) = L2 - L1

Q4: Success?

At first failure, I forgot to account for it hitting its own position in 
the nested loop.

Q5: Lab fun = 10/10

Q6: First lab write that felt perfect I was able to figure some stuff out 
on my own and didn't need much help, it would be nice if you included a 
description or video of how the balls might bounce weird just so we Know
we're doing it correctly.

Q7: Took about 3hrs

Q8: Nope
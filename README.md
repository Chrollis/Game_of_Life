# Game of Life
　This is a self-made version for game of life, whose graphics library is [EasyX](https://docs.easyx.cn/).  
　For programming easily, the matrix templates library [Eigen](http://eigen.tuxfamily.org) is used. What's more, I'm not able enough to manage memory explicitly, its **RAM usage maybe quite bigger** than a normal range-limited game of life.
 I made it by unloading and loading **blocks** that are temporarily saved in a folder named **cache**, which will be deleted all after the program ends.
## Programming
　Though its algorithm isn't perfect enough, if you want to copy this code to reprogram it, **please install EasyX and Eigen in your PC at first**.  
　And using VS2022, create the project as **Windows desktop appliction**.

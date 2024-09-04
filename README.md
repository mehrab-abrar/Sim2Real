# Sim2Real: Implementation of a Quadruped Robot Navigation in a Gymnasium FrozenLake-Inspired Real Environment

This repository is associated with the paper titled "An Open-source Sim2Real Approach for Sensor-independent Robot Navigation in a Grid" which is submitted to the IEEE International Conference on Robotics and Biomimetics (IEEE ROBIO 2024). 

This work focus on navigating a quadruped robot in a real-world grid-like environment inspired by the Gymnasium Frozen Lake — a highly user-friendly and free Application Programming
Interface (API) to develop and test Reinforcement Learning (RL) algorithms. We detail the development of a pipeline to transfer motion policies learned in the Frozen Lake simulation to a physical quadruped robot. The work aims to:

* Develop and implement a straightforward pipeline that transfers policies learned in the Frozen Lake environment to a physical robot.
* Create a minimalistic, low-cost, and open-source quadruped robot that avoids obstacles and navigates in a grid using the learned policies without relying on sensors or additional real-world training.

A demonstration video is available on [YouTube](https://www.youtube.com/watch?v=dDKQaN_zsvU).

The image below shows the 7 steps the quadruped robot follows to avoid the obstacles in a grid and reach the destination without using any localization and mapping sensors. 

![FullSim2Real (1)-min](https://github.com/user-attachments/assets/83f456c4-22ef-4eb2-9a21-f4fd2ba6acb1)








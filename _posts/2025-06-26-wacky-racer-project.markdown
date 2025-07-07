---
layout: post
title:  "Remote-Controlled Car"
date:   2025-06-26 11:19:42 +1200
categories: course embedded-systems
thumbnail: "media/wacky_racer/car.jpg"
---
<div class="work-in-progress">
    <p>
    Hello, <br> the content on this page is still work in progress. Please excuse the poor writing and unformatted images.  <br>
    - Matthew
    </p>
</div>

## Project Outline

In a team of four, I created custom circuit boards and software for a remote control race car.
We designed PCBs for the controller and car, which handled power regulation, motor control, radio, LEDs, and music. The circuitry was designed in Altium Designer, assembled by ourselves, and programmed in C. 

The key skills I learned were how to protect delicate components in pcb layouts, and how to program ARM-based microcontrollers. Our team earned second place, only loosing first when our rabbit's fluff got jammed in the motor gear box.

## Introduction
{% comment %}  background {% endcomment %}  
This project is part of the fourth year embedded systems course (ENCE461) at canterbury university, aka 'wacky racers'.
students compete in teams of four to design, assemble and program a remote controlled race car.

{% comment %}  problem statement {% endcomment %}  
The car must be controlled by an accelerometer which the 'driver' wears on their head.
Both the car and the hat must have certain components, including a SAM4s ARM microcontroller. 

![system diagram]({{ site.baseurl }}/media/wacky_racer/wackyRacersSystemDiagram.png)
   
{% comment %} success criteria {% endcomment %}  
A competition is held at the end of the semmester where the cars must compete to score the most laps within a given time frame.
Bonus marks are awarded for 'dastardly features' which can be used to disable opponents.

## Design

# Schematic
I designed the embedded system for the hat, which is shown in the [schematics]({{ site.baseurl }}/media/wacky_racer/hat-schematic.pdf) below. 

- component detail
- design considerations.

![hat schematic page 1]({{ site.baseurl }}/media/wacky_racer/hat-schematic-page-1.png)
![car schematic page 2]({{ site.baseurl }}/media/wacky_racer/hat-schematic-page-2.png)

# PCB
The circuit board was designed in altium, and then we assembled the PCBs using pick and place machines. 
We tried for a very compact layout, which did end up causing issues during debugging.

![car pcb layout]({{ site.baseurl }}/media/wacky_racer/car-pcb-layout.png){: width="50%"}

![car pcb]({{ site.baseurl }}/media/wacky_racer/car-pcb.jpg){: height="600"}
![hat pcb]({{ site.baseurl }}/media/wacky_racer/hat-pcb.jpg){: height="600" }

# Software
For the software, I implemented a real time task scheduler using the [RIOS](https://www.cs.ucr.edu/~vahid/rios/) system. This handy bit of code provides a great template for non-preemptive task-scheduling in pure C.
https://emjayking.github.io/portfolio/
# Mechanical
Each team was given a base 'tank' to modify as they saw fit. 
But if it aint broke don't fix it, so we left ours in the stock configuration to focus on what truly mattered
\- winning the costume competition.

Our team drew inspiration from Monty Python's 'The Holy Grail'. So naturally, our car was modelled after the fearsome [Rabbit of Caerbannog](https://en.wikipedia.org/wiki/Rabbit_of_Caerbannog). One 3D printed rabbit's head later, and we were sure to crush our foes.

![finished rabbit]({{ site.baseurl }}/media/wacky_racer/car.jpg)

For the controller - Arthur's crown, naturally - we laser cut some ribs and glued them onto a beanie. I wish I had an image of it with the LEDs on, but you'll have to trust that it was suitably majestic.

![arthurs crown]({{ site.baseurl }}/media/wacky_racer/arthurs-crown.jpg)


## Results

Our rabbit was a great success. I've embedded a showcase of the controls below, showing how the accelerometer on the hat controlled the speed of each motor.

<iframe width="100%" height="640" src="https://www.youtube.com/embed/OhbQdVrZ2IU?;&mute=1" frameborder="0" allowfullscreen> </iframe>


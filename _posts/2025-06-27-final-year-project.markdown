---
layout: post
title:  "Final Year Project"
date:   2025-06-27 13:00:00 +1200
categories: course embedded-systems mechatronics
---
<!-- photo of crawler and deployment -->

# Project Outline
- What was done
- What was learned

# Introduction
<!-- - background information -->
uc students do a final year project to earn their honors degree.
usually sponsored by external companies to provide industry experience.
projects done in teams of four, usually with students from different specialisations.

<!-- - problem statement -->
cawthron (link) deploys fixed sensor installations for commercial environmental monitoring. 
The most expensive component of these installations are the sensors, because of their  niche market and high callibration costs.
cawthron asked us to design a robot to move the sensors vertically within a water column to reduce the number of sensors needed.

 <!-- photo of existing installation? -->

<!-- - success criteria -->
The aim was to match the capabilities of cawthron's existing installations. This requires travelling 30 metres deep, surviving for 12 months, and providing a reading at  500mm intervals every hour. 

<!-- - scope -->
cawthron already had several components that we could adapt for our use case. 
Their buoy could be repurposed to mount the mooring line, provide solar power, and send radio signals to shore. 
Our project only needed to design the robot's movement and buoy interface system.
The robots microcontroller and battery management system were also repurposed from cawthron's existing products, so we only had to design the peripherals to implenent each requirement.

<!-- - milestones -->
We split the project into milestones to divide the workload between myself and my teammates. 
I focused on recharging the robot, sending data to the buoy, and the sensor system.

# Design
- Explanation of current design

![Annotated design]({{ site.baseurl }}/media/fyp/annotated-crawler.png)
![underwater deployment]({{ site.baseurl }}/media/fyp/underwater-deployment.png)

## charging Module

We decided to use a wireless charger to transfer power from the buoy to the robot, trading efficiency for mechanical complexity. 
The idea was to use inductive coils for recharging, similar to wireless phone rechargers.

![inductive concept]({{ site.baseurl }}/media/fyp/inductive-charger-concept.png)

![completed modules]({{ site.baseurl }}/media/fyp/completed-modules.jpg)


The inductive coils and electronics were purchased online, and I did several [tests](#inductive-coil-testing) to characterise their performance.
I encased the coils in resin to waterproof them. I tried using a vacuum chamber to remove air from the resin, but I suspect the resin had too little viscosity. In the end the resin ended up boiling and producing a poor finish.

- copperfill pla

![resin boiling in the vacuum chamber]({{ site.baseurl }}/media/fyp/resin-boiling.png)

![coils with mating feature]({{ site.baseurl }}/media/fyp/completed-coils.png)


To control the charger, we designed a custom pcb. This circuit would control when the charger was turned on, and allow the robot to communicate with the buoy via bluetooth. The board was also repurposed to [control the motor](#motor-control). The [schematic]({{ site.baseurl }}/media/fyp/pcb-schematic.pdf) and layout were done in KiCAD.

- technical details
    - microcontroller choice
    - communication protocol
    - power regulator

![pcb schematic]({{ site.baseurl }}/media/fyp/pcb-schematic.png)

![completed pcbs]({{ site.baseurl }}/media/fyp/completed-pcbs.jpg)

## wireless communication

The robot sends data to the buoy via bluetooth. Saltwater is a notorioulsy good attenuator for radio signals, but the radios are in the wireless charging modules, so the signal only has to travel around 100mm. The bluetooth communication was tricky, but I succeeded in getting motor control and live sensor readouts for demonstration purposes.

## Motor Control

The motor housing had it's own pcb so that speed, direction, and position feedback could be handled via RS232. 

![completed motor]({{ site.baseurl }}/media/fyp/motor-pcb.jpg)

## Temperature sensor

Our robot used two sensors, a pressure sensor for positoning, and a temperature sensor for testing the data logging. 
When we started the project, the company that supplied most of Cawthron's components, [blue robotics](https://bluerobotics.com/), did not sell a temperature sensor rated for continuous salt-water immersion. I solved this problem by creating my own housing for one of their temperature sensor pcbs.

I used vacuum forming to create a close fitting shell around the pcb, and sealed it into a stainless steel base using silicone. There were also two 3D printed inserts that mated with the base's internal threading to secure the pcb. The vacuum forming tool several iterations to get right, because the part must have the correct height to cross-section ratio to conform along the part's internal corners. The design positions the actual sensor component as close to the salt-water as possible so that it would respond quickly to changes in water temperature.

- use of internal holes
- 

![sensor iterations]({{ site.baseurl }}/media/fyp/temperature-sensor-iterations.png)

![installed sensor]({{ site.baseurl }}/media/fyp/installed-sensor.png)

# Results

## Inductive Coil Testing

## Wireless testing

## Underwater test
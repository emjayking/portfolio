---
layout: post
title:  "Autonomous Underwater Robot"
date:   2025-06-27 13:00:00 +1200
categories: course mechatronics 
thumbnail: "media/fyp/annotated-crawler.png" 
---
<!-- photo of crawler and deployment -->

<div class="work-in-progress">
    <p>
    Hello, <br> the content on this page is still work in progress. Please excuse the poor writing and unformatted images.  <br>
    - Matthew
    </p>
</div>
# Project Outline
- What was done
    - Designed prototype underwater robot alternative to traditional distributed sensor installations.
    - Personally involved with wireless charging, temperature sensor, and wireless communication.
    - Project created a proof of concept, and was renewed this year to create a second prototype and further refine the product.
- What was learned
    - Gained experience with micropython, rs232, stm32s, bluetooth, and real-time operating systems.
    - Learned to design components for vacuum forming.
    - Gained experience with project lifecycle, going through design, assembly, testing, reflecting, and preparing for the next iteration. 

# Introduction

<!-- - background information -->
At The University of Canterbury, engineering students complete a final year project to earn their honors degree.
These are usually sponsored by external companies looking to screen for graduate roles and provide students with professional experience.
Projects are completed in teams of four, ideally from a mix of specialisations, and report to both a project supervisor from the university and their commercial client.

<!-- - problem statement -->
During my internship at the [Cawthron Institute](https://www.cawthron.org.nz/), I convinced the company to sponsor a final year project. 
My supervisor at the time, Paul, had the idea to develop an underwater robot for sensor deployments.
Currently, Cawthron uses distributed sensor deployments to get water-column data.
These lines of sensors are extremely costly due to individual sensor purchase and recalibration costs.
They also must be reconfigured for each deployment, limiting their flexibility.
There are existing [commercial solutions](https://mclanelabs.com/prawler-videos/), but these are either too expensive, too slow, or otherwise unsuited for Cawthron's operations.
Cawthrons hoped the robot would provide a lower cost alternative (salinity sensors can cost up to $10,000 each), allowing them to sell to smaller marine industries who perhaps were not able to justify the cost.

<!-- - success criteria -->
<!-- The aim was to match the capabilities of cawthron's existing installations. This requires travelling 30 metres deep, surviving for 12 months, and providing a reading at  500mm intervals every hour.  -->
The team was selected by Cawthron, with two mechanical and two mechatronics engineers - myself included.
Our goals were based around the capabilities of Cawthron's existing distributed sensor installations:
<ul>
    <li>Hourly samples</li>
    <li>Depth resolution ≤ 1m</li>
    <li>Depth rating ≥ 30m</li>
    <li>Year-long deployment time</li>
    <li>Remote data transmission</li>
</ul>

<!-- - scope -->
Cawthron provided most of the electrical components from their existing products to adapt to our needs.
The main microcontroller, battery management system, and pressure sensor were provided, leaving us to design the recharging, communictions, and movement systems before encasing them in a chasis and writing an operating system.
One of Cawthrons buoys would be repurposed to mount the mooring line, provide solar power, and send radio signals to shore. 
This meant the recharging and communications system would only have to connect the robot to the buoy.

<!-- - milestones -->
We split the project into milestones to divide the workload between myself and my teammates. 
The mechanical engineers focused on the chassis, movement system and motor enclosure, whilst myself and the other mechatronics engineer designed the recharging, communications, and software components.
In the spirit of brevity I've only detailed my own contributions below, but our full <del>64 page!</del> [report is available]({{ site.baseurl }}/media/fyp/End Of Year Report.pdf) if you would like to see our collected efforts.

# Design

![System Diagram]({{ site.baseurl }}/media/fyp/fyp-system-diagram.png)

<!-- ![Annotated design]({{ site.baseurl }}/media/fyp/annotated-crawler.png)
![underwater deployment]({{ site.baseurl }}/media/fyp/underwater-deployment.png) -->

<div style="display: grid; grid-template-columns: auto auto; grid-template-rows: minmax(auto, 300px); justify-content: center;">
    <img src="{{ site.baseurl }}/media/fyp/annotated-crawler.png" alt="annotated-robot" style="max-height: 100%;">
    <img src="{{ site.baseurl }}/media/fyp/underwater-deployment.png"
    alt="underwater-deployment" style="max-height: 100%;">
</div>

## charging Module

We decided to use a wireless charger to transfer power from the buoy to the robot, trading efficiency for mechanical complexity. 
The idea was to use inductive coils for recharging, similar to wireless phone rechargers.


<!-- ![inductive concept]({{ site.baseurl }}/media/fyp/inductive-charger-concept.png)

![completed modules]({{ site.baseurl }}/media/fyp/completed-modules.jpg) -->

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

Throughout the design, we used iterative testing to inform and validate our decisions.

## Inductive Coil Testing
The inductive power system was purchased off-the-shelf, but the claimed [11~60mm of range](https://www.dfrobot.com/blog-1579.html) seemed dubious (spoiler, it was).

To characterise the performance, I designed and assembled a test-jig that would let us precisely control the coil spacing. I then tested the system's performance using a programmable power-supply and an electronic load, to control each variable. This is important because the efficiency of an inductive circuit depends on the IV (current/voltage) of both the load and supply.

![test jig]({{ site.baseurl }}/media/fyp/inductive-test-jig.jpg)

My results found that the coils could only provide our required 10-Watts of power when operating at 25V supply, drawing 1.5A at 12mm spacing. We were unable to achieve the advertised 25W, even when operating significantly above the recomended ratings. Notably, there is room for further testing with higher load current at longer ranges, but we did not test further as we met our target power requirement.

![inductive results]({{ site.baseurl }}/media/fyp/inductive-charging-data.png)

## Climb testing

Once the robot's drive mechanism was finished, we wished to validate the motor's performance. We mounted a mooring line from the workshop crane, and attached a counter-weight to the robot to mimic neutral bouyancy. The test was a success, and the workshop technicians were kind enough to take a photo of us.

![crane photo]({{ site.baseurl }}/media/fyp/cranePhoto.jpg)

## Underwater test

Bouyed by our success (pun intended), we tested the robot at a local swimming pool. 

<iframe width="560" height="315" src="https://www.youtube.com/embed/1zTDcDDNlZo?si=2geeTVZHhjMmwilL;&mute=1" title="YouTube video player" frameborder="0" allow="accelerometer; clipboard-write; encrypted-media; gyroscope; picture-in-picture" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>
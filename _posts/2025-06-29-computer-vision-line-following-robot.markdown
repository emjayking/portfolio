---
layout: post
title:  "Line-Following Robot"
date:   2025-06-30 13:00:00 +1200
categories: course computer-vision
thumbnail: "media/line-following-robot/race-car.jpg"
---

<!-- <div class="work-in-progress">
    <p>
    Hello, <br> the content on this page is still work in progress. Please excuse the poor writing and unformatted images.  <br>
    - Matthew
    </p>
</div> -->

# Project Outline
I designed an image processing algorithm to control a simple line following robot.
Images were transmitted to an external computer which located the target line and transmitted speed corrections to the robot via radio.
My robot was compared to existing alternatives in an IEEE style report, where I researched the latest in line following algorithms.

From this project I gained an practical experience with analytical computer vision techniques, and an appreciation for the advancements unlocked by modern hardware.
I earned an A for my report, and an A- for the course overall.

# Introduction
This project was completed for one of my final year courses, [Computer Vision](https://courseinfo.canterbury.ac.nz/GetCourseDetails.aspx?course=COSC428&occurrence=25S1(C)&year=2025).
The project focused on reasearching existing computer vision techniques, replicating an aspect then documenting our results.

I decided to research modern line following robots, as they are increasingly used in autonomous vehicles.
Then I created my own simple line following robot, using remote processing to provide better performance than the robot's embedded hardware could provide.
Finally, my results were compiled into an [IEEE style report]({{ site.baseurl }}/media/line-following-robot/COSC_Conference_Paper.pdf).

# Design

For the car's chassis, I reused the tank-rabbit from my [wacky racer project]({{ site.baseurl }}/course/embedded-systems/2025/06/26/wacky-racer-project.html).
My iPhone acted as a wireless bluetooth camera, streaming to my macbook at 30fps.
The macbook locates the target line within a frame, calculates the angular difference between the target line and the vehicle's heading, then uses a proportional controller to generate corrections to the vehicle motor speed, effectively steering the vehicle.
These commands are sent to the vehicle's radio controller via UART, the controller simply acts as a repeater.

<img src="{{ site.baseurl }}/media/line-following-robot/system-diagram.png"
    alt="system-diagram"
    style="display:block; max-width: 70%; margin: auto;">
<figcaption>System Diagram</figcaption>

As you can see below, the camera was secured using the latest in flexible adhesive technology <del>electrical tape</del>. Make no mistake dear reader, this design was intentional, and absolutely <em>not</em> a consequence of ignoring the mechanical design until the last minute.

<img src="{{ site.baseurl }}/media/line-following-robot/race-car.jpg"
    alt="completed car"
    style="display:block; max-width: 70%; margin: auto;">
<figcaption>Robot in action</figcaption>

The image processing pipeline was fairly simple, the image grid below shows an example of each step. 
The score critera selects which of the green lines to follow based on its start pixel and relative angle. The red vertical line represents the robot's heading, and the red line bording the tape is what the score criteria decides to follow.
The relative angle between these two lines then calculated to control the robot's direction.

<ol>
    <li>A raw image is recieved via bluetooth.</li>

    <li>Smoothing with a <a href="https://opencv24-python-tutorials.readthedocs.io/en/latest/py_tutorials/py_imgproc/py_filtering/py_filtering.html#gaussian-filtering">Gaussian Blur</a> to removes noise.</li>

    <li><a href="https://docs.opencv.org/4.x/d7/d4d/tutorial_py_thresholding.html">Otsu's Binarization</a> dynamically thresholds the image.</li>

    <li><a href="https://docs.opencv.org/4.x/da/d22/tutorial_py_canny.html">Canny Edge Detection</a> finds edge pixels.</li>

    <li>The <a href="https://docs.opencv.org/4.x/d6/d10/tutorial_py_houghlines.html">Hough Line Transform</a> links edge pixels to form lines.</li>

    <li>A score criteria is applied to select which line to follow.</li>
</ol>

<div id="computer-vision-image-grid">
    <div id="areaA">
        <img src="{{ site.baseurl }}/media/line-following-robot/raw.png">
        <img src="{{ site.baseurl }}/media/line-following-robot/smoothed.png">
        <img src="{{ site.baseurl }}/media/line-following-robot/Binarised.png">
    </div>
    <div id="areaB">
        <img src="{{ site.baseurl }}/media/line-following-robot/canny-edge.png">
        <img src="{{ site.baseurl }}/media/line-following-robot/hough-line.png">
    </div>
</div>

I calculated speed by linearly mapping the angular error to a %PWM offset which was applied to base forward speed.
This made tuning the controls easy, but the car definitely could be improved with a proper PID system.

<img src="{{ site.baseurl }}/media/line-following-robot/speed-control-diagram.drawio.png"
    alt="speed calculation flowchart"
    style="display:block; max-width: 70%; margin: auto;">
<figcaption>Speed Calculation</figcaption>

# Results

I tested the car by creating right-angle corners with varying radii, and recording the success rate. 
I was only able to reach 50mm/s, a far cry from the world record 2.25m/s.
The primary constraint was my image processing pipeline, which failed to locate the line if the corner was too sharp.
I observed three failure modes during testing.
Understeer and oversteer could be fixed by replacing the linear control with a PID system, but paralax error would require improving the image processing to also calculate the robot's horizontal baseline offset.

<img src="{{ site.baseurl }}/media/line-following-robot/failure-modes.png"
    alt="Failure modes diagram"
    style="display:block; max-width: 70%; margin: auto;">
<figcaption>Failure Modes</figcaption>


---
layout: post
title:  "Line-Following Robot"
date:   2025-06-30 13:00:00 +1200
categories: course computer-vision
thumbnail: "media/line-following-robot/race-car.jpg"
---

# Project Outline
- what was done 
    - Used python OpenCV to create a line following robot
    - Wrote an IEEE style paper to describe results

- what was learned
    - OpenCV and classical computer vision techniques

[Report PDF Download]({{ site.baseurl }}/media/line-following-robot/COSC_Conference_Paper.pdf)

# Design

- Hardware
    - Reused vehicle from [wacky racer project]({{ site.baseurl }}/course/embedded-systems/2025/06/26/wacky-racer-project.html)
    - Used iPhone as camera
    - Streamed video to macbook via bluetooth
    - macbook sent pwm commands to car via radio through uart
    - System Diagram

![System Diagram]({{ site.baseurl }}/media/line-following-robot/system-diagram.png)


![car with camera]({{ site.baseurl }}/media/line-following-robot/race-car.jpg)

- Software (Computer vision)
    - smoothed raw image with gaussian blur
    - Binarised image with dynamic-bimodal thresholding
    - Used Canny edge detection to find lines in the image
    - Used Hough line transform to connect adjacent edge points into lines
    - Used criteria to select which line to follow

![raw image]({{ site.baseurl }}/media/line-following-robot/raw.png)
![smoothed image]({{ site.baseurl }}/media/line-following-robot/smoothed.png)
![binarised image]({{ site.baseurl }}/media/line-following-robot/Binarised.png)
![detected edges]({{ site.baseurl }}/media/line-following-robot/canny-edge.png)
![detected lines]({{ site.baseurl }}/media/line-following-robot/hough-line.png)

# Results

- Frame-rate
- Failure modes
- Algorithm limitations
    - would not center on line
    - bimodal collapse when line lost
    - 

![failure modes]({{ site.baseurl }}/media/line-following-robot/failure-modes.png)




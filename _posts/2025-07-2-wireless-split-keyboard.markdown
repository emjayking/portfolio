---
layout: post
title:  "Wireless Split Keyboard"
date:   2025-07-01 13:00:00 +1200
categories: personal CAD
thumbnail: "media/split-keyboard/completed-keyboard.png"
---

# Overview
I assembled my own wireless corne keyboard using a premade pcb and a custom 3D printed case.
I'm going to use the keyboard as a platform to learn about software development for human-interface-devices. My next goal for this project is to learn Swift by writing an app that reports battery levels for both halves of the keyboard, because macOS only natively recognises a single battery per device.

<!-- ![completed-keyboard]({{ site.baseurl }}/media/split-keyboard/completed-keyboard.png) -->

<img src="{{ site.baseurl }}/media/split-keyboard/completed-keyboard.png" 
    alt="Completed Keyboards"
    style="display:block; max-width: 70%; margin: auto;">
<figcaption>Finished Keyboard</figcaption>

# Introduction
As usual, when I have too much free time, I start looking to fix things that aren't broken. 
There's nothing wrong with my existing keyboard, but I have been interested in the idea of a split keyboard for some time - and a recent tax refund convinced me that it was a sound financial decision.

So with my mind made up, I began looking into split keyboards... and promptly became overwhelmed with design paralysis. I found [this kit](https://typeractive.xyz/pages/build/corne) online, which solved the issues of PCB design, microcontroller selection, and promised to have useful firmware with minimal kludging required.

Unfortunately their pricing is truly egregious, and following a youtube video to tighten some screws is hardly a project, so I decided to make my own case - thus preserving my over-inflated pride as an engineer.

# Assembly
The pcb assembly was very straightforward. Solder some pin-sockets, install a battery, flash some firmware and job done.
The result is a working keyboard that is in desperate need of a decent case.

<!-- ![keyboards without case]({{ site.baseurl }}/media/split-keyboard/pcbs.png) -->

<img src="{{ site.baseurl }}/media/split-keyboard/pcbs.png" 
    alt="Bare PCB"
    style="display:block; max-width: 70%; margin: auto;">
<figcaption>PCBs with keycaps installed.</figcaption>


# Case Design 
I've designed my case with the same style as the commercially available option.
My original plan was to rip the files from their site's meta-data, which kinda worked but the resulting meshes were too damaged to work with.

<!-- ![broken mesh]({{ site.baseurl }}/media/split-keyboard/broken-mesh.png) -->

<img src="{{ site.baseurl }}/media/split-keyboard/broken-mesh.png" 
    alt="Recovered Meshes"
    style="display:block; max-width: 70%; margin: auto;">
<figcaption><i>recovered</i> meshes</figcaption>

I printed the broken meshes to check tolerances before remaking the parts in OnShape. The switch retaining plate was too loose for my standards, so when I redesigned the plate I reduced the clearance to provide a snug fit. 
Finding m2x4 standoffs is seemingly impossible, so I 3D printed those as well. 

<!-- ![remade keyboard]({{ site.baseurl }}/media/split-keyboard/remade-keyboard.png) -->

<img src="{{ site.baseurl }}/media/split-keyboard/remade-keyboard.png" 
    alt="Remade "
    style="display:block; max-width: 70%; margin: auto;">
<figcaption>Completely totally original design</figcaption>

# Future Improvements
Each half of the board has its own battery, but only one half actually reports a battery level.
It appears that I can rebuild the firmware to report each. But, macOS doesn't support bluetooth devices reporting two batteries over the GATT (Generic attribute) protocol <i>- annoying because it is part of the bluetooth standard.</i>
I want to make my own using Swift (the native development language for macOS), but that requires actually <em>learning</em> Swift first, so the project is still on the back burner.


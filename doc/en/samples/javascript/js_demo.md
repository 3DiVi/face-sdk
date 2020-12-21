# Face SDK VideoEngine JS demo. User Guide

This guide provides information on available features of the **Face SDK VideoEngine** JS demo that demonstrates [face detection and tracking](/doc/en/development/face_capturing.md), as well as active liveness estimation.

## Interface of the demo program

<p align="center">
<img width="600" src="/doc/img/js_demo.png"><br>
</p>

* On the left side of the screen, the video stream from the webcam (*Stream*) is displayed.
* To start the Active Liveness check, press the *Start* button. During the processing, a progress bar and the phrase "Wait, detection is in progress ..." are displayed above the window with the video stream. A face is considered to belong to a real person if at least one blink was made during processing and the head was turned at least once.
* The best shot of the face is displayed to the right of the video stream. If the face is not positioned correctly, the browser will display hints during processing, for example, "Please turn your head up", "Please come closer to the camera", etc.
* Press the *Stop* button to stop the Active Liveness check.
* To clear the received data, press the *Clear* button.

# Arduino-Project

A system that adjusts the power of LED lights according to the level of lighting in the environment.

<i><h3>Components:</h3>
> Arduino ESP32         <br>
> PhotoLight Sensor     <br>
> 2 LEDs                <br>
> Resistors 210Ω, 10KΩ  </i><br><br>

The photo light sensor receives values about the environment and sends them to the server. Two LEDs: orange and green, get that values.<br>
Orange LED: uses PWM to adjust its light power according to the level of darkness in the environment. The less light the sensor gets - the more bright the LED illuminates.<br>
Green LED: Turns on if the level of darkness exceeds a certain value, and turns off if the level of darkness is lower than this value.


<img src="https://user-images.githubusercontent.com/77194094/119560517-b5f9be80-bdac-11eb-9cb2-10a7d8d1c3d5.png" width="50%" height="50%">


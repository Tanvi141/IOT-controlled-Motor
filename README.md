# IOT-controlling-motor
Embedded Systems Workshop Project

## Generating Data Analytics Graphs

To gerate the graphs the following steps are followed:

* Run the python script to scrape the data from the oneM2M server.
  ```bash
    python3 start.py
  ```

* Run the python script to produce graphs from the the data from the OneM2M server.
  ```bash
    python3 update.py
  ```

* Save the graphs with the following names so as to view the updated versions on the dashboard.

    * distance1.png
    * distance2.png
    * rpm_o.png
    * daily.png
    * hourly.png

* The Arduino Code flashed on the ESP32 is in the directory "Final_Code"


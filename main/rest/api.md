# REST API documentation

## system info
  Returns json data with infos about current system.

* **URL:** `/system/info`

* **Method:** `GET`

* **Example Response Content:**
    ```json
    {
        "projectName": "example project",
        "projectVersion": "v1.0.0",
        "compileDate": "Jan 1 1970", 
        "idfVersion": "v1.0.0", 
        "model": 1, 
        "cores": 2 
    }
    ```
----
## system clock
  Returns json data with local clock and used timezone.

* **URL:** `/system/clock`

* **Method:** `GET`

* **Example Response Content:**
    ```json
    {
        "clock": "12:30",
        "timezone": "CET-1CEST,M3.5.0,M10.5.0/"
    }
    ```
----
## wifi station
  Return json data with infos about wifi station.

* **URL:** `/wifi/station`

* **Method:** `GET`

* **Example Response Content:**
    ```json
    {
        "ssid": "WifiSsid",
        "ipAddress": "x.x.x.x",
        "macAddress": "x.x.x.x.x.x",
        "channel" : 1,
        "authMode" :  "WPA3"
    }
    ```
----
## mode request
  Return the current mode

* **URL:** `/mode`

* **Method:** `GET`

* **Example Response Content:**
    ```
    1
    ```
----
## mode setter
  Set the mode

* **URL:** `/mode`

* **Method:** `PUT`

* **Example PUT Content:**
    ```
    1
    ```

* **Returned code**
  * 200 at success
  * 404 mode not found
----
## free text setter
  Set the free text showing on all strips

* **URL:** `/freetext`

* **Method:** `PUT`

* **Example PUT Content:**
    ```json
    {
      "lines": ["line1","line2","line3","line4","line5"]
    }
    ```

* **Returned code**
  * 200 at success
  * 400 bad request

----
## running text setter
  Set the running tet showing on last strip

* **URL:** `/runningtext`

* **Method:** `PUT`

* **Example PUT Content:**
    ```json
    {
      "text": "Hello World",
      "speed": 20
    }
    ```

* **Returned code**
  * 200 at success
  * 400 bad request

----
## additional vehicles setter
  Set a list of additional vehicles to show on display

* **URL:** `/additionalvehicles`

* **Method:** `PUT`

* **Example PUT Content:**
    ```json
    {
      "vehicle": [
        {
          "destination": "Herrenkrug",
          "departure": "hh:mm"
        },
        {
          "destination": "Alter Markt",
          "departure": "hh:mm"
        }
      ]
    }
    ```

* **Returned code**
  * 200 with valid body as response, otherwise empty list

----
## additional vehicles getter
  Return a list of additional vehicles

* **URL:** `/additionalvehicles`

* **Method:** `GET`

* **Example GET Content:**
    ```json
    {
      "vehicle": [
        {
          "destination": "Herrenkrug",
          "departure": "hh:mm"
        },
        {
          "destination": "Alter Markt",
          "departure": "hh:mm"
        }
      ]
    }
    ```
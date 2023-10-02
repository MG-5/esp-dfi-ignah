# REST API documentation

## system info
  Returns json data with infos about current system.

* **URL:** `/system/info`

* **Method:** `GET`

* **Example Response Content:**
    ```json
    {
        "projectName": "example project",
        "projectVersion": "42af7d1",  // git commit id
        "compileDate": "Jan 1 1970", 
        "idfVersion": "v4.4.3", 
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
## modes

* 0 -> normal mode (showing vehicles)
* 1 -> like normal mode but last strip is showing running text
* 2 -> free text mode on all lines

## mode getter/setter
  Get/Set the mode

* **URL:** `/mode`

* **Method:** `GET` `PUT`

* **Example PUT Content:**
    ```
    1
    ```

* **Returned code at PUT**
  * 200 at success
  * 404 mode not found
  
----
## free text getter/setter
  Get/Set the free text showing on all strips

* **URL:** `/freetext`

* **Method:** `GET` `PUT`

* **Example GET/PUT Content:**
    ```json
    {
      "lines": ["line1","line2","line3","line4","line5"]
    }
    ```

* **Returned code at PUT**
  * 200 at success
  * 400 bad request

----
## running text getter/setter
  Get/Set the running text showing on last strip

* **URL:** `/runningtext`

* **Method:** `GET` `PUT`

* **Example GET/PUT Content:**
    ```json
    {
      "text": "Hello World",
      "speed": 20
    }
    ```

* **Returned code at PUT**
  * 200 at success
  * 400 bad request

----
## additional vehicles getter/setter
  Get/Set a list of additional vehicles to show on display

* **URL:** `/additionalvehicles`

* **Method:** `GET` `PUT`

* **Example GET/PUT Content:**
    ```json
    {
      "vehicles": [
        {
          "lineNumber": "77",
          "destination": "Herrenkrug",
          "departure": "13:30"
        },
        {
          "lineNumber": "N11",
          "destination": "Alter Markt",
          "departure": "23:15"
        }
      ]
    }
    ```

* **Returned code at PUT**
  * 200 with valid body as response, otherwise empty list
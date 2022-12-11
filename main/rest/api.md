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
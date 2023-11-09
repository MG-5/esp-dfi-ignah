import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { SystemClock } from 'src/app/models/system-clock';
import { SystemInfo } from 'src/app/models/system-info';
import { DfiStationSettings } from 'src/app/models/dfi-station-settings';
import { LightSensorSettings } from 'src/app/models/light-sensor';
import { HostService } from '../host.service';

@Injectable({
  providedIn: 'root'
})
export class SystemService {

  constructor(private httpClient: HttpClient, private hostService: HostService) { }

  getSystemInfo(): Observable<SystemInfo> {
    return this.httpClient.get<SystemInfo>(`${this.hostService.baseUrl}/system/info`);
  }

  getSystemClock(): Observable<SystemClock> {
    return this.httpClient.get<SystemClock>(`${this.hostService.baseUrl}/system/clock`);
  }

  getDfiStationSettings(): Observable<DfiStationSettings> {
    return this.httpClient.get<DfiStationSettings>(`${this.hostService.baseUrl}/dfi/station`);
  }

  setDfiStationSettings(dfiStationSettings: DfiStationSettings): Observable<any> {
    return this.httpClient.put(`${this.hostService.baseUrl}/dfi/station`, dfiStationSettings);
  }

  getLightSensor(): Observable<LightSensorSettings> {
    return this.httpClient.get<LightSensorSettings>(`${this.hostService.baseUrl}/lightsensor`);
  }

  setLightSensor(lightSensor: LightSensorSettings): Observable<any> {
    return this.httpClient.put(`${this.hostService.baseUrl}/lightsensor`, lightSensor);
  }
}

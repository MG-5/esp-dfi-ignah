import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { WifiStationSettings } from 'src/app/models/wifi-station-settings';
import { HostService } from '../host.service';

@Injectable({
  providedIn: 'root'
})
export class WifiService {

  constructor(private httpClient: HttpClient, private hostService: HostService) { }

  getStationSettings(): Observable<WifiStationSettings> {
    return this.httpClient.get<WifiStationSettings>(`${this.hostService.baseUrl}/wifi/station`);
  }
}

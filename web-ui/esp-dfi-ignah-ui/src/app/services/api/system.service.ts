import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { SystemClock } from 'src/app/models/system-clock';
import { SystemInfo } from 'src/app/models/system-info';
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
}

import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { DestinationBlocklist } from '@app/models/destination-blocklist';
import { HostService } from '../host.service';

@Injectable({
  providedIn: 'root'
})
export class DestinationBlocklistService {

  constructor(private httpClient: HttpClient, private hostService: HostService) { }

  getDestinationBlocklist(): Observable<DestinationBlocklist> {
    return this.httpClient.get<DestinationBlocklist>(`${this.hostService.baseUrl}/destinationblocklist`);
  }

  setDestinationBlocklist(destinations: DestinationBlocklist): Observable<DestinationBlocklist> {
    return this.httpClient.put<DestinationBlocklist>(`${this.hostService.baseUrl}/destinationblocklist`, destinations);
  }
}

import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { HostService } from '../host.service';

@Injectable({
  providedIn: 'root'
})
export class ModeService {

  constructor(private httpClient: HttpClient, private hostService: HostService) { }

  getMode(): Observable<number> {
    return this.httpClient.get<number>(`${this.hostService.baseUrl}/mode`);
  }

  setMode(mode: number): Observable<any> {
    return this.httpClient.put(`${this.hostService.baseUrl}/mode`, mode, { responseType: 'text' });
  }
}

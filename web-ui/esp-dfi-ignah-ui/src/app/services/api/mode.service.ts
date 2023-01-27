import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { Mode } from 'src/app/models/mode';
import { HostService } from '../host.service';

@Injectable({
  providedIn: 'root'
})
export class ModeService {

  constructor(private httpClient: HttpClient, private hostService: HostService) { }

  getMode(): Observable<Mode> {
    return this.httpClient.get<Mode>(`${this.hostService.baseUrl}/mode`);
  }

  setMode(mode: Mode): Observable<any> {
    return this.httpClient.put(`${this.hostService.baseUrl}/mode`, mode, { responseType: 'text' });
  }
}

import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { FreeText } from 'src/app/models/free-text';
import { RunningText } from 'src/app/models/running-text';
import { HostService } from '../host.service';

@Injectable({
  providedIn: 'root'
})
export class TextService {

  constructor(private httpClient: HttpClient, private hostService: HostService) { }

  getFreeText(): Observable<FreeText> {
    return this.httpClient.get<FreeText>(`${this.hostService.baseUrl}/freetext`);
  }

  setFreeText(freeText: FreeText): Observable<any> {
    return this.httpClient.put(`${this.hostService.baseUrl}/freetext`, freeText);
  }

  getRunningText(): Observable<RunningText> {
    return this.httpClient.get<RunningText>(`${this.hostService.baseUrl}/runningtext`);
  }

  setRunningText(runningText: RunningText): Observable<any> {
    return this.httpClient.put(`${this.hostService.baseUrl}/runningtext`, runningText);
  }
}

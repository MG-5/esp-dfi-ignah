import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { AdditionalVehicles } from 'src/app/models/additional-vehicles';
import { HostService } from '../host.service';

@Injectable({
  providedIn: 'root'
})
export class VehicleService {

  constructor(private httpClient: HttpClient, private hostService: HostService) { }

  getAdditionalVehicles(): Observable<AdditionalVehicles> {
    return this.httpClient.get<AdditionalVehicles>(`${this.hostService.baseUrl}/additionalvehicles`);
  }

  setAdditionalVehicles(vehicles: AdditionalVehicles): Observable<AdditionalVehicles> {
    return this.httpClient.put<AdditionalVehicles>(`${this.hostService.baseUrl}/additionalvehicles`, vehicles);
  }
}

import { Injectable } from '@angular/core';
import { environment } from 'src/environments/environment';

@Injectable({
  providedIn: 'root'
})
export class HostService {

  public hostname: string = environment.hostname;

  public get baseUrl(): string {
    return `http://${this.hostname}`;
  }

  constructor() { }
}

import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class HostService {

  public hostname: string = 'esp-idf-ignah';

  public get baseUrl(): string {
    return `http://${this.hostname}`;
  }

  constructor() { }
}

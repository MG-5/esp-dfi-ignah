import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class HostService {
  // only relative paths
  public get baseUrl(): string {
    return ``;
  }

  constructor() { }
}

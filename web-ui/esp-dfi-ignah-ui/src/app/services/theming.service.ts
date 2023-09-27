import { DOCUMENT } from '@angular/common';
import { Inject, Injectable } from '@angular/core';

export enum Theme {
  Light,
  Dark
}

@Injectable({
  providedIn: 'root'
})
export class ThemingService {

  readonly lightThemeClass = 'light-theme';

  constructor(@Inject(DOCUMENT) private document: Document) { }

  setLightTheme(): void {
    this.document.documentElement.classList.add(this.lightThemeClass);
  }

  setDarkTheme(): void {
    this.document.documentElement.classList.remove(this.lightThemeClass);
  }

  getCurrentTheme(): Theme {
    if (this.document.documentElement.classList.contains(this.lightThemeClass)) {
      return Theme.Light;
    } else {
      return Theme.Dark;
    }
  }
}

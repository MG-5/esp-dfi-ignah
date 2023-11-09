import { BreakpointObserver, Breakpoints } from '@angular/cdk/layout';
import { Component, OnInit } from '@angular/core';
import { EMPTY, map, Observable } from 'rxjs';
import { navigationEntries } from './data/navigation';
import { Theme, ThemingService } from './services/theming.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  title = 'esp-dfi-ignah-ui';

  isSmallLayout$: Observable<boolean> = EMPTY;

  theme = Theme;

  navigationEntries = navigationEntries;

  constructor(private breakpointObserver: BreakpointObserver, public themingService: ThemingService) { }

  ngOnInit(): void {
    this.isSmallLayout$ = this.breakpointObserver.observe([Breakpoints.Small, Breakpoints.XSmall]).pipe(map(state => state.matches));

    window.matchMedia('(prefers-color-scheme: light)').addEventListener('change', event => {
      if (event.matches) {
        this.themingService.setLightTheme();
      } else {
        this.themingService.setDarkTheme();
      }
  });
  }
}

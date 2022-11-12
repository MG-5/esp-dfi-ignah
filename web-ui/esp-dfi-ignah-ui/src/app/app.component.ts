import { BreakpointObserver, Breakpoints } from '@angular/cdk/layout';
import { Component, OnInit } from '@angular/core';
import { EMPTY, map, Observable } from 'rxjs';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  title = 'esp-dfi-ignah-ui';

  isSmallLayout$: Observable<boolean> = EMPTY;

  constructor(private breakpointObserver: BreakpointObserver) { }

  ngOnInit(): void {
    this.isSmallLayout$ = this.breakpointObserver.observe([Breakpoints.Small, Breakpoints.XSmall]).pipe(map(state => state.matches));
  }
}

import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { Mode } from 'src/app/models/mode';
import { getMode, selectMode, setMode } from 'src/app/store/mode';

@Component({
  selector: 'app-display',
  templateUrl: './display.component.html',
  styleUrls: ['./display.component.scss']
})
export class DisplayComponent implements OnInit {

  Mode = Mode;

  mode$ = this.store.select(selectMode);

  constructor(private store: Store) { }

  ngOnInit(): void {
    this.store.dispatch(getMode());
  }

  saveMode(mode: Mode): void {
    this.store.dispatch(setMode({mode}));
  }
}
